#include "util.h"
typedef struct{
    union{
        long num;
        int reg;
        char* label;
    } type;
    int addressing;
} Operand;

int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst);
Operand parseOperand(Token* t);
