/*This program is an assemblerhedy356ywtyhgh*/

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
        return 0;
    }
    for(i = 1; i < argc; ++i){/*Main program loop*/
        if(fp != NULL){
            freeMemory(fp, &inst, &data);
            
        }
        if(!(fp = getFile(argv[i], AS))){
           printf("\n@@@@@@@@@@ Cannot open input file: %s.as,  skipping @@@@@@@@@@ \n", argv[i]);
           continue;
        }
        printf("\n@@@@@@@@@@ parsing file %s.as @@@@@@@@@@ \n", argv[i]);
        
        
        if(!firstPass(fp, &data.counter, &inst.counter)){
            printf("@@@@@@@@@@ Errors found in file %s.as, skipping @@@@@@@@@@ \n", argv[i]);
            continue;
        }
        applyAsmOffset(data.counter, inst.counter);

        rewind(fp);
        inst.arr = createArr(inst.counter);
        data.arr = createArr(data.counter);

        if(!secondPass(fp, &data, &inst)){
            printf("@@@@@@@@@@ Errors found in file %s.as, skipping @@@@@@@@@@ \n", argv[i]);
            continue;
        }
        printf("@@@@@@@@@@ %s.as transpiled successfully, outputting files @@@@@@@@@@ \n", argv[i]);
        createOuput(argv[i], &inst, &data);

    }
    return 0;
}
