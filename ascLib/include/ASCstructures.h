/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#ifndef _ASCSTRUCTURES_H_
#define _ASCSTRUCTURES_H_

#pragma once
#pragma warning( disable: 4201 )
#pragma warning( disable: 4463 )

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <list>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <smmintrin.h>
#include <immintrin.h>
#include <assert.h>
#include <vector>

#if defined(ASC_DEBUG)
    #define ASC_PRINTF(...)     printf(__VA_ARGS__)
    #define ASC_FPRINTF(...)    fprintf(__VA_ARGS__)
    #define ASC_FFLUSH(x)       fflush(x)
#else
    #define ASC_PRINTF(...)
    #define ASC_FPRINTF(...)
    #define ASC_FFLUSH(x)
#endif

#if defined(_WIN32) || defined(_WIN64)
#define ASC_API      __declspec(dllexport)
#define ASC_API_FUNC __declspec(dllexport)
#define STDCALL      __stdcall
#define ALIGN_DECL(X) __declspec(align(X))
#define __CDECL __cdecl
#else
#define ALIGN_DECL(X) __attribute__ ((aligned(X)))
#define __CDECL
#define __cdecl
#include <limits.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#define ASC_API_FUNC //__attribute__((visibility("default")))
#define ASC_API      //__attribute__((visibility("default")))
#define STDCALL      //__attribute__((stdcall))
#endif

#define OUT_BLOCK   16    // output pixels computed per thread
#define RSCS_BLOCK  4

#define ASCTUNEDATA       1
#define PRINTDATA         0
#define ENABLE_RF         1
#define BOXFILTER         0

#undef  NULL
#define NULL              0
#define MVBLK_SIZE        8
#define BLOCK_SIZE        4
#define BLOCK_SIZE_SHIFT  2
#define LN2               0.6931471805599453
#define FRAMEMUL          16
#define FLOAT_MAX         2241178.0
#define MEMALLOCERROR     1000
#define MEMALLOCERRORU8   1001
#define MEMALLOCERRORMV   1002
#define MAXLTRHISTORY     120

#define SMALL_WIDTH       128
#define SMALL_HEIGHT      64
#define SMALL_AREA        SMALL_WIDTH * SMALL_HEIGHT//13 bits
#define S_AREA_SHIFT      13
#define EXTENSIONSIZE     16

#define NumTSC            10
#define NumSC             10

#define TSC_INT_SCALE     5
#define GAINDIFF_THR      20
#define DETECTIONFORESTS  3

/*--MACROS--*/
#define NMAX(a,b)         ((a>b)?a:b)
#define NMIN(a,b)         ((a<b)?a:b)
#define NABS(a)           (((a)<0)?(-(a)):(a))
#define NAVG(a,b)         ((a+b)/2)

#define CHECK_CM_ERR(ERR) if ((ERR) != CM_SUCCESS) { printf("FAILED at file: %s, line: %d, cmerr: %d\n", __FILE__, __LINE__, ERR); return FAILED; }
#define CM_CHK_RESULT(cm_call)                                  \
do {                                                            \
    int result = cm_call;                                       \
    if (result != CM_SUCCESS) {                                 \
        fprintf(stderr, "Invalid CM call at %s:%d. Error %d: ", \
        __FILE__, __LINE__, result);                            \
        fprintf(stderr, ".\n");                                 \
        exit(EXIT_FAILURE);                                     \
    }                                                           \
} while(false)
#define CHECK_ERR(ERR) if ((ERR) != PASSED) { printf("FAILED at file: %s, line: %d\n", __FILE__, __LINE__); return (ERR); }

#define Clamp(x)           ((x<0)?0:((x>255)?255:x))
#define RF_DECISION_LEVEL 11
#define NEWFEATURE        0
#if NEWFEATURE
#define TSCSTATBUFFER     4
#define VIDEOSTATSBUF     2
#else
#define TSCSTATBUFFER     3
#define VIDEOSTATSBUF     2
#endif
#define SIMILITUDVAL      4

typedef unsigned char      mfxU8;
typedef mfxU8*             pmfxU8;
typedef char               mfxI8;
typedef mfxI8*             pmfxI8;
typedef short              mfxI16;
typedef mfxI16*            pmfxI16;
typedef unsigned short     mfxU16;
typedef mfxU16*            pmfxU16;
typedef unsigned int       mfxU32;
typedef mfxU32*            pmfxU32;
typedef int                mfxI32;
typedef mfxI32*            pmfxI32;
#if defined( _WIN32 ) || defined ( _WIN64 )
typedef unsigned long      mfxUL32;
typedef long               mfxL32;
#else
typedef unsigned int       mfxUL32;
typedef int                mfxL32;
#endif
typedef mfxUL32*           pmfxUL32;
typedef mfxL32*            pmfxL32;
typedef float              mfxF32;
typedef mfxF32*            pmfxF32;
typedef double             mfxF64;
typedef mfxF64*            pmfxF64;
typedef uint64_t           mfxU64;
typedef mfxU64*            pmfxU64;
typedef int64_t            mfxI64;
typedef mfxI64*            pmfxI64;

typedef struct {
    mfxU16
        x,
        y;
} mfxU16Pair;
#if !defined(_WIN32) && !defined(_WIN64)
typedef union _LARGE_INTEGER {
    struct {
        uint32_t LowPart;
        int32_t HighPart;
    } u;
    int64_t QuadPart;
} LARGE_INTEGER;
#endif
typedef enum ASC_LTR_DESICION {
    NO_LTR = false,
    YES_LTR = true,
    FORCE_LTR
} ASC_LTR_DEC;

typedef enum ASCSimilar {
    Not_same,
    Same} ASCSimil;
typedef enum ASCLayers {
    ASCFull_Size,
    ASCSmall_Size} ASClayer;
typedef enum ASCResizing_Target {
    ASCSmall_Target} ASCRT;
typedef enum ASCData_Flow_Direction {
    ASCReference_Frame,
    ASCCurrent_Frame,
    ASCScene_Diff_Frame}ASCDFD;
typedef enum ASCGoP_Sizes {
    Forbidden_GoP,
    Immediate_GoP,
    QuarterHEVC_GoP,
    Three_GoP,
    HalfHEVC_GoP,
    Five_GoP,
    Six_GoP,
    Seven_Gop,
    HEVC_Gop,
    Nine_Gop,
    Ten_Gop,
    Eleven_Gop,
    Twelve_Gop,
    Thirteen_Gop,
    Fourteen_Gop,
    Fifteen_Gop,
    Double_HEVC_Gop} ASCGOP;
typedef enum ASCBufferPosition {
    ASCcurrent_frame_data,
    ASCprevious_frame_data,
    ASCprevious_previous_frame_data,
    ASCSceneVariation_frame_data} ASCBP;
typedef enum ASCGPU_USAGE {
    ASCNo_GPU_Proc,
    ASCDo_GPU_Proc}ASCGU;
typedef enum ASCFrameTypeScan {
    ASCunknown_frame_scan,
    ASCprogressive_frame,
    ASCtopfieldfirst_frame,
    ASCbottomfieldFirst_frame}ASCFTS;
typedef enum ASCFrameFields {
    ASCTopField,
    ASCBottomField}ASCFF;
typedef struct ASCtimming_measurement_var {
    LARGE_INTEGER
        tFrequency,
        tStart,
        tPause[10],
        tStop;
    mfxF64
        timeval,
        calctime,
        calctime2;
}ASCTime;

typedef struct ASCcoordinates {
    mfxI16 
        x;
    mfxI16
        y;
}ASCMv;

typedef struct ASCBaseline {
    mfxI32
        start;
    mfxI32
        end;
}ASCLine;

typedef struct ASCYUV_layer_store {
    mfxU8 
        *data,
        *Y,
        *U,
        *V;
    mfxU32
        width,
        height,
        pitch,
        hBorder,
        wBorder,
        surfaceSize,
        extWidth,
        extHeight;
}ASCYUV;

typedef struct ASCexport_data {
    char*                    filename;
    FILE*                    pDataOut;
}ASCDataWriter;

typedef struct ASCimport_data {
    char*                    filename;
    FILE*                    pDataIn;
}ASCDataReader;

typedef struct ASCSAD_range {
    mfxU16 
        SAD,
        distance;
    ASCMv
        BestMV;
}ASCRsad;

typedef struct ASCImage_details {
    mfxI32
        Original_Width,             //User input
        Original_Height,            //User input
        horizontal_pad,             //User input for original resolution in multiples of FRAMEMUL, derived for the other two layers
        vertical_pad,               //User input for original resolution in multiples of FRAMEMUL, derived for the other two layers
        _cwidth,                    //corrected size if width not multiple of FRAMEMUL
        _cheight,                   //corrected size if height not multiple of FRAMEMUL
        pitch,                      //horizontal_pad + _cwidth + horizontal_pad
        Extended_Width,             //horizontal_pad + _cwidth + horizontal_pad
        Extended_Height,            //vertical_pad + _cheight + vertical_pad
        block_width,                //User input
        block_height,               //User input
        Width_in_blocks,            //_cwidth / block_width
        Height_in_blocks,           //_cheight / block_height
        initial_point,              //(Extended_Width * vertical_pad) + horizontal_pad
        sidesize,                   //_cheight + (1 * vertical_pad)
        endPoint,                   //(sidesize * Extended_Width) - horizontal_pad
        MVspaceSize;                //Pixels_in_Y_layer / block_width / block_height
    char
        *inputFilename;
}ASCImDetails;

typedef struct ASCVideo_characteristics {
    ASCImDetails
        *layer;
    const char
        *inputFile,
        *PDistFile;
    mfxI32
        starting_frame,              //Frame number where the video is going to be accessed
        total_number_of_frames,      //Total number of frames in video file
        processed_frames,            //Number of frames that are going to be processed, taking into account the starting frame
        accuracy,
        key_frame_frequency,
        limitRange,
        maxXrange,
        maxYrange,
        interlaceMode,
        StartingField,
        currentField;
    ASCTime
        timer;
}ASCVidData;

class ASCimageData {
public:
    ASCYUV Image;
    ASCMv
        *pInteger;
    mfxI32
        *pAfdV;
    mfxU32
        CsVal,
        RsVal,
        *pAfd;
    mfxI64
        jtvar,
        mcjtvar,
        var;
    mfxI16
        tcor,
        mcTcor;
    mfxU32
        mu_mv_mag_sq;
    mfxU16
        avgval;
    mfxU16
        *pAvgV;
    mfxU16
        *Cs,
        *Rs,
        *RsCs,
        *SAD,
        *texAcc;
    mfxU16
        texWblocks,
        texHblocks;

    ASC_API void   InitFrame(ASCImDetails *pDetails);
    ASC_API mfxI32 Close();
};

typedef bool(*t_SCDetect)(mfxI32 diffMVdiffVal, mfxU32 RsCsDiff,   mfxU32 MVDiff,   mfxU32 Rs,       mfxU32 AFD,
                          mfxU32 CsDiff,        mfxI32 diffTSC,    mfxU32 TSC,      mfxU32 gchDC,    mfxI32 diffRsCsdiff,
                          mfxU32 posBalance,    mfxU32 SC,         mfxU32 TSCindex, mfxU32 Scindex,  mfxU32 Cs,
                          mfxI32 diffAFD,       mfxU32 negBalance, mfxU32 ssDCval,  mfxU32 refDCval, mfxU32 RsDiff);

typedef struct ASCstats_structure {
    mfxI8
        pdist;
    mfxI32
        frameNum,
        scVal,
        tscVal,
        Schg,
        last_shot_distance;
    mfxU32
        SCindex,
        TSCindex,
        histogram[5],
        Rs,
        Cs,
        SC,
        TSC,
        RsDiff,
        CsDiff,
        RsCsDiff,
        MVdiffVal,
        AbsMVSize,
        AbsMVHSize,
        AbsMVVSize;
    mfxI16
        tcor,
        mcTcor;
    mfxU32
        mu_mv_mag_sq;
    mfxU16
        subFilterType,
        AFD,
		gop_size;
    mfxI32
        ssDCval,
        refDCval,
        diffAFD,
        diffTSC,
        diffRsCsDiff,
        diffMVdiffVal;
    mfxU32
        gchDC,
        posBalance,
        negBalance,
        avgVal,
        res;
    mfxI64
        ssDCint,
        refDCint;
    mfxI32
        Gchg;
    mfxU8
        picType,
        repeatedFrame,
        lastFrameInShot;
    bool
        firstFrame,
        copyFrameDelay,
        fadeIn,
        ltr_flag;
	mfxU64
		distance;
}ASCTSCstat;

typedef struct ASCvideoBuffer {
    ASCimageData
        layer;
    mfxI32
        frame_number,
        forward_reference,
        backward_reference;
} ASCVidSample;

struct VmeSearchPath // sizeof=58
{
    mfxU8 sp[56];
    mfxU8 maxNumSu;
    mfxU8 lenSp;
};

struct MeControl // sizeof=96
{
    VmeSearchPath searchPath;
    mfxU8  reserved[2];
    mfxU16 width;
    mfxU16 height;
    mfxU16 th;
    mfxU16 mre_width;
    mfxU16 mre_height;
    mfxU16 sTh;
    mfxU16 subPrecision;
    mfxU8  reserved2[22];
};

typedef struct ASCextended_storage {
    mfxI32
        average;
    mfxI32
        avgSAD;
    mfxU32
        gopSize,
        lastSCdetectionDistance,
        detectedSch,
        pendingSch;
    ASCTSCstat
        **logic;
    // For Pdistance table selection
    pmfxI8
        PDistanceTable;
    t_SCDetect
        detectFunc;
    ASCLayers
        size;
    mfxU32
        mu_mv_mag_sq;
    bool
        firstFrame;
    ASCimageData
        gainCorrection;
    mfxU8
        control;
}ASCVidRead;

// APP_LT_REF////////////////////////////////////
typedef struct {
    mfxI32  frameOrder;
    mfxI32  Rs;
    mfxI32  Cs;
    mfxI32  AFD;
    mfxI32  TSC;
    mfxI32  MVdiffVal;
    mfxI32  Schg;
}AscLtrData;
/////////////////////////////////////////////////

class ASC {
private:
    mfxU32
        threadsWidth,
        threadsHeight,
        threadsRsCsWidth,
        threadsRsCsHeight,
        threadsGCorWidth,
        threadsGCorHeight,
        threadsAvgLumaWidth,
        threadsAvgLumaHeight,
        threadsTexDiffWidth,
        threadsTexDiffHeight,
        threadsMeWidth,
        threadsMeHeight,
        threadsAFDWidth,
        threadsAFDHeight,
        threadsAccWidth,
        threadsAccHeight,
        threadsHistWidth,
        threadsHistHeight,
        *pDetection;
    mfxI32
        *pRsDiff,
        *pCsDiff,
        *pSADacc,
        *pMVacc;
    mfxU16
        *pHistVal;
    void
        *pData;

    unsigned int
        gpuwidth,
        gpuheight,
        gpupitch,
        gpuRsCsPitch,
        gpuLumaValPitch,
        gpuTexDiffPitch;
    static const int 
        subWidth    = 128,
        subHeight   = 64,
        subGpuPitch = 160,
        subPitch    = 132;
    int
        gpustep_w,
        gpustep_h,
        gpuIntStep_h;
    size_t
        hwSize;
    mfxU32
        hwType,
        version;
    mfxI32
        res;

private:
    ASCVidRead
        *support;
    ASCVidData
        *dataIn;
    ASCVidSample
        **videoData;
    bool
        dataReady,
        GPUProc;
    mfxI32
        _width,
        _height,
        _pitch,
        _res;

    AscLtrData
        ltrData;  // APP_LT_REF
    /**
    ****************************************************************
    * \Brief List of long term reference friendly frame detection
    *
    * The list keeps the history of ltr friendly frame detections,
    * each element in the listis made of a frame number <mfxI32> and
    * its detection <bool> as a pair.
    *
    */
    std::list<std::pair<mfxI32, bool> >
        ltr_check_history;

    typedef void(ASC::*t_resizeImg)(mfxU8 *frame, mfxI32 srcWidth, mfxI32 srcHeight, mfxI32 inputPitch, ASCLayers dstIdx, mfxU32 parity);
    t_resizeImg resizeFunc;
    typedef void(ASC::*t_PutFrame)(mfxU8 *frame);
    t_PutFrame pPutFrameFunc;
    void VidSample_Alloc();
    void VidSample_dispose();
    void VidRead_dispose();
    void SetWidth(mfxI32 Width);
    void SetHeight(mfxI32 Height);
    void SetRes();
    void SetPitch(mfxI32 Pitch);
    void SetGpuPitch(mfxI32 GpuPitch);
    void SetNextField();
    void SetDimensions(mfxI32 Width, mfxI32 Height, mfxI32 Pitch);
    void alloc();
    void InitCPU();
    void Setup_Environment();
    void SetUltraFastDetection();
    void Params_Init();
    void InitStruct();
    void VidRead_Init();
    void VidSample_Init();
    void SubSampleASC_ImagePro(mfxU8 *frame, mfxI32 srcWidth, mfxI32 srcHeight, mfxI32 inputPitch, ASCLayers dstIdx, mfxU32 parity);
    void SubSampleASC_ImageInt(mfxU8 *frame, mfxI32 srcWidth, mfxI32 srcHeight, mfxI32 inputPitch, ASCLayers dstIdx, mfxU32 parity);
	bool CompareStats(mfxU8 current, mfxU8 reference);
    bool FrameRepeatCheck();
    void DetectShotChangeFrame();
    void GeneralBufferRotation();
    bool RunFrame(mfxU8 *frame, mfxU32 parity);
    void Put_LTR_Hint();
    ASC_LTR_DEC Continue_LTR_Mode(mfxU16 goodLTRLimit, mfxU16 badLTRLimit);
	mfxU16 AGOPSelect();
public:
    ASC_API void Init(mfxI32 width, mfxI32 height, mfxI32 pitch, mfxU32 interlaceMode);
    ASC_API void SetControlLevel(mfxU8 level);
    ASC_API void SetGoPSize(mfxU32 GoPSize);
    ASC_API void SetInterlaceMode(mfxU32 interlaceMode);
    ASC_API void ResetGoPSize();
    ASC_API void Close();
    ASC_API void Close(FILE *videoFile, FILE *scReportFile);
    ASC_API void PutFrameProgressive(mfxU8 *frame);
    ASC_API void PutFrameInterlaced(mfxU8 *frame);
    ASC_API bool PutFrame(mfxU8 *frame);
    ASC_API bool PutFrame_t(mfxU8 *frame);
    ASC_API bool Get_Last_frame_Data();
    ASC_API mfxU32 Get_starting_frame_number();
    ASC_API mfxU32 Get_frame_number();
    ASC_API mfxU32 Get_frame_variation_status();
    ASC_API mfxU32 Get_frame_shot_Decision();
    ASC_API mfxU32 Get_frame_last_in_scene();
	ASC_API mfxU16 GetFrameGopSize();
    ASC_API bool Get_GoPcorrected_frame_shot_Decision();
    ASC_API bool Get_LTR_advice();
    ASC_API ASC_LTR_DEC get_LTR_op_hint();
    ASC_API mfxU16 get_subsample_filter_selection();
    ASC_API mfxI32 Get_frame_Spatial_complexity();
    ASC_API mfxI32 Get_frame_Temporal_complexity();
    ASC_API void calc_RsCs_pic(mfxU8 *pSrc, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs);
    ASC_API void printMetadataHeader(FILE *SADOut);
    ASC_API void printStats(ASCTSCstat data, char *filename, FILE *SADOut);
    ASC_API void printStats(mfxI32 index, char *filename, FILE *SADOut);
    ASC_API void setInputFileName(char *filename);
};

#endif //_STRUCTURES_H_
