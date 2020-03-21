//
// INTEL CORPORATION PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
//
// Copyright(C) 2008-2017 Intel Corporation. All Rights Reserved.
//
#include "asc.h"
static mfxI8
    PDISTTbl2[NumTSC*NumSC] =
{
    2, 3, 3, 4, 4, 5, 5, 5, 5, 5,
    2, 2, 3, 3, 4, 4, 5, 5, 5, 5,
    1, 2, 2, 3, 3, 3, 4, 4, 5, 5,
    1, 1, 2, 2, 3, 3, 3, 4, 4, 5,
    1, 1, 2, 2, 3, 3, 3, 3, 3, 4,
    1, 1, 1, 2, 2, 3, 3, 3, 3, 3,
    1, 1, 1, 1, 2, 2, 3, 3, 3, 3,
    1, 1, 1, 1, 2, 2, 2, 3, 3, 3,
    1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static mfxU32 lmt_sc2[NumSC] = { 112, 255, 512, 1536, 4096, 6144, 10752, 16384, 23040, UINT_MAX };
static mfxU32 lmt_tsc2[NumTSC] = { 24, 48, 72, 96, 128, 160, 192, 224, 256, UINT_MAX };

int TableLookUp(mfxU32 limit, mfxU32 *table, mfxU32 comparisonValue) {
    for (int pos = 0; pos < (int)limit; pos++) {
        if (comparisonValue < table[pos])
            return pos;
    }
    return limit;
}

void ASC::Setup_Environment() {

    dataIn->accuracy = 1;

    dataIn->layer->Original_Width  = SMALL_WIDTH;
    dataIn->layer->Original_Height = SMALL_HEIGHT;
    dataIn->layer->_cwidth  = SMALL_WIDTH;
    dataIn->layer->_cheight = SMALL_HEIGHT;

    dataIn->layer->block_width      = 8;
    dataIn->layer->block_height     = 8;
    dataIn->layer->vertical_pad     = 0;
    dataIn->layer->horizontal_pad   = 0;
    dataIn->layer->Extended_Height  = dataIn->layer->vertical_pad + SMALL_HEIGHT + dataIn->layer->vertical_pad;
    dataIn->layer->Extended_Width   = dataIn->layer->horizontal_pad + SMALL_WIDTH  + dataIn->layer->horizontal_pad;
    dataIn->layer->pitch            = dataIn->layer->Extended_Width;
    dataIn->layer->Height_in_blocks = dataIn->layer->_cheight / dataIn->layer->block_height;
    dataIn->layer->Width_in_blocks  = dataIn->layer->_cwidth  / dataIn->layer->block_width;
    dataIn->layer->sidesize         = dataIn->layer->_cheight + (1 * dataIn->layer->vertical_pad);
    dataIn->layer->initial_point    = (dataIn->layer->Extended_Width * dataIn->layer->vertical_pad) + dataIn->layer->horizontal_pad;
    dataIn->layer->MVspaceSize      = (dataIn->layer->_cheight / dataIn->layer->block_height) * (dataIn->layer->_cwidth / dataIn->layer->block_width); 
}
void ASC::Params_Init() {
    dataIn->accuracy                = 1;
    dataIn->processed_frames        = 0;
    dataIn->PDistFile               = "pdist.txt";
    dataIn->total_number_of_frames  = -1;
    dataIn->starting_frame          = 0;
    dataIn->key_frame_frequency     = INT_MAX;
    dataIn->limitRange              = 0;
    dataIn->maxXrange               = 32;
    dataIn->maxYrange               = 32;
    dataIn->interlaceMode           = 0;
    dataIn->StartingField           = ASCTopField;
    dataIn->currentField            = ASCTopField;
    ImDetails_Init(dataIn->layer);

    dataIn->timer.calctime          = 0.0;
    dataIn->timer.calctime2         = 0.0;
    dataIn->timer.timeval           = 0.0;
}

ASC_API void ASC::SetInterlaceMode(mfxU32 interlaceMode) {
    if(interlaceMode > ASCbottomfieldFirst_frame) {
        printf("\nError: Interlace Mode invalid, valid values are: 0 (progressive), 1 (TFF), 2 (BFF)\n");
        exit(3008);
    }
    else
        dataIn->interlaceMode = interlaceMode;
    dataIn->StartingField = ASCTopField;
    if (dataIn->interlaceMode != ASCprogressive_frame) {
        if (dataIn->interlaceMode == ASCbottomfieldFirst_frame)
            dataIn->StartingField = ASCBottomField;
        resizeFunc = &ASC::SubSampleASC_ImageInt;
        pPutFrameFunc = &ASC::PutFrameInterlaced;
    }
    else {
        resizeFunc = &ASC::SubSampleASC_ImagePro;
        pPutFrameFunc = &ASC::PutFrameProgressive;
    }
    dataIn->currentField = dataIn->StartingField;
}

void ASCimageData::InitFrame(ASCImDetails *pDetails) {
    mfxU32
        imageSpaceSize  = pDetails->Extended_Height * pDetails->Extended_Width * 2,
        mvSpaceSize     = (pDetails->_cheight * pDetails->_cwidth) >> 6,
        texSpaceSize    = (pDetails->_cheight * pDetails->_cwidth) >> 4;

    Image.extHeight         = pDetails->Extended_Height;
    Image.extWidth          = pDetails->Extended_Width;
    Image.pitch             = pDetails->Extended_Width;
    Image.height            = pDetails->_cheight;
    Image.width             = pDetails->_cwidth;
    Image.hBorder           = pDetails->vertical_pad;
    Image.wBorder           = pDetails->horizontal_pad;
    Image.data              = NULL;
    Image.Y                 = NULL;
    Image.U                 = NULL;
    Image.V                 = NULL;
    //Memory Allocation
#if (defined( _WIN32 ) || defined ( _WIN64 )) && !defined (__GNUC__)
    Image.data = (mfxU8  *)_aligned_malloc(imageSpaceSize,                0x1000);
    SAD        = (mfxU16 *)_aligned_malloc(sizeof(mfxU16) * mvSpaceSize,  0x1000);
    Rs         = (mfxU16 *)_aligned_malloc(sizeof(mfxU16) * texSpaceSize, 0x1000);
    Cs         = (mfxU16 *)_aligned_malloc(sizeof(mfxU16) * texSpaceSize, 0x1000);
    RsCs       = (mfxU16 *)_aligned_malloc(sizeof(mfxU16) * texSpaceSize, 0x1000);
    pInteger   = (ASCMv  *)_aligned_malloc(sizeof(ASCMv)  * mvSpaceSize,  0x1000);
#else
    Image.data = (mfxU8  *)aligned_alloc(0x1000, imageSpaceSize);
    SAD        = (mfxU16 *)aligned_alloc(0x1000, sizeof(mfxU16) * mvSpaceSize);
    Rs         = (mfxU16 *)aligned_alloc(0x1000, sizeof(mfxU16) * texSpaceSize);
    Cs         = (mfxU16 *)aligned_alloc(0x1000, sizeof(mfxU16) * texSpaceSize);
    RsCs       = (mfxU16 *)aligned_alloc(0x1000, sizeof(mfxU16) * texSpaceSize);
    pInteger   = (ASCMv  *)aligned_alloc(0x1000, sizeof(ASCMv)  * mvSpaceSize);
#endif
    //Pointer conf.
    memset(Image.data, 0, sizeof(mfxU8) * imageSpaceSize);
    Image.Y = Image.data + pDetails->initial_point;
    memset(Rs, 0, sizeof(mfxU16) * texSpaceSize);
    memset(Cs, 0, sizeof(mfxU16) * texSpaceSize);
    memset(RsCs, 0, sizeof(mfxU16) * texSpaceSize);
    memset(pInteger, 0, sizeof(ASCMv)  * mvSpaceSize);
    var = 0;
    jtvar = 0;
    mcjtvar = 0;
    tcor = 0;
    mcTcor = 0;
    mu_mv_mag_sq= 0;
}

mfxI32 ASCimageData::Close() {
    mfxI32
        res = 0;
#if defined(_WIN32) || defined(_WIN64)
    if (Rs)
        _aligned_free(Rs);
    if (Cs)
        _aligned_free(Cs);
    if (RsCs)
        _aligned_free(RsCs);
    if (pInteger)
        _aligned_free(pInteger);
    if (SAD)
        _aligned_free(SAD);
#else
    if (Rs)
        free(Rs);
    if (Cs)
        free(Cs);
    if (RsCs)
        free(RsCs);
    if (pInteger)
        free(pInteger);
    if (SAD)
        free(SAD);
#endif
    Rs = NULL;
    Cs = NULL;
    RsCs = NULL;
    pInteger = NULL;
    SAD = NULL;
#if defined(_WIN32) || defined(_WIN64)
    _aligned_free(Image.data);
#else
    free(Image.data);
#endif
    Image.data = NULL;
    Image.Y = NULL;
    Image.U = NULL;
    Image.V = NULL;

    return res;
}

void ASC::VidSample_Alloc() {
    for (mfxI32 i = 0; i < VIDEOSTATSBUF; i++)
        videoData[i]->layer.InitFrame(dataIn->layer);
}

void ASC::VidSample_dispose() {
    for(mfxI32 i = 0; i < VIDEOSTATSBUF; i++) {
        if(videoData[i] != NULL) {
            videoData[i]->layer.Close();
            delete (videoData[i]);
        }
    }
}
void ASC::VidRead_dispose() {
    if(support->logic != NULL) {
        for(mfxI32 i = 0; i < TSCSTATBUFFER; i++)
            delete support->logic[i];
        delete[] support->logic;
    }
    if(support->gainCorrection.Image.data != NULL)
        support->gainCorrection.Close();
}

void ASC::alloc() {
    VidSample_Alloc();
}

void ASC::InitCPU() {
    alloc();
}

void ASC::InitStruct() {
    dataIn = NULL;
    support = NULL;
    videoData = NULL;
    resizeFunc = NULL;
}

void ASC::VidRead_Init() {
    support->control = 0;
    support->average = 0;
    support->avgSAD = 0;
    support->gopSize = 1;
    support->pendingSch = 0;
    support->lastSCdetectionDistance = 0;
    support->detectedSch = 0;
    support->logic = new ASCTSCstat *[TSCSTATBUFFER];
    ASCTSCstat_Init(support->logic);
    support->PDistanceTable = PDISTTbl2;
    support->detectFunc = NULL;
    support->size = ASCSmall_Size;
    support->firstFrame = true;
    support->gainCorrection.Image.data = NULL;
    support->gainCorrection.Image.Y = NULL;
    support->gainCorrection.Image.U = NULL;
    support->gainCorrection.Image.V = NULL;
    support->gainCorrection.Cs = NULL;
    support->gainCorrection.Rs = NULL;
    support->gainCorrection.RsCs = NULL;
    support->gainCorrection.pInteger = NULL;
    support->gainCorrection.SAD = NULL;
    support->gainCorrection.pAfd = NULL;
    support->gainCorrection.pAfdV = NULL;
    support->gainCorrection.pAvgV = NULL;
    support->gainCorrection.texAcc = NULL;
    
    support->gainCorrection.InitFrame(dataIn->layer);
}

void ASC::VidSample_Init() {
    for(mfxI32 i = 0; i < VIDEOSTATSBUF; i++) {
        nullifier(&videoData[i]->layer);
        imageInit(&videoData[i]->layer.Image);
        videoData[i]->frame_number = -1;
        videoData[i]->forward_reference = -1;
        videoData[i]->backward_reference = -1;
    }
}

void ASC::SetUltraFastDetection() {
    support->size = ASCSmall_Size;
    support->detectFunc = NULL;
    resizeFunc = &ASC::SubSampleASC_ImagePro;
    pPutFrameFunc = &ASC::PutFrameProgressive;
}

void ASC::SetWidth(mfxI32 Width) {
    if(Width < SMALL_WIDTH) {
        printf("\nError: Width value is too small, it needs to be bigger than %i\n", SMALL_WIDTH);
        exit(3003);
    }
    else
        _width = Width;
}

void ASC::SetHeight(mfxI32 Height) {
    if(Height < SMALL_HEIGHT) {
        printf("\nError: Height value is too small, it needs to be bigger than %i\n", SMALL_HEIGHT);
        exit(3004);
    }
    else
        _height = Height;
}

void ASC::SetRes() {
    if (_width < SMALL_WIDTH || _height < SMALL_HEIGHT) {
        printf("\nError: Dimensions have not been set");
        exit(3005);
    }
    else
        _res = _width * _height;
}

void ASC::SetPitch(mfxI32 Pitch) {
    if(_width < SMALL_WIDTH) {
        printf("\nError: Width value has not been set, init the variables first\n");
        exit(3006);
    }

    if(Pitch < _width) {
        printf("\nError: Pitch value is too small, it needs to be bigger than %i\n", _width);
        exit(3005);
    }
    else
        _pitch = Pitch;
}

void ASC::SetGpuPitch(mfxI32 sysPitch) {
    if (_width < SMALL_WIDTH) {
        printf("\nError: Width value has not been set, init the variables first\n");
        exit(3006);
    }

    if (sysPitch < _width) {
        printf("\nError: Pitch value is too small, it needs to be bigger than %i\n", _width);
        exit(3005);
    }
    else
        gpupitch = sysPitch;
}

void ASC::SetNextField() {
    if(dataIn->interlaceMode != ASCprogressive_frame)
        dataIn->currentField = !dataIn->currentField;
}

void ASC::SetDimensions(mfxI32 Width, mfxI32 Height, mfxI32 Pitch) {
    SetWidth(Width);
    SetHeight(Height);
    SetRes();
    SetPitch(Pitch);
}

ASC_API void ASC::Init(mfxI32 Width, mfxI32 Height, mfxI32 Pitch, mfxU32 interlaceMode) {
    InitStruct();
    dataIn = new ASCVidData;
    dataIn->layer = NULL;
    dataIn->layer = new ASCImDetails;
    videoData = new ASCVidSample *[VIDEOSTATSBUF];
    support = new ASCVidRead;
    for (mfxI32 i = 0; i < VIDEOSTATSBUF; i++)
        videoData[i] = new ASCVidSample;
    Params_Init();
    SetDimensions(Width, Height, Pitch);
    Setup_Environment();
    VidSample_Init();
   
    GPUProc = false;
    InitCPU();
    
    VidRead_Init();
    SetUltraFastDetection();
    SetInterlaceMode(interlaceMode);
    dataReady = false;

    TimeStart(&dataIn->timer);
}

ASC_API void ASC::SetControlLevel(mfxU8 level) {
    if(level >= RF_DECISION_LEVEL) {
        printf("\nWarning: Control level too high, shot change detection disabled! (%i)\n", level);
        printf("Control levels 0 to %i, smaller value means more sensitive detection\n", RF_DECISION_LEVEL);
    }
    support->control = level;
}

ASC_API void ASC::SetGoPSize(mfxU32 GoPSize) {
    if(GoPSize > Double_HEVC_Gop) {
        printf("\nError: GoPSize is too big! (%i)\n", GoPSize);
        exit(3002);
    }
    else if(GoPSize == Forbidden_GoP) {
        printf("\nError: GoPSize value cannot be zero!\n");
        exit(3001);
    }
    else if(GoPSize > HEVC_Gop && GoPSize <= Double_HEVC_Gop)
        printf("\nWarning: Your GoPSize is larger than usual! (%i)\n", GoPSize);

    support->gopSize = GoPSize;
    support->pendingSch = 0;
}

ASC_API void ASC::ResetGoPSize() {
    SetGoPSize(Immediate_GoP);
}

ASC_API void ASC::Close() {
    if(videoData) {
        VidSample_dispose();
        delete[] videoData;
        videoData = NULL;
    }

    if(support) {
        VidRead_dispose();
        delete support;
        support = NULL;
    }
    
    if(dataIn) {
        delete dataIn->layer;
        delete dataIn;
        dataIn = NULL;
    }
}

ASC_API void ASC::Close(FILE *videoFile, FILE *scReportFile) {
    fclose(videoFile);
    fclose(scReportFile);
    Close();
}

void SubSample_Point_p(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch, pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch, unsigned short *avgLuma);
void SubSample_Point_t(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch, pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch, unsigned short *avgLuma);
void SubSample_Point_b(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch, pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch, unsigned short *avgLuma);

void ASC::SubSampleASC_ImagePro(mfxU8 *frame, mfxI32 srcWidth, mfxI32 srcHeight, mfxI32 inputPitch, ASCLayers dstIdx, mfxU32 /*parity*/) {
    /*parity;*/
    ASCImDetails *pIDetDst = &dataIn->layer[dstIdx];
    mfxU8 *pDst = videoData[ASCCurrent_Frame]->layer.Image.Y;
    mfxU16 *avgLuma = &videoData[ASCCurrent_Frame]->layer.avgval;

    int dstWidth = pIDetDst->Original_Width;
    int dstHeight = pIDetDst->Original_Height;
    int dstPitch = pIDetDst->pitch;

    SubSample_Point_p(frame, srcWidth, srcHeight, inputPitch, pDst, dstWidth, dstHeight, dstPitch, avgLuma);
}

void ASC::SubSampleASC_ImageInt(mfxU8 *frame, mfxI32 srcWidth, mfxI32 srcHeight, mfxI32 inputPitch, ASCLayers dstIdx, mfxU32 parity) {

    ASCImDetails *pIDetDst = &dataIn->layer[dstIdx];
    mfxU8 *pDst = videoData[ASCCurrent_Frame]->layer.Image.Y;
    mfxU16 *avgLuma = &videoData[ASCCurrent_Frame]->layer.avgval;

    int dstWidth = pIDetDst->Original_Width;
    int dstHeight = pIDetDst->Original_Height;
    int dstPitch = pIDetDst->pitch;

    if(!parity)
        SubSample_Point_t(frame, srcWidth, srcHeight, inputPitch, pDst, dstWidth, dstHeight, dstPitch, avgLuma);
    else
        SubSample_Point_b(frame, srcWidth, srcHeight, inputPitch, pDst, dstWidth, dstHeight, dstPitch, avgLuma);
}


//
// SubSample pSrc into pDst, using point-sampling of source pixels
// Corrects the position on odd lines in case the input video is
// interlaced
//
extern void SubSample_Point_p(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch,
    pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch,
    unsigned short *avgLuma) {
    unsigned int
        step_w = srcWidth / dstWidth,
        step_h = srcHeight / dstHeight;
    unsigned int
        need_correction = !(step_h % 2),
        correction = 0;
    unsigned int
        sumAll = 0;
    unsigned int y = 0;

    for (y = 0; y < dstHeight; y++) {
        correction = (y % 2) & need_correction;
        for (unsigned int x = 0; x < dstWidth; x++) {

            pmfxU8 ps = pSrc + (((y * step_h) + correction) * srcPitch) + (x * step_w);
            pmfxU8 pd = pDst + (y * dstPitch) + x;

            pd[0] = ps[0];
            sumAll += ps[0];
        }
    }
    *avgLuma = (unsigned short)(sumAll / (SMALL_WIDTH * SMALL_HEIGHT));
}

extern void SubSample_Point_t(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch,
    pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch,
    unsigned short *avgLuma) {
    unsigned int
        step_w = srcWidth / dstWidth,
        step_h = srcHeight / dstHeight;
    unsigned int
        need_correction = (step_h % 2),
        correction = 0;
    unsigned int
        sumAll = 0;
    unsigned int y = 0;

    for (y = 0; y < dstHeight; y++) {
        correction = (y % 2) & need_correction;
        for (unsigned int x = 0; x < dstWidth; x++) {

            pmfxU8 ps = pSrc + (((y * step_h) + correction) * srcPitch) + (x * step_w);
            pmfxU8 pd = pDst + (y * dstPitch) + x;

            pd[0] = ps[0];
            sumAll += ps[0];
        }
    }
    *avgLuma = (unsigned short)(sumAll >> 13);
}

extern void SubSample_Point_b(pmfxU8 pSrc, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcPitch,
    pmfxU8 pDst, unsigned int dstWidth, unsigned int dstHeight, unsigned int dstPitch,
    unsigned short *avgLuma) {
    unsigned int
        step_w = srcWidth / dstWidth,
        step_h = srcHeight / dstHeight;
    unsigned int
        need_correction = !(step_h % 2),
        correction = 0;
    unsigned int
        sumAll = 0;
    unsigned int y = 0;

    for (y = 0; y < dstHeight; y++) {
        correction = !(y % 2) | need_correction;
        for (unsigned int x = 0; x < dstWidth; x++) {

            pmfxU8 ps = pSrc + (((y * step_h) + correction) * srcPitch) + (x * step_w);
            pmfxU8 pd = pDst + (y * dstPitch) + x;

            pd[0] = ps[0];
            sumAll += ps[0];
        }
    }
    *avgLuma = (unsigned short)(sumAll >> 13);
}


void RsCsCalc(ASCVidSample **videoData, ASCImDetails vidCar, ASCVidRead *support) {
    ASCYUV
        *pFrame = &videoData[ASCCurrent_Frame]->layer.Image;
    pmfxU8
        ss = pFrame->Y;
    mfxU32
        hblocks = (pFrame->height >> BLOCK_SIZE_SHIFT) /*- 2*/,
        wblocks = (pFrame->width  >> BLOCK_SIZE_SHIFT) /*- 2*/;
    mfxI16
        diff = videoData[ASCReference_Frame]->layer.avgval - videoData[ASCCurrent_Frame]->layer.avgval;
    ss = videoData[ASCReference_Frame]->layer.Image.Y;
    if(!support->firstFrame && abs(diff) >= GAINDIFF_THR)
        GainOffset(&ss, &support->gainCorrection.Image.Y, (mfxU16)vidCar._cwidth, (mfxU16)vidCar._cheight, (mfxU16)vidCar.Extended_Width, diff);
    ss = videoData[ASCCurrent_Frame]->layer.Image.Y;

    RsCsCalc_4x4(ss, pFrame->pitch, wblocks, hblocks, videoData[ASCCurrent_Frame]->layer.Rs, videoData[ASCCurrent_Frame]->layer.Cs);
    RsCsCalc_bound(videoData[ASCCurrent_Frame]->layer.Rs, videoData[ASCCurrent_Frame]->layer.Cs, videoData[ASCCurrent_Frame]->layer.RsCs, &videoData[ASCCurrent_Frame]->layer.RsVal, &videoData[ASCCurrent_Frame]->layer.CsVal, wblocks, hblocks);
}

bool Hint_LTR_op_on(mfxU32 RsCsDiff, mfxU32 MVDiff, mfxU32 AFD);

#if ENABLE_RF
mfxI32 ShotDetect(ASCimageData Data, ASCimageData DataRef, ASCImDetails imageInfo, ASCTSCstat *current, ASCTSCstat *reference, t_SCDetect /*detectFunc*/, mfxU8 controlLevel, mfxU8 /*print*/) {
    /*detectFunc;*/
#else
mfxI32 ShotDetect(ASCimageData Data, ASCimageData DataRef, ASCImDetails imageInfo, ASCTSCstat *current, ASCTSCstat *reference, t_SCDetect detectFunc) {
#endif

    pmfxU8
        ssFrame = Data.Image.Y,
        refFrame = DataRef.Image.Y;
    pmfxU16
        objRs = Data.Rs,
        objCs = Data.Cs,
        refRs = DataRef.Rs,
        refCs = DataRef.Cs;
    mfxU32
        histTOT = 0;
    
    current->RsCsDiff = 0;
    current->Schg = -1;
    current->Gchg = 0;

    RsCsCalc_diff(objRs, objCs, refRs, refCs, 2*imageInfo.Width_in_blocks, 2*imageInfo.Height_in_blocks, &current->RsDiff, &current->CsDiff);
    ImageDiffHistogram(ssFrame, refFrame, imageInfo.Extended_Width, imageInfo._cwidth, imageInfo._cheight, current->histogram, &current->ssDCint, &current->refDCint);

    if(reference->Schg)
        current->last_shot_distance = 1;
    else
        current->last_shot_distance++;

    current->RsDiff >>= 9;
    current->CsDiff >>= 9;
    current->RsCsDiff      = (current->RsDiff*current->RsDiff)  + (current->CsDiff*current->CsDiff);
    histTOT                = current->histogram[0] + current->histogram[1] + current->histogram[2] + current->histogram[3] + current->histogram[4];
    current->ssDCval       = (mfxI32)current->ssDCint >> 13;
    current->refDCval      = (mfxI32)current->refDCint >> 13;
    current->gchDC         = NABS(current->ssDCval - current->refDCval);
    current->Gchg          = current->ssDCval - current->refDCval;
    current->posBalance    = (current->histogram[3] + current->histogram[4]) >> 6;
    current->negBalance    = (current->histogram[0] + current->histogram[1]) >> 6;
    current->diffAFD       = current->AFD - reference->AFD;
    current->diffTSC       = current->TSC - reference->TSC;
    current->diffRsCsDiff  = current->RsCsDiff - reference->RsCsDiff;
    current->diffMVdiffVal = current->MVdiffVal - reference->MVdiffVal;
    mfxI32
        SChange = SCDetectRF(
            current->diffMVdiffVal, current->RsCsDiff,   current->MVdiffVal,
            current->Rs,            current->AFD,        current->CsDiff,
            current->diffTSC,       current->TSC,        current->gchDC,
            current->diffRsCsDiff,  current->posBalance, current->SC,
            current->TSCindex,      current->SCindex,    current->Cs,
            current->diffAFD,       current->negBalance, current->ssDCval,
            current->refDCval,      current->RsDiff,     current->tcor,
            current->mcTcor,        controlLevel);

    //reference->lastFrameInShot = current->Schg;

    current->ltr_flag = Hint_LTR_op_on(current->RsCsDiff, current->MVdiffVal, current->AFD);
    current->subFilterType = FilterSelectRF(
        current->diffMVdiffVal,
        current->RsCsDiff,
        current->MVdiffVal,
        current->Rs,
        current->AFD,
        current->CsDiff,
        current->diffTSC,
        current->TSC,
        current->gchDC,
        current->diffRsCsDiff,
        current->posBalance,
        current->SC,
        current->TSCindex,
        current->SCindex,
        current->Cs,
        current->diffAFD,
        current->negBalance,
        current->ssDCval,
        current->refDCval,
        current->RsDiff,
        current->mu_mv_mag_sq,
        current->tcor,
        current->mcTcor);
    return SChange;
}

mfxU16 ASC::AGOPSelect()
{
	mfxU16
		agop_result = 4;
	ASCTSCstat
		* current = support->logic[ASCcurrent_frame_data];
	agop_result = AGOPSelectRF(
		current->diffMVdiffVal,
		current->RsCsDiff,
		current->MVdiffVal,
		current->Rs,
		current->AFD,
		current->CsDiff,
		current->diffTSC,
		current->TSC,
		current->gchDC,
		current->diffRsCsDiff,
		current->posBalance,
		current->SC,
		current->TSCindex,
		current->SCindex,
		current->Cs,
		current->diffAFD,
		current->negBalance,
		current->ssDCval,
		current->refDCval,
		current->RsDiff,
		current->mu_mv_mag_sq,
		current->tcor,
		current->mcTcor);

	mfxU64
		distance = 0;
	if (!current->repeatedFrame)
	{
		mfxI64
			val = ((mfxI32)support->logic[ASCcurrent_frame_data]->diffMVdiffVal - (mfxI32)support->logic[ASCprevious_frame_data]->diffMVdiffVal);
		val *= val;
		distance = (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->RsCsDiff - (mfxI32)support->logic[ASCprevious_frame_data]->RsCsDiff);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->MVdiffVal - (mfxI32)support->logic[ASCprevious_frame_data]->MVdiffVal);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->Rs - (mfxI32)support->logic[ASCprevious_frame_data]->Rs);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->AFD - (mfxI32)support->logic[ASCprevious_frame_data]->AFD);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->CsDiff - (mfxI32)support->logic[ASCprevious_frame_data]->CsDiff);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->diffTSC - (mfxI32)support->logic[ASCprevious_frame_data]->diffTSC);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->TSC - (mfxI32)support->logic[ASCprevious_frame_data]->TSC);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->gchDC - (mfxI32)support->logic[ASCprevious_frame_data]->gchDC);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->diffRsCsDiff - (mfxI32)support->logic[ASCprevious_frame_data]->diffRsCsDiff);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->posBalance - (mfxI32)support->logic[ASCprevious_frame_data]->posBalance);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->SC - (mfxI32)support->logic[ASCprevious_frame_data]->SC);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->diffAFD - (mfxI32)support->logic[ASCprevious_frame_data]->diffAFD);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->negBalance - (mfxI32)support->logic[ASCprevious_frame_data]->negBalance);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->ssDCval - (mfxI32)support->logic[ASCprevious_frame_data]->ssDCval);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->refDCval - (mfxI32)support->logic[ASCprevious_frame_data]->refDCval);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->RsDiff - (mfxI32)support->logic[ASCprevious_frame_data]->RsDiff);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->mu_mv_mag_sq - (mfxI32)support->logic[ASCprevious_frame_data]->mu_mv_mag_sq);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->mcTcor - (mfxI32)support->logic[ASCprevious_frame_data]->mcTcor);
		val *= val;
		distance += (mfxU64)val;

		val = ((mfxI32)support->logic[ASCcurrent_frame_data]->tcor - (mfxI32)support->logic[ASCprevious_frame_data]->tcor);
		val *= val;
		distance += (mfxU64)val;
	}

	support->logic[ASCcurrent_frame_data]->distance = distance;

	return agop_result;
}

mfxU16 ASC::GetFrameGopSize()
{
	return support->logic[ASCprevious_frame_data]->gop_size; //this is done after buffer rotation
}

typedef std::function<bool(
	std::pair<mfxU32, mfxU32>,
	std::pair<mfxU32, mfxU32>)>
	MV_Comparator;

void MotionAnalysis(ASCVidRead *support, ASCVidData *dataIn, ASCVidSample *videoIn, ASCVidSample *videoRef, mfxU32 *TSC, mfxU16 *AFD, mfxU32 *MVdiffVal, mfxU32 *AbsMVSize, mfxU32 *AbsMVHSize, mfxU32 *AbsMVVSize, ASCLayers lyrIdx) {
	MV_Comparator Comparison = [](
		std::pair<mfxU32, mfxU32> mv_size1,
		std::pair<mfxU32, mfxU32> mv_size2
		)
	{
		return mv_size1.second != mv_size2.second ? mv_size1.second > mv_size2.second : mv_size1.first < mv_size2.first;
	};
	mfxU32//24bit is enough
        valb = 0;
    mfxU32
        acc = 0;
	/*--Motion Estimation--*/
	*MVdiffVal = 0;
	*AbsMVSize = 0;
	*AbsMVHSize = 0;
	*AbsMVVSize = 0;
	std::map<mfxU32, mfxU32>
		mv_freq;
    mfxI16
        diff = (int)videoIn->layer.avgval - (int)videoRef->layer.avgval;

    ASCimageData
        *referenceImageIn = &videoRef->layer;

    if (abs(diff) >= GAINDIFF_THR) {
        referenceImageIn = &support->gainCorrection;
    }
    support->average = 0;
    videoIn->layer.var = 0;
    videoIn->layer.jtvar = 0;
    videoIn->layer.mcjtvar = 0;
    for (mfxU16 i = 0; i < dataIn->layer[lyrIdx].Height_in_blocks; i++) {
        mfxU16 prevFPos = i << 4;
        for (mfxU16 j = 0; j < dataIn->layer[lyrIdx].Width_in_blocks; j++) {
            mfxU16 fPos = prevFPos + j;
            //acc += ME_simple(support, fPos, dataIn->layer, currentImageIn, &videoRef->layer, true, dataIn);
            acc += ME_simple(support, fPos, dataIn->layer, &videoIn->layer, referenceImageIn, true, dataIn);
            valb += videoIn->layer.SAD[fPos];
            *MVdiffVal += (videoIn->layer.pInteger[fPos].x - videoRef->layer.pInteger[fPos].x) * (videoIn->layer.pInteger[fPos].x - videoRef->layer.pInteger[fPos].x);
            *MVdiffVal += (videoIn->layer.pInteger[fPos].y - videoRef->layer.pInteger[fPos].y) * (videoIn->layer.pInteger[fPos].y - videoRef->layer.pInteger[fPos].y);
            *AbsMVHSize += (videoIn->layer.pInteger[fPos].x * videoIn->layer.pInteger[fPos].x);
            *AbsMVVSize += (videoIn->layer.pInteger[fPos].y * videoIn->layer.pInteger[fPos].y);
			mfxU32
				mv_size = (videoIn->layer.pInteger[fPos].x * videoIn->layer.pInteger[fPos].x) + (videoIn->layer.pInteger[fPos].y * videoIn->layer.pInteger[fPos].y);
			*AbsMVSize += mv_size;
			mv_freq[mv_size]++;
        }
    }
	std::set<std::pair<mfxU32, mfxU32>, MV_Comparator>
		mv_size_sorted(mv_freq.begin(), mv_freq.end(), Comparison);
	videoIn->layer.var = videoIn->layer.var * 10 / SMALL_WIDTH / SMALL_HEIGHT;
	videoIn->layer.jtvar = videoIn->layer.jtvar * 10 / SMALL_WIDTH / SMALL_HEIGHT;
	videoIn->layer.mcjtvar = videoIn->layer.mcjtvar * 10 / SMALL_WIDTH / SMALL_HEIGHT;
	support->mu_mv_mag_sq = mv_size_sorted.begin()->first;
	if (!support->mu_mv_mag_sq)
		support->mu_mv_mag_sq = mv_size_sorted.begin()->second;
	support->average /= (SMALL_WIDTH * SMALL_HEIGHT / 64);//>>= 7;
	*AbsMVHSize /= (SMALL_WIDTH * SMALL_HEIGHT / 64);//>>= 7;
	*AbsMVVSize /= (SMALL_WIDTH * SMALL_HEIGHT / 64);//>>= 7;
	*AbsMVSize /= (SMALL_WIDTH * SMALL_HEIGHT / 64);//>>= 7;
    if (videoIn->layer.var == 0)
    {
        if (videoIn->layer.jtvar == 0)
            videoIn->layer.tcor = 100;
        else
            videoIn->layer.tcor = (mfxI16)NMIN(1000 * videoIn->layer.jtvar, 2000);

        if (videoIn->layer.mcjtvar == 0)
            videoIn->layer.mcTcor = 100;
        else
            videoIn->layer.mcTcor = (mfxI16)NMIN(1000 * videoIn->layer.mcjtvar, 2000);
    }
    else
    {
        videoIn->layer.tcor = (mfxI16)(100 * videoIn->layer.jtvar / videoIn->layer.var);
        videoIn->layer.mcTcor = (mfxI16)(100 * videoIn->layer.mcjtvar / videoIn->layer.var);
    }
    *TSC = valb >> 8;
    *AFD = (mfxU16)(acc / (SMALL_WIDTH * SMALL_HEIGHT));//Picture area is 2^13, and 10 have been done before so it needs to shift 3 more.
    *MVdiffVal = *MVdiffVal >> 7;
}

bool ASC::CompareStats(mfxU8 current, mfxU8 reference){
    if(current > 2 || reference > 2 || current == reference) {
        printf("Error: Invalid stats comparison\n");
        exit(-666);
    }
    mfxU8 comparison = 0;
    if (dataIn->interlaceMode == ASCprogressive_frame) {
        comparison += support->logic[current]->AFD == 0;
        comparison += support->logic[current]->RsCsDiff == 0;
        comparison += support->logic[current]->TSCindex == 0;
        comparison += support->logic[current]->negBalance <= 3;
        comparison += support->logic[current]->posBalance <= 20;
        comparison += ((support->logic[current]->diffAFD <= 0) && (support->logic[current]->diffTSC <= 0));
        comparison += (support->logic[current]->diffAFD <= support->logic[current]->diffTSC);

        if (comparison == 7)
            return Same;
    }
    else if ((dataIn->interlaceMode == ASCbottomfieldFirst_frame) || (dataIn->interlaceMode == ASCtopfieldfirst_frame)) {
        comparison += support->logic[current]->AFD == support->logic[current]->TSC;
        comparison += support->logic[current]->AFD <= 9;
        comparison += support->logic[current]->gchDC <= 1;
        comparison += support->logic[current]->RsCsDiff <= 9;
        comparison += ((support->logic[current]->diffAFD <= 1) && (support->logic[current]->diffTSC <= 1));
        comparison += (support->logic[current]->diffAFD <= support->logic[current]->diffTSC);

        if (comparison == 6)
            return Same;
    }
    else {
        printf("Error: Invalid interlace mode for stats comparison\n");
        exit(-666);
    }
    return Not_same;
}

bool ASC::FrameRepeatCheck() {
    mfxU8 reference = ASCprevious_frame_data;
    if(dataIn->interlaceMode > ASCprogressive_frame)
        reference = ASCprevious_previous_frame_data;
    return(CompareStats(ASCcurrent_frame_data, reference));
}

void ASC::DetectShotChangeFrame() {
    support->logic[ASCcurrent_frame_data]->frameNum = videoData[ASCCurrent_Frame]->frame_number;
    support->logic[ASCcurrent_frame_data]->firstFrame = support->firstFrame;
    support->logic[ASCcurrent_frame_data]->avgVal     = videoData[ASCCurrent_Frame]->layer.avgval;
    /*---------RsCs data--------*/
    support->logic[ASCcurrent_frame_data]->Rs  = videoData[ASCCurrent_Frame]->layer.RsVal;
    support->logic[ASCcurrent_frame_data]->Cs  = videoData[ASCCurrent_Frame]->layer.CsVal;
    support->logic[ASCcurrent_frame_data]->SC  = videoData[ASCCurrent_Frame]->layer.RsVal + videoData[ASCCurrent_Frame]->layer.CsVal;
    support->logic[ASCcurrent_frame_data]->res = (mfxU32)_res;
    if (support->firstFrame) {
        support->logic[ASCcurrent_frame_data]->TSC                = 0;
        support->logic[ASCcurrent_frame_data]->AFD                = 0;
        support->logic[ASCcurrent_frame_data]->TSCindex           = 0;
        support->logic[ASCcurrent_frame_data]->SCindex            = 0;
        support->logic[ASCcurrent_frame_data]->Schg               = 0;
        support->logic[ASCcurrent_frame_data]->Gchg               = 0;
        support->logic[ASCcurrent_frame_data]->picType            = 0;
        support->logic[ASCcurrent_frame_data]->lastFrameInShot    = 0;
        support->logic[ASCcurrent_frame_data]->pdist              = 0;
        support->logic[ASCcurrent_frame_data]->MVdiffVal          = 0;
        support->logic[ASCcurrent_frame_data]->RsCsDiff           = 0;
        support->logic[ASCcurrent_frame_data]->last_shot_distance = 0;
        support->logic[ASCcurrent_frame_data]->tcor               = 0;
        support->logic[ASCcurrent_frame_data]->mcTcor             = 0;
        support->logic[ASCcurrent_frame_data]->mu_mv_mag_sq       = 0;
		support->logic[ASCcurrent_frame_data]->gop_size           = 4;//Default GoPSize for AVC encoder
        support->logic[ASCcurrent_frame_data]->firstFrame         = support->firstFrame;
        support->firstFrame = false;
    }
    else {
        /*--------Motion data-------*/
        MotionAnalysis(support, dataIn, videoData[ASCCurrent_Frame], videoData[ASCReference_Frame], &support->logic[ASCcurrent_frame_data]->TSC, &support->logic[ASCcurrent_frame_data]->AFD, &support->logic[ASCcurrent_frame_data]->MVdiffVal, &support->logic[ASCcurrent_frame_data]->AbsMVSize, &support->logic[ASCcurrent_frame_data]->AbsMVHSize, &support->logic[ASCcurrent_frame_data]->AbsMVVSize, (ASCLayers)0);
        support->logic[ASCcurrent_frame_data]->TSCindex = TableLookUp(NumTSC, lmt_tsc2, support->logic[ASCcurrent_frame_data]->TSC);
        support->logic[ASCcurrent_frame_data]->SCindex  = TableLookUp(NumSC,  lmt_sc2,  support->logic[ASCcurrent_frame_data]->SC);
        support->logic[ASCcurrent_frame_data]->pdist    = support->PDistanceTable[(support->logic[ASCcurrent_frame_data]->TSCindex * NumSC) +
                                                          support->logic[ASCcurrent_frame_data]->SCindex];
        support->logic[ASCcurrent_frame_data]->TSC >>= 5;
        support->logic[ASCcurrent_frame_data]->tcor = videoData[ASCCurrent_Frame]->layer.tcor;
        support->logic[ASCcurrent_frame_data]->mcTcor = videoData[ASCCurrent_Frame]->layer.mcTcor;
        support->logic[ASCcurrent_frame_data]->mu_mv_mag_sq = support->mu_mv_mag_sq;
        /*------Shot Detection------*/
        support->logic[ASCcurrent_frame_data]->Schg = ShotDetect(videoData[ASCCurrent_Frame]->layer, videoData[ASCReference_Frame]->layer, *dataIn->layer, support->logic[ASCcurrent_frame_data], support->logic[ASCprevious_frame_data], support->detectFunc, support->control, 1);
        support->logic[ASCprevious_frame_data]->lastFrameInShot = (mfxU8)support->logic[ASCcurrent_frame_data]->Schg;
        support->logic[ASCcurrent_frame_data]->repeatedFrame = FrameRepeatCheck();
    }
	support->logic[ASCcurrent_frame_data]->gop_size = AGOPSelect();

#if ASCTUNEDATA
	FILE
		*dataFile = NULL;
	ASCTSCstat *
		current = support->logic[ASCcurrent_frame_data];

	fopen_s(&dataFile, "stats_shotdetect.txt", "a+");

	fprintf(dataFile, "%s\t%i\t%u\t%u\t%u\t%i\t%u\t%u\t%u\t%u\t%u\t%u\t%i\t%i\t%u\t%u\t%u\t%i\t%i\t%i\t%i\t%u\t%u\t%i\t%i\t%i\t%llu\t%i\t%i\n",
		dataIn->layer->inputFilename,
		current->frameNum,
		current->Rs,
		current->Cs,
		current->SC,
		current->AFD,
		current->TSC,
		current->RsDiff,
		current->CsDiff,
		current->RsCsDiff,
		current->MVdiffVal,
		current->avgVal,
		current->ssDCval,
		current->refDCval,
		current->gchDC,
		current->posBalance,
		current->negBalance,
		current->diffAFD,
		current->diffTSC,
		current->diffRsCsDiff,
		current->diffMVdiffVal,
		current->SCindex,
		current->TSCindex,
		current->tcor,
		current->mcTcor,
		current->mu_mv_mag_sq,
		current->distance,
		current->Schg,
		current->gop_size);
	fclose(dataFile);
#endif

#if (VIDEOSTATSBUF <= 3)
        dataIn->processed_frames++;
#endif
}

bool ASC::RunFrame(mfxU8 *frame, mfxU32 parity) {
    videoData[ASCCurrent_Frame]->frame_number = videoData[ASCReference_Frame]->frame_number + 1;
    (this->*(resizeFunc))(frame, _width, _height, _pitch, (ASCLayers)0, parity);
    if (!GPUProc) {
        RsCsCalc(videoData, *dataIn->layer, support);
#if 0
        FILE
            *dataFile = NULL;
        ASCYUV
            *pFrame = NULL;

        fopen_s(&dataFile, "Yonly_small_128x64.yuv", "ab");
        if (device) {
            pFrame = &support->gainCorrection.Image;
            res = queue->EnqueueCopyGPUToCPU(videoData[ASCReference_Frame]->layer.Image.sData, support->gainCorrection.Image.data, e);
            CM_CHK_RESULT(res);
            e->GetStatus(status);
            while (status != CM_STATUS_FINISHED)
                e->GetStatus(status);
            for (int i = 0; i < 64; i++)
                fwrite(pFrame->Y + (i * 128), 1, pFrame->width, dataFile);
        }
        else {
            pFrame = &videoData[ASCCurrent_Frame]->layer.Image;
            for (int i = 0; i < 64; i++)
                fwrite(pFrame->Y + (i * pFrame->pitch), 1, pFrame->width, dataFile);
        }
        fclose(dataFile);
#endif
        DetectShotChangeFrame();
    }
    GeneralBufferRotation();

    return 1;
    //return(!support->logic[ASCprevious_frame_data]->firstFrame);
}








//
// CPU Dispatcher
//




/**
***********************************************************************
* \Brief Adds LTR friendly frame decision to list
*
* Adds frame number and ltr friendly frame decision pair to list, but
* first checks if the size of the list is same or less to MAXLTRHISTORY,
* if the list is longer, then it removes the top elements of the list
* until it is MAXLTRHISTORY - 1, then it adds the new pair to the bottom
* of the list.
*
* \return none
*/
void ASC::Put_LTR_Hint() {
    mfxI16
        list_size = (mfxI16)ltr_check_history.size();
    for (mfxI16 i = 0; i < list_size - (MAXLTRHISTORY - 1); i++)
        ltr_check_history.pop_front();
    ltr_check_history.push_back(std::make_pair(videoData[ASCCurrent_Frame]->frame_number, support->logic[ASCcurrent_frame_data]->ltr_flag));
}

/**
***********************************************************************
* \Brief Checks LTR friendly decision history per frame and returns if
*        LTR operation should be turn on or off.
*
* Travels the LTR friendly decision list backwards checking for frequency
* and amount of true/false LTR friendly frame decision, based on the
* good and bad limit inputs, if bad limit condition is reached first,
* then it inmediately returns 0 (zero) which means to stop LTR operation
*
* \param goodLTRLimit      [IN] - Amount of true values to determine
*                                 if the sequence should run in LTR mode.
* \param badLTRLimit       [IN] - Amount of consecutive false values to
*                                 stop LTR mode.
*
* \return ASC_LTR_DEC to flag stop(false)/continue(true) or FORCE LTR operation
*/
ASC_LTR_DEC ASC::Continue_LTR_Mode(mfxU16 goodLTRLimit, mfxU16 badLTRLimit) {
    size_t
        goodLTRCounter = 0,
        goodLTRRelativeCount = 0,
        badLTRCounter = 0,
        list_size = ltr_check_history.size();
    /*std::list<std::pair<mfxI32, bool>>::reverse_iterator
    ltr_list_it     = ltr_check_history.rbegin(),
    ltr_list_it_end = ltr_check_history.rend();*/
    std::list<std::pair<mfxI32, bool> >::iterator
        ltr_list_it = std::prev(ltr_check_history.end());
    goodLTRLimit = goodLTRLimit > MAXLTRHISTORY ? MAXLTRHISTORY : goodLTRLimit;
    //When a scene change happens, all history is discarded
    if (Get_frame_shot_Decision()) {
        ltr_check_history.resize(0);
        list_size = 0;
    }
    //If not enough history then let it be LTR
    if (list_size < badLTRLimit)
        return YES_LTR;
    //Travel trhough the list to determine if LTR operation should be kept on
    mfxU16
        bkp_size = (mfxU16)list_size;
    while ((bkp_size > 1) && (goodLTRCounter < goodLTRLimit)) {
        auto scd = ltr_list_it->second;
        if (!scd) {
            badLTRCounter++;
            goodLTRRelativeCount = 0;
        }
        if (badLTRCounter >= badLTRLimit)
            return NO_LTR;
        goodLTRCounter += (mfxU16)ltr_list_it->second;
        goodLTRRelativeCount += (mfxU16)ltr_list_it->second;
        if (goodLTRRelativeCount >= badLTRLimit)
            badLTRCounter = 0;
        ltr_list_it = std::prev(ltr_list_it);
        bkp_size--;
    }
    if (goodLTRCounter >= goodLTRLimit)
        return FORCE_LTR;
    else if (goodLTRRelativeCount >= (size_t)NMIN(badLTRLimit, list_size - 1) && badLTRCounter < goodLTRRelativeCount)
        return YES_LTR;
    else
        return NO_LTR;
}

bool Hint_LTR_op_on(mfxU32 RsCsDiff, mfxU32 MVDiff, mfxU32 AFD) {
    bool
        conditions[2];
    conditions[0] = RsCsDiff < ((-133.9 * log(AFD)) + 406.7);
    conditions[1] = (MVDiff < ((-25.97 * log(RsCsDiff)) + 185.71));
    return (conditions[0] && conditions[1]);
}

ASC_API void ASC::PutFrameProgressive(mfxU8 *frame) {
    dataReady = RunFrame(frame, ASCTopField);
}

ASC_API void ASC::PutFrameInterlaced(mfxU8 *frame) {
    dataReady = RunFrame(frame, dataIn->currentField);
    SetNextField();
}

ASC_API bool ASC::PutFrame(mfxU8 *frame) {
    (this->*(pPutFrameFunc))(frame);
    return(dataReady);
}

ASC_API bool ASC::PutFrame_t(mfxU8 *frame) {
    TimeStart(&dataIn->timer, 2);
    (this->*(pPutFrameFunc))(frame);
    //dataIn->timer.calctime += CatchTime(&dataIn->timer, 2, 3, "Shot change calc:", PRINTDATA);
    return(dataReady);
}

ASC_API bool ASC::Get_Last_frame_Data() {
    if(dataReady)
        GeneralBufferRotation();
    else
        printf("Warning: Trying to grab data not ready\n");
    return(dataReady);
}

ASC_API mfxU32 ASC::Get_starting_frame_number() {
    return dataIn->starting_frame;
}

ASC_API mfxU32 ASC::Get_frame_number() {
    if(dataReady)
        return support->logic[ASCcurrent_frame_data]->frameNum;
    else
        return NULL;
}

ASC_API mfxU32 ASC::Get_frame_variation_status() {
#if NEWFEATURE
    if (dataReady)
        return support->logic[ASCSceneVariation_frame_data]->Schg;
    else
#endif
        return NULL;
}

ASC_API mfxU32 ASC::Get_frame_shot_Decision() {
    if(dataReady)
        return support->logic[ASCcurrent_frame_data]->Schg;
    else
        return NULL;
}

ASC_API mfxU32 ASC::Get_frame_last_in_scene() {
    if(dataReady)
        return support->logic[ASCcurrent_frame_data]->lastFrameInShot;
    else
        return NULL;
}

ASC_API bool ASC::Get_GoPcorrected_frame_shot_Decision() {
    if(dataReady)
        return (support->detectedSch > 0);
    else
        return NULL;
}
ASC_API mfxI32 ASC::Get_frame_Spatial_complexity() {
    if(dataReady)
        return support->logic[ASCcurrent_frame_data]->SCindex;
    else
        return NULL;
}

ASC_API mfxI32 ASC::Get_frame_Temporal_complexity() {
    if(dataReady)
        return support->logic[ASCcurrent_frame_data]->TSCindex;
    else
        return NULL;
}

ASC_API bool ASC::Get_LTR_advice() {
    if (dataReady)
#if NODELAY
        return support->logic[ASCprevious_frame_data]->ltr_flag;
#else
        return support->logic[ASCcurrent_frame_data]->ltr_flag;
#endif
    else
        return NULL;
}
/**
***********************************************************************
* \Brief Tells if LTR mode should be on/off or forced on.
*
* \return  ASC_LTR_DEC to flag stop(false)/continue(true) or force (2)
*          LTR operation
*/
ASC_API ASC_LTR_DEC ASC::get_LTR_op_hint() {
    return Continue_LTR_Mode(50, 5);
}

/**
***********************************************************************
* \Brief Returns filter selection based on frames characteristics.
*
* \return  Ffmpeg filter value for subsampling
*
*/
ASC_API mfxU16 ASC::get_subsample_filter_selection() {
    if (dataReady)
#if NODELAY
        return support->logic[ASCprevious_frame_data]->subFilterType;
#else
        return support->logic[ASCcurrent_frame_data]->subFilterType;
#endif
    else
        return NULL;
}

ASC_API void ASC::printMetadataHeader(FILE *SADOut) {
    fprintf(SADOut, "frameNum\tRs\tCs\tSC\tAFD\tTSC\tTSCindex\tSCindex\tTcor\tMCTcor\tRsDiff\tCsDiff\tRsCsDiff\t");
    fprintf(SADOut, "MvDiff\tAFDdiff\tTSCdiff\tMVSecDiff\tRsCsSecDiff\tDCvalue\tPosBalance\tNegBalance\tPDist\tSCh\tGCh\tpicType\tGop_Size\n");
}

ASC_API void ASC::printStats(ASCTSCstat data, char *filename, FILE *SADOut) {
    int n = 0;
    if(filename)
        n = (int)strlen(filename);
    if (n > 4)
        n -= 4;
    if (SADOut != NULL) {
        //fprintf(SADOut, "%.*s", n, filename);
        fprintf(SADOut, "\t%i", data.frameNum);
        fprintf(SADOut, "\t%u", data.Rs);
        fprintf(SADOut, "\t%u", data.Cs);
        fprintf(SADOut, "\t%u", data.SC);
        fprintf(SADOut, "\t%u", data.AFD);
        fprintf(SADOut, "\t%u", data.TSC);
        fprintf(SADOut, "\t%u", data.TSCindex);
        fprintf(SADOut, "\t%u", data.SCindex);
        fprintf(SADOut, "\t%i", data.tcor);
        fprintf(SADOut, "\t%i", data.mcTcor);
        fprintf(SADOut, "\t%i", data.RsDiff);
        fprintf(SADOut, "\t%i", data.CsDiff);
        fprintf(SADOut, "\t%i", data.RsCsDiff);
        fprintf(SADOut, "\t%i", data.MVdiffVal);
        fprintf(SADOut, "\t%i", data.diffAFD);
        fprintf(SADOut, "\t%i", data.diffTSC);
        fprintf(SADOut, "\t%i", data.diffMVdiffVal);
        fprintf(SADOut, "\t%i", data.diffRsCsDiff);
        fprintf(SADOut, "\t%u", data.ssDCval);
        fprintf(SADOut, "\t%u", data.posBalance);
        fprintf(SADOut, "\t%u", data.negBalance);
        fprintf(SADOut, "\t%i", data.pdist);
        fprintf(SADOut, "\t%i", data.Schg);
        fprintf(SADOut, "\t%i", data.Gchg);
        fprintf(SADOut, "\t%u", data.picType);
        fprintf(SADOut, "\t%u\n", data.gop_size);
        fflush(SADOut);

    }
}

ASC_API void ASC::printStats(mfxI32 index, char *filename, FILE *SADOut) {
    printStats(*support->logic[index], filename, SADOut);
}

ASC_API void ASC::setInputFileName(char *filename) {
    dataIn->inputFile = filename;
    dataIn->layer->inputFilename = const_cast<char*>(dataIn->inputFile);
}

void bufferRotation(void *Buffer1, void *Buffer2) {
    void
        *transfer;
    transfer = Buffer2;
    Buffer2  = Buffer1;
    Buffer1  = transfer;
}

void ASC::GeneralBufferRotation() {
    ASCVidSample
        *videoTransfer;
    ASCTSCstat
        *metaTransfer;

#if (VIDEOSTATSBUF > 2)
    if ((support->logic[ASCSceneVariation_frame_data]->copyFrameDelay == 1 && support->logic[ASCprevious_frame_data]->Schg == 1) || support->logic[ASCSceneVariation_frame_data]->Schg == 1){
        videoTransfer = videoData[0];
        videoData[0] = videoData[VIDEOSTATSBUF - 1];
        videoData[VIDEOSTATSBUF - 1] = videoTransfer;
        memcpy(support->logic[ASCSceneVariation_frame_data], support->logic[ASCprevious_frame_data], sizeof(ASCTSCstat));
        support->logic[ASCSceneVariation_frame_data]->Schg = 1;
    }
#endif
    if (support->logic[ASCcurrent_frame_data]->repeatedFrame) {
        videoData[ASCReference_Frame]->frame_number				= videoData[ASCCurrent_Frame]->frame_number;
        support->logic[ASCprevious_frame_data]->frameNum		= videoData[ASCCurrent_Frame]->frame_number;
        support->logic[ASCcurrent_frame_data]->Schg				= 0;
        support->logic[ASCprevious_frame_data]->Schg			= 0;
        support->logic[ASCprevious_previous_frame_data]->Schg	= 0;
		support->logic[ASCprevious_frame_data]->CsDiff			= support->logic[ASCcurrent_frame_data]->CsDiff;
		support->logic[ASCprevious_frame_data]->diffAFD			= support->logic[ASCcurrent_frame_data]->diffAFD;
		support->logic[ASCprevious_frame_data]->AFD				= support->logic[ASCcurrent_frame_data]->AFD;
		support->logic[ASCprevious_frame_data]->diffMVdiffVal	= support->logic[ASCcurrent_frame_data]->diffMVdiffVal;
		support->logic[ASCprevious_frame_data]->diffRsCsDiff	= support->logic[ASCcurrent_frame_data]->diffRsCsDiff;
		support->logic[ASCprevious_frame_data]->diffTSC			= support->logic[ASCcurrent_frame_data]->diffTSC;
		support->logic[ASCprevious_frame_data]->distance		= support->logic[ASCcurrent_frame_data]->distance;
		support->logic[ASCprevious_frame_data]->gchDC			= support->logic[ASCcurrent_frame_data]->gchDC;
		support->logic[ASCprevious_frame_data]->gop_size		= support->logic[ASCcurrent_frame_data]->gop_size;
		support->logic[ASCprevious_frame_data]->histogram[0]	= support->logic[ASCcurrent_frame_data]->histogram[0];
		support->logic[ASCprevious_frame_data]->histogram[2]	= support->logic[ASCcurrent_frame_data]->histogram[1];
		support->logic[ASCprevious_frame_data]->histogram[3]	= support->logic[ASCcurrent_frame_data]->histogram[2];
		support->logic[ASCprevious_frame_data]->histogram[4]	= support->logic[ASCcurrent_frame_data]->histogram[3];
		support->logic[ASCprevious_frame_data]->histogram[5]	= support->logic[ASCcurrent_frame_data]->histogram[4];
		support->logic[ASCprevious_frame_data]->mcTcor			= support->logic[ASCcurrent_frame_data]->mcTcor;
		support->logic[ASCprevious_frame_data]->mu_mv_mag_sq	= support->logic[ASCcurrent_frame_data]->mu_mv_mag_sq;
		support->logic[ASCprevious_frame_data]->MVdiffVal		= support->logic[ASCcurrent_frame_data]->MVdiffVal;
		support->logic[ASCprevious_frame_data]->negBalance		= support->logic[ASCcurrent_frame_data]->negBalance;
		support->logic[ASCprevious_frame_data]->posBalance		= support->logic[ASCcurrent_frame_data]->posBalance;
		support->logic[ASCprevious_frame_data]->refDCint		= support->logic[ASCcurrent_frame_data]->refDCint;
		support->logic[ASCprevious_frame_data]->refDCval		= support->logic[ASCcurrent_frame_data]->refDCval;
		support->logic[ASCprevious_frame_data]->Rs				= support->logic[ASCcurrent_frame_data]->Rs;
		support->logic[ASCprevious_frame_data]->RsCsDiff		= support->logic[ASCcurrent_frame_data]->RsCsDiff;
		support->logic[ASCprevious_frame_data]->RsDiff			= support->logic[ASCcurrent_frame_data]->RsDiff;
		support->logic[ASCprevious_frame_data]->SC				= support->logic[ASCcurrent_frame_data]->SC;
		support->logic[ASCprevious_frame_data]->SCindex			= support->logic[ASCcurrent_frame_data]->SCindex;
		support->logic[ASCprevious_frame_data]->scVal			= support->logic[ASCcurrent_frame_data]->scVal;
		support->logic[ASCprevious_frame_data]->ssDCint			= support->logic[ASCcurrent_frame_data]->ssDCint;
		support->logic[ASCprevious_frame_data]->ssDCval			= support->logic[ASCcurrent_frame_data]->ssDCval;
		support->logic[ASCprevious_frame_data]->tcor			= support->logic[ASCcurrent_frame_data]->tcor;
		support->logic[ASCprevious_frame_data]->TSC				= support->logic[ASCcurrent_frame_data]->TSC;
		support->logic[ASCprevious_frame_data]->TSCindex		= support->logic[ASCcurrent_frame_data]->TSCindex;
		support->logic[ASCprevious_frame_data]->tscVal			= support->logic[ASCcurrent_frame_data]->tscVal;
    }
    else {
        videoTransfer = videoData[0];
        videoData[0] = videoData[1];
        videoData[1] = videoTransfer;

        metaTransfer = support->logic[ASCprevious_previous_frame_data];
        support->logic[ASCprevious_previous_frame_data] = support->logic[ASCprevious_frame_data];
        support->logic[ASCprevious_frame_data] = support->logic[ASCcurrent_frame_data];
        support->logic[ASCcurrent_frame_data] = metaTransfer;
    }
}