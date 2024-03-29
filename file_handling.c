#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_handling.h"
#include "mem_array.h"
#include "symbol_table.h"
/*acquiring FILE* using FILETYPE extension for writing/reading.
 *function callee must verify that filePtr != NULL*/
FILE* getFile(char* fileName, FILETYPE type)
{
    int nameLen = strlen(fileName);
    char* namePtr = (char*)malloc(nameLen + 5);/*maximum appending size is 5*/
    char* append;
    FILE* filePtr;

    if(namePtr == NULL){
        printf("MEM_ERROR: Could not allocate memory for string\n");
        printf("Terminating program...\n");
        exit(1);
    }

    strcpy(namePtr, fileName);
    /*point to file extension string according to type*/
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
    strcat(namePtr+nameLen, append);/*append file extension*/
    filePtr = fopen(namePtr, type == AS ? "r" : "w");
    free(namePtr);
    return filePtr;
}

/*prints the memory pictures in an appropriate format for .ob file*/
void createOb(FILE* ob, encodedAsm* inst, encodedAsm* data)
{
    int i;
    int d;
    fprintf(ob, "%d %d\n", inst->counter, data->counter);
    for(i = 0; i < inst->counter; ++i)
        fprintf(ob,"%.07d %.06lx\n", STARTADDRESS + i, inst->arr[i]);
    for(d = 0; d < data->counter; ++d)
        fprintf(ob,"%.07d %.06lx\n", STARTADDRESS + i + d, data->arr[d]);
}

/*checks which files should be created and calls the appropriate functions.
 *will print errors and skip a file if the program runs into IO issue.*/
void createOuput(char* fileName, encodedAsm* inst, encodedAsm* data)
{
    FILE* ob;
    if(areExterns()){
        FILE* ext = getFile(fileName, EXT);
        if(ext == NULL){
            printf("ERROR: Could not open file %s.ext for writing. Check your privileges.\n", fileName);
        }
        else{
            createExt(ext);
            if(fclose(ext)){
                printf("Error was encountred trying to close an output file.\n");
            }
        }
    }
    if(areEntries()){
        FILE* ent = getFile(fileName, ENT);
        if(ent == NULL){
            printf("ERROR: Could not open file %s.ent for writing. Check your privileges.\n", fileName);
        }
        else{
            createEnt(ent);
            if (fclose(ent)){
                printf("Error was encountred trying to close an output file.\n");
            }
        }
    }
    ob = getFile(fileName, OB);
    if(ob == NULL){
        printf("ERROR: Could not open file %s.ob for writing. Check your privileges.\n", fileName);
    }
    else{
        createOb(ob, inst, data);
        if(fclose(ob)){
            printf("Error was encountred trying to close an output file.\n");
        }
    }
}
/*Frees memory, closes files and cleans all data between different files*/
void freeMemory(FILE* fp, encodedAsm* inst, encodedAsm* data)
{
    if(fclose(fp)){
        printf("Error was encountred trying to close an input file.\n");
    }
    resetPicture(inst);
    resetPicture(data);
    clearSymbolTable();
    clearExternalList();
}
