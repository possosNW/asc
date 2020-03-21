/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#pragma once
#ifndef _ASC_C_IMPL_H_
#define _ASC_C_IMPL_H_
#include "asc_common_impl.h"


void ME_SAD_8x8_Block_Search_C(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU16 *bestSAD, int *bestX, int *bestY);
void ME_SAD_8x8_Block_FSearch_C(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU32 *bestSAD, int *bestX, int *bestY);
void RsCsCalc_4x4_C(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs,
    pmfxU16 pCs);
void RsCsCalc_bound_C(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame,
    pmfxU32 pCsFrame, int wblocks, int hblocks);
void RsCsCalc_diff_C(pmfxU16 pRs0, pmfxU16 pCs0, pmfxU16 pRs1, pmfxU16 pCs1, int wblocks,
    int hblocks, pmfxU32 pRsDiff, pmfxU32 pCsDiff);
void RsCsCalc_diff_2_C(pmfxU32 pAccRs, pmfxU32 pAccCs, mfxU32 len, pmfxU32 pRsDiff,
    pmfxU32 pCsDiff);
void ImageDiffHistogram_C(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height,
    mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC);
void GainOffset_C(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch,
    mfxI16 gainDiff);
mfxI16 AvgLumaCalc_C(pmfxU32 pAvgLineVal, int len);
void calc_RsCs_pic_C(mfxU8 *pPicY, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs);
void ME_VAR_8x8_Block_C(mfxU8 *pSrc, mfxU8 *pRef, mfxU8 *pMCref, mfxI16 srcAvgVal,
	mfxI16 refAvgVal, mfxU32 srcPitch, mfxU32 refPitch, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar);

#endif //_ASC_C_IMPL_H_