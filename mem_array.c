#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem_array.h"
/*Creates an array of counter elements of type memWord.
 *Callee need to check against NULL for memory allocation error.*/
unsigned long* createArr(int counter){
    unsigned long* ptr =  (unsigned long*)calloc(counter, sizeof(unsigned long));
    if(ptr == NULL){
        printf("MEM_ERROR: Could not allocate memory for program picture\n");
        printf("Terminating program...\n");
        exit(1);
    }
    return ptr;
}
void resetPicture(encodedAsm* pic){
    free(pic->arr);
    pic->arr = NULL;
    pic->counter = 0;
}


/*====================================================================*/
static ExternList extList;
/*List to keep track of mention of external labels*/
static void incrementLabel(ExternLabel* l, unsigned long newAddress)
{
    if(++l->addresses.counter == l->addresses.size){
        /*Reallocate*/
        unsigned long newSize = l->addresses.size * 1.5;
        unsigned long* newArr = (unsigned long*)realloc(l->addresses.arr, newSize);
        if(newArr == NULL){
            printf("MEM_ERROR: Could not allocate memory for externList\n");
            printf("Terminating program...\n");
            exit(1);
        }
        l->addresses.arr = newArr;
        l->addresses.size = newSize;
    }
    l->addresses.arr[l->addresses.counter] = newAddress;
}
void addExternLabel(char* label, unsigned long address)
{
    ExternLabel* current = extList.head;
    ExternLabel* new;
    while(current != NULL){
        if(!strcmp(current->label, label)){
            incrementLabel(current, address);
            return;
        }
        current = current->nextLabel;
    }
    new = (ExternLabel*)malloc(sizeof(ExternLabel));
    if(new == NULL){
        printf("MEM_ERROR: Could not allocate memory for extern label\n");
        printf("Terminating program...\n");
        exit(1);
    }
    strcpy(new->label, label);
    /*Some basic size*/
    new->addresses.arr = (unsigned long*)malloc(sizeof(unsigned long) * 10);
    if(new->addresses.arr == NULL){
        printf("MEM_ERROR: Could not allocate memory for extern label\n");
        printf("Terminating program...\n");
        exit(1);
    }
    new->addresses.counter = 0;
    new->addresses.arr[0] = address;
    new->addresses.size = 10;
    new->nextLabel = NULL;
    if(extList.head == NULL){
        extList.head = new;
        extList.tail = new;
    }
    else{
        extList.tail->nextLabel = new;
        extList.tail = new;
    }
} 
#if 0 
void addToList(ExternLabel* label)
{
    if(extList.head == NULL){
        extList.head = label;
    }
    else{
        extList.tail->nextLabel = label;
        extList.tail = label;
    }
}
#endif

void createExt(FILE* ext)
{
    ExternLabel* l = extList.head;
    while(l != NULL){
        int i;
        for(i = 0; i <= l->addresses.counter; ++i){
            fprintf(ext, "%s %.07lu\n", l->label, l->addresses.arr[i]);
        }
        l = l->nextLabel;
    }
}
#if 0
void printList()
{
    ExternLabel* l = extList.head;
    while(l != NULL){
        printf("%s %.07lu\n", l->label, l->address);
        l = l->nextLabel;
    }
}
#endif

int areExterns(){
    return extList.head != NULL;
}
void clearExternalList()
{
    ExternLabel* current;
    ExternLabel* n = extList.head;
    while(n != NULL){
        current = n;
        n = n->nextLabel;
        free(current->addresses.arr);
        free(current);
    }
    extList.head = NULL;
    extList.tail = NULL;
}
