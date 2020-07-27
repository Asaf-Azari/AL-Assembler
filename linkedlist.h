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

int exists(char* newLabel);
void appendNode(node* n);
void clearAndInitList();