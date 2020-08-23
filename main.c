/*Authors: Asaf Azari and Lior Merkel Golan
 *This program is an assembler for an assembly language defined in maman 14,
 *semester 2020b leaflet.
 *The assembler was written with the -ansi flag in mind(c90 standard) and so
 *it tries to be compliant as much as possible with the c90 ansi standard.
 * 
 *The assembler works in two passes, the first checks for syntax errors,
 *asserts that there are no illegal commands in the assembly program,
 *saves the labels used in the assembly program and adjusts counters
 *to later encode the instructions.
 *The second pass attempts to encode the assembly instructions and further validating
 *that the assembly program is legal.

 *If both passes are completed sucssessfully, the assembler will try to create
 *output files that contain the linkable\loadable version of our program along with linking
 *information(external label references and entry information for other assembly
 *source code) if there is any.
 *
 *The second pass cannot start if the first pass failed and likewise output
 *creation cannot occur if the second pass failed.
 *
 *Throughout the assembling proccess, the assembler will raise errors if
 *it encountered any.
 **/
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
    FILE* fp = NULL;/*current file*/
    inst.arr = NULL;/*instruction memory picture*/
    data.arr = NULL;/*data memory picture*/

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
