#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "symbol_table.h"
#include "first_pass.h"
#include "asm_tables.h"


#define TRUE 1
#define FALSE 0
int firstPass(FILE* fp)
{
    char line[MAX_LINE_LENGTH + 2];
    Token word;
    int lineCounter = 0, dataCounter = 0, instCounter = 0;
    int i = 0, lineLen;
    int index1, index2;
    char errorFlag = FALSE;
    char labelFlag;

    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){
        int cmdIndex;
        i = 0;
        labelFlag = FALSE;
        lineLen = strlen(line);
        ++lineCounter;

        while(isspace(line[i])) ++i; /*skipping leading whitespaces*/
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        getWord(line, &i, &index1, &index2);
        storeWord(&word, line+index1, index2-index1+1);

        if(line[index2] == ':'){
            if (isValidLabel(word.currentWord, word.len, lineCounter)){
                labelFlag = TRUE;
                getWord(line, &i, &index1, &index2);
                storeWord(&word, line+index1, index2-index1+1);
            }
            else{
                errorFlag = TRUE;
                continue; 
            }
        }
        if(line[index1] == '.'){
            switch(isValidAsmOpt(word.currentWord, lineCounter)){
                case ERROR:
                    errorFlag = TRUE;
                    continue;
                    break;
                case DATA:/*validate and if labelFlag, add to table*/
                    break;
                case STRING:/*validate and if labelFlag, add to table*/
                    break;
                case ENTRY:/*Ignore, only on second pass*/
                    break;
                case EXTERN:/*isValidlabel and insert into table if not.*/
                    break;
                default:
                    break;
            }
        }
        else
            if((cmdIndex = isOp(word.currentWord)) != -1){

            }
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
void storeWord(Token* t, char* line, int len)
{
    t->len = len;
    strncpy(t->currentWord, line, len);
    t->currentWord[len] = '\0';/*Apparently strncpy doesn't append a null byte*/
}
/*asserts that word is a valid assembly instruction.
 *Must be one of the following: .data, .string, .entry, .extern*/
int isValidAsmOpt(char* asmOpt, int lineCounter)
{
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
/*TODO: need to check if existing label declared as external was previously
 *declared as external*/
int isValidLabel(char* word, int wordLen, int lineCounter)
{
    int i;
    wordLen -= 1;/*excluding ':', TODO: change MAXLABELSIZE? I think we should have at most 31 characters excluding ':'?*/
    word[wordLen] = '\0';
    if(wordLen > MAXLABELSIZE){
        printf("ERROR: label exceeds maximum length of %d ; at line: %d\n",MAXLABELSIZE-1, lineCounter);
        return 0;
    }
    if(!isalpha(word[0])){
        printf("ERROR: label must start with a letter <a-z> or <A-Z> ; at line: %d\n",lineCounter);
        return 0;
    }
    for(i = 0; i < wordLen; i++){
        if(!isalnum(word[i])){
            printf("ERROR: label must be fully alphanumeric and start with a letter ; at line: %d\n",lineCounter);
            return 0;
        }
    }
    if(isKeyword(word)){
        printf("ERROR: label cannot be a saved keyword ; at line: %d\n",lineCounter);
        return 0;
    }
    if(exists(word)){
        printf("ERROR: Duplicate label definition ; at line: %d\n",lineCounter);
        return 0;
    }
    return 1; /*Valid label*/
}
int isOp(char* op)
{   
    int i;
    for(i = 0; CMD[i].cmdName; i++){
        if (!strcmp(op,CMD[i].cmdName))
            return i;  
    }
    return -1;
}
