/*Might need to change the name, but I think bundeling them together
 *makes it easier to pass into secondPass.*/
typedef struct{
    int counter;
    unsigned long* arr;
} encodedAsm;
unsigned long* createArr(int counter);
void freeArr(unsigned long* arr);
