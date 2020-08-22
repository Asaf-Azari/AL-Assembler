#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "symbol_table.h"
#include "asm_tables.h"
#include "first_pass.h"
#include "constants.h"

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
            printf("ERROR:%d: Line exceeds maximum length of %d characters\n" ,lineCounter,MAX_LINE_LENGTH);
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

            if (isValidLabel(word.currentWord, word.len, lineCounter)){
                labelFlag = TRUE;
                strncpy(labelTemp, word.currentWord, word.len);
                labelTemp[word.len] = '\0';

                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);
                if(line[index1] == '\0'){
                    printf("ERROR:%d: empty label definition \n", lineCounter);                  
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
                                decleration error? - Where?*/
                    if(!exists(labelTemp))
                        addLabel(labelTemp, TRUE, FALSE, *dataCounter);
                    else{
                        printf("ERROR:%d: Duplicate label definition\n" ,lineCounter);
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
                        printf("ERROR:%d: Duplicate label definition\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue; 
                    }
                }
                *dataCounter += strLen;
            }

            else if(asmOpt == ENTRY){
            }

            else if(asmOpt == EXTERN){
                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);
                if (isValidLabel(word.currentWord, word.len, lineCounter)){
                    getWord(line, &i, &index1, &index2);
                    if(line[index1]=='\0'){/*No text after label*/
                        if(exists(word.currentWord)&&!isExtern(word.currentWord)){/*TODO: do we care?*/
                            printf("ERROR:%d: Duplicate label\n" ,lineCounter);
                            errorFlag = TRUE;
                            continue;  
                        }
                        else{    
                            addLabel(word.currentWord, FALSE, TRUE, 0);
                        }
                    }
                    else{
                        printf("ERROR:%d: extranous text after label\n" ,lineCounter);
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
            int commaIndex;/*index pointing to comma*/
            int params = CMD[cmdIndex].numParams;/*number of operands for the command*/
            if(labelFlag){
                if(exists(labelTemp)){/*TODO: Do you think we should change the other ones to this format? it avoids the else - i mean we can but does it matter?*/
                    printf("ERROR:%d: Duplicate label definition\n", lineCounter);
                    errorFlag = TRUE;
                    continue;
                }
                addLabel(labelTemp, FALSE, FALSE, *instCounter);
            }
            ++*instCounter;/*Count operand*/
            /*TODO: make validation into a function?*/
            #if 0
            while(isspace(line[i]))
                    ++i;
            if(params == 0 && line[i] != '\0'){/*operands given to 0 operand command*/
                    printf("ERROR:%d: command \"%s\" does not accepts operands \n", lineCounter, CMD[cmdIndex].cmdName);
                    errorFlag = TRUE;
                    continue;
            }
            else if(params != 0 && line[i] == '\0'){/*No operands given*/
                    printf("ERROR:%d: command \"%s\" accepts %d operand%s, none given \n", 
                            lineCounter,
                            CMD[cmdIndex].cmdName,
                            params,
                            (params > 1) ? "s" : "");
                    errorFlag = TRUE;
                    
                    continue;
            }
            #endif
            if(!validateOpNum(line, &i, cmdIndex, lineCounter)){
                errorFlag = TRUE;
                continue;
            }
            if((commaIndex = validateCommas(line, i, cmdIndex, lineCounter)) == -1){
                errorFlag = TRUE;
                continue;
            }
            #if 0 
            while(line[i]){
                if(line[i] == ','){/*Counting commas in line*/
                    ++commas;
                    commaIndex = i;
                }
                i++;
            }
            if(params == 1 && commas > 0){
                errorFlag = TRUE;
                printf("ERROR:%d: command \"%s\" accepts 1 operand, extranous comma \n",
                lineCounter,
                CMD[cmdIndex].cmdName);

                continue;
            }
            else if(params == 2 && commas != 1){
                errorFlag = TRUE;
                printf("ERROR:%d: command \"%s\" accepts 2 operands, %s \n",
                lineCounter,
                CMD[cmdIndex].cmdName,
                (commas > 1) ? "extranous comma" : "missing comma");

                continue;
            }
            #endif
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
            printf("ERROR:%d: unrecognized instruction \"%s\" \n", lineCounter, word.currentWord);
            errorFlag = TRUE;
            continue;
        }
    }/*While fgets*/
    if(!errorFlag && *instCounter+*dataCounter+STARTADDRESS-1 > MAXADDRESS){
        printf("ERROR: Program exceeds maximum address space of %d\n", MAXADDRESS);
        errorFlag = TRUE;
    }
    return !errorFlag;
}
int verifyOperand(const char* line, const COMMANDS* cmd, int params, int* instCounter, int lineCounter)
{
    Token label;/*Hold label for validation*/

    /*Immediate number*/
    if(line[0] == '#'){
        if(!(cmd->viableOperands & (params == 1 ? OP1_IMMEDIATE : OP2_IMMEDIATE))){
            printf("ERROR:%d: command \"%s\" does not accept number as %s operand \n", 
                    lineCounter,
                    cmd->cmdName,
                    (params == 1) ? "1st" : "2nd");
            return FALSE;
        }
        if(!isNum(&line[1], NULL, FALSE)){
            printf("ERROR:%d: invalid number \n", lineCounter);
            return FALSE;
        }
        ++*instCounter;
    }

    /*Register*/
    else if(isReg(line)){
        if(!(cmd->viableOperands & (params == 1 ? OP1_REG : OP2_REG))){
            printf("ERROR:%d: command \"%s\" does not accept register as %s operand \n",
                    lineCounter,
                    cmd->cmdName,
                    (params == 1) ? "1st" : "2nd");
            return FALSE;
        }
    }

    /*Relative*/
    else if(line[0] == '&'){
        int i = 0;
        if(!(cmd->viableOperands & ((params == 1) ? OP1_RELATIVE : OP2_RELATIVE))){
            printf("ERROR:%d: command \"%s\" does not support relative addressing at %s operand \n",
            lineCounter,
            cmd->cmdName,
            (params == 1) ? "1st" : "2nd");
        }
        while(!isspace(line[i]) && line[i] != ',')
            ++i;
        /*if(line[i] == '\0'){ TODO: decide if we want to have this
            printf("ERROR:%d: no label after '&' \n", lineCounter);
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
                printf("ERROR:%d: extranous operand \n", lineCounter);
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
        printf("ERROR:%d: missing %s operand \n", 
                lineCounter,
                params == 1 ? "1st" : "2nd");
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
int consumeComma(const char* line, int* i) /*TODO: we dont use this do we*/
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
int boundOp(const char* line, int* i) /*TODO: we dont use this do we*/
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
    printf("ERROR:%d: invalid assembler instruction \n", lineCounter);
    return ERROR;
}
/*asserts that a given label is a valid label.
 *a label can have up to 31 characters, begin with a letter 
 *from the range <a-z>/<A-Z> and must be fully alphanumeric*/
int isValidLabel(char* word, int wordLen, int lineCounter)
{
    int i;
    if(wordLen > MAXLABELSIZE){
        printf("ERROR:%d: label exceeds maximum length of %d \n",lineCounter, MAXLABELSIZE);
        return FALSE;
    }
    if(!isalpha(word[0])){
        printf("ERROR:%d: label must start with a letter <a-z> or <A-Z> \n",lineCounter);
        return FALSE;
    }
    for(i = 0; i < wordLen; i++){
        if(!isalnum(word[i])){
            printf("ERROR:%d: label must be fully alphanumeric and start with a letter \n",lineCounter);
            return FALSE;
        }
    }
    if(isKeyword(word)){
        printf("ERROR:%d: label cannot be a saved keyword \n",lineCounter);
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
        printf("ERROR:%d: String must be enclosed in double quotes\n",lineCounter);
        return -1;
    }
    for (i = start; i <= end; i++){
        if(line[i] < ' ' || line[i] > '~'){
            printf("ERROR:%d: String must be fully composed of printable ASCII characters\n",lineCounter);
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
            i = numSuffix - &line[0]; 
            dataArgs++;
            if(line[i] == ','){
                i++;
                commaCount++;
            }                  
        }
        else{
            storeWord(&t, &line[i], numSuffix - &line[i]+1);
            printf("ERROR:%d: Invalid number, read: \"%s\" \n", lineCounter, t.currentWord);                  
            return -1;
        }
    }
    if(dataArgs != commaCount+1){ /*sanity check that args are 1 more than commas*/
        printf("ERROR:%d: missing number\n", lineCounter);
        return -1;
    }
    return dataArgs;
}


int validateOpNum(const char* line, int* i, int cmdIndex, int lineCounter)
{
    int params = CMD[cmdIndex].numParams;
    while (isspace(line[*i]))
        ++*i;
    if (params == 0 && line[*i] != '\0'){ /*operands given to 0 operand command*/
        printf("ERROR:%d: command \"%s\" does not accepts operands \n", lineCounter, CMD[cmdIndex].cmdName);
        return FALSE;
    }
    else if (params != 0 && line[*i] == '\0'){ /*No operands given*/
        printf("ERROR:%d: command \"%s\" accepts %d operand%s, none given \n",
               lineCounter,
               CMD[cmdIndex].cmdName,
               params,
               (params > 1) ? "s" : "");
        return FALSE;
    }
    if (params == 0 && line[*i] != '\0'){ /*operands given to 0 operand command*/
        printf("ERROR:%d: command \"%s\" does not accepts operands \n", lineCounter, CMD[cmdIndex].cmdName);
        return FALSE;
    }
    else if (params != 0 && line[*i] == '\0'){ /*No operands given*/
        printf("ERROR:%d: command \"%s\" accepts %d operand%s, none given \n",
               lineCounter,
               CMD[cmdIndex].cmdName,
               params,
               (params > 1) ? "s" : "");
        return FALSE;
    }
    return TRUE;
}
int validateCommas(const char* line, int i, int cmdIndex, int lineCounter)
{
    int commas = 0;
    int commaIndex;
    int params = CMD[cmdIndex].numParams;
    while (line[i]){
        if (line[i] == ','){ /*Counting commas in line*/
            ++commas;
            commaIndex = i;
        }
        i++;
    }
    if (params == 1 && commas > 0){
        printf("ERROR:%d: command \"%s\" accepts 1 operand, extranous comma \n",
               lineCounter,
               CMD[cmdIndex].cmdName);
        return -1;
    }
    else if (params == 2 && commas != 1){
        printf("ERROR:%d: command \"%s\" accepts 2 operands, %s \n",
               lineCounter,
               CMD[cmdIndex].cmdName,
               (commas > 1) ? "extranous comma" : "missing comma");
        return -1;
    }
    return commaIndex;
}