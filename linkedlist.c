/** LINKED LIST 
 * 1.  pointer to next item
 * 2.  label
 * 3.  data/instruction
 * 4.  extern/not extern
 * 5.  address**/

/*TODO: move MAXADDRESS?*/
#define MAXADDRESS 2097151
#include <string.h>

/*Linked list structures*/
typedef struct node{
    struct node*  nextPtr;
    char*         label;
    unsigned char isData;
    unsigned char isExtern;
    unsigned int  address;/*TODO: maybe signed*/
} node;
typedef struct list{
    node* head;
    node* tail;
} list;
/*TODO: Generally test functions.

/*Checks if label exists in symbol table*/
int exists(list* l, char* newLabel)
{
    node* currentNode = l->head;
    while(currentNode != NULL){
        if(!strcmp(currentNode->label, newLabel))
            return 1;
        currentNode = currentNode->nextPtr;
    }
    return 0;
}
/*Creates node to be added to list*/
/*TOOD: check if can return by value. probably not considering it should be freed as scope ends
 *TODO: remember to check that label isn't one of the saved keywords i.e r1, r2 etc.*/
/*node* createNode(char* nodeLabel, unsigned char nodeProperties, unsigned int address)
{
    node* newNode = (node*) malloc(sizeof(node));
    newNode->label = nodeLabel;
}*/

/*append node to the list, if the list is empty, appoint node as head*/
void appendNode(list* l, node* n)
{
    if(l->head == NULL){
        l->head = n;
        l->tail = n;
    }
    l->tail->nextPtr = n;
    l->tail = n;
}