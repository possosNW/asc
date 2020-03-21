/*//////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2012-2017 Intel Corporation. All Rights Reserved.
//
*/
#include "asc_common_opt.h"

void calc_RSCS_4x4_C(mfxU8 *pSrc, mfxI32 pitch, mfxI32 *RS, mfxI32 *CS) {
    mfxI32 i, j;
    mfxU8 *pS = pSrc;
    mfxU8 *pS2 = pSrc + pitch;
    mfxI32 Rs, Cs;

    Cs = 0;
    Rs = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            Cs += (pS[j] > pS[j + 1]) ? (pS[j] - pS[j + 1]) : (pS[j + 1] - pS[j]);
            Rs += (pS[j] > pS2[j]) ? (pS[j] - pS2[j]) : (pS2[j] - pS[j]);
        }
        pS += pitch;
        pS2 += pitch;
    }

    *CS += Cs >> 4;
    *RS += Rs >> 4;
}