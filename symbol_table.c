/** LINKED LIST 
 * 1.  pointer to next item
 * 2.  label
 * 3.  data/instruction
 * 4.  extern/not extern
 * 5.  address**/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "asm_tables.h"
#include "symbol_table.h"


static list l;
/*TODO: Generally test functions.*/

/*Checks if label exists in symbol table*/
int exists(char newLabel[])
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, newLabel))
            return TRUE;
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}

int isExtern(char Label[])
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, Label))
            return currentNode->isExtern; /*TODO: make sure this returns by value*/ 
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}

int makeEntry(char Label[])
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, Label)){
            currentNode->isEntry = TRUE;
            return TRUE;
        }
            
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}
/*append node to the list, if the list is empty, appoint node as head*/
void addNode(node* n)
{
    if(l.head == NULL){
        l.head = n;
        l.tail = n;
    }
    else{
        l.tail->nextPtr = n;
        l.tail = n;
    }
}
/*Creates node to be added to list*/
/*TOOD: check if can return by value. probably not considering it should be freed as scope ends*/
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address)
{
    node* newNode = (node*) malloc(sizeof(node));
    strcpy(newNode->label, nodeLabel);
    newNode->isData = isData;
    newNode->isExtern = isExtern;
    newNode->address = address;
    newNode->isEntry = FALSE; /*TODO: function to change a nodes status as enry as it is only added at the second pass*/
    addNode(newNode);
}

void updateSymbolTable(int instCounter, int dataCounter)
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        if(!currentNode->isExtern)
            currentNode->address += (currentNode->isData)? STARTADDRESS + instCounter : STARTADDRESS;
        currentNode = currentNode->nextPtr;
    }
}

void clearSymbolTable()
{
    node* current;
    node* n = l.head;
    while(n != NULL){
        current = n;
        n = n->nextPtr;
        free(current);
    }
    l.tail = NULL;
    l.head = NULL;
}

/*just for testing*/
void checkSymbolTable()
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        printf("label - %s , is data %c , is extern %c, is entry %c, address - %d\n", currentNode->label, currentNode->isData, currentNode->isExtern, currentNode->isEntry, currentNode->address);
        currentNode = currentNode->nextPtr;
    }
}