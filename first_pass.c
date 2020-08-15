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
int firstPass(FILE* fp, int* dataCounter, int* instCounter)
{
    char line[MAX_LINE_LENGTH + 2];
    Token word;/*Struct holding current word and its' length*/
    int lineCounter = 0;
    int i = 0, lineLen;
    int index1, index2;
    char errorFlag = FALSE;
    char labelFlag;
    char labelTemp[MAXLABELSIZE+1]; /*for adding the label to the symbol table later TODO: maybe use a token?*/
    
    *dataCounter = 0;
    *instCounter = 0;
    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){/*Parsing lines*/
        int cmdIndex;/*Index holding place inside CMD array*/
        i = 0; 
        labelFlag = FALSE;
        ++lineCounter;
        lineLen = strlen(line);

        while(isspace(line[i])) ++i; /*skipping leading whitespaces*/
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        getWord(line, &i, &index1, &index2);
        storeWord(&word, &line[index1], index2-index1+1);

        if(line[index2] == ':'){
            word.len -= 1;/*excluding ':'*/
            word.currentWord[word.len] = '\0';

            /*TODO: need to decide what to do with .extern+regular decleration*/
            if (isValidLabel(word.currentWord, word.len, lineCounter)){
                labelFlag = TRUE;
                strncpy(labelTemp, word.currentWord, word.len);
                labelTemp[word.len] = '\0';

                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);
            }
            else{
                errorFlag = TRUE;
                continue; 
            }
        }
        if(line[index1] == '.'){
            int dataArgs = 0, commaCount = 0;
            char* numSuffix;
            switch(isValidAsmOpt(word.currentWord, lineCounter)){

                case ERROR:
                    errorFlag = TRUE;
                    continue;
                    break;

                case DATA:/*validate and if labelFlag, add to table*/
                    while(line[i]!='\0'){
                        if(isspace(line[i])){
                            i++;
                            continue;
                        }
                        if(isNum(&line[i], &numSuffix)){
                            i = numSuffix - &line[0]; /*TODO: Does this converts the memory address of the pointer to an array index?🤔🤔🤔*/
                            dataArgs++;
                            if(line[i] == ','){
                                i++;
                                commaCount++;
                            }                  
                        }
                        else{
                            printf("ERROR: Invalid number; at line: %d\n", lineCounter);
                            errorFlag = TRUE;
                            break;
                        }
                    }
                    if(line[i] != '\0') /* if loop exited early, erros found*/
                        continue;
                    if(dataArgs != commaCount+1){ /*sanity check that args are 1 more than commas*/
                        printf("ERROR: missing number; at line: %d\n", lineCounter);
                        errorFlag = TRUE;
                        continue;
                    }
                    if(labelFlag){
                        if(!exists(labelTemp))
                            addLabel(labelTemp, TRUE, FALSE, *dataCounter);
                        else{
                            printf("ERROR: Duplicate label; at line: %d\n" ,lineCounter);
                            errorFlag = TRUE;
                            continue; 
                        }
                    }
                    *dataCounter += dataArgs;
                    break;

                case STRING:/*validate and if labelFlag, add to table*/
                    /*index2 is pointing at 'g' of ".string", index2+1 is the correct one*/
                    index1 = index2+1;
                    index2 = lineLen-1;
                    while(isspace(line[index1]))
                        index1++;
                    while(isspace(line[index2]))
                        index2--;
                    if(line[index1]!='"'||line[index2]!='"' || index1 == index2){
                        printf("ERROR: invalid string ; at line: %d\n",lineCounter);
                        errorFlag = TRUE;
                        continue;
                    }
                    if(labelFlag){
                        if(!exists(labelTemp))
                            addLabel(labelTemp, TRUE, FALSE, *dataCounter);
                        else{
                            printf("ERROR: Duplicate label; at line: %d\n" ,lineCounter);
                            errorFlag = TRUE;
                            continue; 
                        }
                    }
                    *dataCounter += index1+index2;/*with terminatil null character*/
                    break;
                case ENTRY:/*Ignore, only on second pass*/ /*need to validate rest of line? https://opal.openu.ac.il/mod/ouilforum/discuss.php?d=2858172&p=6847092#p6847092*/ 
                    break;
                case EXTERN:/*isValidlabel and insert into table if not.*/
                    getWord(line, &i, &index1, &index2);
                    storeWord(&word, &line[index1], index2-index1+1);
                    if (isValidLabel(word.currentWord, word.len, lineCounter)){
                        getWord(line, &i, &index1, &index2);
                        if(line[index1]=='\0'){/*No text after label*/
                            if(!exists(word.currentWord))
                                addLabel(word.currentWord, FALSE, TRUE, 0);
                        }
                        else
                        {
                            printf("ERROR: extranous text after label; at line: %d\n" ,lineCounter);
                            errorFlag = TRUE;
                            continue;
                        }
                    }
                    else{
                        errorFlag = TRUE;
                        continue;
                    }
                    break;
                default:
                    break;
            }
        }
        else if((cmdIndex = isOp(word.currentWord)) != -1){
            int lineError = FALSE;
            int start = i;/*index pointing to first operand*/
            int wordIdx;
            int commas = 0;/*Counting commas*/
            int commaIndex = -1;/*index pointing to comma*/
            int params = CMD[cmdIndex].numParams;/*number of operands for the command*/
            if(labelFlag){
                if(exists(labelTemp)){
                    printf("ERROR: label \"%s\" already declared in file ; at line: %d\n",
                            labelTemp,
                            lineCounter);
                    errorFlag = TRUE;
                    continue;
                }
                addLabel(labelTemp, FALSE, FALSE, *instCounter);
            }
            ++*instCounter;/*Count operand*/

            while(isspace(line[i]))
                    ++i;
            if(params == 0 && line[i] != '\0'){/*operands given to 0 operand command*/
                    printf("ERROR: command \"%s\" does not take operands ; at line: %d\n", CMD[cmdIndex].cmdName, lineCounter);
                    errorFlag = TRUE;
                    continue;
            }
            else if(params != 0 && line[i] == '\0'){/*No operands given*/
                    printf("ERROR: command \"%s\" takes %d operands ; at line: %d\n", 
                            CMD[cmdIndex].cmdName,
                            params,
                            lineCounter);
                    errorFlag = TRUE;
                    
                    continue;
            }
            while(line[i]){
                if(line[i] == ','){/*Counting commas in line*/
                    ++commas;
                    commaIndex = i;
                }
                i++;
            }
            if(params == 1 && commas > 0){
                errorFlag = lineError = TRUE;
                printf("ERROR: command \"%s\" accepts 1 operand, extranous comma ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                lineCounter);

                continue;
            }
            else if(params == 2 && commas != 1){
                errorFlag = lineError = TRUE;
                printf("ERROR: command \"%s\" accepts 2 operands, %s ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                (commas > 1) ? "extranous comma" : "missing comma",
                lineCounter);

                continue;
            }
            /*TODO: lineError is currently use to break out of the loop
             *since break within the switch case isn't breaking the loop.
             *maybe we should just switch to if else if statements?*/
            while(params && !lineError){

                wordIdx = (params == MAXPARAM) ? commaIndex+1 : start;/*According to number of params, let index point to word*/
                while(isspace(line[wordIdx]))
                    ++wordIdx;
                if(!singleToken(&line[wordIdx])){/*If there's more than one token/no token*/
                    printf("ERROR: invalid operand ; at line: %d\n", lineCounter);
                    errorFlag = lineError = TRUE;
                    break;
                }

                switch(line[wordIdx]){
                    case '#':/*Immediate number*/
                        /*If our command does not take a number as it's 1st/2nd operand*/
                        if(!(CMD[cmdIndex].viableOperands & (params == 1 ? OP1_IMMEDIATE : OP2_IMMEDIATE) )){
                            printf("ERROR: command \"%s\" does not accept number as %s operand ; at line: %d\n", 
                                    CMD[cmdIndex].cmdName,
                                    (params == 1) ? "1st" : "2nd",
                                    lineCounter);
                            errorFlag = lineError = TRUE;
                            break;
                        }

                        ++wordIdx;/*Increment to start of number*/
                        if(!isNum(&line[wordIdx], NULL)){
                            /*TODO:error solution suggestions?*/
                            printf("ERROR: invalid number ; at line: %d\n", lineCounter);
                            errorFlag = lineError = TRUE;
                            break;
                        }
                        ++*instCounter;/*Count number*/
                        break;
                    case 'r':/*Register*/
                        ++wordIdx;/*Increment to register number*/
                        /*If our command doesn't take a register as it's 1st/2nd operand*/
                        if(isReg(&line[wordIdx]) && (CMD[cmdIndex].viableOperands & (params == 1 ? OP1_REG : OP2_REG))){
                            printf("ERROR: command \"%s\" does not accept register as %s operand ; at line: %d\n",
                                    CMD[cmdIndex].cmdName,
                                    (params == 1) ? "1st" : "2nd",
                                    lineCounter);
                            errorFlag = lineError = TRUE;
                        }
                        break;
                    case '&':
                        if(!(CMD[cmdIndex].viableOperands & OP1_RELATIVE)){
                            printf("ERROR: command \"%s\" does not support relative addressing ; at line: %d\n",
                            CMD[cmdIndex].cmdName,
                            lineCounter);
                            errorFlag = lineError = TRUE;
                        }
                        ++*instCounter;/*Count label*/
                        break;
                    case '\0':/*Missing operand/s*/
                        printf("ERROR: command \"%s\" is missing operand\\s ; at line: %d\n",
                                    CMD[cmdIndex].cmdName,
                                    lineCounter);
                        errorFlag = lineError = TRUE;
                        break;
                    default:
                        ++*instCounter;/*Count label*/
                        break;
                }

                --params;/*Decrement params to get to previous operand*/
            }/*While end*/
        }/*else if Op*/
    }/*While fgets*/
}
int singleToken(const char* line)
{
    char tokenFlag = FALSE;
    int i = 0;
    /*TODO: currently it checks for both two words not seperated
     *by a comma AND if we're missing an operand.
     *do we want this sort of behaviour? tsk tsk.🤔*//*what the shit is that*/
    while(line[i] && line[i] != ','){
        if(!isspace(line[i])){
            if(tokenFlag)
                return FALSE;
            else{
                tokenFlag = TRUE;
                while(line[i] && !isspace(line[i]) && line[i] != ',')
                    ++i;
                continue;
            }
        }
        ++i;
    }
    return tokenFlag;
}
/*int isNum(const char* line)
{
    char** numSuffix;
    long int num = strtol(line, numSuffix, 10);
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
}*/
/*Checks if a given bounded word is a register*/
int isReg(const char* line)
{
    return ((*line - '0') < 8) && (isspace(*line+1) || (*line+1 == ','));
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
    *index2 = (line[*i] == '\0')? *i :*i-1; /*when no word have been found*/
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
int isValidLabel(char* word, int wordLen, int lineCounter)
{
    int i;
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

/*is num but recieves the suffix pointer*/
int isNum(const char* line, char** numSuffix)
{
    char* localSuffix;
    long int num = strtol(line, &localSuffix, 10);
    if(numSuffix == NULL){
        numSuffix = &localSuffix;
    }
    else{
        *numSuffix = localSuffix;
    }
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
