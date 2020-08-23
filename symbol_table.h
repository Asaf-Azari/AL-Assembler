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
/*Returns the memory address of a label if it exists.
 *returns -1 otherwise.*/
long getAddress(char label[]);
/*Checks if label exists in the symbol table*/
int exists(char* newLabel);
/*Checks if a given label is external.
 *Does NOT verify that the label exists within the table.*/
int isExtern(char label[]);
/*returns wheter there are any entry marked labels in the table*/
int areEntries();
/*prints entry marked labels in a format appropriate to a .ent file to a given FILE* */
void createEnt(FILE* ent);
/*Marks a label within the symbol table with the entry property.*/
int makeEntry(char label[]);
/*According to the symbol's type(inst/data), applies an address offset
 *to enable loadable assembly from memory*/
void applyAsmOffset(int dataCounter, int instCounter);
/*Creates a label with given parameters and adds it to the symbol table.*/
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address);
/*Free all allocated symbols*/
void clearSymbolTable();
