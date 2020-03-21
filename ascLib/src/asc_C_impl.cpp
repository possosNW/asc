/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#include "asc_C_impl.h"
#include "asc_common_opt.h"

void ME_SAD_8x8_Block_Search_C(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU16 *bestSAD, int *bestX, int *bestY) {
    for (int y = 0; y < yrange; y += SAD_SEARCH_VSTEP) {
        for (int x = 0; x < xrange; x += SAD_SEARCH_VSTEP) {/*x++) {*/
            pmfxU8
                pr = pRef + (y * pitch) + x,
                ps = pSrc;
            mfxU16
                SAD = 0;
            for (int i = 0; i < 8; i++) {
                SAD += (mfxU16)abs(pr[0] - ps[0]);
                SAD += (mfxU16)abs(pr[1] - ps[1]);
                SAD += (mfxU16)abs(pr[2] - ps[2]);
                SAD += (mfxU16)abs(pr[3] - ps[3]);
                SAD += (mfxU16)abs(pr[4] - ps[4]);
                SAD += (mfxU16)abs(pr[5] - ps[5]);
                SAD += (mfxU16)abs(pr[6] - ps[6]);
                SAD += (mfxU16)abs(pr[7] - ps[7]);
                pr += pitch;
                ps += pitch;
            }
            if (SAD < *bestSAD) {
                *bestSAD = SAD;
                *bestX = x;
                *bestY = y;
            }
        }
    }
}

void ME_SAD_8x8_Block_FSearch_C(mfxU8 *pSrc, mfxU8 *pRef, int pitch, int xrange, int yrange,
    mfxU32 *bestSAD, int *bestX, int *bestY) {
    for (int y = 0; y < yrange; y++) {
        for (int x = 0; x < xrange; x++) {
            pmfxU8
                pr = pRef + (y * pitch) + x,
                ps = pSrc;
            mfxU32
                SAD = 0;
            for (int i = 0; i < 8; i++) {
                SAD += abs(pr[0] - ps[0]);
                SAD += abs(pr[1] - ps[1]);
                SAD += abs(pr[2] - ps[2]);
                SAD += abs(pr[3] - ps[3]);
                SAD += abs(pr[4] - ps[4]);
                SAD += abs(pr[5] - ps[5]);
                SAD += abs(pr[6] - ps[6]);
                SAD += abs(pr[7] - ps[7]);
                pr += pitch;
                ps += pitch;
            }
            if (SAD < *bestSAD) {
                *bestSAD = SAD;
                *bestX = x;
                *bestY = y;
            }
        }
    }
}

void RsCsCalc_4x4_C(pmfxU8 pSrc, int srcPitch, int wblocks, int hblocks, pmfxU16 pRs, pmfxU16 pCs) {
    mfxU32 count = 0;
    pSrc += (4 * srcPitch) + 4;
    for (mfxI16 i = 0; i < hblocks - 2; i++)
    {
        for (mfxI16 j = 0; j < wblocks - 2; j++)
        {
            mfxU16 accRs = 0;
            mfxU16 accCs = 0;

            for (mfxI32 k = 0; k < 4; k++)
            {
                for (mfxI32 l = 0; l < 4; l++)
                {
                    mfxU16 dRs = (mfxU16)abs(pSrc[l] - pSrc[l - srcPitch]) >> 2;
                    mfxU16 dCs = (mfxU16)abs(pSrc[l] - pSrc[l - 1]) >> 2;
                    accRs += dRs * dRs;
                    accCs += dCs * dCs;
                }
                pSrc += srcPitch;
            }
            pRs[i * wblocks + j] = accRs;
            pCs[i * wblocks + j] = accCs;

            pSrc -= 4 * srcPitch;
            pSrc += 4;
            count++;
        }
        pSrc -= 4 * (wblocks - 2);
        pSrc += 4 * srcPitch;
    }
}

void RsCsCalc_bound_C(pmfxU16 pRs, pmfxU16 pCs, pmfxU16 pRsCs, pmfxU32 pRsFrame, pmfxU32 pCsFrame, int wblocks, int hblocks) {
    mfxI32 len = wblocks * hblocks;
    mfxU16 accRs = 0;
    mfxU16 accCs = 0;

    for (mfxI32 i = 0; i < len; i++) {
        accRs += pRs[i] >> 7;
        accCs += pCs[i] >> 7;
        pRsCs[i] = (pRs[i] + pCs[i]) >> 1;
    }

    *pRsFrame = accRs;
    *pCsFrame = accCs;
}

void RsCsCalc_diff_C(pmfxU16 pRs0, pmfxU16 pCs0,
    pmfxU16 pRs1, pmfxU16 pCs1,
    int wblocks, int hblocks,
    pmfxU32 pRsDiff, pmfxU32 pCsDiff) {
    mfxU32 len = wblocks * hblocks;
    mfxU16 accRs = 0;
    mfxU16 accCs = 0;

#if 0
    mfxI32
        pos = 0;
    FILE *
        data = NULL;
    data = fopen("cs_data.txt", "a");
    for (mfxI32 j = 0; j < hblocks; j++) {
        for (mfxI32 i = 0; i < wblocks; i++) {
            pos = (j * wblocks) + i;
            accRs += abs((pRs0[pos] >> 5) - (pRs1[pos] >> 5));
            accCs += abs((pCs0[pos] >> 5) - (pCs1[pos] >> 5));
            fprintf(data, "%i\t", (pCs0[pos] >> 5) - (pCs1[pos] >> 5));
        }
        fprintf(data, "\n");
    }
    fclose(data);
#else
    for (mfxU32 i = 0; i < len; i++)
    {
        accRs += (mfxU16)abs((pRs0[i] >> 5) - (pRs1[i] >> 5));
        accCs += (mfxU16)abs((pCs0[i] >> 5) - (pCs1[i] >> 5));
    }
#endif
    *pRsDiff = accRs;
    *pCsDiff = accCs;
}

void RsCsCalc_diff_2_C(pmfxU32 pAccRs, pmfxU32 pAccCs, mfxU32 len, pmfxU32 pRsDiff, pmfxU32 pCsDiff) {
    *pRsDiff = 0;
    *pCsDiff = 0;

    for (mfxU32 i = 0; i < len; i++)
    {
        *pRsDiff += pAccRs[i];
        *pCsDiff += pAccCs[i];
    }
}

void ImageDiffHistogram_C(pmfxU8 pSrc, pmfxU8 pRef, int pitch, int width, int height,
    mfxU32 histogram[5], mfxI64 *pSrcDC, mfxI64 *pRefDC) {
    mfxI64 srcDC = 0;
    mfxI64 refDC = 0;

    histogram[0] = 0;
    histogram[1] = 0;
    histogram[2] = 0;
    histogram[3] = 0;
    histogram[4] = 0;

    for (mfxI32 i = 0; i < height; i++)
    {
        for (mfxI32 j = 0; j < width; j++)
        {
            int s = pSrc[j];
            int r = pRef[j];
            int d = s - r;

            srcDC += s;
            refDC += r;

            if (d < -HIST_THRESH_HI)
                histogram[0]++;
            else if (d < -HIST_THRESH_LO)
                histogram[1]++;
            else if (d < HIST_THRESH_LO)
                histogram[2]++;
            else if (d < HIST_THRESH_HI)
                histogram[3]++;
            else
                histogram[4]++;
        }
        pSrc += pitch;
        pRef += pitch;
    }
    *pSrcDC = srcDC;
    *pRefDC = refDC;
}

void GainOffset_C(pmfxU8 *pSrc, pmfxU8 *pDst, mfxU16 width, mfxU16 height, mfxU16 pitch, mfxI16 gainDiff) {
    pmfxU8
        ss = *pSrc,
        dd = *pDst;
    for (mfxU16 i = 0; i < height; i++) {
        for (mfxU16 j = 0; j < width; j++) {
            mfxI16
                val = ss[j + i * pitch] - gainDiff;
            dd[j + i * pitch] = (mfxU8)NMIN(NMAX(val, 0), 255);
        }
    }
    *pSrc = *pDst;
}


mfxI16 AvgLumaCalc_C(pmfxU32 pAvgLineVal, int len) {
    mfxU32
        acc = 0;
    mfxI16
        avgVal = 0;
    for (int i = 0; i < len; i++)
        acc += pAvgLineVal[i];
    avgVal = (mfxI16)(acc >> 9);
    return avgVal;
}

void calc_RsCs_pic_C(mfxU8 *pPicY, mfxI32 width, mfxI32 height, mfxI32 pitch, mfxF64 &RsCs) {
    mfxI32 i, j;
    mfxI32 Rs, Cs;
    mfxU8 *pS = pPicY;

    Rs = Cs = 0;
    for (i = 4; i < (height - 4); i += 4) {
        for (j = 4; j < (width - 4); j += 4) {
            pS = pPicY + i * pitch + j;
            calc_RSCS_4x4_C(pS, pitch, &Rs, &Cs);
        }
    }

    mfxI32 w4 = (width - 8) >> 2;
    mfxI32 h4 = (height - 8) >> 2;
    mfxF64 d1 = 1.0 / (mfxF64)(w4*h4);
    mfxF64 drs = (mfxF64)Rs * d1;
    mfxF64 dcs = (mfxF64)Cs * d1;

    RsCs = sqrt(drs * drs + dcs * dcs);
}

void ME_VAR_8x8_Block_C(mfxU8 *pSrc, mfxU8 *pRef, mfxU8 *pMCref, mfxI16 srcAvgVal, mfxI16 refAvgVal, mfxU32 srcPitch, mfxU32 refPitch, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar)
{
	/*__m128i srcAvg = _mm_set1_epi16((mfxU16)srcAvgVal);
	__m128i refAvg = _mm_set1_epi16((mfxU16)refAvgVal);

	__m128i src[8],
			ref[8],
		accuVar = { 0 },
		accuJtvar = { 0 };

	#pragma unroll
	for (mfxU8 i = 0; i < 8; i++)
	{
		src[i] = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)&pSrc[i * srcPitch]));
		ref[i] = _mm_cvtepu8_epi16(_mm_loadu_si128((__m128i *)&pRef[i * refPitch]));
		src[i] = _mm_sub_epi16(src[i], srcAvg);
		ref[i] = _mm_sub_epi16(ref[i], refAvg);
		accuVar = _mm_add_epi16(_mm_mullo_epi16(src[i], src[i]),accuVar);
		accuJtvar = _mm_add_epi16(_mm_mullo_epi16(src[i], ref[i]), accuJtvar);
	}*/

	for (mfxU8 i = 0; i < 8; i++) {
		for (mfxU8 j = 0; j < 8; j++) {
			var += ((mfxI32)(pSrc[(i * srcPitch) + j] - srcAvgVal) * (mfxI32)(pSrc[(i * srcPitch) + j] - srcAvgVal));
			jtvar += ((mfxI32)(pSrc[(i * srcPitch) + j] - srcAvgVal) * (mfxI32)(pRef[(i * refPitch) + j] - refAvgVal));
			jtMCvar += ((mfxI32)(pSrc[(i * srcPitch) + j] - srcAvgVal) * (mfxI32)(pMCref[(i * refPitch) + j] - refAvgVal));
		}
	}
}
