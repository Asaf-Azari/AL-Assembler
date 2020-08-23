#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "symbol_table.h"
#include "asm_tables.h"
#include "first_pass.h"
#include "constants.h"

/*First pass of our assembler.
 *first pass checks for syntax errors, add our programs'
 *labels to the symbol table and adjust our instruction
 *and data counters to allow later allocation of memory for memory pictures.
 *returns false if any errors were met and true otherwise.
 */
int firstPass(FILE* fp, int* dataCounter, int* instCounter)
{
    char line[MAX_LINE_LENGTH + 2];
    Token word;/*Struct holding current word and its length*/
    int lineCounter = 0;
    int lineLen;
    /*Indicies to iterate over the line*/
    int i = 0, index1, index2;
    char errorFlag = FALSE;

    char labelFlag;/*denotes if a label needs to be added for current line*/
    char labelTemp[MAXLABELSIZE+1];/*for adding the label to the symbol table later*/
    
    /*reset IC and DC*/
    *dataCounter = 0;
    *instCounter = 0;

    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){/*Parsing lines*/
        int cmdIndex;/*Index holding place inside CMD array*/
        i = 0; 
        labelFlag = FALSE;/*new line, reset labelFlag*/
        ++lineCounter;/*counting line number*/
        lineLen = strlen(line);

        if(line[lineLen-1] != '\n' && !feof(fp)){/*line longer than 80 characters*/
            char c;
            printf("ERROR:%d: Line exceeds maximum length of %d characters\n" ,lineCounter,MAX_LINE_LENGTH);
            errorFlag = TRUE;
            while((c = fgetc(fp)) != '\n' && c != EOF);/*consume remaining input after 81+ line*/
            continue;
        }

        while(isspace(line[i])) ++i; /*skipping leading whitespaces*/
        if(line[i] == ';' || line[i] == '\0'){/*skipping line if empty/comment*/
            continue;
        }

        /*set indicies to point to the next word in line
         *and store said word*/
        getWord(line, &i, &index1, &index2);
        storeWord(&word, &line[index1], index2-index1+1);

        if(line[index2] == ':'){/*if label*/

            /*adjusting word.currentWord to call validation function*/
            word.len -= 1;/*excluding ':'*/
            word.currentWord[word.len] = '\0';

            if(isValidLabel(word.currentWord, word.len, lineCounter)){/*if we've read a valid label*/
                labelFlag = TRUE;
                strncpy(labelTemp, word.currentWord, word.len);/*save label to add to symbol table*/
                labelTemp[word.len] = '\0';

                getWord(line, &i, &index1, &index2);/*next word*/
                storeWord(&word, &line[index1], index2-index1+1);
                if(line[index1] == '\0'){/*if no more words on line*/
                    printf("ERROR:%d: empty label definition \n", lineCounter);                  
                    errorFlag = TRUE;
                    continue;
                }
            }
            else{/*not a valid label definition*/
                errorFlag = TRUE;
                continue; 
            }
        }/*if label*/

        if(line[index1] == '.'){/*if assembly opt*/

            /*if a valid assembly opt*/
            int asmOpt = isValidAsmOpt(word.currentWord, lineCounter);
            if(asmOpt == ERROR){
                errorFlag = TRUE;
                continue;
            }

            else if(asmOpt == DATA){/*.data*/
                int dataArgs = validateData(line, i, lineCounter);/*count number of arguments*/
                if(dataArgs == -1){/*invalid data*/
                    errorFlag = TRUE;
                    continue;
                }
                if(labelFlag){/*label at the beginning of the line*/
                    if(!exists(labelTemp))/*not already declared*/
                        addLabel(labelTemp, TRUE, FALSE, *dataCounter);/*add it to symbol table*/
                    else{/*label already exists*/
                        printf("ERROR:%d: Duplicate label definition\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue; 
                    }
                }
                *dataCounter += dataArgs;/*add number of arguments of .data to DC*/
            }

            else if(asmOpt == STRING){/*.string*/
                int strLen = validateString(line, index2+1, lineLen-1, lineCounter);
                if(strLen == -1){/*invalid string*/
                    errorFlag = TRUE;
                    continue;
                }
                if(labelFlag){/*label at the beginning of the line*/
                    if(!exists(labelTemp))/*not already declared*/
                        addLabel(labelTemp, TRUE, FALSE, *dataCounter);/*add it to symbol table*/
                    else{/*label already exists*/
                        printf("ERROR:%d: Duplicate label definition\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue; 
                    }
                }
                *dataCounter += strLen;/*add number of characters in .string to DC*/
            }

            else if(asmOpt == ENTRY){/*.entry*/
                /*entry is checked on second pass*/
            }

            else if(asmOpt == EXTERN){/*.extern*/
                /*get label name*/
                getWord(line, &i, &index1, &index2);
                storeWord(&word, &line[index1], index2-index1+1);

                if(isValidLabel(word.currentWord, word.len, lineCounter)){
                    getWord(line, &i, &index1, &index2);/*check for extranous text*/
                    if(line[index1]=='\0'){/*no text after label*/
                        if(exists(word.currentWord)&&!isExtern(word.currentWord)){/*if there's another non-extern decleration*/
                            printf("ERROR:%d: Duplicate label definition\n" ,lineCounter);
                            errorFlag = TRUE;
                            continue;
                        }
                        else{
                            addLabel(word.currentWord, FALSE, TRUE, 0);
                        }
                    }
                    else{/*extra text after label name*/
                        printf("ERROR:%d: extranous text after label\n" ,lineCounter);
                        errorFlag = TRUE;
                        continue;
                    }
                }
                else{/*invalid label name*/
                    errorFlag = TRUE;
                    continue;
                }
            }
        }

        else if((cmdIndex = isOp(word.currentWord)) != -1){/*command*/
            int start = i;/*index pointing to first operand*/
            int wordIdx;/*index pointing to current operand*/
            int commaIndex;/*index pointing to comma*/
            int params = CMD[cmdIndex].numParams;/*number of operands for the command*/

            if(labelFlag){/*label at the beginning of the line*/
                if(!exists(labelTemp)){/*not already declared*/
                    addLabel(labelTemp, FALSE, FALSE, *instCounter);
                }
                else{/*exists already*/
                    printf("ERROR:%d: Duplicate label definition\n", lineCounter);
                    errorFlag = TRUE;
                    continue;
                }
            }

            ++*instCounter;/*count operand in IC*/
            if(!validateOpNum(line, &i, cmdIndex, lineCounter)){/*proper number of operands given to the command*/
                errorFlag = TRUE;
                continue;
            }
            if((commaIndex = validateCommas(line, i, cmdIndex, lineCounter)) == -1){/*proper number give to the command*/
                errorFlag = TRUE;
                continue;
            }

            while(params){/*while there's operands to verify*/
                wordIdx = (params == MAXPARAM) ? commaIndex+1 : start;/*according to number of params, let index point to operand*/
                while(isspace(line[wordIdx]))/*skip whitespace*/
                    ++wordIdx;
                if(!singleToken(&line[wordIdx], params, lineCounter)){/*if there's more than one token/no token*/
                    errorFlag = TRUE;
                    break;
                }

                /*verify proper operands for command and adjust instruction counter accordingly*/
                if(!verifyOperand(&line[wordIdx], &CMD[cmdIndex], params, instCounter, lineCounter)){
                    errorFlag = TRUE;
                    break;
                }

                --params;/*decrement params to get to first operand(if exists)*/
            }/*While end*/

        }/*else if Op*/

        else{/*Not a label, asmOpt or command*/
            printf("ERROR:%d: unrecognized instruction \"%s\" \n", lineCounter, word.currentWord);
            errorFlag = TRUE;
            continue;
        }
    }/*While fgets*/

    /*=====================================================================================*/
    /*Finished parsing the file*/

    /*if no errors found on first pass and total number of assembly instructions
     *are within memory size*/
    if(!errorFlag && *instCounter+*dataCounter+STARTADDRESS-1 > MAXADDRESS){
        printf("ERROR: Program exceeds maximum address space of %d\n", MAXADDRESS);
        errorFlag = TRUE;
    }
    return !errorFlag;/*return wheter or not we had an error on first pass*/
}

/*asserts the syntax, type and usage of a given operand for a given command.
 *reports error if found any and adjusts IC according to operand type.
 *returns false if errors were met and true otherwise.*/
int verifyOperand(const char* line, const COMMANDS* cmd, int params, int* instCounter, int lineCounter)
{
    Token label;/*Holds label for validation*/

    /*cmd->viableOperands & <param> checks the validity of using an operand of a given type
     *with the given command*/

    /*Immediate number*/
    if(line[0] == '#'){
        if(!(cmd->viableOperands & ((params == 1) ? OP1_IMMEDIATE : OP2_IMMEDIATE))){
            printf("ERROR:%d: command \"%s\" does not accept number as %s operand \n", 
                    lineCounter,
                    cmd->cmdName,
                    (params == 1) ? "1st" : "2nd");
            return FALSE;
        }
        if(!isNum(&line[1], NULL, FALSE, lineCounter)){/*invalid number*/
            return FALSE;
        }
        ++*instCounter;/*count numbers' memory word*/
    }

    /*Register*/
    else if(isReg(line)){
        if(!(cmd->viableOperands & ((params == 1) ? OP1_REG : OP2_REG))){
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
            return FALSE;
        }
        while(line[i] != '\0' && !isspace(line[i]) && line[i] != ',')
            ++i;/*iterates over label characters*/

        storeWord(&label, line+1, i-1);/*store the label, line+1 to exclude &*/
        if(!isValidLabel(label.currentWord, label.len, lineCounter)){/*Checks validity of label*/
            return FALSE;
        }
        ++*instCounter;/*count jump memory word*/
    }

    /*Label*/
    else{
        int i = 0;
        while(line[i] != '\0' && !isspace(line[i]) && line[i] != ',')
            ++i;/*iterates over label characters*/
        storeWord(&label, line, i);/*store the label*/
        if(!isValidLabel(label.currentWord, label.len, lineCounter)){/*Checks validity of label*/
            return FALSE;
        }
        ++*instCounter;/*count labels' address memory word*/
    }

    /*no errors encountered*/
    return TRUE;
}
/*Verifies that there exists a single operand for each side of the comma(if there's a comma).
 *returns false if no tokens were read or more than 1 tokens were read and true otherwise.*/
int singleToken(const char* line, int params, int lineCounter)
{
    char tokenFlag = FALSE;/*flag denoting token read*/
    int i = 0;
    while(line[i] && line[i] != ','){/*while there's input and no comma read*/
        if(!isspace(line[i])){/*non-whitespace character*/
            if(tokenFlag){/*if already read a token, another token would be illegal*/
                printf("ERROR:%d: extranous operand \n", lineCounter);
                return FALSE;
            }
            else{/*first token read, read remaining characters of said token and continue*/
                tokenFlag = TRUE;
                while(line[i] && !isspace(line[i]) && line[i] != ',')
                    ++i;
                continue;
            }
        }
        ++i;
    }
    if(!tokenFlag){/*no tokens read, missing an operand*/
        printf("ERROR:%d: missing %s operand \n", 
                lineCounter,
                params == 1 ? "1st" : "2nd");
    }
    return tokenFlag;
}
/*asserts that word is a valid assembly instruction.
 *Must be one of the following: .data, .string, .entry, .extern
 *returns an enum value according to the type of instruction.*/
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
 *from the range <a-z>/<A-Z> and must be fully alphanumeric.
 *returns false if errors were met and true otherwise.*/
int isValidLabel(char* word, int wordLen, int lineCounter)
{
    int i;
    if(wordLen > MAXLABELSIZE){/*label bigger than MAXLABELSIZE*/
        printf("ERROR:%d: label exceeds maximum length of %d \n",lineCounter, MAXLABELSIZE);
        return FALSE;
    }
    if(!isalpha(word[0])){/*beginning with non-alpha character*/
        printf("ERROR:%d: label must start with a letter <a-z> or <A-Z> \n",lineCounter);
        return FALSE;
    }
    for(i = 0; i < wordLen; i++){
        if(!isalnum(word[i])){/*non-alphanumeric character in the label*/
            printf("ERROR:%d: label must be fully alphanumeric and start with a letter \n",lineCounter);
            return FALSE;
        }
    }
    if(isKeyword(word)){/*if label is a saved keyword*/
        printf("ERROR:%d: label cannot be a saved keyword \n",lineCounter);
        return FALSE;
    }
    return TRUE; /*Valid label*/
}
/*asserts that a given char* represents a valid integer
 *number between a range dictated by isData.
 *if numSuffix != NULL, the function points *numSuffix to the text after the number.
 *returns false if errors were met and true otherwise.*/
int isNum(const char* line, char** numSuffix, char isData, int lineCounter)
{
    /*Minimum and maximum value for number*/
    long int min = isData ? ASM_DATA_MIN_INT : ASM_INST_MIN_INT;/*instruction range*/
    long int max = isData ? ASM_DATA_MAX_INT : ASM_INST_MAX_INT;/*.data range*/

    char* localSuffix;/*char* pointing to characters after number*/
    Token t;
    long int num = strtol(line, &localSuffix, 10);
    if(numSuffix == NULL){/*no numSuffix is given*/
        numSuffix = &localSuffix;
    }
    else{/*points *numSuffix to text after number*/
        *numSuffix = localSuffix;
    }
    if(line == *numSuffix){/*no digits read*/
        storeWord(&t, line, *numSuffix - line+1);
        printf("ERROR:%d: No number found; read: \"%s\" \n", lineCounter, t.currentWord);
        return FALSE;
    }
    if(num < min || num > max){/*number outside of range*/
        printf("ERROR:%d: Number out of range\n",lineCounter);
        return FALSE;
    }
    while(**numSuffix != '\0'){/*characters after number*/
        if(!isspace(**numSuffix)){
            if(**numSuffix == ',')/*a comma is valid inside a .data instruction*/
                return TRUE;
            else{/*any other non-whitespace character is illegal after a number*/
                storeWord(&t, line, *numSuffix - line+1);
                printf("ERROR:%d: illegal characters after number; read: \"%s\" \n", lineCounter,t.currentWord);
                return FALSE;
            }
        }
        ++*numSuffix;
    }
    return TRUE;
}
/*asserts that a given char* represents a valid string(after a .string instruction).
 *-1 return value indicates there was an error, otherwise returns number of characters
 *inside the string(including null terminating byte).*/
int validateString(const char* line, int start, int end, int lineCounter)
{
    int i;

    /*adjusting indicies until non-whitespace character is met*/
    while(isspace(line[start]))
        start++;
    while(isspace(line[end]))
        end--;

    if(line[start]!='"'||line[end]!='"' || start == end){/*anything other then "<printable ASCII characters>" is illegal*/
        printf("ERROR:%d: String must be enclosed in double quotes\n",lineCounter);
        return -1;
    }
    for(i = start; i <= end; i++){/*looping over the strings' characters*/
        if(line[i] < ' ' || line[i] > '~'){/*outisde of printable ASCII characters range*/
            printf("ERROR:%d: String must be fully composed of printable ASCII characters\n",lineCounter);
            return -1;
        }
    }
    return end-start;/*returns the length of the string(including null byte)*/
}
/*validates legality of .data arguments.
 *returns -1 to indicate error, otherwise returns number of arguments read.*/
int validateData(const char* line, int i, int lineCounter)
{
    char* numSuffix;/*points to after characters after number read*/
    int dataArgs = 0;/*counts the number of numbers read*/
    int commaCount = 0;/*counts the number of commas read*/
    while(line[i] != '\0'){/*while there's input*/
        if(isspace(line[i])){/*skip whitespace*/
            i++;
            continue;
        }
        if(isNum(&line[i], &numSuffix, TRUE, lineCounter)){/*if found a valid number*/
            i = numSuffix - &line[0]; /*adjust i to point to text after number*/
            dataArgs++;
            if(line[i] == ','){/*count comma if found one*/
                i++;
                commaCount++;
            }                  
        }
        else{/*not a valid number*/                
            return -1;
        }
    }
    if(dataArgs != commaCount+1){ /*asserts right number of commas*/
        printf("ERROR:%d: missing number\n", lineCounter);
        return -1;
    }
    return dataArgs;/*returns number of arguments read*/
}
/*validates number of operands given to a command is valid.
 *returns false if given too many\little operands according to command pointed to by
 *cmdIndex and true otherwise.*/
int validateOpNum(const char* line, int* i, int cmdIndex, int lineCounter)
{
    int params = CMD[cmdIndex].numParams;/*locally store number of parameters expected from the command*/
    while (isspace(line[*i]))/*skip leading whitespace*/
        ++*i;
    if(params == 0 && line[*i] != '\0'){/*operands given to 0 operand command*/
        printf("ERROR:%d: command \"%s\" does not accepts operands \n", lineCounter, CMD[cmdIndex].cmdName);
        return FALSE;
    }
    else if(params != 0 && line[*i] == '\0'){/*no operands given to a command expecting paramaters*/
        printf("ERROR:%d: command \"%s\" accepts %d operand%s, none given \n",
               lineCounter,
               CMD[cmdIndex].cmdName,
               params,
               (params > 1) ? "s" : "");
        return FALSE;
    }
    return TRUE;
}
/*validates number of commas given to a command is valid.
 *returns -1 if there are too many\little commas according to command pointed to by
 *cmdIndex and the index pointing to the comma in the line otherwise.*/
int validateCommas(const char* line, int i, int cmdIndex, int lineCounter)
{
    int commas = 0;/*number of commas counted*/
    int commaIndex;/*index pointing to comma in line*/
    int params = CMD[cmdIndex].numParams;/*number of parameters expected by command*/
    while(line[i] != '\0'){
        if (line[i] == ','){ /*Counting commas in line*/
            ++commas;
            commaIndex = i;/*point to comma*/
        }
        i++;
    }
    if(params == 1 && commas > 0){/*commas for command taking 1 operand*/
        printf("ERROR:%d: command \"%s\" accepts 1 operand, extranous comma \n",
               lineCounter,
               CMD[cmdIndex].cmdName);
        return -1;
    }
    else if(params == 2 && commas != 1){/*extra/missing commas for command taking 2 operands*/
        printf("ERROR:%d: command \"%s\" accepts 2 operands, %s \n",
               lineCounter,
               CMD[cmdIndex].cmdName,
               (commas > 1) ? "extranous comma" : "missing comma");
        return -1;
    }
    return commaIndex;
}
