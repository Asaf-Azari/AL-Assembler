/**
 *Program description TODO 
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "symbol_table.h"
#include "assembler.h"
#include "asm_tables.h"
#include "first_pass.h"



/*acquiring FILE* with FILETYPE extension for writing/reading.*/
FILE* getFile(char* fileName, FILETYPE type)
{
    int nameLen = strlen(fileName);
    char* namePtr = (char*)malloc(nameLen + 5);/*maximum appending size is 5*/
    char* append;
    FILE* filePtr;


    strcpy(namePtr, fileName);
    switch(type)
    {
        case OB:
            append = OB_TYPE;
            break;
        case AS:
            append = AS_TYPE;
            break;
        case EXT:
            append = EXT_TYPE;
            break;
        case ENT:
            append = ENT_TYPE;
            break;
    }
    strcat(namePtr+nameLen, append);
    filePtr = fopen(namePtr, type == AS ? "r" : "w");
    free(namePtr);
    return filePtr;
}
int main(int argc, char** argv)
{
    int i;
    FILE* fp = NULL;
    if(argc == 1){/*no arguments*/
        printf("ERROR: No arguments supplied\n");
        return 0;/*TODO: Return 1?*/
    }
    for(i = 1; i < argc; ++i){/*Main program loop*/
        clearSymbolTable();
        if(!(fp = getFile(argv[i], AS))){/*TODO: fclose after done? */
           printf("ERROR: Cannot open input file: %s.as skipping \n", argv[i]);
        }
        
        /*printf("%s\n", CMD[0].cmdName); testing CMD inclusion*/
        if(firstPass(fp)){
        }
        /*if(!parse2(fp)){
        
        }*/


    }
}
/** LINKED LIST 
 * 1.  pointer to next item
 * 2.  label
 * 3.  data/instruction
 * 4.  extern/not extern
 * 5.  address**/

/** instruction counter, starts at 100, Data counter **/


/** commands */
