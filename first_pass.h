#include <stdio.h>
#include "asm_tables.h"

enum{/*denoting which assembly instruction was found*/
    ERROR,
    DATA,
    STRING,
    ENTRY,
    EXTERN
} asmInst;

int firstPass(FILE* fp, int* dataCounter, int* instCounter);
int isValidLabel(char* word, int wordLen, int lineCounter);
int isValidAsmOpt(char* asmOpt, int lineCounter);
int isNum(const char* line, char** numSuffix, char isData, int lineCounter);
int singleToken(const char* line, int params, int lineCounter);
int verifyOperand(const char* line, const COMMANDS* cmd, int params, int* instCounter, int lineCounter);
int validateString(const char* line, int start, int end, int lineCounter);
int validateData(const char* line, int i, int lineCounter);
int validateOpNum(const char* line, int* i, int cmdIndex, int lineCounter);
int validateCommas(const char* line, int i, int cmdIndex, int lineCounter);
