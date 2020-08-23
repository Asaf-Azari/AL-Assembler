#include "util.h"
/*Struct used in secondPass to hold the addressing type
 *of a given operand and its value*/
typedef struct{
    union{
        long num;
        int reg;
        char* label;
    } type;
    int addressing;
} Operand;

/*functions decleration*/
int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst);
Operand parseOperand(Token* t);
