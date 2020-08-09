#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "asm_tables.h"
#include "first_pass.h"


#define TRUE 1
#define FALSE 0
/*TODO: move to some constants header*/
#define ASM_MIN_INT -1048576
#define ASM_MAX_INT  1048575
int firstPass(FILE* fp)
{
    char line[MAX_LINE_LENGTH + 2];
    Token word;/*Struct holding current word and its' length*/
    int lineCounter = 0, dataCounter = 0, instCounter = 0;
    int i = 0;
    int index1, index2;
    char errorFlag = FALSE;
    char labelFlag;

    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){/*Parsing lines*/
        int cmdIndex;/*Index holding place inside CMD array*/
        i = 0; 
        labelFlag = FALSE;
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
                case ENTRY:/*Ignore, only on second pass*/ /*need to validate rest of line? https://opal.openu.ac.il/mod/ouilforum/discuss.php?d=2858172&p=6847092#p6847092*/ 
                    break;
                case EXTERN:/*isValidlabel and insert into table if not.*/
                    break;
                default:
                    break;
            }
        }
        else if((cmdIndex = isOp(word.currentWord)) != -1){
            int lineError = FALSE;
            int start = i;
            int commas = 0;
            int commaIndex = -1;
            while(isspace(line[i]))
                    ++i;
            if(CMD[cmdIndex].numParams == 0 && line[i] != '\0'){/*Paramaters given to 0 param command*/
                    printf("ERROR: command \"%s\" does not take operands ; at line: %d\n", CMD[cmdIndex].cmdName, lineCounter);
                    errorFlag = TRUE;
                    continue;
            }
            while(line[i]){
                if(line[i] == ','){
                    ++commas;
                    commaIndex = i;
                }
                i++;
            }
            if(CMD[cmdIndex].numParams == 1 && commas > 0){
                errorFlag = lineError = TRUE;
                printf("ERROR: command \"%s\" accepts 1 operand, extranous comma ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                lineCounter);
                continue;
            }
            else if(CMD[cmdIndex].numParams == 2 && commas != 1){
                errorFlag = lineError = TRUE;
                printf("ERROR: command \"%s\" accepts 2 operands, extranous or lack of comma ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                lineCounter);
                continue;
            }
            if(!lineError){
                switch(line[start]){
                    case '#':
                        ++start;
                        if(!isNum(line+start)){
                            /*TODO:error solution suggestions?*/
                            printf("ERROR: invalid number ; at line: %d\n", lineCounter);
                            errorFlag = lineError = TRUE;
                            break;
                        }
                        if(!(CMD[cmdIndex].viableOperands & OP1_IMMEDIATE)){
                            printf("ERROR: command \"%s\" does not take number as 1st operand ; at line: %d\n", 
                                    CMD[cmdIndex].cmdName,
                                    lineCounter);
                            errorFlag = lineError = TRUE;
                            break;
                        }
                        break;
                    case 'r':
                        break;
                    default:
                }
            }
        }/*if Op*/
        }
    }/*End while*/
}
/*Checks if a given bounded word is a number*/
/*TODO: do we need to give a specific error for out of range numbers?*/
int isNum(const char* line)
{
    char** numSuffix;
    char c;
    long int num = strtol(line, numSuffix, 10);
    /*strtol manpage: "If there were no digits at all, strtol() stores the 
     *original value of nptr in *endptr (and returns 0)."*/
    if(line == *numSuffix){
        return FALSE;
    }
    if(num < ASM_MIN_INT || num > ASM_MAX_INT){
        return FALSE;
    }
    while(**numSuffix != '\0'){
        if(!isspace(**numSuffix)){
            if(**numSuffix == ',')
                return TRUE;
            else
                return FALSE;
        }
        ++*numSuffix;
    }
    return TRUE;
}
/*Checks if a given bounded word is a register*/
int isReg(const char* line, int start, int end)
{
    return (start != end) && (line[start+1] - '0' < 8) && (start+2 == end);
}
/*Consumes and returns number of commas between two words.
 *increments line index.*/
int consumeComma(const char* line, int* i)
{
    int commas = 0;
    int j;
    for(j = 0; j < 2; ++j){
        while(isspace(line[*i]))
            ++*i;
        if(line[*i] == ','){
            ++commas;
            ++*i;
        }
    }
    return commas;
}
/*bounds operand in line.
 *returns beginning position and increments line index*/
int boundOp(const char* line, int* i)
{
    int startPos;
    while(isspace(*(line+*i)))
        ++*i;
    startPos = *i;
    while(*(line+*i) != '\0' && !isspace(*(line+*i)) && *(line+*i) != ',')
        ++*i;
    return startPos;
}
/*checks if a given word is a saved keyword used by the assembly language.*/
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
void getWord(char* line, int* i, int* index1, int* index2)/*TODO: add a flag variable so we can bound a word by space or by comma?*/
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
    wordLen -= 1;/*excluding ':'*/
    word[wordLen] = '\0';
    if(wordLen > MAXLABELSIZE){
        printf("ERROR: label exceeds maximum length of %d ; at line: %d\n",MAXLABELSIZE, lineCounter);
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
    if(exists(word)){/*TODO: we said that we should seperate exists from the rest of the function*/
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
