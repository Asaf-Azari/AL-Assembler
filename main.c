#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "symbol_table.h"
#include "mem_array.h"
#include "asm_tables.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_handling.h"


int main(int argc, char** argv)
{
    int i;
    encodedAsm inst, data;
    FILE* fp = NULL;
    inst.arr = NULL;
    data.arr = NULL;

    if(argc == 1){/*no arguments*/
        printf("ERROR: No arguments supplied\n");
        return 0;/*TODO: Return 1?*/
    }
    for(i = 1; i < argc; ++i){/*Main program loop*/
        if(fp != NULL){
            freeMemory(fp, &inst, &data);
            
        }
        if(!(fp = getFile(argv[i], AS))){/*TODO: fclose after done? */
           printf("ERROR: Cannot open input file: %s.as,  skipping \n", argv[i]);
           continue;

        }
        
        if(!firstPass(fp, &data.counter, &inst.counter)){
            printf("Errors found in file %s.as, skipping \n", argv[i]);
            fclose(fp);
            continue;
        }
        applyAsmOffset(data.counter, inst.counter);

        rewind(fp);
        inst.arr = createArr(inst.counter);
        data.arr = createArr(data.counter);
        if(inst.arr == NULL || data.arr == NULL){
            printf("MEM_ERROR: Could not allocate memory for %s\n", 
                    inst.arr == NULL ? "instruction picture" : "data picture");
            printf("Terminating program...\n");
            clearSymbolTable();
            fclose(fp);
            /*TODO: free inst/data?*/
            return 0;/*TODO: Return 1?*/
        }
        if(!secondPass(fp, &data, &inst)){
            printf("Errors found in file %s.as, skipping \n", argv[i]);
            continue;
        }
        createOuput(argv[i], &inst, &data);
    }
    return 0;
}
