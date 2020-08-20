#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"
#include "masks.h"
#include "mem_array.h"
#include "constants.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "second_pass.h"

enum{
    IMMEDIATE,
    DIRECT,
    RELATIVE,
    REG
}addressingType;

int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst)
{
    char line[MAX_LINE_LENGTH + 2];
    Token toke;
    char errorFlag = FALSE;
    int lineCounter = 0;
    int lineLen;
    int i = 0;
    int index1, index2;
    int dataIdx = 0, instIdx = 0;
    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){
        int cmdIndex;
        i = 0;
        ++lineCounter;
        lineLen = strlen(line);

        while(isspace(line[i]))
            ++i;
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        getWord(line, &i, &index1, &index2);
        storeWord(&toke, &line[index1], index2-index1+1);

        if(line[index2] == ':'){/* skipping labels*/
            getWord(line, &i, &index1, &index2);
            storeWord(&toke, &line[index1], index2-index1+1);
        }

        if(line[index1] == '.'){
            if(!strcmp(toke.currentWord, ".data")){/*Encode data*/
                char* numSuffix;
                long int num;
                num = strtol(&line[i], &numSuffix, 10);/*Fence posting*/
                while(numSuffix[0] != '\0'){/*While there's numbers to read*/

                    data->arr[dataIdx++] = ENCODE_DATA_NUM(num);/*Encoding*/

                    i = numSuffix - &line[0];
                    /*Skipping over commas and whitespace*/
                    while(isspace(line[i]) || line[i] == ',')
                        ++i;
                    num = strtol(&line[i], &numSuffix, 10);/*Read next number*/
                }
            }
            else if(!strcmp(toke.currentWord, ".string")){/*Encode string*/

                index1 = index2+1;
                index2 = lineLen-1;
                while(isspace(line[index1]))
                    index1++;
                while(isspace(line[index2]))
                    index2--;
                /*Starting after '"'*/
                index1++;/* fence posting*/
                for(; index1 < index2; ++index1){/*TODO: correct index2 to be correct*/
                    data->arr[dataIdx++] = ENCODE_DATA_STRING(line[index1]);
                }
                data->arr[dataIdx++] = '\0';/*Terminating null character*/
            }
            else if(!strcmp(toke.currentWord, ".entry")){
                getWord(line, &i, &index1, &index2);
                storeWord(&toke, &line[index1], index2-index1+1);
                if(!makeEntry(toke.currentWord)){
                    printf("ERROR: label \"%s\" was not declared in file ; at line: %d\n",
                            toke.currentWord,
                            lineCounter);
                    errorFlag = TRUE;
                }
            }
            continue;
        }
        else{/*Operator*/
            Operand op;
            int commaIndex = i;
            int opNum = 1;
            int addWords = 0;

            cmdIndex = isOp(toke.currentWord);
            inst->arr[instIdx] = CMD[cmdIndex].mask;

            while(line[commaIndex] != ',' && line[commaIndex] != '\0')
                ++commaIndex;
            /*TODO: find comma index*/
            while(opNum <= CMD[cmdIndex].numParams){
                int startIndex = (opNum == 1) ? i : commaIndex+1;
                int endIndex = (opNum == 1) ? commaIndex-1 : lineLen-1;
                /*TODO:cleanup*/
                int adderShift = (opNum == 1 && CMD[cmdIndex].numParams == 2) ? 13 : 8;/*TODO: change to defines*/
                while(isspace(line[startIndex]))
                    ++startIndex;
                while(isspace(line[endIndex]))
                    --endIndex;

                storeWord(&toke, &line[startIndex], endIndex-startIndex+1);
                op = parseOperand(&toke);

                if(op.addressing == IMMEDIATE){
                    /*mask*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, IMMEDIATE, adderShift);
                    /*encode number into instIdx+addWords*/
                    ++addWords;
                    inst->arr[instIdx+addWords] = ENCODE_WORD_NUM(op.type.num);
                }

                else if(op.addressing == DIRECT){
                    long address;/*TODO: verify if signed long is enough to hold address - its minimum 32 bits*/
                    char external;
                    /*mask*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, DIRECT, adderShift);
                    /*exists*/
                    if((address = getAddress(op.type.label)) == -1){
                        printf("ERROR: label \"%s\" was not declared in file ; at line: %d\n",
                                op.type.label,
                                lineCounter);
                        errorFlag = TRUE;
                    }
                    
                    ++addWords;
                    external = isExtern(op.type.label);
                    if(external)
                        addExternLabel(op.type.label, instIdx+addWords+STARTADDRESS);
                    inst->arr[instIdx+addWords] = ENCODE_WORD_ADDRESS(address, external);/*first bit*/
                }

                else if(op.addressing == RELATIVE){
                    long address;/*TODO: verify if signed long is enough to hold address- GUARANTEED 32 BITS*/
                    /*unsigned long mask = 0;*/
                    /*mask*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, RELATIVE, adderShift);
                    /*exists*/
                    if((address = getAddress(op.type.label)) == -1){
                        printf("ERROR: label \"%s\" was not declared in file ; at line: %d\n",
                                op.type.label,
                                lineCounter);
                        errorFlag = TRUE;
                    }
                    /*extern and error*/
                    if(isExtern(op.type.label)){
                        printf("ERROR: cannot jump to external label \"%s\" ; at line: %d\n",
                                op.type.label,
                                lineCounter);
                        errorFlag = TRUE;
                    }
                    /*encode address jump into instIdx+addWords*/
                    ++addWords;
                    inst->arr[instIdx+addWords] = ENCODE_JUMP_DISTANCE(address, instIdx+STARTADDRESS);
                }

                else if(op.addressing == REG){
                    /*mask according to register number*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(op.type.reg, REG, adderShift);/*TODO:one argument operators are encoding into destination*/
                }
                opNum++;
            }
        instIdx += addWords + 1;/*additional words with current one*/
        }
    }
    return !errorFlag;
}

Operand parseOperand(Token* t)
{
    Operand op;
    if(t->currentWord[t->len-1] == ','){
        --t->len;
        t->currentWord[t->len] = '\0';
    }
    if(t->currentWord[0] == '#'){
        op.type.num = strtol(&t->currentWord[1], NULL, 10);
        op.addressing = IMMEDIATE;
    }
    else if(t->currentWord[0] == '&'){
        op.type.label = &t->currentWord[1];
        op.addressing = RELATIVE;
    }
    else if(t->len == 2 && t->currentWord[0] == 'r' && (t->currentWord[t->len-1] >= '0' && t->currentWord[t->len-1] <= '7')){
        op.type.reg = t->currentWord[t->len-1] - '0';
        op.addressing = REG;
    }
    else{
        op.type.label = &t->currentWord[0];
        op.addressing = DIRECT;
    }
    return op;
}
