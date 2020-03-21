#ifndef _MOTIONESTIMATIONENGINE_H_
#define _MOTIONESTIMATIONENGINE_H_

#include "ASCstructures.h"
#include "asc_CPU_dispatcher.h"
//#include "CPUdetect.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

void MotionRangeDeliveryF(mfxI16 xLoc, mfxI16 yLoc, mfxI16 *limitXleft, mfxI16 *limitXright, mfxI16 *limitYup, mfxI16 *limitYdown, ASCImDetails dataIn);

/* 4x4 Block size Functions */
bool MVcalcSAD4x4(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU32 *bestSAD, mfxI32 *distance);
/* 8x8 Block size Functions */
bool MVcalcSAD8x8(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails *dataIn, mfxU32 *bestSAD, mfxI32 *distance);
mfxU16 __cdecl ME_simple(ASCVidRead *videoIn, mfxI32 fPos, ASCImDetails *dataIn, ASCimageData *scale, ASCimageData *scaleRef, bool first, ASCVidData *limits);
/* All Block sizes -general case- */
bool MVcalcSAD(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxI32 fPos,mfxI32 xLoc, mfxI32 yLoc, mfxU32 *bestSAD, mfxI32 *distance);
/* ------------------------------ */
void SearchLimitsCalc(mfxI32 xLoc, mfxI32 yLoc, mfxI32 *limitXleft, mfxI32 *limitXright, mfxI32 *limitYup, mfxI32 *limitYdown, ASCImDetails dataIn, mfxI32 range, ASCMv mv, ASCVidData limits);
mfxF32 Dist(ASCMv vector);
mfxI32 DistInt(ASCMv vector);
void MVpropagationCheck(mfxI32 xLoc, mfxI32 yLoc, ASCImDetails dataIn, ASCMv *propagatedMV);

#endif //_MOTIONESTIMATIONENGINE_H_