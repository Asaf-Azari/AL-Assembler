#ifndef ASM_FILE_HANDLE
#define ASM_FILE_HANDLE
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

/*acquiring FILE* using FILETYPE extension for writing/reading.
 *function callee must verify that filePtr != NULL*/
FILE* getFile(char* fileName, FILETYPE type);

/*checks which files should be created and calls the appropriate functions.
 *will print errors and skip a file if the program runs into IO issue.*/
void createOuput(char* fileName, encodedAsm* inst, encodedAsm* data);

/*Frees memory, closes files and cleans all data between different files*/
void freeMemory(FILE* fp, encodedAsm* inst, encodedAsm* data);
#endif
