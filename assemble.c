#include "assemble.h"

char *instructions[] = {"add", "sub", "slt", "or", "nand", "addi",
                        "slti", "ori", "lui", "lw", "sw", "beq", "jalr", "j", "halt"};

int main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();

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

    as2mc(pSymTab, symTabLen, assp);

    fclose(assp);
    fclose(machp);
    return 0;
}

int isWSpace(char ch) {
    return ch == ' ' || ch == '\t';
}

int isLetter(char ch) {
    return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}


//
//char *getToken(char *line, int pos) {
//
//    char *inputCopy = malloc((strlen(line) + 1));
//
//    char *p = NULL;
//
//    if (inputCopy != NULL) {
//        strcpy(inputCopy, line);
//
//        p = strtok(inputCopy, DELIM);
//
//        while (p != NULL && pos-- != 0) {
//            p = strtok(NULL, DELIM);
//        }
//
//        if (p != NULL) {
//            size_t n = strlen(p);
//            memmove(inputCopy, p, n + 1);
//
//            p = realloc(inputCopy, n + 1);
//        }
//
//        if (p == NULL) {
//            free(inputCopy);
//        }
//    }
//
//    return p;
//}



void getToken(char *line, char **token, int pos) {
    if (line != NULL) {
        *token = malloc(strlen(line) + 1);
        strcpy(*token, line);
        *token = strtok(*token, DELIM);
        pos--;
    }
    while (token != NULL && pos-- > 0) {
        *token = strtok(NULL, DELIM);
    }
}

int getSymVal(struct symbolTable *symT, int symTabLen, char *sym) {
    for (int i = 0; i < symTabLen; ++i)
        if (strcmp(symT[i].symbol, sym) == 0)
            return symT[i].value;
    return 0;
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
    char *token;
    int i = 0;
    while (fgets(line, LINESIZE, inputFile)) {
        printf("%d\t%s", lineNo, line);

        if (!isWSpace(line[0])) {
            token = strtok(line, DELIM);

            for (int j = 0; j < i; ++j)
                if (strcmp(token, symT[j].symbol) == 0) {
                    printf("Duplicate definition of label '%s' in lines %d and %d\n", token, j, lineNo);
                    exit(1);
                }
            strcpy(symT[i].symbol, token);
            symT[i++].value = lineNo;
        }
        lineNo++;
    }
    rewind(inputFile);
    free(line);
}

int hex2int(char *hex) {
    return strtol(hex, NULL, 0);
}

void as2mc(struct symbolTable *symT, int symTabLen, FILE *inputFile) {
    int instCnt = 0;
    char *token;
    char *line = malloc(LINESIZE);
    struct instruction currInst;
    while (fgets(line, LINESIZE, inputFile)) {
        currInst.PC = instCnt++;

        if (isWSpace(line[0])) {
            getToken(line, &token, 1);
        } else {
            getToken(line, &token, 2);
        }

        currInst.mnemonic = malloc(strlen(token + 1));
        strcpy(currInst.mnemonic, token);

        if (strcmp(currInst.mnemonic, ".fill") == 0) {
            getToken(NULL, &token, 1);
            if (isLetter(token[0])) {
                currInst.intInst = getSymVal(symT, symTabLen, token);
            } else {
                currInst.intInst = atoi(token);
            }

        } else if (strcmp(currInst.mnemonic, ".space") == 0) {

            // TODO

        } else {

            int found = 0;
            int instructionsLen = NORTYPE + NOITYPE + NOJTYPE;

            for (int i = 0; i < instructionsLen; ++i) {
                if (strcmp(currInst.mnemonic, instructions[i]) == 0) {
                    if (i < NORTYPE) {
                        currInst.instType = RTYPE;
                    } else if (i < NORTYPE + NOITYPE) {
                        currInst.instType = ITYPE;
                    } else {
                        currInst.instType = JTYPE;
                    }
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                printf("Invalid '%s' mnemonic in line %d\n", currInst.mnemonic, currInst.PC);
                exit(1);
            }


            printf("%s\t%d\n", currInst.mnemonic, currInst.instType);

        }

//        printf("%s\n", token);
    }

    free(line);
}
