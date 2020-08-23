#ifndef ASM_MEM_ARRAY
#define ASM_MEM_ARRAY
#include "constants.h"
#include "stdlib.h"

#define BASE_EXTERN_SIZE 10
typedef struct{/*Holding memory picture along with associated counter*/
    int counter;
    unsigned long* arr;
} encodedAsm;

/*extern list struct definitions*/
typedef struct ExternLabel{
    struct{
        unsigned long  size;
        unsigned long  counter;
        unsigned long* arr;
    }addresses;
    struct ExternLabel* nextLabel;
    char label[MAXLABELSIZE + 1];
} ExternLabel;
typedef struct externList{
    ExternLabel* head;
    ExternLabel* tail;
} ExternList;
/*Used by the user of the list to add a label reference at a given address.*/
void addExternLabel(char* label, unsigned long address);
/*Given a file, prints addresses in code in which external
 *symbols are referenced*/
void createExt(FILE* ext);
/*Returns true if there are extern declerations in the code
 *and false otherwise.*/
int areExterns();
/*Creates an array with <counter> number of unsigned long type.*/
unsigned long* createArr(int counter);
/*Free memory pictures' memory.*/
void resetPicture(encodedAsm* pic);
/*Iterates over each extern label and frees its array of references and
 *then the node containing it.*/
void clearExternalList();
#endif
