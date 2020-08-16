#include "constants.h"
typedef struct node{
    struct node*  nextPtr;
    char          label[MAXLABELSIZE];
    unsigned char isData;
    unsigned char isExtern;
    unsigned char isEntry;
    unsigned int  address;/*TODO: maybe signed*/
} node;
typedef struct list{
    node* head;
    node* tail;
} list;

int exists(char* newLabel);
int isExtern(char Label[]);
int makeEntry(char Label[]);
void updateSymbolTable(int instCounter, int dataCounter);
void checkSymbolTable();
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address);
void clearSymbolTable();
