#include "assemble.h"

int main(int argc, char **argv) {
    FILE *assp, *machp;

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

//    if ((assp = fopen("../test/prog1.as", "r")) == NULL) {
//        printf("%s cannot be opened\n", argv[1]);
//        exit(1);
//    }
//    if ((machp = fopen("../test/prog1.mc", "w+")) == NULL) {
//        printf("%s cannot be opened\n", argv[2]);
//        exit(1);
//    }

    int i, symTabLen = findSymTabLen(assp);
    struct symbolTable *pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));

    for (i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char *) malloc(10);

    printf("%s\n", argv[1]);
    fillSymTab(pSymTab, assp);

    printf("\n\npSymTab:\n");
    for (i = 0; i < symTabLen; ++i)
        printf("%d\t%s\n", pSymTab[i].value, pSymTab[i].symbol);

    printf("\noutput:\n");
    as2mc(pSymTab, symTabLen, assp, machp);

    fclose(assp);
    fclose(machp);
    return 0;
}

int isWSpace(char ch) {
    return ch == ' ' || ch == '\t';
}

int isLetter(char a) {
    return a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z';
}

// finds int value of the offset
// - if offset is decimal, converts to int
// - if offset is symbol, searches in the symTable
int getOffsetVal(struct symbolTable *symT, int symTabLen, char *sym, int lineNo) {
    int offset, overflow;
    if (strlen(sym) > 6) {
        overflow = 1;
    } else if (!isLetter(sym[0])) {
        offset = atoi(sym);
        overflow = offset < -32768 || offset > 32767;
    } else {
        int i;
        for (i = 0; i < symTabLen; ++i)
            if (strcmp(symT[i].symbol, sym) == 0)
                return symT[i].value;
        printf("Err1: Using undefined label '%s' on line '%d'\n", sym, lineNo);
        exit(1);
    }
    if (overflow) {
        printf("Err3: Offset overflow for '%s' on line '%d'\n", sym, lineNo);
        exit(1);
    }
    return offset;
}

// counts number of defined symbols in inputFile
int findSymTabLen(FILE *inputFile) {
    int count = 0;
    char *line = (char *) malloc(LINESIZE);
    while (fgets(line, LINESIZE, inputFile)) {
        if (!isWSpace(line[0])) count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}

// puts all symbols and their values (zero-based lineNo) in symTable
void fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    char *line = (char *) malloc(LINESIZE);
    char *token;
    int i = 0, j;
    while (fgets(line, LINESIZE, inputFile)) {
        printf("%d\t%s", lineNo, line);

        if (!isWSpace(line[0])) {
            token = strtok(line, DELIM);

            for (j = 0; j < i; ++j)
                if (strcmp(token, symT[j].symbol) == 0) {
                    printf("Err2: Duplicate definition of label '%s' in lines '%d' and '%d'\n",
                           token, symT[j].value, lineNo);
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

char *int2hex16(int a) {
    char *lower = (char *) malloc(9);
    sprintf(lower, "%x", a);
    if (a < 0x0) {
        lower[3] = lower[7];
        lower[2] = lower[6];
        lower[1] = lower[5];
        lower[0] = lower[4];
    } else if (a < 0x10) {
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x100) {
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x1000) {
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
    lower[4] = '\0';
    return lower;
}

int hex2int(char *hex) {
    return (int) strtol(hex, NULL, 16);
}

// converts assembly program to decimal machine code
// - reads assembly program from assp
// - writes machine code to machp
void as2mc(struct symbolTable *symT, int symTabLen, FILE *assp, FILE *machp) {
    char *instructions[] = {"add", "sub", "slt", "or", "nand", "addi", "slti", "ori",
                            "lui", "lw", "sw", "beq", "jalr", "j", "halt"};

    char hexTable[] = "0123456789abcdef";

    int i, instCnt = 0;
    char *token, *imm, *line = (char *) malloc(LINESIZE);
    struct instruction *currInst = (struct instruction *) malloc(sizeof(struct instruction));

    while (fgets(line, LINESIZE, assp)) {
        instCnt++;
        token = strtok(line, DELIM);

        if (!isWSpace(line[0]))
            token = strtok(NULL, DELIM);

        currInst->mnemonic = token;

        // case 1 : directive
        if (strcmp(currInst->mnemonic, ".fill") == 0) {
            token = strtok(NULL, DELIM);
            currInst->intInst = getOffsetVal(symT, symTabLen, token, instCnt);
            sprintf(currInst->inst, "%x", currInst->intInst);

            // case 2 : instructions
        } else {
            int found = 0;
            int instructionsLen = NORTYPE + NOITYPE + NOJTYPE;

            // determine type of instruction
            for (i = 0; i < instructionsLen; ++i) {
                if (strcmp(currInst->mnemonic, instructions[i]) == 0) {
                    if (i < NORTYPE)
                        currInst->instType = RTYPE;
                    else if (i < NORTYPE + NOITYPE)
                        currInst->instType = ITYPE;
                    else
                        currInst->instType = JTYPE;

                    // fill opcode
                    currInst->inst[1] = hexTable[i];

                    found = 1;
                    break;
                }
            }

            if (!found) {
                printf("Err4: Invalid opcode for mnemonic '%s' in line '%d'\n", currInst->mnemonic, instCnt);
                exit(1);
            }

            currInst->inst[8] = '\0';

            switch (currInst->instType) {
                case RTYPE:
                    token = strtok(NULL, DELIM);
                    currInst->rd = atoi(token);
                    token = strtok(NULL, DELIM);
                    currInst->rs = atoi(token);
                    token = strtok(NULL, DELIM);
                    currInst->rt = atoi(token);

                    currInst->inst[0] = '0';
                    currInst->inst[2] = hexTable[currInst->rs];
                    currInst->inst[3] = hexTable[currInst->rt];
                    currInst->inst[4] = hexTable[currInst->rd];
                    currInst->inst[5] = '0';
                    currInst->inst[6] = '0';
                    currInst->inst[7] = '0';
                    break;

                case ITYPE:
                    token = strtok(NULL, DELIM);
                    currInst->rt = atoi(token);

                    if (strcmp(currInst->mnemonic, "lui") == 0) {
                        currInst->rs = 0;
                    } else {
                        token = strtok(NULL, DELIM);
                        currInst->rs = atoi(token);
                    }

                    if (strcmp(currInst->mnemonic, "jalr") == 0) {
                        currInst->imm = 0;
                    } else if (strcmp(currInst->mnemonic, "beq") == 0) {
                        token = strtok(NULL, DELIM);
                        currInst->imm = getOffsetVal(symT, symTabLen, token, instCnt) - instCnt;
                    } else {
                        token = strtok(NULL, DELIM);
                        currInst->imm = getOffsetVal(symT, symTabLen, token, instCnt);
                    }

                    imm = int2hex16(currInst->imm);

                    currInst->inst[0] = '0';
                    currInst->inst[2] = hexTable[currInst->rs];
                    currInst->inst[3] = hexTable[currInst->rt];
                    currInst->inst[4] = imm[0];
                    currInst->inst[5] = imm[1];
                    currInst->inst[6] = imm[2];
                    currInst->inst[7] = imm[3];
                    break;

                case JTYPE:
                    if (strcmp(currInst->mnemonic, "halt") == 0) {
                        currInst->imm = 0;
                    } else {
                        token = strtok(NULL, DELIM);
                        currInst->imm = getOffsetVal(symT, symTabLen, token, instCnt);
                    }

                    imm = int2hex16(currInst->imm);

                    currInst->inst[0] = '0';
                    currInst->inst[2] = '0';
                    currInst->inst[3] = '0';
                    currInst->inst[4] = imm[0];
                    currInst->inst[5] = imm[1];
                    currInst->inst[6] = imm[2];
                    currInst->inst[7] = imm[3];
            }
            // obtain decimal equivalent of hex instruction
            currInst->intInst = hex2int(currInst->inst);
        }
        // standard output (debug)
        printf("%d\t%-15d\t(hex 0x%s)\n", instCnt - 1, currInst->intInst, currInst->inst);

        // output file
        fprintf(machp, "%d\n", currInst->intInst);
    }
    free(line);
}
