#include<stdio.h>
#include<stdlib.h>

struct symbolTable {
    int value;
    char *symbol;
};

int findSymTabLen(FILE *inputFile);

void fillSymTab(struct symbolTable *symT, FILE *inputFile);

int main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();
    if (argc < 3) {
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if ((assp = fopen(argv[1], "r")) == NULL) {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((machp = fopen(argv[2], "w+")) == NULL) {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }


    int symTabLen = findSymTabLen(assp);
    struct symbolTable *pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char *) malloc(10);
    fillSymTab(pSymTab, assp);

    // here you can place your code for the assembler

    fclose(assp);
    fclose(machp);
    return 0;
}

int findSymTabLen(FILE *inputFile) {
    int count = 0;
    size_t lineSize;
    char *line;
    // write code to find the number of symbols in the assembly program
    free(line);
    return count;
}

void fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    size_t lineSize;
    char *line;
    int i = 0;
    // write code to struct the symbol table
    free(line);
}
