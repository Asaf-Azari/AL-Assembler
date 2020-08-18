#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "bit_masks.h" 
#include "mem_array.h"
#include "constants.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "second_pass.h"
int secondPass(FILE* fp, encodedAsm* data, encodedAsm* inst)
{
    char line[MAX_LINE_LENGTH + 2];
    Token toke;
    char errorFlag = FALSE;
    int lineCounter = 0;
    int i = 0;
    int index1, index2;
    int dataIdx = 0, instIdx = 0;
    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){
        int cmdIndex;
        i = 0;
        ++lineCounter;

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

                    data->arr[dataIdx++].memory = num;/*Encoding*/

                    i = numSuffix - &line[0];
                    /*Skipping over commas and whitespace*/
                    while(isspace(line[i]) || line[i] == ',')
                        ++i;
                    num = strtol(&line[i], &numSuffix, 10);/*Read next number*/
                }
            }
            else if(!strcmp(toke.currentWord, ".string")){/*Encode string*/

                getWord(line, &i, &index1, &index2);
                /*Starting after '"'*/
                for(index1++; index1 != index2; ++index1){/*TODO: correct index2 to be correct*/
                    data->arr[dataIdx++].memory = line[index1];
                }
                data->arr[dataIdx++].memory = '\0';/*Terminating null character*/
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

typedef struct{
    union{
        int num;
        int reg;
        char* label;
    } type;
    int addresing;
} Operand;
        else{/*Operator*/
            Operand op;
            int opNum = 1;
            int addWords = 0;
            cmdIndex = isOp(toke.currentWord);
            inst->arr[instIdx].memory = CMD[cmdIndex].mask;
            /*TODO: find comma index*/
            while(opNum <= CMD[cmdIndex].numParams){
                int opIndex = (opNum == 1) ? i : commaIndex+1
                storeWord(&toke, &line[opIndex], commaIndex - opIndex + 1);
                op = parseOperand(&toke);
                switch(op.addresing){
                    case IMMEDIATE:
                        /*mask*/
                        /*encode number into instIdx+opNum*/
                        ++addWords;
                        break;
                    case DIRECT:
                        /*mask*/
                        /*exists*/
                        /*extern and add to extern table*/
                        /*encode label address into instIdx+opNum*/
                        ++addWords;
                        break;
                    case RELATIVE:
                        /*mask*/
                        /*exists*/
                        /*extern and error*/
                        /*encode address jump into instIdx+opNum*/
                        ++addWords;
                        break;
                    case REG:
                        /*mask according to register number*/
                        break;
                }
            }
        }
    }
}

/*

 while(notcomma){findcomma};
                    switch(firstoperand-type){
                        case 0:
                        mask 
                        directaddressing(word, instIdx+operand)
                        case 1:
                        mask
                        1. label exists;
                        2. isextern;
                        miunyashir(label, instidx+operand)
                        case 2:
                        mask
                        1. label exists;
                        2. isextern; error!
                        miunyashir(label, instidx+operand, instIdx)
                        case 3:
                        mask;

                    }

*/




/*DATA num/string  = 24 signed

 *immidiet addressing = 3 bits + 21 signed bits
 *relative addressing = 3 bits + 21 signed

 *direct addressing = 3 bits + 21 unsigned
 
 *opcode = 24 unsigned bits*/
