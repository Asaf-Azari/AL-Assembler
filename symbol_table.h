#include "constants.h"
#include "stdlib.h"

/*list structers definitions*/
typedef struct Node{
    struct   Node*  nextPtr;
    char          label[MAXLABELSIZE+1];
    unsigned char isData;
    unsigned char isExtern;
    unsigned char isEntry;
    unsigned long  address;
} Node;
typedef struct{
    Node* head;
    Node* tail;
} List;

long getAddress(char label[]);
int exists(char* newLabel);
int isExtern(char label[]);
int areEntries();
void createEnt(FILE* ent);
int makeEntry(char label[]);
void applyAsmOffset(int dataCounter, int instCounter);
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address);
void clearSymbolTable();
