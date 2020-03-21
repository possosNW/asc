#pragma once
#ifndef _ASC_SSE4_IMPL_H_
#define _ASC_SSE4_IMPL_H_
#include "asc_common_impl.h"

void ME_SAD_8x8_Block_Search_SSE4(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU16 *bestSAD, int *bestX, int *bestY);
void ME_SAD_8x8_Block_FSearch_SSE4(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU32 *bestSAD, int *bestX, int *bestY);
void RsCsCalc_4x4_SSE4(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs,
    pmfxU16 pCs);
void RsCsCalc_bound_SSE4(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame,
    pmfxU32 pCsFrame, int wblocks, int hblocks);
void RsCsCalc_diff_SSE4(pmfxU16 pRs0, pmfxU16 pCs0, pmfxU16 pRs1, pmfxU16 pCs1, int wblocks,
    int hblocks, pmfxU32 pRsDiff, pmfxU32 pCsDiff);
void RsCsCalc_diff_2_SSE4(pmfxU32 pAccRs, pmfxU32 pAccCs, mfxU32 len, pmfxU32 pRsDiff,
    pmfxU32 pCsDiff);
void ImageDiffHistogram_SSE4(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height,
    mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC);
void GainOffset_SSE4(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch,
    mfxI16 gainDiff);
mfxI16 AvgLumaCalc_SSE4(pmfxU32 pAvgLineVal, int len);
void calc_RSCS_4x4_SSE41(mfxU8 *pSrc, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs);

#endif //_ASC_SSE4_IMPL_H_