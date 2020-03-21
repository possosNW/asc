#ifndef _ARGUMENT_H_
#define _ARGUMENT_H_

#include "asc.h"
#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/types.h>
#include <unistd.h>
#include <limits>
#endif

bool print_help(void);                 /*Print the Usage / Help text*/
bool parseInput(mfxI32 argc, char *argv[], char** inputFile, mfxU32 *Width, mfxU32 *Height, mfxU32 *NumFrames, mfxU32 *interlaceMode); /*parse command line argments*/
void ReadFrameYonly(FILE *videoFile, mfxU8 *dst, mfxU32 Width, mfxU32 Height);

#endif //_ARGUMENT_H_