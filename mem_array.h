typedef struct{/*Holding memory picture along with associated counter*/
    int counter;
    unsigned long* arr;
} encodedAsm;
unsigned long* createArr(int counter);
void freeArr(unsigned long* arr);
