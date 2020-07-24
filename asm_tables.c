#include "asm_tables.h"

const COMMANDS CMD[] = {
    {"mov", 0, 0, 2, OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"cmp", 1, 0, 2, OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_IMMEDIATE|OP2_LABEL|OP2_REG},
    {"add", 2, 1, 2, OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"sub", 2, 2, 2, OP1_IMMEDIATE|OP1_LABEL|OP1_REG|OP2_LABEL|OP2_REG},
    {"lea", 4, 0, 2, OP1_LABEL|OP2_LABEL|OP2_REG},
    {"clr", 5, 1, 1, OP2_LABEL|OP2_REG},
    {"not", 5, 2, 1, OP2_LABEL|OP2_REG},
    {"inc", 5, 3, 1, OP2_LABEL|OP2_REG},
    {"dec", 5, 4, 1, OP2_LABEL|OP2_REG},
    {"jmp", 9, 1, 1, OP2_LABEL|OP2_RELATIVE},
    {"bne", 9, 2, 1, OP2_LABEL|OP2_RELATIVE},
    {"jsr", 9, 3, 1, OP2_LABEL|OP2_RELATIVE},
    {"red", 12, 0, 1, OP2_LABEL|OP2_REG},
    {"prn", 13, 0, 1, OP2_IMMEDIATE|OP2_LABEL|OP2_REG},
    {"rts", 14, 0, 0, NO_OP},
    {"stop", 15, 0, 0, NO_OP}
    };