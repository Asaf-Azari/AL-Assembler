#include "mem_array.h"
#include <stdlib.h>
typedef enum
{
    AS,
    OB,
    ENT,
    EXT
} FILETYPE;
#define AS_TYPE ".as"
#define OB_TYPE ".ob"
#define EXT_TYPE ".ext"
#define ENT_TYPE ".ent"

FILE* getFile(char* fileName, FILETYPE type);
void createOuput(char* fileName, encodedAsm* inst, encodedAsm* data);
void freeMemory(FILE* fp, encodedAsm* inst, encodedAsm* data);