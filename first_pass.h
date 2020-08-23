#include <stdio.h>
#include "asm_tables.h"

enum{/*denoting which assembly instruction was found*/
    ERROR,
    DATA,
    STRING,
    ENTRY,
    EXTERN
} asmInst;
/*First pass of our assembler.
 *first pass checks for syntax errors, add our programs'
 *labels to the symbol table and adjust our instruction
 *and data counters to allow later allocation of memory for memory pictures.
 *returns false if any errors were met and true otherwise.
 */
int firstPass(FILE* fp, int* dataCounter, int* instCounter);
/*asserts that a given label is a valid label.
 *a label can have up to 31 characters, begin with a letter 
 *from the range <a-z>/<A-Z> and must be fully alphanumeric.
 *returns false if errors were met and true otherwise.*/
int isValidLabel(char* word, int wordLen, int lineCounter);
/*asserts that word is a valid assembly instruction.
 *Must be one of the following: .data, .string, .entry, .extern
 *returns an enum value according to the type of instruction.*/
int isValidAsmOpt(char* asmOpt, int lineCounter);
/*asserts that a given char* represents a valid integer
 *number between a range dictated by isData.
 *if numSuffix != NULL, the function points *numSuffix to the text after the number.
 *returns false if errors were met and true otherwise.*/
int isNum(const char* line, char** numSuffix, char isData, int lineCounter);
/*Verifies that there exists a single operand for each side of the comma(if there's a comma).
 *returns false if no tokens were read or more than 1 tokens were read and true otherwise.*/
int singleToken(const char* line, int params, int lineCounter);
/*Verifies that there exists a single operand for each side of the comma(if there's a comma).
 *returns false if no tokens were read or more than 1 tokens were read and true otherwise.*/
int verifyOperand(const char* line, const COMMANDS* cmd, int params, int* instCounter, int lineCounter);
/*asserts that a given char* represents a valid string(after a .string instruction).
 *-1 return value indicates there was an error, otherwise returns number of characters
 *inside the string(including null terminating byte).*/
int validateString(const char* line, int start, int end, int lineCounter);
/*validates legality of .data arguments.
 *returns -1 to indicate error, otherwise returns number of arguments read.*/
int validateData(const char* line, int i, int lineCounter);
/*validates number of operands given to a command is valid.
 *returns false if given too many\little operands according to command pointed to by
 *cmdIndex and true otherwise.*/
int validateOpNum(const char* line, int* i, int cmdIndex, int lineCounter);
/*validates number of commas given to a command is valid.
 *returns -1 if there are too many\little commas according to command pointed to by
 *cmdIndex and the index pointing to the comma in the line otherwise.*/
int validateCommas(const char* line, int i, int cmdIndex, int lineCounter);
