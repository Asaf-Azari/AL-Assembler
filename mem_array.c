#include <stdlib.h>
#include "mem_array.h"
/*Creates an array of counter elements of type memWord.
 *Callee need to check against NULL for memory allocation error.*/
memWord* createArr(int counter){
    return (memWord*)calloc(counter, sizeof(memWord));
}
/*Releases memWord array memory*/
void freeArr(memWord* arr){
    free(arr);
}
 
