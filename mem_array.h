#ifndef ASM_MEM_ARRAY
#define ASM_MEM_ARRAY
#include "constants.h"
#include "stdlib.h"
typedef struct{/*Holding memory picture along with associated counter*/
    int counter;
    unsigned long* arr;
} encodedAsm;
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
void addExternLabel(char* label, unsigned long address);
void incrementLabel(ExternLabel* l, unsigned long newAddress);
void addToList(ExternLabel* label);
void printList();
void createExt(FILE* ext);
int noExtern();
unsigned long* createArr(int counter);
void resetPicture(encodedAsm* pic);
void clearExternalList();
#endif
