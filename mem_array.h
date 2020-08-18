/*Might need to change the name, but I think bundeling them together
 *makes it easier to pass into secondPass.*/
typedef struct{
    unsigned int memory : 24;
} memWord;
typedef struct{
    int counter;
    memWord* arr;
} encodedAsm;
memWord* createArr(int counter);
void freeArr(memWord* arr);
