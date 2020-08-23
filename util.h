
#ifndef ASM_UTIL
#define ASM_UTIL
#include "constants.h"
/*structure used to hold current word as a null terminated string.
 *used when comparing words in line is required.*/
typedef struct{
    int len;
    char currentWord[MAX_LINE_LENGTH + 2];
} Token;

void getWord(char* line, int* i, int* index1, int* index2);
void storeWord(Token* t, const char* line, int len);
int isOp(char* op);
int isKeyword(char* word);
int isReg(const char* line);
#endif
