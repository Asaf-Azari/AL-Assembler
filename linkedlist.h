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

int exists(list* l, char* newLabel);
void appendNode(list* l, node* n);