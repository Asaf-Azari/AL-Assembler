#include <stdlib.h>
/*Creates an array of counter elements of type unsigned long.
 *Callee need to check against NULL for memory allocation error.*/
unsigned long* createArr(int counter){
    return (unsigned long*)malloc(counter * sizeof(unsigned long));
}
/*Releases unsigned long array memory*/
void freeArr(unsigned long* arr){
    free(arr);
}
