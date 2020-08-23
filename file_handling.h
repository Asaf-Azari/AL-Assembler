#include "mem_array.h"
#include <stdlib.h>
typedef enum/*enum denoting file types for reading/writing*/
{
    AS,
    OB,
    ENT,
    EXT
} FILETYPE;
/*string macros to append extensions*/
#define AS_TYPE ".as"
#define OB_TYPE ".ob"
#define EXT_TYPE ".ext"
#define ENT_TYPE ".ent"

FILE* getFile(char* fileName, FILETYPE type);
void createOuput(char* fileName, encodedAsm* inst, encodedAsm* data);
void freeMemory(FILE* fp, encodedAsm* inst, encodedAsm* data);
