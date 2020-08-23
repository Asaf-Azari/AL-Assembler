#ifndef ASM_UTIL
#define ASM_UTIL
#include "constants.h"
/*structure used to hold current word as a null terminated string.
 *used when comparing words in line is required.*/
typedef struct{
    int len;
    char currentWord[MAX_LINE_LENGTH + 2];
} Token;
/*Sets given indicies to the bounds of a word starting at i*/
void getWord(char* line, int* i, int* index1, int* index2);
/*stores a substring given by index and length in a Token datatype*/
void storeWord(Token* t, const char* line, int len);
/*returns command index from the constant command table and -1 if it doesn't exists*/
int isOp(char* op);
/*checks if a given word is a saved keyword used by the assembly language.*/
int isKeyword(char* word);
/*Checks if a given bounded word is a register*/
int isReg(const char* line);
#endif
