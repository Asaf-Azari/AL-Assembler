#include <stdio.h>
#define MAX_LINE_LENGTH 80 /*TODO: move to another file*/
#define MAXLABELSIZE 31
#define MAXOPTSIZE 7
typedef struct{/*current word length and array to hold.
        *We're doing alot of copying in validation functions where
        *we can just store it once and pass it on to them,
        *they should be able to operate considering they have context*/
    int len;
    char currentWord[MAX_LINE_LENGTH + 2];
} Token;
enum{
    ERROR,
    DATA,
    STRING,
    ENTRY,
    EXTERN
} asmInst;

int isKeyword(char* word);
int firstPass(FILE* fp);
void getWord(char* line, int* i, int* index1, int* index2);
void storeWord(Token* t, char* line, int len);
int isValidLabel(char* word, int wordLen, int lineCounter);
int isValidAsmOpt(char* asmOpt, int lineCounter);
int isOp(char* op);
