/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#ifndef _IOFUNCTIONS_H_
#define _IOFUNCTIONS_H_

#include "ASCstructures.h"

static inline mfxF64 TimeMeasurement(LARGE_INTEGER start, LARGE_INTEGER stop, LARGE_INTEGER frequency) {
    return((stop.QuadPart - start.QuadPart) * mfxF64(1000.0) / frequency.QuadPart);
}

void TimeStart(ASCTime* timer);
void TimeStart(ASCTime* timer, int index);
void TimeStop(ASCTime* timer);
mfxF64 CatchTime(ASCTime *timer, char* message, int print);
mfxF64 CatchTime(ASCTime *timer, int index, char* message, int print);
mfxF64 CatchTime(ASCTime *timer, int indexInit, int indexEnd, char* message, int print);

void imageInit(ASCYUV *buffer);
void nullifier(ASCimageData *Buffer);
void ImDetails_Init(ASCImDetails *Rdata);
void ASCTSCstat_Init(ASCTSCstat **logic);


mfxI32 ASClogBase2aligned(mfxI32 number);
void ASCU8AllocandSet(pmfxU8 *ImageLayer, mfxI32 imageSize);
void PdYuvImage_Alloc(ASCYUV *pImage, mfxI32 dimVideoExtended);
void PdMVector_Alloc(ASCMv **MV, mfxI32 mvArraysize);

mfxF64 ASCTimeMeasurement(LARGE_INTEGER start,LARGE_INTEGER stop,LARGE_INTEGER frequency);
mfxU32 ASCvideoFileOpen(FILE **videoFile, char* videoFilename, mfxU32 Width, mfxU32 Height);
void ReadFrame(FILE *videoFile,ASCVidData dataIn, mfxU8 *dst);
void CopyFrame(mfxU8 *pSrc, mfxU32 srcPitch, ASCVidData dataIn, ASCVidSample *videoIn, mfxI32 frame_number, mfxI32 reference_number);


#endif //_IOFUNCTIONS_H_