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
/*Second pass of our assembler.
 *Second pass attempts to create the memory picture.
 *It will encode line by line into the right memory picture (data/instruction)
 *and will handle marking labels as entry and log external references for both the ext and ent files.
 *Will raise errors if encountered an unknown label, if relative addresing is
 *called upon an external label or if there was an attempt to declare an unkown label as entry
 *returns false if any errors were met and true otherwise.
 */
int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst);
/*reads the operand given by Token and returns its addressing type and value*/
Operand parseOperand(Token* t);
