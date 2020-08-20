#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_handling.h"
#include "mem_array.h"
#include "symbol_table.h"
/*acquiring FILE* with FILETYPE extension for writing/reading.*/
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

    /*strcpy(namePtr, fileName);*/
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
    /*strcat(namePtr+nameLen, append);*/
    /*TODO: use sprintf instead?*/
    sprintf(namePtr, "%s%s", fileName, append);
    filePtr = fopen(namePtr, type == AS ? "r" : "w");
    free(namePtr);
    return filePtr;
}

/*TODO: go over all the files to correct style*/
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
void createOuput(char* fileName, encodedAsm* inst, encodedAsm* data)
{
    FILE* ob;
    if(!noExtern()){
        FILE* ext = getFile(fileName, EXT);
        if(ext == NULL){
            printf("ERROR: Could not open file %s.ext for writing. Check your privileges.\n", fileName);
        }
        createExt(ext);
        if(fclose(ext)){
            printf("Error was encountred trying to close an output file.\n");
        }
    }
    if(noEntry()){
        FILE* ent = getFile(fileName, ENT);
        if(ent == NULL){
            printf("ERROR: Could not open file %s.ent for writing. Check your privileges.\n", fileName);
        }
        createEnt(ent);
        if(fclose(ent)){
            printf("Error was encountred trying to close an output file.\n");
        }
    }
    ob = getFile(fileName, OB);
    if(ob == NULL){
        printf("ERROR: Could not open file %s.ob for writing. Check your privileges.\n", fileName);
    }
    createOb(ob, inst, data);
    if(fclose(ob)){
        printf("Error was encountred trying to close an output file.\n");
    }
}
void freeMemory(FILE* fp, encodedAsm* inst, encodedAsm* data)
{
    if(!fclose(fp)){
        printf("Error was encountred trying to close an input file.\n");
    }
    resetPicture(inst);
    resetPicture(data);
    clearSymbolTable();
    clearExternalList();
}