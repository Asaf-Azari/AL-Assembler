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
