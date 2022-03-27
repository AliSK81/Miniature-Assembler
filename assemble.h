#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define LINESIZE 255
#define DELIM "\t, \n"
#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2

struct symbolTable {
    int value;
    char *symbol;
};

struct instruction {
    size_t instType;
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};

int findSymTabLen(FILE *);

void fillSymTab(struct symbolTable *, FILE *);

void formInst(struct instruction, FILE *);

int hex2int(char *);

void int2hex16(char *, int);

int isWSpace(char);

void as2mc(struct symbolTable *, int, FILE *);

void getToken(char *, char **, int);

int isLetter(char ch);

int getSymVal(struct symbolTable *symT, int symTabLen, char *sym);