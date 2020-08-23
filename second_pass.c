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

enum{/*Addressing types for visual control flow*/
    IMMEDIATE,
    DIRECT,
    RELATIVE,
    REG
}addressingType;

/*Second pass of our assembler.
 *Second pass attempts to create the memory picture.
 *It will encode line by line into the right memory picture (data/instruction)
 *and will handle marking labels as entry and log external references for both the ext and ent files.
 *Will raise errors if encountered an unknown label, if relative addresing is
 *called upon an external label or if there was an attempt to declare an unkown label as entry
 *returns false if any errors were met and true otherwise.
 */
int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst)
{
    char line[MAX_LINE_LENGTH + 2];
    Token toke; /*Struct holding current word and its length*/
    char errorFlag = FALSE;

    int lineCounter = 0;
    int lineLen;

    /*indicies for iterating over the line*/
    int i = 0;
    int index1, index2;
    /*local IC and DC to encode instructions*/
    int dataIdx = 0, instIdx = 0;
    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){/*While there's input*/
        int cmdIndex;/*index pointing to a command in the constant command table*/
        i = 0;
        ++lineCounter;
        lineLen = strlen(line);

        while(isspace(line[i]))/*Skipping leading whitespace*/
            ++i;
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        getWord(line, &i, &index1, &index2);/*get the first word*/
        storeWord(&toke, &line[index1], index2-index1+1);

        if(line[index2] == ':'){/*skipping label definitions*/
            getWord(line, &i, &index1, &index2);
            storeWord(&toke, &line[index1], index2-index1+1);
        }

        if(line[index1] == '.'){/*if assembly opt*/
            if(!strcmp(toke.currentWord, ".data")){/*Encode data*/
                char* numSuffix;/*points to text after number*/
                long int num;
                num = strtol(&line[i], &numSuffix, 10);/*Fence posting*/
                while(line[i] != '\0'){/*While there's numbers to read*/
                    
                    data->arr[dataIdx++] = ENCODE_DATA_NUM(num);/*Encoding*/

                    i = numSuffix - &line[0];/*i points to text after number*/
                    /*Skipping over commas and whitespace*/
                    while(isspace(line[i]) || line[i] == ',')
                        ++i;
                    num = strtol(&line[i], &numSuffix, 10);/*Read next number*/
                }
            }
            else if(!strcmp(toke.currentWord, ".string")){/*Encode string*/
                index1 = index2+1;
                index2 = lineLen-1;
                /*adjust indicies to point to enclosing "*/
                while(isspace(line[index1]))
                    index1++;
                while(isspace(line[index2]))
                    index2--;
                /*Starting after '"'*/
                index1++;/*fence posting*/
                for(; index1 < index2; ++index1){
                    /*encode each character into memory word and increment local IC*/
                    data->arr[dataIdx++] = ENCODE_DATA_STRING(line[index1]);
                }
                data->arr[dataIdx++] = '\0';/*encoding terminating null character*/
            }
            else if(!strcmp(toke.currentWord, ".entry")){/*marking label is entry*/
                getWord(line, &i, &index1, &index2);
                storeWord(&toke, &line[index1], index2-index1+1);
                if(!makeEntry(toke.currentWord)){/*try to mark label as entry, if fails, raise error*/
                    printf("ERROR:%d: label \"%s\" not found, cannot add entry point\n",
                            lineCounter,
                            toke.currentWord);
                    errorFlag = TRUE;
                }
            }
            continue;
        }

        else{/*command*/
            Operand op;/*holding operand value and type*/
            int commaIndex = i;/*index pointing to comma*/
            int opNum = 1;/*starting to encode first operand*/
            int addWords = 0;/*number of additional memory words to add(according to immediate/direct/relative)*/

            cmdIndex = isOp(toke.currentWord);/*get commmand and do initial masking*/
            inst->arr[instIdx] = CMD[cmdIndex].mask;

            while(line[commaIndex] != ',' && line[commaIndex] != '\0')/*find comma for 2 operand comands*/
                ++commaIndex;
            while(opNum <= CMD[cmdIndex].numParams){/*encode based on the number of parameters*/
                /*indicies to bound operand*/
                int startIndex = (opNum == 1) ? i : commaIndex+1;
                int endIndex = (opNum == 1) ? commaIndex-1 : lineLen-1;

                /*bit shift according to which register we're encoding*/
                int adderShift = (opNum == 1 && CMD[cmdIndex].numParams == 2) ? SRC_REG : DEST_REG;

                /*bound operand*/
                while(isspace(line[startIndex]))/*skip whitespace*/
                    ++startIndex;
                while(isspace(line[endIndex]))
                    --endIndex;

                storeWord(&toke, &line[startIndex], endIndex-startIndex+1);
                op = parseOperand(&toke);/*parse operand and handle based on addressing method*/

                if(op.addressing == IMMEDIATE){
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, IMMEDIATE, adderShift);/*encode command*/
                    ++addWords;
                    inst->arr[instIdx+addWords] = ENCODE_WORD_NUM(op.type.num);/*encode secondary word*/
                }

                else if(op.addressing == DIRECT){
                    long address;
                    char external;
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, DIRECT, adderShift);/*encode command*/
                    if((address = getAddress(op.type.label)) == -1){
                        printf("ERROR:%d: label \"%s\" was not declared\n",
                                lineCounter,
                                op.type.label);
                        errorFlag = TRUE;
                    }
                    
                    ++addWords;
                    external = isExtern(op.type.label);
                    if(external)
                        addExternLabel(op.type.label, instIdx+addWords+STARTADDRESS);/*mark for exporting to .ext*/
                    inst->arr[instIdx+addWords] = ENCODE_WORD_ADDRESS(address, external);/*encode secondary word*/
                }

                else if(op.addressing == RELATIVE){
                    long address;/*for calculating relative jump*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(0, RELATIVE, adderShift);/*encode command*/
                    if((address = getAddress(op.type.label)) == -1){
                        printf("ERROR:%d: label \"%s\" was not declared\n",
                                lineCounter,
                                op.type.label);
                        errorFlag = TRUE;
                    }
                    if(isExtern(op.type.label)){
                        printf("ERROR:%d: cannot jump to external label \"%s\"\n",
                                lineCounter,
                                op.type.label);
                        errorFlag = TRUE;
                    }

                    /*encode address jump into next word*/
                    ++addWords;
                    inst->arr[instIdx+addWords] = ENCODE_JUMP_DISTANCE(address, instIdx+STARTADDRESS);/*calculate end encode jump distance*/
                }

                else if(op.addressing == REG){
                    /*mask according to register number*/
                    inst->arr[instIdx] |= ENCODE_METHOD_REG(op.type.reg, REG, adderShift);
                }
                opNum++;/*next operand*/
            }
            instIdx += addWords + 1;/*count this memory word with additional word encoded*/
        }/*else command*/
    }/*while fgets*/
    return !errorFlag;
}
/*reads the operand given by Token and returns its addressing type and value*/
Operand parseOperand(Token* t)
{
    Operand op;
    if(t->currentWord[t->len-1] == ','){/*ignore comma*/
        --t->len;
        t->currentWord[t->len] = '\0';
    }
    if(t->currentWord[0] == '#'){/*Immediate addressing*/
        op.type.num = strtol(&t->currentWord[1], NULL, 10);
        op.addressing = IMMEDIATE;
    }
    else if(t->currentWord[0] == '&'){/*relative addressing*/
        op.type.label = &t->currentWord[1];
        op.addressing = RELATIVE;
    }
    else if(t->len == 2 && t->currentWord[0] == 'r' && (t->currentWord[t->len-1] >= '0' && t->currentWord[t->len-1] <= '7')){/*register*/
        op.type.reg = t->currentWord[t->len-1] - '0';
        op.addressing = REG;
    }
    else{
        op.type.label = &t->currentWord[0];/*direct addressing*/
        op.addressing = DIRECT;
    }
    return op;
}
