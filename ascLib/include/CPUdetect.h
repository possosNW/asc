#ifndef _CPUDETECT_H_
#define _CPUDETECT_H_

#include "ASCstructures.h"
#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
//
// CPU Dispatcher
// 1) global CPU flags are initialized at startup
// 2) each stub configures a function pointer on first call
//

static mfxI32 CpuFeature_SSE41() {
#if defined(_WIN32) || defined(_WIN64)
    mfxI32 info[4], mask = (1 << 19);    // SSE41
    __cpuidex(info, 0x1, 0);
    return ((info[2] & mask) == mask);
#else
    return((__builtin_cpu_supports("sse4.1")));
#endif
}
#if defined(_WIN32) || defined(_WIN64)
static mfxI32 CpuFeature_AVX() {
    mfxI32 info[4], mask = (1 << 27) | (1 << 28);    // OSXSAVE,AVX
    __cpuidex(info, 0x1, 0);
    if ((info[2] & mask) == mask)
        return ((_xgetbv(_XCR_XFEATURE_ENABLED_MASK) & 0x6) == 0x6);
    return 0;
}
#endif

static mfxI32 CpuFeature_AVX2() {
#if defined(_WIN32) || defined(_WIN64)
    mfxI32 info[4], mask = (1 << 5); // AVX2
    if (CpuFeature_AVX()) {
        __cpuidex(info, 0x7, 0);
        return ((info[1] & mask) == mask);
    }
    return 0;
#else
    return((__builtin_cpu_supports("avx2")));
#endif
}

static const int g_SSE4 = CpuFeature_SSE41();
static const int g_AVX2 = CpuFeature_AVX2();



//
// end Dispatcher
//


#endif