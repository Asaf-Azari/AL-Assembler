/** LINKED LIST 
 * 1.  pointer to next item
 * 2.  label
 * 3.  data/instruction
 * 4.  extern/not extern
 * 5.  address**/

/*TODO: move MAXADDRESS?*/
#define MAXADDRESS 2097151
#include <string.h>
#include "linkedlist.h" /* TODO: change to symboltable.h maybe?*/

static list l;
/*TODO: Generally test functions.
/*Checks if label exists in symbol table*/
int exists(char newLabel[])
{
    node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, newLabel))
            return 1;
        currentNode = currentNode->nextPtr;
    }
    return 0;
}

/*append node to the list, if the list is empty, appoint node as head*/
void addNode(node* n)
{
    if(l.head == NULL){
        l.head = n;
        l.tail = n;
    }
    l.tail->nextPtr = n;
    l.tail = n;
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
    addNode(&newNode);
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