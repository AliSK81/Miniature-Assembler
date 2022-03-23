#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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


//    if ((assp = fopen("..\\p.as", "r")) == NULL) {
//        printf("%s cannot be opened\n", argv[1]);
//        exit(1);
//    }
//    if ((machp = fopen("..\\p.mc", "w+")) == NULL) {
//        printf("%s cannot be opened\n", argv[2]);
//        exit(1);
//    }


    int symTabLen = findSymTabLen(assp);
    struct symbolTable *pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char *) malloc(10);


    printf("program.as:\n");

    fillSymTab(pSymTab, assp);

    // here you can place your code for the assembler


    printf("\n\npSymTab:\n");

    for (int i = 0; i < symTabLen; ++i) {
        printf("%d\t%s\n", pSymTab[i].value, pSymTab[i].symbol);
    }

    fclose(assp);
    fclose(machp);
    return 0;
}

//int findSymLen(char *line)
//{
//    int count = 0;
//    char *ch = &line[0];
//
//    while (*ch && *ch != '#') {
//        if (*ch != ' ' && *(ch-1) == ' ')
//            count++;
//        ch++;
//    }
//    return count == 2;
//}


int endl(char ch) {
    return ch == '\0' || ch == '#';
}

int isWSpace(char ch) {
    return ch == ' ' || ch == '\t';
}

int findSymLen(char *line) {
    int count = 0;
    char *ch = &line[0];

    while (!endl(*ch)) {
        while (!endl(*ch) && !isWSpace(*ch)) ch++;
        while (!endl(*ch) && isWSpace(*ch)) ch++;
        count++;
    }

    if (count < 3) {
        int halt = strstr(line, "halt") - line;
        if (halt <= 0)
            return 0;
    }

    int symLen = 0;
    ch = &line[0];
    while (!endl(*ch) && !isWSpace(*ch)) {
        symLen++;
        ch++;
    }

    return symLen;
}


char *fillSymbol(char *line, char *symbol, int symLen) {
    strncpy(symbol, line, symLen);
    symbol[symLen] = '\0';
    return symbol;
}


int findSymTabLen(FILE *inputFile) {
    int count = 0;
    size_t lineSize = 255;
    char *line = malloc(lineSize);

    while (fgets(line, lineSize, inputFile)) {
        if (findSymLen(line) != 0)count++;
    }

    rewind(inputFile);

    free(line);
    return count;
}

void fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    size_t lineSize = 255;
    char *line = malloc(lineSize);
    int i = 0;

    while (fgets(line, lineSize, inputFile)) {
        printf("%d\t%s", lineNo, line);
        int symLen = findSymLen(line);
        if (symLen != 0) {
            fillSymbol(line, symT[i].symbol, symLen);
            for (int j = 0; j < i; ++j)
                if (strcmp(symT[i].symbol, symT[j].symbol) == 0) {
                    printf("The symbol '%s' already exist on line '%d'. (lineNo: %d)\n", symT[i].symbol, j, lineNo);
                    exit(1);
                }
            symT[i++].value = lineNo;
        }
        lineNo++;
    }

    free(line);
}
