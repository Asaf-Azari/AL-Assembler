#include <ctype.h>
#include <string.h>
#include "util.h"
void getWord(char* line, int* i, int* index1, int* index2)/*TODO: add a flag variable so we can bound a word by space or by comma?*/
{
    while(isspace(line[*i])) 
            ++*i;
    *index1 = *i;
    while(!isspace(line[*i]) && line[*i] != '\0') 
        ++*i;
    *index2 = (line[*index1] == '\0')? *i :*i-1; /*when no word have been found*/
}
void storeWord(Token* t, const char* line, int len)
{
    t->len = len;
    strncpy(t->currentWord, line, len);
    t->currentWord[len] = '\0';/*Apparently strncpy doesn't append a null byte*/
}
