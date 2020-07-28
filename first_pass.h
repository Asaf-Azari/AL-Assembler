/*TODO: we might want to actually include stdio.h here, https://stackoverflow.com/questions/29117606/how-do-i-detect-if-stdio-h-is-included*/
int isKeyword(char* word);
int firstPass(FILE* fp);
void getWord(char* line, int* i, int* index1, int* index2);
int isValidLabel(char* line, int index1, int index2, int lineCounter);
int isValidAsmOpt(char* line, int index1, int index2, int lineCounter);
int isOp(char* line, int index1, int index2);
#define MAXLABELSIZE 32
#define MAXOPTSIZE 7
enum{
    ERROR,
    DATA,
    STRING,
    ENTRY,
    EXTERN
} asmInst;
