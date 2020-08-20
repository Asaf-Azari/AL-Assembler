#ifndef ASM_MASKS
#define ASM_MASKS 

#define FIRST24 16777215UL /*2^24-1*/
#define ENCODE_METHOD_REG(reg, adder, shift) ( (((unsigned long)adder << 3UL)|((unsigned long)reg)) << (unsigned long)shift )
#define ENCODE_WORD_ADDRESS(address, isExtern) (((unsigned long)!isExtern +1UL) | (address << 3)) 
#define ENCODE_WORD_NUM(num) ( (((unsigned long)num << 3UL) | 4UL) & FIRST24 )
#define ENCODE_DATA_STRING(c) (unsigned long)c & FIRST24
#define ENCODE_DATA_NUM(num) (unsigned long)num & FIRST24
#define ENCODE_JUMP_DISTANCE(dest, src) (((unsigned long)(dest - (src)) << 3) | 4) & FIRST24

#endif
