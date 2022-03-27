#include "assemble.h"

char *instructions[] = {"add", "sub", "slt", "or", "nand", "addi",
                        "slti", "ori", "lui", "lw", "sw", "beq", "jalr", "j", "halt"};

int main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();

    int i;

    char hexTable[] = "0123456789ABCDEF";

//    char r[5] = "1111";
//    int2hex16(r, 5);
//    printf("%s\n", r);

//    char nm[]={'0','9','0', '1', '0', '0', '6'};
//    printf("%d\n", hex2int(nm));


//    char hexstring[] = {'0','9','0','1','0','0','0','6', '\0'};
//    char hex[] = {'0', '9'};
//    int number = strtol(hexstring, NULL, 16);
//    printf("%d\n", hex2int(hexstring));


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
    for (i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char *) malloc(10);


    printf("program.as:\n");
    fillSymTab(pSymTab, assp);

//    printf("\n\npSymTab:\n");
//    for (int i = 0; i < symTabLen; ++i) {
//        printf("%d\t%s\n", pSymTab[i].value, pSymTab[i].symbol);
//    }

//    as2mc(pSymTab, symTabLen, assp);

    printf("\n\noutput:\n");


    int instCnt = 0;
    char *token;
    char *line = malloc(LINESIZE);
    struct instruction currInst;
    while (fgets(line, LINESIZE, assp)) {
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
                currInst.intInst = getSymVal(pSymTab, symTabLen, token);
            } else {
                currInst.intInst = atoi(token);
            }

            sprintf(currInst.inst, "%X", currInst.intInst);

        } else if (strcmp(currInst.mnemonic, ".space") == 0) {

            // TODO

        } else {

            int found = 0;
            int instructionsLen = NORTYPE + NOITYPE + NOJTYPE;

            for (i = 0; i < instructionsLen; ++i) {
                if (strcmp(currInst.mnemonic, instructions[i]) == 0) {
                    if (i < NORTYPE) {
                        currInst.instType = RTYPE;
                    } else if (i < NORTYPE + NOITYPE) {
                        currInst.instType = ITYPE;
                    } else {
                        currInst.instType = JTYPE;
                    }
                    found = 1;

                    // opcode
                    currInst.inst[1] = hexTable[i];

                    break;
                }
            }

            if (found == 0) {
                printf("Invalid '%s' mnemonic in line %d\n", currInst.mnemonic, currInst.PC);
                exit(1);
            }


            if (currInst.instType == RTYPE) {
                getToken(NULL, &token, 1);
                currInst.rd = atoi(token);
                getToken(NULL, &token, 1);
                currInst.rs = atoi(token);
                getToken(NULL, &token, 1);
                currInst.rt = atoi(token);

                currInst.inst[0] = '0';
                currInst.inst[2] = hexTable[currInst.rs];
                currInst.inst[3] = hexTable[currInst.rt];
                currInst.inst[4] = hexTable[currInst.rd];
                currInst.inst[5] = '0';
                currInst.inst[6] = '0';
                currInst.inst[7] = '0';
                currInst.inst[8] = '\0';
            } else if (currInst.instType == ITYPE) {

                getToken(NULL, &token, 1);
                currInst.rt = atoi(token);

                if (strcmp(currInst.mnemonic, "lui") == 0) {
                    currInst.rs = 0;
                } else {
                    getToken(NULL, &token, 1);
                    currInst.rs = atoi(token);
                }

                if (strcmp(currInst.mnemonic, "jalr") == 0) {
                    currInst.imm = 0;
                } else {
                    getToken(NULL, &token, 1);
                    if (isLetter(token[0])) {
                        currInst.imm = getSymVal(pSymTab, symTabLen, token);
                    } else {
                        currInst.imm = atoi(token);
                    }
                }

                currInst.inst[0] = '0';
                currInst.inst[2] = hexTable[currInst.rs];
                currInst.inst[3] = hexTable[currInst.rt];
                char imm[5];
                int2hex16(imm, currInst.imm);
                currInst.inst[4] = imm[0];
                currInst.inst[5] = imm[1];
                currInst.inst[6] = imm[2];
                currInst.inst[7] = imm[3];
                currInst.inst[8] = '\0';
            } else if (currInst.instType == JTYPE) {

                if (strcmp(currInst.mnemonic, "halt") == 0) {
                    currInst.imm = 0;
                } else {
                    getToken(NULL, &token, 1);
                    if (isLetter(token[0])) {
                        currInst.imm = getSymVal(pSymTab, symTabLen, token);
                    } else {
                        currInst.imm = atoi(token);
                    }
                }

                currInst.inst[0] = '0';
                currInst.inst[2] = '0';
                currInst.inst[3] = '0';
                char imm[5];
                int2hex16(imm, currInst.imm);
                currInst.inst[4] = imm[0];
                currInst.inst[5] = imm[1];
                currInst.inst[6] = imm[2];
                currInst.inst[7] = imm[3];
                currInst.inst[8] = '\0';
            }


            currInst.intInst = hex2int(currInst.inst);

//            printf("%s\t%d\n", currInst.mnemonic, currInst.instType);
        }
        printf("%d\t%-15d\t(hex 0x%s)\n", currInst.PC, currInst.intInst, currInst.inst);
//        printf("%s\n", token);
    }

    free(line);


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
    int i;
    for (i = 0; i < symTabLen; ++i)
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
    int i = 0, j;
    while (fgets(line, LINESIZE, inputFile)) {
        printf("%d\t%s", lineNo, line);

        if (!isWSpace(line[0])) {
            token = strtok(line, DELIM);

            for (j = 0; j < i; ++j)
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

void int2hex16(char *lower, int a) {
    sprintf(lower, "%X", a);
    if (a < 0x10) {
        lower[4] = '\0';
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x100) {
        lower[4] = '\0';
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x1000) {
        lower[4] = '\0';
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
}


int hex2int(char *hex) {
    return (int) strtol(hex, NULL, 16);
}

void as2mc(struct symbolTable *symT, int symTabLen, FILE *inputFile) {

}
