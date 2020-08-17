#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "second_pass.h"
int secondPass(FILE* fp, int dataCounter, int instCounter)
{
    char line[MAX_LINE_LENGTH + 2];
    Token toke;
    char errorFlag = FALSE;
    int lineCounter = 0;
    int i = 0;
    int index1, index2;
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

        if(line[index2] == ':'){
            getWord(line, &i, &index1, &index2);
            storeWord(&toke, &line[index1], index2-index1+1);
        }

        #define FIRST24 2097151
        if(line[index1] == '.'){
            if(!strcmp(toke.currentWord, ".data")){/*TODO:Encode data*/
                char* numSuffix;
                long int num;
                num = strtol(&line[i], &numSuffix, 10);/*Fence posting*/
                while(numSuffix[0] != '\0'){/*While there's numbers to read*/

                    /*TODO: Encode the number*/
                    num &= FIRST24;


                    /*Skipping over commas and whitespace*/
                    i = numSuffix - &line[0];
                    while(isspace(line[i]) || line[i] == ',')
                        ++i;
                    num = strtol(&line[i], &numSuffix, 10);/*Read next number*/
                }
            }
            else if(!strcmp(toke.currentWord, ".string")){/*TODO:Encode string*/

                getWord(line, &i, &index1, &index2);
                /*Starting after '"'*/
                for(index1++; index1 != index2; ++index1){
                    /*TODO: encode character*/
                }
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
        }

    }
}
/*DATA num/string  = 24 signed

 *immidiet addressing = 3 bits + 21 signed bits
 *relative addressing = 3 bits + 21 signed

 *direct addressing = 3 bits + 21 unsigned
 
 *opcode = 24 unsigned bits*/

unsigned long i = 2097151;

unsigned long upper = ~(ULONG_MAX << 21) & i;
unsigned long ARE = 4;
upper << 3 | ARE