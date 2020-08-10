#define MAXLABELSIZE 31
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
void addLabel(char* nodeLabel, unsigned char isData, unsigned char isExtern, unsigned int address);
void clearSymbolTable();
