#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "linkedlist.h"
#include "first_pass.h"

#define MAX_LINE_LENGTH 80 /*TODO: move to another file*/
#define TRUE 1
#define FALSE 0
/*
read line
skip whitespace */
int firstPass(FILE* fp)
{
    char line[MAX_LINE_LENGTH + 2];
    int lineCounter = 0, dataCounter = 0, instCounter = 0;
    int i = 0, lineLen;
    int index1, index2;
    char errorFlag = FALSE;
    char labelFlag;

    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){
        labelFlag = FALSE;
        lineLen = strlen(line);
        ++lineCounter;

        while(isspace(line[i])) ++i;
        if(line[i] == ';' || line[i] == '\0'){
            continue;
        }

        index1 = i;
        while(!isspace(line[i]) && line[i] != '\0') ++i;/*TODO: Might need to check for '\0' after?*/
        index2 = i-1;

/*isValidLabel:
check for valid name
check if already in list
check if saved keyword
*/

        if(line[index2] == ':'){
            if (isValidLabel(line, index1, index2, lineCounter)){
                labelFlag = TRUE;
                getWord(line, &i, &index1, &index2);
            }
            else{
                errorFlag = TRUE;
            }
        }
        if(line[index1] == '.'){
            switch(isValidAsmOpt(line, index1+1, index2, lineCounter)){
                case ERROR:
                    errorFlag = TRUE;
                    break;
                case DATA:
                    break;
                case STRING:
                    break;
                case ENTRY:
                    break;
                case EXTERN:
                break;

            }
        }
        else
            if(isOp(line, index1, index2)){

            }

    }
}
int isKeyword(char line[],int index1,int index2)
{
}
void getWord(char* line, int* i, int* index1, int* index2)
{
    while(isspace(line[*i])) 
            ++*i;
    *index1 = *i;
    while(!isspace(line[*i]) && line[*i] != '\0') 
        ++*i;
    *index2 = *i-1;
}
/*Validates that word is a valid assembly instruction*/
int isValidAsmOpt(char* line, int index1, int index2, int lineCounter)
{
    char opt[MAXOPTSIZE];
    int wordLen = index2 - index1 + 1;
    if(wordLen > MAXOPTSIZE){ /*max opt size*/
        printf("ERROR: invalid assembler instruction at line: %d\n", lineCounter);
        return ERROR;
    }
    strncpy(opt, line+index1, wordLen);
    if(!strcmp(opt, "data"))
        return DATA;
    if(!strcmp(opt, "string"))
        return STRING;
    if(!strcmp(opt, "entry"))
        return ENTRY;
    if(!strcmp(opt, "extern"))
        return EXTERN;
}
int isValidLabel(char line[],int index1,int index2,int lineCounter)
{
    int j;
    char word[MAXLABELSIZE];
    if(index2 - index1 + 1 > MAXLABELSIZE){
        printf("")
    }
    if(!isalpha(line[index1])){

    }
    for(j = index1; j < index2; j++){
        if(!isalnum(line[j])){

        }
    if(iskeyword(line, index1, index2-1)){}
    if(exists(line, index1, )){}
