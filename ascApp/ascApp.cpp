/*Main program*/

#include "asc.h"
#include "argument.h"


void OpenFile(FILE **videoFile, FILE **reportFile, char *Filename, mfxU32 Width, mfxU32 Height, mfxU32 *NumFrames) {
#if defined(_WIN32) || defined(_WIN64)
	if(fopen_s(reportFile, "pdist.txt", "a")) exit(1);
    fopen_s(videoFile, Filename, "rb");
    *NumFrames = 0;// NMIN(ASCvideoFileOpen(videoFile, Filename, Width, Height), *NumFrames);
#else
	*reportFile = fopen("pdist.txt", "a");
	if(!(*reportFile)) exit(1);
    *videoFile = fopen(Filename, "rb");
	*NumFrames = 0;//need to implement a function to determine number of frames in yuv file
#endif
	
}
void ReadFrameYonly(FILE *videoFile, mfxU8 *dst, mfxU32 Width, mfxU32 Height) {
	fread(dst,1, Width * Height,videoFile);
#if defined(_WIN32) || defined(_WIN64)
    _fseeki64(videoFile,(mfxI64)(Height * Width / 2),SEEK_CUR);
#else
	fseek(videoFile, (Height * Width / 2),SEEK_CUR);//needs better solution for large files (> 2GB)
#endif
}
void printResults(char *filename, FILE *fileOut, mfxU32 frameNum, mfxU32 isShotChange, mfxU32 isLastFrameInShot, mfxU8 tooMuchVariation) {
	int n = (int)strlen(filename);
    if (n > 4)
		n -= 4;
	if (fileOut != NULL) fprintf(fileOut, "%.*s\t%i\t%i\t%i\n", n, filename, frameNum, isShotChange, isLastFrameInShot);
}

void printResults(char *filename, FILE *fileOut, mfxU32 frameNum, mfxU32 isShotChange, mfxU32 isLastFrameInShot, mfxU8 tooMuchVariation, mfxF64 rscs) {
	int n = (int)strlen(filename);
	if (n > 4)
		n -= 4;
	if (fileOut != NULL) fprintf(fileOut, "%.*s\t%i\t%i\t%i\t%.3f\n", n, filename, frameNum, isShotChange, isLastFrameInShot, rscs);
}

mfxI32 __cdecl main(mfxI32 argc, char* argv[]) {
	FILE
		*videoFile = NULL,
		*reportFile = NULL;
	char
		*inputFilename = NULL;
	mfxU32
		Width = 0,
		Height = 0,
		NumFrames = 0,
		interlaceMode = 0,
		pos = 0;
	mfxU8
		*frame = NULL;
	ASC
		SCD;
	mfxF64
		rscs = 0.0;

	if(parseInput(argc, argv, &inputFilename, &Width, &Height, &NumFrames, &interlaceMode)) exit(0);
	SCD.Init(Width, Height, Width, interlaceMode);
	SCD.setInputFileName(inputFilename);
	SCD.SetGoPSize(HEVC_Gop);
    SCD.SetControlLevel(0);
	OpenFile(&videoFile, &reportFile, inputFilename, Width, Height, &NumFrames);
	frame = new mfxU8[Width * Height];
	for (mfxU32 i = 0; i < 500; i++) {
		ReadFrameYonly(videoFile, frame, Width, Height);
		if (SCD.PutFrame(frame)) {
			SCD.Print_Stats(frame, Width, Height, Width);
			//SCD.calc_RaCa_pic(frame, Width, Height, Width, rscs);
			printResults(inputFilename, reportFile, SCD.Get_frame_number(), SCD.Get_frame_shot_Decision(), SCD.Get_frame_last_in_scene(), SCD.Get_frame_variation_status(), rscs);
		}
	    //printf("%i\r", i + 1);
		if (interlaceMode > ASCprogressive_frame) {
			if (SCD.PutFrame(frame)) {
				SCD.Print_Stats(frame, Width, Height, Width);
				printResults(inputFilename, reportFile, SCD.Get_frame_number(), SCD.Get_frame_shot_Decision(), SCD.Get_frame_last_in_scene(), SCD.Get_frame_variation_status());
			}
			//printf("%i\r", i + 1);
		}
	}
	if(SCD.Get_Last_frame_Data())
		printResults(inputFilename, reportFile, SCD.Get_frame_number(), SCD.Get_frame_shot_Decision(), SCD.Get_frame_last_in_scene(), SCD.Get_frame_variation_status(), rscs);
	SCD.Close(videoFile, reportFile);
	delete[] frame;
	return(0);
}