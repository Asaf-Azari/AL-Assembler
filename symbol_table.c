#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "asm_tables.h"
#include "symbol_table.h"

/*static list decleration. users can't directly access*/
static List l;

/*Returns the memory address of label.*/
long getAddress(char label[])
{
    Node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, label))
            return currentNode->address;
        currentNode = currentNode->nextPtr;
    }
    return -1;
}
/*Checks if label exists in the symbol table*/
int exists(char newLabel[])
{
    Node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, newLabel))
            return TRUE;
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}
/*Checks if a given label is external.
 *Does NOT verify that the label exists within the table.*/
int isExtern(char label[])
{
    Node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, label))
            return currentNode->isExtern;
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}

/*Marks a label within the symbol table with entry property.*/
int makeEntry(char label[])
{
    Node* currentNode = l.head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, label)){
            currentNode->isEntry = TRUE;
            return TRUE;
        }
            
        currentNode = currentNode->nextPtr;
    }
    return FALSE;
}
/*Append node to the list, if the list is empty, appoint node as head.
 *Should not be used by used by the user as it is an implementation detail.*/
static void addNode(Node* n)
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
/*Creates a label with given parameters and adds it to the symbol table.*/
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address)
{
    Node* newNode = (Node*) malloc(sizeof(Node));
    if(newNode == NULL){
        printf("MEM_ERROR: Could not allocate memory for label \"%s\"", nodeLabel);
        printf("Terminating program...\n");
        exit(1);
    }
    strcpy(newNode->label, nodeLabel);
    newNode->isData = isData;
    newNode->isExtern = isExtern;
    newNode->address = address;
    newNode->isEntry = FALSE;
    newNode->nextPtr = NULL;
    addNode(newNode);
}

/*According to the symbol's type(inst/data), applies an address offset
 *to enable loadable assembly from memory*/
void applyAsmOffset(int dataCounter, int instCounter)
{
    Node* currentNode = l.head;
    while(currentNode != NULL){
        if(!currentNode->isExtern)
            currentNode->address += (currentNode->isData)? STARTADDRESS + instCounter : STARTADDRESS;
        currentNode = currentNode->nextPtr;
    }
}

/*Free all allocated symbols*/
void clearSymbolTable()
{
    Node* current;
    Node* n = l.head;
    while(n != NULL){
        current = n;
        n = n->nextPtr;
        free(current);
    }
    l.tail = NULL;
    l.head = NULL;
}
int areEntries(){
    Node* n = l.head;
    while(n != NULL){
        if(n->isEntry){
            return TRUE;
        }
        n = n->nextPtr;
    }
    return FALSE;
}
void createEnt(FILE* ent){
   Node* n = l.head;
    while(n != NULL){
        if(n->isEntry)
            fprintf(ent,"%s %.07ld\n", n->label,n->address);
        n = n->nextPtr;
    }
}
