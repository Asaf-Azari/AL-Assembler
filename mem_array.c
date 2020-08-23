#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem_array.h"

/*Creates an array with <counter> number of unsigned long type.*/
unsigned long* createArr(int counter)
{
    unsigned long* ptr =  (unsigned long*)calloc(counter, sizeof(unsigned long));
    if(ptr == NULL){
        printf("MEM_ERROR: Could not allocate memory for program picture\n");
        printf("Terminating program...\n");
        exit(1);
    }
    return ptr;
}
/*Free memory pictures' memory.*/ 
void resetPicture(encodedAsm* pic)
{
    free(pic->arr);
    pic->arr = NULL;
    pic->counter = 0;
}


/*====================================================================*/
/*Linked list with nodes containing dynamic arrays.
 *each array holds extern label references in code for later usage in printing a .ext file.
 *User will not interact with the structure directly as it is an implementation detail.*/
static ExternList extList;

/*Used by addExternLabel to increment a number of references a label has in
 *code(and save the reference address) if it is already in the list.
 *The method reallocates (1.5 * original_size) when the array is
 *full to acheive amotrized reallocating cost.*/
static void incrementLabel(ExternLabel* l, unsigned long newAddress)
{
    if(++l->addresses.counter == l->addresses.size){/*if additional reference incurs memory reallocation*/
        /*reallocate*/
        unsigned long newSize = l->addresses.size * 1.5;
        unsigned long* newArr = (unsigned long*)realloc(l->addresses.arr, newSize);
        if(newArr == NULL){/*reallocation failed*/
            printf("MEM_ERROR: Could not allocate memory for externList\n");
            printf("Terminating program...\n");
            exit(1);
        }
        l->addresses.arr = newArr;
        l->addresses.size = newSize;
    }
    l->addresses.arr[l->addresses.counter] = newAddress;/*save new reference*/
}
/*Used by the user of the list to add a label reference at a given address.*/
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
    new = (ExternLabel*)malloc(sizeof(ExternLabel));/*does not exist in list*/
    if(new == NULL){
        printf("MEM_ERROR: Could not allocate memory for extern label\n");
        printf("Terminating program...\n");
        exit(1);
    }
    /*label initialization*/
    strcpy(new->label, label);
    new->addresses.arr = (unsigned long*)malloc(sizeof(unsigned long) * BASE_EXTERN_SIZE);
    if(new->addresses.arr == NULL){
        printf("MEM_ERROR: Could not allocate memory for extern label\n");
        printf("Terminating program...\n");
        exit(1);
    }
    new->addresses.counter = 0;
    new->addresses.arr[0] = address;
    new->addresses.size = BASE_EXTERN_SIZE;
    new->nextLabel = NULL;
    if(extList.head == NULL){/*external list is empty*/
        extList.head = new;
        extList.tail = new;
    }
    else{/*append to external list*/
        extList.tail->nextLabel = new;
        extList.tail = new;
    }
} 
/*Given a file, prints addresses in code in which external
 *symbols are referenced*/
void createExt(FILE* ext)
{
    ExternLabel* l = extList.head;
    while(l != NULL){/*no external declerations*/
        int i;
        for(i = 0; i <= l->addresses.counter; ++i){/*iterate over references for a given label*/
            fprintf(ext, "%s %.07lu\n", l->label, l->addresses.arr[i]);
        }
        l = l->nextLabel;
    }
}

/*Returns true if there are extern declerations in the code
 *and false otherwise.*/
int areExterns(){
    return extList.head != NULL;
}
/*Iterates over each extern label and frees its array of references and
 *then the node containing it.*/
void clearExternalList()
{
    ExternLabel* current;
    ExternLabel* n = extList.head;
    while(n != NULL){
        current = n;
        n = n->nextLabel;
        free(current->addresses.arr);/*free array*/
        free(current);/*free node*/
    }
    extList.head = NULL;
    extList.tail = NULL;
}
