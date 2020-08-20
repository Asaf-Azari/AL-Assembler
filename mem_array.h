typedef struct{
    unsigned int memory : 24;
} memWord;
typedef struct{/*Holding memory picture along with associated counter*/
    int counter;
    memWord* arr;
} encodedAsm;
memWord* createArr(int counter);
void freeArr(memWord* arr);
