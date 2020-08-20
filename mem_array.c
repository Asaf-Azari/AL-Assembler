#include <stdlib.h>
#include "mem_array.h"
/*Creates an array of counter elements of type memWord.
 *Callee need to check against NULL for memory allocation error.*/
unsigned long* createArr(int counter){
    return (unsigned long*)calloc(counter, sizeof(unsigned long));
}
/*Releases memWord array memory*/
void freeArr(unsigned long* arr){
    free(arr);
}
 
