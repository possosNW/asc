#pragma once
#ifndef _ASC_CPU_DISPATCHER_H_
#define _ASC_CPU_DISPATCHER_H_

#include "CPUdetect.h"
#include "asc_C_impl.h"
#include "asc_SSE4_impl.h"
#include "asc_AVX2_impl.h"

//
// CPU Dispatcher
//
static int g_AVX2_available = CpuFeature_AVX2();
static int g_SSE4_available = CpuFeature_SSE41();

typedef void(*t_GainOffset)(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch, mfxI16 gainDiff);
typedef void(*t_RsCsCalc)(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs, pmfxU16 pCs);
typedef void(*t_RsCsCalc_bound)(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame, pmfxU32 pCsFrame, int wblocks, int hblocks);
typedef void(*t_RsCsCalc_diff)(pmfxU16 pRs0, pmfxU16 pCs0, pmfxU16 pRs1, pmfxU16 pCs1, int wblocks, int hblocks, pmfxU32 pRsDiff, pmfxU32 pCsDiff);
typedef void(*t_ImageDiffHistogram)(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height, mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC);
typedef mfxI16(*t_AvgLumaCalc)(pmfxU32 pAvgLineVal, int len);
typedef void(*t_ME_SAD_8x8_Block_Search)(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange, mfxU16 *bestSAD, int *bestX, int *bestY);
typedef void(*t_ME_SAD_8x8_Block_FSearch)(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange, mfxU32 *bestSAD, int *bestX, int *bestY);
typedef void(*t_CalcRsCs)(mfxU8 *pSrc, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs);
typedef void(*t_ME_VAR_8x8_Block)(mfxU8 *pSrc, mfxU8 *pRef, mfxU8 *pMCref, mfxI16 srcAvgVal, mfxI16 refAvgVal, mfxU32 srcPitch, mfxU32 refPitch, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar);

inline void ME_VAR_8x8_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU8 *pMCref, mfxI16 srcAvgVal, mfxI16 refAvgVal, mfxU32 srcPitch, mfxU32 refPitch, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar) {
	static t_ME_VAR_8x8_Block f = g_AVX2_available ? ME_VAR_8x8_Block_AVX2 : ME_VAR_8x8_Block_C;
	(*f)(pSrc, pRef, pMCref, srcAvgVal, refAvgVal, srcPitch, refPitch, var, jtvar, jtMCvar);
}

inline void ME_SAD_8x8_Block_Search(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange, mfxU16 *bestSAD, int *bestX, int *bestY) {
    static t_ME_SAD_8x8_Block_Search f = g_AVX2_available ? ME_SAD_8x8_Block_Search_AVX2 : (g_SSE4_available ? ME_SAD_8x8_Block_Search_SSE4 : ME_SAD_8x8_Block_Search_C);
    //static t_ME_SAD_8x8_Block_Search f = ME_SAD_8x8_Block_Search_C;
    (*f)(pSrc, pRef, pitch, xrange, yrange, bestSAD, bestX, bestY);
}

inline void ME_SAD_8x8_Block_FSearch(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange, mfxU32 *bestSAD, int *bestX, int *bestY) {
    static t_ME_SAD_8x8_Block_FSearch f = g_AVX2_available ? ME_SAD_8x8_Block_FSearch_AVX2 : (g_SSE4_available ? ME_SAD_8x8_Block_FSearch_SSE4 : ME_SAD_8x8_Block_FSearch_C);
    //static t_ME_SAD_8x8_Block_FSearch f = ME_SAD_8x8_Block_FSearch_C;
    (*f)(pSrc, pRef, pitch, xrange, yrange, bestSAD, bestX, bestY);
}

inline void GainOffset(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch, mfxI16 gainDiff) {
    static t_GainOffset f = g_AVX2_available ? GainOffset_AVX2 : (g_SSE4_available ? GainOffset_SSE4 : GainOffset_C);
    //static t_GainOffset f = GainOffset_C;
    (*f)(pSrc, pDst, width, height, pitch, gainDiff);
}

inline void RsCsCalc_4x4(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs, pmfxU16 pCs) {
    static t_RsCsCalc f = g_AVX2_available ? RsCsCalc_4x4_AVX2 : (g_SSE4_available ? RsCsCalc_4x4_SSE4 : RsCsCalc_4x4_C);
    //static t_RsCsCalc f = RsCsCalc_4x4_C;
    (*f)(pSrc, srcPitch, wblocks, hblocks, pRs, pCs);
}

inline void RsCsCalc_bound(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame, pmfxU32 pCsFrame, int wblocks, int hblocks) {
    static t_RsCsCalc_bound f = g_AVX2_available ? RsCsCalc_bound_AVX2 : (g_SSE4_available ? RsCsCalc_bound_SSE4 : RsCsCalc_bound_C);
    //static t_RsCsCalc_bound f = RsCsCalc_bound_C;
    (*f)(pRs, pCs, pRsCs, pRsFrame, pCsFrame, wblocks, hblocks);
}

inline void RsCsCalc_diff(pmfxU16 pRs0, pmfxU16 pCs0, pmfxU16 pRs1, pmfxU16 pCs1, int wblocks, int hblocks, pmfxU32 pRsDiff, pmfxU32 pCsDiff) {
    static t_RsCsCalc_diff f = g_AVX2_available ? RsCsCalc_diff_AVX2 : (g_SSE4_available ? RsCsCalc_diff_SSE4 : RsCsCalc_diff_C);
    //static t_RsCsCalc_diff f = RsCsCalc_diff_C;
    (*f)(pRs0, pCs0, pRs1, pCs1, wblocks, hblocks, pRsDiff, pCsDiff);
}

inline void ImageDiffHistogram(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height, mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC) {
    static t_ImageDiffHistogram f = g_AVX2_available ? ImageDiffHistogram_AVX2 : (g_SSE4_available ? ImageDiffHistogram_SSE4 : ImageDiffHistogram_C);
    //static t_ImageDiffHistogram f = ImageDiffHistogram_C;
    (*f)(pSrc, pRef, pitch, width, height, histogram, pSrcDC, pRefDC);
}

inline mfxI16 AvgLumaCalc(pmfxU32 pAvgLineVal, int len) {
    static t_AvgLumaCalc f = g_AVX2_available ? AvgLumaCalc_AVX2 : (g_SSE4_available ? AvgLumaCalc_SSE4 : AvgLumaCalc_C);
    //static t_AvgLumaCalc f = AvgLumaCalc_C;
    return (*f)(pAvgLineVal, len);
}

ASC_API inline void ASC::calc_RsCs_pic(mfxU8 *pSrc, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs) {
    static t_CalcRsCs f = g_AVX2_available ? calc_RSCS_4x4_AVX2 : (g_SSE4_available ? calc_RSCS_4x4_SSE41 : calc_RsCs_pic_C);
    (*f)(pSrc, width, height, pitch, RsCs);
}

//
// end Dispatcher
//

#endif //_ASC_CPU_DISPATCHER_H_