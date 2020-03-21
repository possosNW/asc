/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#include "IOfunctions.h"

void TimeStart(ASCTime* timer) {
#if defined (_WIN32) || (_WIN64)
    QueryPerformanceFrequency(&timer->tFrequency);
    QueryPerformanceCounter(&timer->tStart);
#else
    (void)timer;
#endif
}

void TimeStart(ASCTime* timer, int index) {
#if defined (_WIN32) || (_WIN64)
    QueryPerformanceCounter(&timer->tPause[index]);
#else
    (void)timer;
    (void)index;
#endif
}

void TimeStop(ASCTime* timer) {
#if defined (_WIN32) || (_WIN64)
    QueryPerformanceCounter(&timer->tStop);
#else
    (void)timer;
#endif
}

mfxF64 CatchTime(ASCTime *timer, const char* message)
{
    (void)message;

    mfxF64
        timeval = 0.0;
    timeval = TimeMeasurement(timer->tStart, timer->tStop, timer->tFrequency);
    ASC_PRINTF("%s %0.3f ms.\n", message, timeval);
    return timeval;
}

mfxF64 CatchTime(ASCTime *timer, int index, const char* message) {
    (void)message;
#if defined (_WIN32) || (_WIN64)
    mfxF64
        timeval = 0.0;
    QueryPerformanceCounter(&timer->tPause[index]);
    timeval = TimeMeasurement(timer->tStart, timer->tPause[index], timer->tFrequency);
    ASC_PRINTF("%s %0.3f ms.\n", message, timeval);
    return timeval;
#else
    (void)timer;
    (void)index;

    return 0.0;
#endif
}

mfxF64 CatchTime(ASCTime *timer, int indexInit, int indexEnd, const char* message) {
    (void)message;
#if defined (_WIN32) || (_WIN64)
    mfxF64
        timeval = 0.0;
    QueryPerformanceCounter(&timer->tPause[indexEnd]);
    timeval = TimeMeasurement(timer->tPause[indexInit], timer->tPause[indexEnd], timer->tFrequency);
    ASC_PRINTF("%s %0.3f ms.\n", message, timeval);
    return timeval;
#else
    (void)timer;
    (void)indexInit;
    (void)indexEnd;

    return 0.0;
#endif
}
void imageInit(ASCYUV *buffer) {
    memset(buffer, 0, sizeof(ASCYUV));
}

void nullifier(ASCimageData *Buffer) {
    memset(Buffer, 0, sizeof(ASCimageData));
}

void ImDetails_Init(ASCImDetails *Rdata) {
    memset(Rdata, 0, sizeof(ASCImDetails));
}

void ASCTSCstat_Init(ASCTSCstat **logic) {
	for(int i = 0; i < TSCSTATBUFFER; i++)
	{
		logic[i] = new ASCTSCstat;
        memset(logic[i],0,sizeof(ASCTSCstat));
	}
}

mfxI32 ASClogBase2aligned(mfxI32 number) {
    mfxI32 data = (mfxI32)(ceil((log((double)number) / LN2)));
    return (mfxI32)pow(2,(double)data);
}

void ASCU8AllocandSet(pmfxU8 *ImageLayer, mfxI32 imageSize) {
#if defined (_WIN32) || (_WIN64)
    *ImageLayer = (pmfxU8)_aligned_malloc(imageSize, 0x1000);
#else
    *ImageLayer = (pmfxU8)aligned_alloc(0x1000, imageSize);
#endif
    if(*ImageLayer == NULL)
        exit(MEMALLOCERRORU8);
    else
        memset(*ImageLayer,255,imageSize);
}

void PdYuvImage_Alloc(ASCYUV *pImage, mfxI32 dimVideoExtended) {
    mfxI32
		dimAligned;
    dimAligned = ASClogBase2aligned(dimVideoExtended);
    ASCU8AllocandSet(&pImage->data,dimAligned);
}

void PdYuvImage_GPUAlloc(ASCYUV *pImage, mfxI32 dimVideoExtended) {
    mfxI32
        dimAligned;
    dimAligned = ASClogBase2aligned(dimVideoExtended);
    ASCU8AllocandSet(&pImage->data, dimAligned);
}

void PdMVector_Alloc(ASCMv **MV, mfxI32 mvArraysize){
    mfxI32
		MVspace = sizeof(ASCMv) * mvArraysize;
    *MV = new ASCMv[MVspace];//(MVector*) malloc(MVspace);
    if(*MV == NULL)
        exit(MEMALLOCERRORMV);
    else
        memset(*MV,0,MVspace);
}

void PdRsCs_Alloc(pmfxU16 *RCs, mfxI32 mvArraysize) {
    mfxI32
		MVspace = sizeof(mfxU16) * mvArraysize;
#if defined (_WIN32) || (_WIN64)
    *RCs = (pmfxU16)_aligned_malloc(MVspace, 0x1000);
#else
    *RCs = (pmfxU16)aligned_alloc(0x1000, MVspace);
#endif
    
    if(*RCs == NULL)
        exit(MEMALLOCERROR);
    else
        memset(*RCs,0,MVspace);
}

void PdSAD_Alloc(pmfxU16 *SAD, mfxI32 mvArraysize) {
    mfxI32
		MVspace = sizeof(mfxU16) * mvArraysize;
#if defined (_WIN32) || (_WIN64)
    *SAD = (pmfxU16)_aligned_malloc(MVspace, 0x1000);
#else
    *SAD = (pmfxU16)aligned_alloc(0x1000, MVspace);
#endif
    if(*SAD == NULL)
        exit(MEMALLOCERROR);
    else
        memset(*SAD,0,MVspace);
}

mfxF64 ASCTimeMeasurement(LARGE_INTEGER start,LARGE_INTEGER stop,LARGE_INTEGER frequency) {
    return((stop.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart);
}


mfxU32 ASCvideoFileOpen(FILE **videoFile, char* videoFilename, mfxU32 Width, mfxU32 Height) {
#if defined (_WIN32) || (_WIN64)
    WIN32_FILE_ATTRIBUTE_DATA
		fileInfo;
    LARGE_INTEGER
		fileSize;
    size_t lenght = strlen(videoFilename) + 1;
    wchar_t *fileName = (wchar_t*)malloc(sizeof(wchar_t) * lenght);
    memset(fileName,0,lenght);
    mbstowcs_s(&lenght,fileName,lenght,videoFilename,lenght - 1);
    GetFileAttributesEx(fileName,GetFileExInfoStandard,&fileInfo);
    free(fileName);
    fileSize.HighPart = fileInfo.nFileSizeHigh;
    fileSize.LowPart = fileInfo.nFileSizeLow;
    fopen_s(videoFile,videoFilename,"rb");
	return((mfxU32)(fileSize.QuadPart/(Width * Height * 3 / 2)));
#else
    /*(videoFile);
    (videoFilename);
    (Width);
    (Height);*/
    return 0;
#endif
}

void ReadFrame(FILE *videoFile,ASCVidData dataIn, mfxU8 *dst) {
#if defined (_WIN32) || (_WIN64)
	fread(dst,1,dataIn.layer[ASCFull_Size].Original_Width * dataIn.layer[ASCFull_Size].Original_Height,videoFile);
    _fseeki64(videoFile,(mfxI64)(dataIn.layer[ASCFull_Size].Original_Height * dataIn.layer[ASCFull_Size].Original_Width / 2),SEEK_CUR);
#else
    /*(videoFile);
    (dataIn);
    (dst);*/
#endif
}
