/*this file contains general utilities for parsing and manipulating text*/ 
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "asm_tables.h"
/*Sets given indicies to the bounds of a word starting at i*/
void getWord(char* line, int* i, int* index1, int* index2)
{
    while(isspace(line[*i])) 
            ++*i;
    *index1 = *i;/*first character in word*/
    while(!isspace(line[*i]) && line[*i] != '\0') 
        ++*i;
    *index2 = (line[*index1] == '\0')? *i :*i-1; /*when no word have been found*/
}
/*stores a substring given by index and length in a Token datatype*/
void storeWord(Token* t, const char* line, int len)
{
    t->len = len;
    strncpy(t->currentWord, line, len);
    t->currentWord[len] = '\0';/*strncpy doesn't append a null byte*/
}
/*returns command index from the constant command table and -1 if it doesn't exists*/
int isOp(char* op)
{   
    int i;
    for(i = 0; CMD[i].cmdName; i++){
        if (!strcmp(op,CMD[i].cmdName))
            return i;  
    }
    return -1;
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
/*Checks if a given bounded word is a register*/
int isReg(const char* line)
{
    return (line[0] == 'r') && line[1] >= '0' && line[1] <= '7' && (isspace(line[2]) || (line[2] == ',' || line[2] == '\0'));
}
