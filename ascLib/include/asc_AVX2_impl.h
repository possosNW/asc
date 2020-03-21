#pragma once
#ifndef _ASC_AVX2_IMPL_H_
#define _ASC_AVX2_IMPL_H_
#include "asc_common_impl.h"

/*typedef union _MMINTRIN_TYPE(32) __m256i {
#if !defined(_MSC_VER)
//
// To support GNU compatible intialization with initializers list,
// make first union member to be of int64 type.
//
__int64 m256i_gcc_compatibility[4];
#endif
__int8 m256i_i8[32];
__int16 m256i_i16[16];
__int32 m256i_i32[8];
__int64 m256i_i64[4];
unsigned __int8 m256i_u8[32];
unsigned __int16 m256i_u16[16];
unsigned __int32 m256i_u32[8];
unsigned __int64 m256i_u64[4];
};*/

void ME_SAD_8x8_Block_Search_AVX2(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU16 *bestSAD, int *bestX, int *bestY);
void ME_SAD_8x8_Block_FSearch_AVX2(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU32 *bestSAD, int *bestX, int *bestY);
void RsCsCalc_4x4_AVX2(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs,
    pmfxU16 pCs);
void RsCsCalc_bound_AVX2(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame,
    pmfxU32 pCsFrame, int wblocks, int hblocks);
void RsCsCalc_diff_AVX2(pmfxU16 pRs0, pmfxU16 pCs0, pmfxU16 pRs1, pmfxU16 pCs1, int wblocks,
    int hblocks, pmfxU32 pRsDiff, pmfxU32 pCsDiff);
void ImageDiffHistogram_AVX2(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height,
    mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC);
void GainOffset_AVX2(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch,
    mfxI16 gainDiff);
mfxI16 AvgLumaCalc_AVX2(pmfxU32 pAvgLineVal, int len);
void calc_RSCS_4x4_AVX2(mfxU8 *pSrc, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs);
void ME_VAR_8x8_Block_AVX2(mfxU8 *pSrc, mfxU8 *pRef, mfxU8 *pMCref, mfxI16 srcAvgVal,
	mfxI16 refAvgVal, mfxU32 srcPitch, mfxU32 refPitch, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar);

#endif //_ASC_AVX2_IMPL_H_