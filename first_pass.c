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
            switch(isValidAsmOpt(line, index1, index2, lineCounter)){
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
                default:
                    break;
            }
        }
        /*else
            if(isOp(line, index1, index2)){

            }*/
    }
}
/*checks if a given word is a saved keyword used by the assembly language.
 *uses length of the word to avoid unnecessary strcmp.*/
int isKeyword(char* word)
{
    char* keywords[] = {"mov","cmp","add","sub","lea","clr","not","inc",
                        "dec","jmp","bne","jsr","red","prn","rts","stop",
                        ".data",".entry",".string",".extern",
                        "r0","r1","r2","r3","r4","r5","r6","r7"};
    int i, sizeArr = sizeof(keywords) / sizeof(char*);
    for(i = 0; i < sizeArr; ++i)
        if(!strcmp(keywords[i], word))
            return TRUE;
    return FALSE;
}
/*adjusts indicies to bound a word*/
void getWord(char* line, int* i, int* index1, int* index2)
{
    while(isspace(line[*i])) 
            ++*i;
    *index1 = *i;
    while(!isspace(line[*i]) && line[*i] != '\0') 
        ++*i;
    *index2 = *i-1;
}
/*asserts that word is a valid assembly instruction.
 *Must be one of the following: .data, .string, .entry, .extern*/
int isValidAsmOpt(char* line, int index1, int index2, int lineCounter)
{
    char asmOpt[MAXOPTSIZE];
    strncpy(asmOpt, line+index1, index2-index1+1);

    if(!strcmp(asmOpt, ".data"))
        return DATA;
    if(!strcmp(asmOpt, ".string"))
        return STRING;
    if(!strcmp(asmOpt, ".entry"))
        return ENTRY;
    if(!strcmp(asmOpt, ".extern"))
        return EXTERN;
    printf("ERROR: invalid assembler instruction at line: %d\n", lineCounter);
    return ERROR;
}
/*asserts that a given label is a valid label.
 *a label can have up to 31 characters, begin with a letter 
 *from the range <a-z>/<A-Z> and must be fully alphanumeric*/
int isValidLabel(char line[],int index1,int index2,int lineCounter)
{
    int j;
    char word[MAXLABELSIZE];
    int wordLen = index2 - index1 + 1;
    if(wordLen > MAXLABELSIZE){
        printf("ERROR: label exceeds maximum length of %d ; at line: %d\n",MAXLABELSIZE-1, lineCounter);
        return 0;
    }
    if(!isalpha(line[index1])){
        printf("ERROR: label must start with a letter <a-z> or <A-Z> ; at line: %d\n",lineCounter);
        return 0;
    }
    for(j = index1; j < index2; j++){
        if(!isalnum(line[j])){
            printf("ERROR: label must start with a letter and be fully alphanumeric ; at line: %d\n",lineCounter);
            return 0;
        }
    }

    strncpy(word, line+index1, wordLen-1);
    if(isKeyword(word)){
        printf("ERROR: label cannot be a saved keyword; at line: %d\n",lineCounter);
        return 0;
    }
    /*TODO:Need to decide where we declare our linked list and wheter we pass it on here
     * or make it global. probably the former?*/
    if(exists(word)){
        printf("ERROR: Duplicate label definition ; at line: %d\n",lineCounter);
        return 0;
    }
}
