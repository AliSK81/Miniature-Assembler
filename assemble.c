#include "assemble.h"


int main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();

    char *instructions[] = {"add", "sub", "slt", "or", "nand", "addi",
                            "slti", "ori", "lui", "lw", "sw", "beq", "jalr"};

    char hexTable[] = "0123456789ABCDEF";


//    if (argc < 3) {
//        printf("***** Please run this program as follows:\n");
//        printf("***** %s assprog.as machprog.m\n", argv[0]);
//        printf("***** where assprog.as is your assembly program\n");
//        printf("***** and machprog.m will be your machine code.\n");
//        exit(1);
//    }
//    if ((assp = fopen(argv[1], "r")) == NULL) {
//        printf("%s cannot be opened\n", argv[1]);
//        exit(1);
//    }
//    if ((machp = fopen(argv[2], "w+")) == NULL) {
//        printf("%s cannot be opened\n", argv[2]);
//        exit(1);
//    }


    if ((assp = fopen("..\\p.as", "r")) == NULL) {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((machp = fopen("..\\p.mc", "w+")) == NULL) {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }


    int symTabLen = findSymTabLen(assp);
    struct symbolTable *pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char *) malloc(10);


    printf("program.as:\n");
    fillSymTab(pSymTab, assp);

    printf("\n\npSymTab:\n");
    for (int i = 0; i < symTabLen; ++i) {
        printf("%d\t%s\n", pSymTab[i].value, pSymTab[i].symbol);
    }

    fclose(assp);
    fclose(machp);
    return 0;
}

int isWSpace(char ch) {
    return ch == ' ' || ch == '\t';
}

int findSymTabLen(FILE *inputFile) {
    int count = 0;
    char *line = malloc(LINESIZE);
    while (fgets(line, LINESIZE, inputFile)) {
        if (!isWSpace(line[0])) count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}

void fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    char *line = malloc(LINESIZE);
    int i = 0;
    while (fgets(line, LINESIZE, inputFile)) {
        printf("%d\t%s", lineNo, line);

        if (!isWSpace(line[0])) {
            char *label = strtok(line, DELIM);

            for (int j = 0; j < i; ++j)
                if (strcmp(label, symT[j].symbol) == 0) {
                    printf("Duplicate definition of label '%s' in lines %d and %d\n", label, j, lineNo);
                    exit(1);
                }
            strcpy(symT[i].symbol, label);
            symT[i++].value = lineNo;
        }
        lineNo++;
    }
    rewind(inputFile);
    free(line);
}
