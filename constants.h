/*This file contains constants used across the program.*/
#ifndef ASM_CONSTANTS

#define ASM_CONSTANTS

#define MAX_LINE_LENGTH 80 
#define MAXLABELSIZE 31
#define MAXOPTSIZE 7
#define MAXPARAM 2
#define MAXADDRESS 2097151
#define STARTADDRESS 100

#define FALSE 0
#define TRUE 1

/*maximum integer sizes*/
#define ASM_INST_MIN_INT -1048576/* (2^21)/2 */
#define ASM_INST_MAX_INT  1048575/* ((2^21)/2) - 1 */

#define ASM_DATA_MIN_INT  -8388608/* (2^24)/2 */
#define ASM_DATA_MAX_INT  8388607 /* ((2^24)/2) - 1 */

#endif
