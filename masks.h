#ifndef ASM_MASKS
#define ASM_MASKS 

#define ENCODE_ADDRESS_REG(reg, adder, shift) ( ((adder << 3)|(reg)) << shift )
#define ENCODE_NUM(num) ((num << 3) | 4)

#endif
