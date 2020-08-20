#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem_array.h"
/*Creates an array of counter elements of type memWord.
 *Callee need to check against NULL for memory allocation error.*/
unsigned long* createArr(int counter){
    return (unsigned long*)calloc(counter, sizeof(unsigned long));
}
void resetPicture(encodedAsm* pic){
    free(pic->arr);
    pic->arr = NULL;
    pic->counter = 0;
}

static ExternList extList;
/*List to keep track of mention of external labels*/
void addExternLabel(char* label, unsigned long address)
{
    ExternLabel* new = malloc(sizeof(ExternLabel));
    if(new == NULL){
        printf("MEM_ERROR: Could not allocate memory for extern label\n");
        printf("Terminating program...\n");
        exit(1);
    }
    strcpy(new->label, label);
    new->address = address;
    new->nextLabel = NULL;
    addToList(new);
} 
void addToList(ExternLabel* label)
{
    if(extList.head == NULL){
        extList.head = label;
        extList.tail = label;
    }
    else{
        extList.tail->nextLabel = label;
        extList.tail = label;
    }
}

void createExt(FILE* ext)
{
    ExternLabel* l = extList.head;
    while(l != NULL){
        fprintf(ext, "%s %.07lu\n", l->label, l->address);
        l = l->nextLabel;
    }
}
void printList()
{
    ExternLabel* l = extList.head;
    while(l != NULL){
        printf("%s %.07lu\n", l->label, l->address);
        l = l->nextLabel;
    }
}

int noExtern(){
    return extList.head == NULL;
}
void clearExternalList()
{
    ExternLabel* current;
    ExternLabel* n = extList.head;
    while(n != NULL){
        current = n;
        n = n->nextLabel;
        free(current);
    }
    extList.tail = NULL;
    extList.head = NULL;
}