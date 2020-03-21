#ifndef _TREE_H_
#define _TREE_H_

#include "ASCstructures.h"


bool SCDetectRF(mfxI32 diffMVdiffVal, mfxU32 RsCsDiff, mfxU32 MVDiff, mfxU32 Rs, mfxU32 AFD,
    mfxU32 CsDiff, mfxI32 diffTSC, mfxU32 TSC, mfxU32 gchDC, mfxI32 diffRsCsdiff,
    mfxU32 posBalance, mfxU32 SC, mfxU32 TSCindex, mfxU32 Scindex, mfxU32 Cs,
    mfxI32 diffAFD, mfxU32 negBalance, mfxU32 ssDCval, mfxU32 refDCval, mfxU32 RsDiff,
    mfxI16 tcor, mfxI16 mcTcor, mfxU8 control);

mfxU16 FilterSelectRF(
    mfxI32             , mfxU32 RsCsDiff  , mfxU32 MVDiff , mfxU32 Rs      , mfxU32 AFD,
    mfxU32 CsDiff      , mfxI32 diffTSC   , mfxU32 TSC    , mfxU32 gchDC   , mfxI32 diffRsCsdiff,
    mfxU32 posBalance  , mfxU32 SC        , mfxU32        , mfxU32         , mfxU32 Cs,
    mfxI32 diffAFD     , mfxU32 negBalance, mfxU32 ssDCval, mfxU32 refDCval, mfxU32 RsDiff,
    mfxU32 mu_mv_mag_sq, mfxI16 mcTcor    , mfxI16);

mfxU16 AGOPSelectRF(
	mfxI32 diffMVdiffVal, mfxU32 RsCsDiff, mfxU32 MVDiff, mfxU32 Rs, mfxU32 AFD,
	mfxU32 CsDiff, mfxI32 diffTSC, mfxU32 TSC, mfxU32 gchDC, mfxI32 diffRsCsdiff,
	mfxU32 posBalance, mfxU32 SC, mfxU32 TSCindex, mfxU32 Scindex, mfxU32 Cs,
	mfxI32 diffAFD, mfxU32 negBalance, mfxU32 ssDCval, mfxU32 refDCval, mfxU32 RsDiff,
	mfxU32 mu_mv_mag_sq, mfxI16 mcTcor, mfxI16 tcor);

#endif //_TREE_H_