#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_LINE_LENGTH 80
#define TRUE 1
#define FALSE 0
/*
read line
skip whitespace */
int isKeyword(char* word)
{
}
int firstPass(FILE* fp)
{
    char line[MAX_LINE_LENGTH + 2];
    int lineCounter = 0, dataCounter = 0, instCounter = 0;
    int i = 0, lineLen;
    int index1, index2;
    char errorFlag = FALSE;
    char labelFlag;

    while(fgets(line, MAX_LINE_LENGTH + 2, fp)){
        labelFlag = FALSE;
        lineLen = strlen(line);
        ++lineCounter;

        while(isspace(line[i])) ++i;
        if(line[i] == ';' || line[i] == '\0'){
            continue;
        }

        index1 = i;
        while(!isspace(line[i]) && line[i] != '\0') ++i;
        index2 = i-1;

/*isValidLabel:
check for valid name
check if already in list
check if saved keyword
*/

        if(line[index2] == ':')
            if (isValidLabel(line, index1, index2));
                labelFlag = TRUE;

        if(line[index1] == '.')
            if (isValidAsmOpt(line, index1, index2));
        else
            if(isOp(line, index1, index2));

    }
}

