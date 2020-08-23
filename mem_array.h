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

void addExternLabel(char* label, unsigned long address);
void createExt(FILE* ext);
int areExterns();
unsigned long* createArr(int counter);
void resetPicture(encodedAsm* pic);
void clearExternalList();
#endif
