#include "constants.h"
typedef struct node{
    struct node*  nextPtr;
    char          label[MAXLABELSIZE+1];
    unsigned char isData;
    unsigned char isExtern;
    unsigned char isEntry;
    unsigned long  address;/*TODO: change to something with at least 21 bits*/
} node;
typedef struct list{
    node* head;
    node* tail;
} list;

long getAddress(char label[]);
int exists(char* newLabel);
int isExtern(char label[]);
int makeEntry(char label[]);
void updateSymbolTable(int dataCounter, int instCounter);
void checkSymbolTable();
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address);
void clearSymbolTable();
