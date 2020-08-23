/* TODO This program is an assemblerhedy356ywtyhgh*/
/* TODO grep for TODOs #META*/
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
    /*structures holding instruction and data pictures along with their counters*/
    encodedAsm inst, data;
    FILE* fp = NULL;
    inst.arr = NULL;
    data.arr = NULL;

    if(argc == 1){/*no arguments*/
        printf("ERROR: No arguments supplied\n");
        return 0;
    }
    for(i = 1; i < argc; ++i){/*Iterating over input files*/
        if(fp != NULL){/*If last file was valid, clean memory*/
            freeMemory(fp, &inst, &data);
        }
        if(!(fp = getFile(argv[i], AS))){
           printf("\n@@@@@@@@@@ Cannot open input file: %s.as,  skipping @@@@@@@@@@ \n", argv[i]);
           continue;
        }
        printf("\n@@@@@@@@@@ parsing file %s.as @@@@@@@@@@ \n", argv[i]);
        
        
        if(!firstPass(fp, &data.counter, &inst.counter)){/*if no errors were found on first pass*/
            printf("@@@@@@@@@@ Errors found in file %s.as, skipping @@@@@@@@@@ \n", argv[i]);
            continue;
        }
        applyAsmOffset(data.counter, inst.counter);/*adjust memory addresses to start from 100*/

        rewind(fp);/*rewind file handler to re-read content*/
        /*allocate memory for pictures*/
        inst.arr = createArr(inst.counter);
        data.arr = createArr(data.counter);

        if(!secondPass(fp, &data, &inst)){/*if no errors were found on second pass*/
            printf("@@@@@@@@@@ Errors found in file %s.as, skipping @@@@@@@@@@ \n", argv[i]);
            continue;
        }
        printf("@@@@@@@@@@ %s.as transpiled successfully, outputting files @@@@@@@@@@ \n", argv[i]);
        createOuput(argv[i], &inst, &data);/*create output files*/

    }
    return 0;
}
