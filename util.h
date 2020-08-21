#include "constants.h"

#ifndef ASM_UTIL
#define ASM_UTIL
typedef struct{/*current word length and array to hold.
        *We're doing alot of copying in validation functions where
        *we can just store it once and pass it on to them,
        *they should be able to operate considering they have context*/
    int len;
    char currentWord[MAX_LINE_LENGTH + 2];
} Token;
void getWord(char* line, int* i, int* index1, int* index2);
void storeWord(Token* t, const char* line, int len);
#endif
