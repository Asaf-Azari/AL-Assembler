#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "symbol_table.h"
#include "asm_tables.h"
#include "first_pass.h"
#include "constants.h"

/*TODO: Do we need to check for dataCounter+instCounter > MAXADDRESS ? or do we do this in encoding the final file?*/
int firstPass(FILE* fp, int* dataCounter, int* instCounter)
{
    char line[MAX_LINE_LENGTH + 2];
    Token word;/*Struct holding current word and its' length*/
    int lineCounter = 0;
    int i = 0, lineLen;
    /*TODO:Still think we need to have a cleaner way to describe going over the line.
     * maybe bundling index1 with index2 and call it something like WordBound(similar to Token
     * but just for bounding. maybe even have a Token and TokenCopy*/
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

        if(line[lineLen-1] != '\n' && !feof(fp)){/*line longer than 80 characters*/
            char c;
            printf("ERROR: Line exceeds maximum length of %d characters; at line: %d\n" ,MAX_LINE_LENGTH,lineCounter);
            errorFlag = TRUE;
            while((c = fgetc(fp)) != '\n' && c != EOF);
            continue;
        }

        while(isspace(line[i])) ++i; /*skipping leading whitespaces*/
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        getWord(line, &i, &index1, &index2);
        storeWord(&word, &line[index1], index2-index1+1);

        if(line[index2] == ':'){/*Label*/
            word.len -= 1;/*excluding ':'*/
            word.currentWord[word.len] = '\0';

            /*TODO: need to decide what to do with .extern+regular decleration*/
            if (isValidLabel(word.currentWord, word.len, lineCounter)){
                labelFlag = TRUE;
                strncpy(labelTemp, word.currentWord, word.len);
                labelTemp[word.len] = '\0';

                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);
                if(line[index1] == '\0'){
                    printf("ERROR: empty label definition ; at line: %d\n", lineCounter);                  
                    errorFlag = TRUE;
                    continue;
                }
            }
            else{
                errorFlag = TRUE;
                continue; 
            }
        }

        if(line[index1] == '.'){

            int asmOpt = isValidAsmOpt(word.currentWord, lineCounter);
            if(asmOpt == ERROR){
                errorFlag = TRUE;
                continue;
            }

            else if(asmOpt == DATA){
                int dataArgs = validateData(line, i, lineCounter);
                if(dataArgs == -1){
                    errorFlag = TRUE;
                    continue;
                }
                if(labelFlag){/*TODO: Don't we have a problem where we don't add labels into the table and then we might miss a double
                                decleration error?*/
                    if(!exists(labelTemp))
                        addLabel(labelTemp, TRUE, FALSE, *dataCounter);
                    else{
                        printf("ERROR: Duplicate label; at line: %d\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue; 
                    }
                }
                *dataCounter += dataArgs;
            }

            else if(asmOpt == STRING){
                int strLen = validateString(line, index2+1, lineLen-1, lineCounter);
                if(strLen == -1){
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
                *dataCounter += strLen;
            }

            else if(asmOpt == ENTRY){
            }

            else if(asmOpt == EXTERN){/*TODO: put into validation function*/
                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);
                if (isValidLabel(word.currentWord, word.len, lineCounter)){
                    getWord(line, &i, &index1, &index2);
                    if(line[index1]=='\0'){/*No text after label*/
                        if(!exists(word.currentWord))/*TODO: do we need to add an error if not extern??*/
                            addLabel(word.currentWord, FALSE, TRUE, 0);
                    }
                    else{
                        printf("ERROR: extranous text after label; at line: %d\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue;
                    }
                }
                else{
                    errorFlag = TRUE;
                    continue;
                }
            }
        }

        else if((cmdIndex = isOp(word.currentWord)) != -1){/*Operator*/
            int start = i;/*index pointing to first operand*/
            int wordIdx;
            int commas = 0;/*Counting commas*/
            int commaIndex = -1;/*index pointing to comma*/
            int params = CMD[cmdIndex].numParams;/*number of operands for the command*/
            if(labelFlag){
                if(exists(labelTemp)){/*TODO: Do you think we should change the other ones to this format? it avoids the else*/
                    printf("ERROR: Duplicate label; at line: %d\n", lineCounter);
                    errorFlag = TRUE;
                    continue;
                }
                addLabel(labelTemp, FALSE, FALSE, *instCounter);
            }
            ++*instCounter;/*Count operand*/
            /*TODO: make validation into a function?*/
            while(isspace(line[i]))
                    ++i;
            if(params == 0 && line[i] != '\0'){/*operands given to 0 operand command*/
                    printf("ERROR: command \"%s\" does not accepts operands ; at line: %d\n", CMD[cmdIndex].cmdName, lineCounter);
                    errorFlag = TRUE;
                    continue;
            }
            else if(params != 0 && line[i] == '\0'){/*No operands given*/
                    printf("ERROR: command \"%s\" accepts %d operand%s, none given ; at line: %d\n", 
                            CMD[cmdIndex].cmdName,
                            params,
                            (params > 1) ? "s" : "",
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
                errorFlag = TRUE;
                printf("ERROR: command \"%s\" accepts 1 operand, extranous comma ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                lineCounter);

                continue;
            }
            else if(params == 2 && commas != 1){
                errorFlag = TRUE;
                printf("ERROR: command \"%s\" accepts 2 operands, %s ; at line: %d\n",
                CMD[cmdIndex].cmdName,
                (commas > 1) ? "extranous comma" : "missing comma",
                lineCounter);

                continue;
            }
            while(params){
                wordIdx = (params == MAXPARAM) ? commaIndex+1 : start;/*According to number of params, let index point to word*/
                while(isspace(line[wordIdx]))
                    ++wordIdx;
                if(!singleToken(&line[wordIdx], params, lineCounter)){/*If there's more than one token/no token*/
                    errorFlag = TRUE;
                    break;
                }

                if(!verifyOperand(&line[wordIdx], &CMD[cmdIndex], params, instCounter, lineCounter)){
                    errorFlag = TRUE;
                    break;
                }

                --params;/*Decrement params to get to previous operand*/
            }/*While end*/
        }/*else if Op*/
        else{
            printf("ERROR: unrecognized instruction \"%s\" ; at line: %d\n", word.currentWord, lineCounter);
            errorFlag = TRUE;
            continue;
        }
    }/*While fgets*/
    return !errorFlag;
}
int verifyOperand(const char* line, const COMMANDS* cmd, int params, int* instCounter, int lineCounter)
{
    Token label;/*Hold label for validation*/

    /*Immediate number*/
    if(line[0] == '#'){
        if(!(cmd->viableOperands & (params == 1 ? OP1_IMMEDIATE : OP2_IMMEDIATE))){
            printf("ERROR: command \"%s\" does not accept number as %s operand ; at line: %d\n", 
                    cmd->cmdName,
                    (params == 1) ? "1st" : "2nd",
                    lineCounter);
            return FALSE;
        }
        if(!isNum(&line[1], NULL, FALSE)){
            printf("ERROR: invalid number ; at line: %d\n", lineCounter);
            return FALSE;
        }
        ++*instCounter;
    }

    /*Register*/
    else if(isReg(line)){
        if(!(cmd->viableOperands & (params == 1 ? OP1_REG : OP2_REG))){
            printf("ERROR: command \"%s\" does not accept register as %s operand ; at line: %d\n",
                    cmd->cmdName,
                    (params == 1) ? "1st" : "2nd",
                    lineCounter);
            return FALSE;
        }
    }

    /*Relative*/
    else if(line[0] == '&'){
        int i = 0;
        if(!(cmd->viableOperands & ((params == 1) ? OP1_RELATIVE : OP2_RELATIVE))){
            printf("ERROR: command \"%s\" does not support relative addressing at %s operand ; at line: %d\n",
            cmd->cmdName,
            (params == 1) ? "1st" : "2nd",
            lineCounter);
        }
        while(!isspace(line[i]) && line[i] != ',')
            ++i;
        /*if(line[i] == '\0'){ TODO: decide if we want to have this
            printf("ERROR: no label after '&' ; at line: %d\n", lineCounter);
            return FALSE;
        }*/
        storeWord(&label, line+1, i-1);
        if(!isValidLabel(label.currentWord, label.len, lineCounter)){
            return FALSE;
        }
        ++*instCounter;/*Count label*/
    }

    /*Label*/
    else{
        int i = 0;
        while(!isspace(line[i]) && line[i] != ',')
            ++i;
        storeWord(&label, line, i);
        if(!isValidLabel(label.currentWord, label.len, lineCounter)){
            return FALSE;
        }
        ++*instCounter;/*Count label*/
    }

    return TRUE;
}
int singleToken(const char* line, int params, int lineCounter)
{
    char tokenFlag = FALSE;
    int i = 0;
    /*TODO: currently it checks for both two words not seperated
     *by a comma AND if we're missing an operand.
     *do we want this sort of behaviour? tsk tsk.🤔*//*what the shit is that*/
    while(line[i] && line[i] != ','){
        if(!isspace(line[i])){
            if(tokenFlag){
                printf("ERROR: extranous operand ; at line: %d\n", lineCounter);
                return FALSE;
            }
            else{
                tokenFlag = TRUE;
                while(line[i] && !isspace(line[i]) && line[i] != ',')
                    ++i;
                continue;
            }
        }
        ++i;
    }
    if(!tokenFlag){
        printf("ERROR: missing %s operand ; at line: %d\n", 
                params == 1 ? "1st" : "2nd",
                lineCounter);
    }
    return tokenFlag;
}
/*Checks if a given bounded word is a register*/
int isReg(const char* line)
{
    return (line[0] == 'r') && (line[1] - '0' < 8) && (isspace(line[2]) || (line[2] == ','));
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
        return FALSE;
    }
    if(!isalpha(word[0])){
        printf("ERROR: label must start with a letter <a-z> or <A-Z> ; at line: %d\n",lineCounter);
        return FALSE;
    }
    for(i = 0; i < wordLen; i++){
        if(!isalnum(word[i])){
            printf("ERROR: label must be fully alphanumeric and start with a letter ; at line: %d\n",lineCounter);
            return FALSE;
        }
    }
    if(isKeyword(word)){
        printf("ERROR: label cannot be a saved keyword ; at line: %d\n",lineCounter);
        return FALSE;
    }
    return TRUE; /*Valid label*/
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
int isNum(const char* line, char** numSuffix, char isData)
{
    /*Minimum and maximum value for number*/
    long int min = isData ? ASM_DATA_MIN_INT : ASM_INST_MIN_INT;
    long int max = isData ? ASM_DATA_MAX_INT : ASM_INST_MAX_INT;

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
    if(num < min || num > max){
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
int validateString(const char* line, int start, int end, int lineCounter)
{
    int i;

    while(isspace(line[start]))
        start++;
    while(isspace(line[end]))
        end--;

    if(line[start]!='"'||line[end]!='"' || start == end){
        printf("ERROR: String must be enclosed in double quotes; at line: %d\n",lineCounter);
        return -1;
    }
    for (i = start; i <= end; i++){
        if(line[i] < ' ' || line[i] > '~'){
            printf("ERROR: String must be fully composed of printable ASCII characters: %d\n",lineCounter);
            return -1;
        }
    }
    return end-start;
}
int validateData(const char* line, int i, int lineCounter)
{
    char* numSuffix;
    int dataArgs = 0;
    int commaCount = 0;
    Token t;
    while(line[i] != '\0'){
        if(isspace(line[i])){
            i++;
            continue;
        }
        if(isNum(&line[i], &numSuffix, TRUE)){
            i = numSuffix - &line[0]; /*TODO: Does this converts the memory address of the pointer to an array index?🤔🤔🤔*/
            dataArgs++;
            if(line[i] == ','){
                i++;
                commaCount++;
            }                  
        }
        else{
            storeWord(&t, &line[i], numSuffix - &line[i]+1);
            printf("ERROR: Invalid number, read: \"%s\" ; at line: %d\n", t.currentWord, lineCounter);                  
            return -1;
        }
    }
    if(dataArgs != commaCount+1){ /*sanity check that args are 1 more than commas*/
        printf("ERROR: missing number; at line: %d\n", lineCounter);
        return -1;
    }
    return dataArgs;
}
