#ifndef ME_HEADER
#define ME_HEADER

#include "ASCstructures.h"
#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define _mm_loadh_epi64(a, ptr) _mm_castps_si128(_mm_loadh_pi(_mm_castsi128_ps(a), (__m64 *)(ptr)))
#define _mm_movehl_epi64(a, b) _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(b)))

mfxU16 ME_SAD_4x4_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_cvtsi32_si128(*(int *)&pSrc[0*srcPitch]);
    __m128i s1 = _mm_cvtsi32_si128(*(int *)&pSrc[1*srcPitch]);
    __m128i r0 = _mm_cvtsi32_si128(*(int *)&pRef[0*refPitch]);
    __m128i r1 = _mm_cvtsi32_si128(*(int *)&pRef[1*refPitch]);
    __m128i s2 = _mm_cvtsi32_si128(*(int *)&pSrc[2*srcPitch]);
    __m128i s3 = _mm_cvtsi32_si128(*(int *)&pSrc[3*srcPitch]);
    __m128i r2 = _mm_cvtsi32_si128(*(int *)&pRef[2*refPitch]);
    __m128i r3 = _mm_cvtsi32_si128(*(int *)&pRef[3*refPitch]);

    s0 = _mm_unpacklo_epi32(s0, s1);
    r0 = _mm_unpacklo_epi32(r0, r1);
    s2 = _mm_unpacklo_epi32(s2, s3);
    r2 = _mm_unpacklo_epi32(r2, r3);

    s0 = _mm_sad_epu8(s0, r0);
    s2 = _mm_sad_epu8(s2, r2);

    s0 = _mm_add_epi32(s0, s2);

    return (mfxU16)_mm_extract_epi16(s0, 0);
}

mfxU16 ME_SAD_8x8_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[0*srcPitch]), (__m128i *)&pSrc[1*srcPitch]);
    __m128i r0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[0*refPitch]), (__m128i *)&pRef[1*refPitch]);
    __m128i s1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[2*srcPitch]), (__m128i *)&pSrc[3*srcPitch]);
    __m128i r1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[2*refPitch]), (__m128i *)&pRef[3*refPitch]);
    __m128i s2 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[4*srcPitch]), (__m128i *)&pSrc[5*srcPitch]);
    __m128i r2 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[4*refPitch]), (__m128i *)&pRef[5*refPitch]);
    __m128i s3 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[6*srcPitch]), (__m128i *)&pSrc[7*srcPitch]);
    __m128i r3 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[6*refPitch]), (__m128i *)&pRef[7*refPitch]);

    s0 = _mm_sad_epu8(s0, r0);
    s1 = _mm_sad_epu8(s1, r1);
    s2 = _mm_sad_epu8(s2, r2);
    s3 = _mm_sad_epu8(s3, r3);

    s0 = _mm_add_epi32(s0, s1);
    s2 = _mm_add_epi32(s2, s3);

    s0 = _mm_add_epi32(s0, s2);
    s2 = _mm_movehl_epi64(s2, s0);
    s0 = _mm_add_epi32(s0, s2);

    return (mfxU16)_mm_extract_epi16(s0, 0);
}

mfxU16 ME_preSAD_8x8_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[0*srcPitch]), (__m128i *)&pSrc[1*srcPitch]);
    __m128i r0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[0*refPitch]), (__m128i *)&pRef[1*refPitch]);
    __m128i s1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[2*srcPitch]), (__m128i *)&pSrc[3*srcPitch]);
    __m128i r1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[2*refPitch]), (__m128i *)&pRef[3*refPitch]);

    s0 = _mm_sad_epu8(s0, r0);
    s1 = _mm_sad_epu8(s1, r1);

    s0 = _mm_add_epi32(s0, s1);
    s1 = _mm_movehl_epi64(s1, s0);
    s0 = _mm_add_epi32(s0, s1);

    return (mfxU16)_mm_extract_epi16(s0, 0);
}

mfxU16 ME_SAD_8x16_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[0*srcPitch]), (__m128i *)&pSrc[1*srcPitch]);
    __m128i r0 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[0*refPitch]), (__m128i *)&pRef[1*refPitch]);
    __m128i s1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[2*srcPitch]), (__m128i *)&pSrc[3*srcPitch]);
    __m128i r1 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[2*refPitch]), (__m128i *)&pRef[3*refPitch]);
    __m128i s2 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[4*srcPitch]), (__m128i *)&pSrc[5*srcPitch]);
    __m128i r2 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[4*refPitch]), (__m128i *)&pRef[5*refPitch]);
    __m128i s3 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[6*srcPitch]), (__m128i *)&pSrc[7*srcPitch]);
    __m128i r3 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[6*refPitch]), (__m128i *)&pRef[7*refPitch]);
    __m128i s4 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[8*srcPitch]), (__m128i *)&pSrc[9*srcPitch]);
    __m128i r4 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[8*refPitch]), (__m128i *)&pRef[9*refPitch]);
    __m128i s5 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[10*srcPitch]), (__m128i *)&pSrc[11*srcPitch]);
    __m128i r5 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[10*refPitch]), (__m128i *)&pRef[11*refPitch]);
    __m128i s6 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[12*srcPitch]), (__m128i *)&pSrc[13*srcPitch]);
    __m128i r6 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[12*refPitch]), (__m128i *)&pRef[13*refPitch]);
    __m128i s7 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pSrc[14*srcPitch]), (__m128i *)&pSrc[15*srcPitch]);
    __m128i r7 = _mm_loadh_epi64(_mm_loadl_epi64((__m128i *)&pRef[14*refPitch]), (__m128i *)&pRef[15*refPitch]);

    s0 = _mm_sad_epu8(s0, r0);
    s1 = _mm_sad_epu8(s1, r1);
    s2 = _mm_sad_epu8(s2, r2);
    s3 = _mm_sad_epu8(s3, r3);
    s4 = _mm_sad_epu8(s4, r4);
    s5 = _mm_sad_epu8(s5, r5);
    s6 = _mm_sad_epu8(s6, r6);
    s7 = _mm_sad_epu8(s7, r7);

    s0 = _mm_add_epi32(s0, s1);
    s2 = _mm_add_epi32(s2, s3);
    s4 = _mm_add_epi32(s4, s5);
    s6 = _mm_add_epi32(s6, s7);

    s0 = _mm_add_epi32(s0, s2);
    s4 = _mm_add_epi32(s4, s6);

    s0 = _mm_add_epi32(s0, s4);
    s4 = _mm_movehl_epi64(s4, s0);
    s0 = _mm_add_epi32(s0, s4);

    return (mfxU16)_mm_extract_epi16(s0, 0);
}

mfxU16 ME_SAD_16x8_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[0*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[0*refPitch]));
    __m128i s1 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[1*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[1*refPitch]));
    __m128i s2 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[2*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[2*refPitch]));
    __m128i s3 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[3*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[3*refPitch]));
    __m128i s4 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[4*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[4*refPitch]));
    __m128i s5 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[5*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[5*refPitch]));
    __m128i s6 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[6*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[6*refPitch]));
    __m128i s7 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[7*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[7*refPitch]));

    s0 = _mm_add_epi32(s0, s1);
    s2 = _mm_add_epi32(s2, s3);
    s4 = _mm_add_epi32(s4, s5);
    s6 = _mm_add_epi32(s6, s7);

    s0 = _mm_add_epi32(s0, s2);
    s4 = _mm_add_epi32(s4, s6);

    s0 = _mm_add_epi32(s0, s4);
    s4 = _mm_movehl_epi64(s4, s0);
    s0 = _mm_add_epi32(s0, s4);

    return (mfxU16)_mm_extract_epi16(s0, 0);
}

mfxU16 ME_SAD_16x16_Block(mfxU8 *pSrc, mfxU8 *pRef, mfxU32 srcPitch, mfxU32 refPitch)
{
    __m128i s0 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[0*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[0*refPitch]));
    __m128i s1 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[1*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[1*refPitch]));
    __m128i s2 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[2*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[2*refPitch]));
    __m128i s3 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[3*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[3*refPitch]));
    __m128i s4 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[4*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[4*refPitch]));
    __m128i s5 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[5*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[5*refPitch]));
    __m128i s6 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[6*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[6*refPitch]));
    __m128i s7 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[7*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[7*refPitch]));
    __m128i s8 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[8*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[8*refPitch]));
    __m128i s9 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[9*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[9*refPitch]));
    __m128i s10 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[10*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[10*refPitch]));
    __m128i s11 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[11*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[11*refPitch]));
    __m128i s12 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[12*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[12*refPitch]));
    __m128i s13 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[13*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[13*refPitch]));
    __m128i s14 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[14*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[14*refPitch]));
    __m128i s15 = _mm_sad_epu8(_mm_loadu_si128((__m128i *)&pSrc[15*srcPitch]), _mm_loadu_si128((__m128i *)&pRef[15*refPitch]));

    s0 = _mm_add_epi32(s0, s1);
    s2 = _mm_add_epi32(s2, s3);
    s4 = _mm_add_epi32(s4, s5);
    s6 = _mm_add_epi32(s6, s7);
    s8 = _mm_add_epi32(s8, s9);
    s10 = _mm_add_epi32(s10, s11);
    s12 = _mm_add_epi32(s12, s13);
    s14 = _mm_add_epi32(s14, s15);

    s0 = _mm_add_epi32(s0, s2);
    s4 = _mm_add_epi32(s4, s6);
    s8 = _mm_add_epi32(s8, s10);
    s12 = _mm_add_epi32(s12, s14);

    s0 = _mm_add_epi32(s0, s4);
    s8 = _mm_add_epi32(s8, s12);

    s0 = _mm_add_epi32(s0, s8);
    s8 = _mm_movehl_epi64(s8, s0);
    s0 = _mm_add_epi32(s0, s8);

    return (mfxU16)_mm_cvtsi128_si32(s0);
}

mfxU32 ME_SAD_4x4_Block_Search_25(mfxI32 *x, mfxI32 *y, mfxU8 *src, mfxU8 *ref, mfxU32 stride) {
    mfxU32
        result = 0xffffffff;
    mfxI32
        limit = 2*stride;
    for(int i= -limit;i<=limit;i+=stride)
        for(int j=-limit;j<=limit;j+=stride) {
            mfxU32
                r = ME_SAD_4x4_Block(src, ref + (i + stride * j), stride, stride);
            if( r < result ) {
                result = r;
                *x = i;
                *y = j;
            }
        }
    return result;
}

mfxU32 ME_SAD_4x4_Block_Search_5x5(mfxI32 *x, mfxI32 *y, mfxU8 *src, mfxU8 *ref, mfxU32 stride) {
    mfxU32 
        result = 0xffffffff;
    for(int i = -2; i <= 2; ++i )
        for(int j = -2; j <= 2; ++j ) {
            mfxU32
                r = ME_SAD_4x4_Block(src, ref + (i + stride * j), stride, stride);
            if( r < result ) {
                result = r;
                *x = i;
                *y = j;
            }
        }
    return result;
}

mfxU32 ME_SAD_8x8_Block_Search_25(mfxI32 *x, mfxI32 *y, mfxU8 *src, mfxU8 *ref, mfxU32 stride)
{
    mfxU32
        result = 0xffffffff;
    mfxI32
        limit = 2*stride;
    for(int i=-limit;i<=limit;i+=stride)
        for(int j=-limit;j<=limit;j+=stride) {
            mfxU32
                r = ME_SAD_8x8_Block(src, ref + (i + stride * j), stride, stride);
            if( r < result ) {
                result = r;
                *x = i;
                *y = j;
            }
        }
    return result;
}

mfxU32 ME_SAD_8x8_Block_Search_5x5(mfxI32 *x, mfxI32 *y, mfxU8 *src, mfxU8 *ref, mfxU32 stride) {
    mfxU32
        result = 0xffffffff;
    for(int i = -2; i <= 2; ++i )
        for(int  j = -2; j <= 2; ++j ) {
            mfxU32
                r = ME_SAD_8x8_Block(src, ref + (i + stride * j), stride, stride);
            if( r < result ) {
                result = r;
                *x = i;
                *y = j;
            }
        }
    return result;
}

#endif
