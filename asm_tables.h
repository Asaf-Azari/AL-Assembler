#define NO_OP 0

#define OP1_IMMEDIATE 1
#define OP1_LABEL 2
#define OP1_RELATIVE 4
#define OP1_REG 8

#define OP2_IMMEDIATE 16
#define OP2_LABEL 32
#define OP2_RELATIVE 64
#define OP2_REG 128

typedef struct COMMANDS{
    char*         cmdName;
    unsigned char opCode;
    unsigned char funct;
    unsigned char numParams;

    unsigned char viableOperands;  /*need to define flags in enum/defines */
    /*0 = IMMEDIATE
     *1 = LABEL
     *2 = RELATIVE
     *3 = REG*/
} COMMANDS;
extern const COMMANDS CMD[];
