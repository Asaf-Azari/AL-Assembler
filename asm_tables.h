#ifndef ASM_TABLES
#define ASM_TABLES

/*macros for defining a commands' legal operands.
 *used to create a bitmask denoting legal operands.*/
#define NO_OP 0
#define OP1_IMMEDIATE 1
#define OP1_LABEL 2
#define OP1_RELATIVE 4
#define OP1_REG 8

#define OP2_IMMEDIATE 16
#define OP2_LABEL 32
#define OP2_RELATIVE 64
#define OP2_REG 128

/*structure holding important information about a given
 *command. used in the CMD aray to give program-wide info
 *about the assembly instructions*/
typedef struct COMMANDS{
    char*         cmdName;/*command name*/
    unsigned long mask;/*basic bit pattern for command*/
    unsigned char numParams;/*number of parameters this command expects*/

    unsigned char viableOperands;/*mask denoting viable operands*/
    /*Addresing methods:
     *0 = IMMEDIATE
     *1 = LABEL
     *2 = RELATIVE
     *3 = REG*/
} COMMANDS;
/*including the extern decleration in the header ensures access to the CMD array
 *in every file including this header*/
extern const COMMANDS CMD[];
#endif
