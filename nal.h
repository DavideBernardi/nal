/*To do:

Possible testing strategy: have a different #ifdef INSTRUCT #endif for each instruction, when testing the instruction just compile with only -DINSTRUCT, so the rest of the file is parsed except for the defined instruction.
This way can also test only 2 functions and how they interact together.
Left To Do:

   IMPORTANT: FINISH IFEQUAL CHECKING BECAUSE AT THE MOMENT IT IS VERY BAD I.E. IT JUST CHECKS IF THE STRINGS ARE THE SAME
ADD UNIT TESTING FOR:
   insertInputNum()
   insertInputStrings()


Possible Improvements:
   The vList is trash, make it a sorted list or a hash table for god's sake

   in insertInputStrings since we use scanf we risk overflow errors.

   in ifequal and ifgreater we need a larger infrastructure to check whether the varcons to compare are of the same type and don't just look the same
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "vList.h"


/*Maximum size of a single %s pulled from the file,
Note: This is only used when initially reading in each space-separated string,
      actual words are then stored in properly allocated memory.*/
#define MAXWORDSIZE 1000
#define ERRORLINELENGTH 75
#define SYNTAXERRORLENGTH 50

/*Number of words that are part of each of these rule's syntax
i.e. <IN2STR> = "IN2STR", "(", "STRVAR", ",", "STRVAR", ")"
6 words in total*/
#define IN2STRWORDS 6
#define INNUMWORDS 4
#define RANDWORDS 4
#define INCWORDS 4
#define SETWORDS 3

/*Used in testing*/
#define WORDSINTEST1 4
#define ERRORSTRINGLEN 1000

/*Used in ROT()*/
#define ROTCHAR 13
#define ALPHABET 26
#define ROTNUM 5
#define SYMSAMOUNT 34
#define NUMBASE 10

/*Maximum amount of chars of user input (in in2str or innum)
Note: max is 1000, last char is end of string*/
#define MAXINPUTSTRLEN 1001
#define MAXINPUTNUMLEN 1001

/*Used in setRandom()*/
#define RANMAX 99
#define RANMIN 0
/*Maximum amount of chars the random number can be + 1 for \0*/
#define MAXRANCHARS 3

#define MAXDOUBLESIZE 1100
#define EPSILON 0.000000000000000001

#define strsame(A,B) (strcmp(A, B)==0)

typedef enum {FALSE, TRUE} bool;
typedef enum {NOTEXECUTED, EXECUTED} instr;
typedef enum {NOTEXEC, EXECPASS, EXECFAIL} cond;
typedef enum {NOCOMP, SMALLER, EQUAL, GREATER} comp;
typedef enum {NUM, STR, ROTSTR} vartype;

typedef struct nalFile{
   char **words;
   int currWord;
   int totWords;
   struct nalFile *prev;
} nalFile;

typedef struct intNode{
   int data;
   struct intNode *next;
}intNode;

typedef struct intList{
   intNode *head;
   int size;
}intList;

/*Testing*/
void test(void);
void testTokenization(void);
void testParsingFunctions(void);
void testInterpFunctions(void);
void testGetString(char const* word, char const* realStr);
void testROT(void);

/*ERROR message functions*/
void nalERROR(nalFile *nf, vList *vl, char* const msg);
void syntaxERROR(nalFile *nf, vList *vl, char const *prevWord, char const *expWord,  int index);
void ERROR(char* const msg);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);
char *allocString(const char *str);

/*Base Functions*/
void checkInput(int argc, char const *argv[]);
FILE *getFile(char const file[]);

/*These are used to read in and tokenize the file*/
intList *getWordSizes(FILE *fp);
int wordLength(char const *currWord, FILE *fp, intList *wordLens);
void tokenizeFile(nalFile *nf, FILE *fp, intList *wordLengths);
void getWord(char *word, FILE* fp);
void strAppend(char *word, char c);
bool multiWordString(char const *word);
/*These are for an int linked intList used within getWordSizes*/
intNode *allocateNode(int i);
void freeList(intList **nf);

/*Functions used to play with the nalFile structures*/
nalFile *initNalFile(void);
void setupNalFile(nalFile *nf, char const file[]);
void terminateNalFile(nalFile **nf);
void terminateAllNalFiles(nalFile *nf);
void wordStep(nalFile *nf, vList *vl, int s);

/*Syntax*/
void program(nalFile *nf, vList *vl);
void instrs(nalFile *nf, vList *vl);
void instruct(nalFile *nf, vList *vl);
instr file(nalFile *nf, vList *vl);
instr nalAbort(nalFile *nf, vList *vl);
instr input(nalFile *nf, vList *vl);
instr in2str(nalFile *nf, vList *vl);
instr innum(nalFile *nf, vList *vl);
instr jump(nalFile *nf, vList *vl);
instr nalPrint(nalFile *nf, vList *vl);
instr nalRnd(nalFile *nf, vList *vl);
instr nalIfcond(nalFile *nf, vList *vl);
cond nalIfequal(nalFile *nf, vList *vl);
cond nalIfgreater(nalFile *nf, vList *vl);
instr nalInc(nalFile *nf, vList *vl);
instr nalSet(nalFile *nf, vList *vl);

bool isstrcon(char const *word);
bool isnumvar(char const *word);
bool isstrvar(char const *word);
bool isnumcon(char const *word);
bool isvar(char const *word);
bool iscon(char const *word);
bool isvarcon(char const *word);
bool isstr(char const *word);
bool isnum(char const *word);

bool validVar(char const *word, char c);

/*Interpreting*/
char *getString(char const* word);
char ROT(char c);
char ROTbase(char c, char base, int rotVal, int alphabet);
bool isnumber(char c);

void insertInputStrings(nalFile *nf, vList *vl, char **varnames);
void insertInputNum(nalFile *nf, vList *vl, char* name);

void print(nalFile *nf, vList *vl, char *varcon);

void setRandom(vList *vl, char *name);

void setVariable(nalFile *nf, vList *vl);
bool validSet(char *name, char *val);

void incVar(nalFile *nf, vList *vl, char *name);

bool condEqual(nalFile *nf, vList *vl, char *varcon1, char *varcon2);
void skipToMatchingBracket(nalFile *nf, vList *vl);
comp compStrings(nalFile *nf, vList *vl, char *str1, char *str2);
char *extractStr(nalFile* nf, vList *vl, char *str);
comp compNums(nalFile *nf, vList *vl, char *num1, char *num2);
double extractNum(nalFile* nf, vList *vl, char *num);
comp compDoubles(double n1, double n2);
