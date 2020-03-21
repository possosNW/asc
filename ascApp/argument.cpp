#include "argument.h"
/**
 * Usage / Help Text
 */
const char *help_text[] =
{
"",
" ascApp - console app",
" Usage : ascApp.exe -i file.yuv -w 352 -h 288 -n 10 -m 1",
"           : Typical File Parameters",
" -i   FILE : Input YUV File (Required)",
" -w   NUM  : Width [Default 352]",
" -h   NUM  : Height [Default 288]",
" -n   NUM  : Number of frames to process [Default all]",
"             if 0 then all frames in yuv file are processed",
" -m   NUM  : Interlace Mode [0 Invalid (Do not use), 1 Progressive, 2 TFF, 3 BFF]",
"",
"",
0
};

static char error_argument_missing[]    =    "ERROR : Argument missing";
static char error_input_missing[]       =    "ERROR : input filename required";
static char error_input_missing2[]      =    "ERROR : input pdist filename required";
static char error_bad_integer[]         =    "ERROR : bad integer value";
static char error_input_invalid[]       =    "ERROR : pdist cannot be 0, use 1 for no B frames";

bool print_help(void)
{
    int i = 0;
    while(help_text[i])
        printf("%s\n", help_text[i++]);
    return 0;
}

static bool parse_unsigned_long(char *str, mfxU32 *val, char *err) {
    char *end_ptr;
    *val = 0;
    if(!str) {
        err =  error_argument_missing;
        return 1;
    }
    *val = strtol(str, &end_ptr, 10);
    if (*end_ptr) {
        err = error_bad_integer;
        return 1;
    }
    return 0;
}

static bool parse_signed_long(char *str, mfxI32 *val, char *err) {
    char *end_ptr;
    *val = 0;
    if(!str) {
        err = error_argument_missing;
        return 1;
    }
    *val = strtol(str, &end_ptr, 10);
    if (*end_ptr) {
        err = error_bad_integer;
        return 1;
    }
    return 0;
}

bool parseInput(mfxI32 argc, char *argv[], char** inputFile, mfxU32 *Width, mfxU32 *Height, mfxU32 *NumFrames, mfxU32 *interlaceMode)
{
    bool
		failed = false;
	mfxU32
		use_of_gpu = 0;
    char
		*error_string = NULL;
    if(argc <= 1)    {
        /* No Parameters Passed */
        print_help();
        failed     =    true;
        return failed;
    }
    for(int i = 1; i < argc && !failed; i++) {
        if(argv[i][0] == '-'){
            /* help */
            if(strcmp(&argv[i][1],"help") == 0) {
                print_help();
                failed = true;
                return failed;
            }            
            /* switches */
            switch(argv[i][1]) {
            case 'i':             
                *inputFile = argv[++i];
                break;
            case 'w':
                failed = parse_unsigned_long(argv[++i], Width, error_string);
                break;
            case 'h':
                failed = parse_unsigned_long(argv[++i], Height, error_string);
                break;
            case 'n':             
                failed = parse_unsigned_long(argv[++i], NumFrames, error_string);
				if(*NumFrames == 0) *NumFrames = INT32_MAX;
                break;
			case 'm':             
                failed = parse_unsigned_long(argv[++i], interlaceMode, error_string);
				if(*interlaceMode > 3 || *interlaceMode < 1){
					printf("Interlace mode value is invalid, please check your configuration.\n");
					print_help();
					failed = true;
					return failed;
				}
                break;
            default:
                print_help();
                break;
            }
        } 
        else {
            /* No Parameters Passed */
            print_help();
            failed = true;
            return failed;
        }
    }
    /* logic */    
    if(!inputFile) {
        failed = true;
        error_string = error_input_missing;
    }
	 if(failed) {
        if(error_string!= NULL) printf("%s", error_string);
            print_help();
    }
    return failed;
}

