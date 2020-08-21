#include "asm_tables.h"
#include <stdio.h>

#define CREATE_MASK(op, funct) ((op << 18) | (funct << 3) | 4)

const COMMANDS CMD[] = {
    {"mov",  CREATE_MASK(0, 0), 2,   OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"cmp",  CREATE_MASK(1, 0), 2,   OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_IMMEDIATE|OP2_LABEL|OP2_REG},
    {"add",  CREATE_MASK(2, 1), 2,   OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"sub",  CREATE_MASK(2, 2), 2,   OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"lea",  CREATE_MASK(4, 0), 2,   OP1_LABEL|OP2_LABEL|OP2_REG},
    {"clr",  CREATE_MASK(5, 1), 1,   OP1_LABEL|OP1_REG},
    {"not",  CREATE_MASK(5, 2), 1,   OP1_LABEL|OP1_REG},
    {"inc",  CREATE_MASK(5, 3), 1,   OP1_LABEL|OP1_REG},
    {"dec",  CREATE_MASK(5, 4), 1,   OP1_LABEL|OP1_REG},
    {"jmp",  CREATE_MASK(9, 1), 1,   OP1_LABEL|OP1_RELATIVE},
    {"bne",  CREATE_MASK(9, 2), 1,   OP1_LABEL|OP1_RELATIVE},
    {"jsr",  CREATE_MASK(9, 3), 1,   OP1_LABEL|OP1_RELATIVE},
    {"red",  CREATE_MASK(12, 0), 1,  OP1_LABEL|OP1_REG},
    {"prn",  CREATE_MASK(13, 0), 1,  OP1_IMMEDIATE|OP1_LABEL|OP1_REG},
    {"rts",  CREATE_MASK(14, 0), 0,  NO_OP},
    {"stop", CREATE_MASK(15, 0), 0,  NO_OP},
    {NULL,   CREATE_MASK(0, 0), 0,   NO_OP}
    };
