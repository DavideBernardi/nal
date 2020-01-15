/*
NOTES:
If a double is set as the index after <JUMP>, only the integer part
is considered (not closest integer approximation, no automatic abort)

When taking in input from the user, the string has a maximum size of 1000000
(Which then gets reallocated appropriately), however if the input string is
larger than that, overflow happens with no error messages.


ISSUES:

The vList is trash, make it a sorted list or a hash table for god's sake

error in extractStr/Num()
   When opening file which tries to use uninitialized
   variable (both string and num) the array which stores
   the names of variables is not freed;

Not sure what happens when using JUMP inside a (or multiple) conditional(s).
Or jumping from outside a conditonal to inside.
Just real confusing
(Note: At the moment my code miraculously handles this okay because instrs()
is written such that if it is parsing the last word and the last word is "}",
it will keep reparsing it until it closes all open parenthesis and then exit
normally.)


POSSIBLE TESTING STRATEGY:
      Have a different #ifdef INSTRUCT #endif for
      each instruction, when testing the instruction just compile with only
      -DINSTRUCT, so the rest of the file is parsed except for the defined
      instruction.
      This way can also test only 2 functions and how they interact together.


POSSIBLE IMPROVEMENTS:




IMPROVE CODE FOR:
      in2str()
      innum()
      jump() [low priority]
      nalRnd()
      nalInc()
      nalIfEqual() [high priority]
      nalIfGreater() [high priority]
      + condGreater and condEqual can be a single function




ADD UNIT TESTING FOR:
      setupNalFile()
      terminateAllNalFiles()
      extractStr()
      extractNum()
      insertInputStrings()
      insertInputNum()
      print()
      setRandom()
      skipToMatchingBracket()
      condEqual()
      condGreater()
      compString()
      compNums()
      compDoubles()
      incVar()
      setVariable()
      validSet()
      allocString()

GO OVER PREVIOUS TESTING AS SOME FUNCTIONS HAVE CHANGED A LOT
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "vList.h"

/*Used in testing*/
#define WORDSINTEST1 4
#define TESTWORDSIZE 1000

/*Maximum size of a single %s pulled from the file.
      Note: This is only used when initially reading in each
      space-separated string,actual words are then stored in
      properly allocated memory.*/
#define MAXWORDSIZE 10000

/*Size of words in a syntax rule i.e. "JUMP", "IN2STR", "VARCON", ...*/
#define MAXSYNTAXWORDSIZE 100

/*Used when converting a double to a string*/
#define MAXDOUBLESIZE 10000

/*Used when comparing two doubles*/
#define EPSILON 0.000000000000000001

/*Base lengths of each of the error messages (these are then resized
based on the individual message so no risk of overlow)*/
#define TOKENIZEERRORLENGTH 100
#define SYNTAXERRORLENGTH 100
#define INDEXERRORLENGTH 150
#define NALERRORLENGTH 100
#define VARIABLEERRORLENGTH 100

/*Number of words that are part of each of these rule's syntax
i.e. <IN2STR> = "IN2STR", "(", "STRVAR", ",", "STRVAR", ")"
6 words in total*/
#define IN2STRWORDS 6
#define INNUMWORDS 4
#define RANDWORDS 4
#define INCWORDS 4
#define SETWORDS 3

/*Used in ROT()*/
#define ROTCHAR 13
#define ALPHABET 26
#define ROTNUM 5
#define SYMSAMOUNT 34
#define NUMBASE 10

/*Maximum amount of chars of user input (in in2str or innum)
Note: max is 1000, last char is end of string*/
#define MAXINPUTSTRLEN 1000000
#define MAXINPUTNUMLEN MAXDOUBLESIZE

/*Used in setRandom()*/
#define RANMAX 99
#define RANMIN 0
/*Used to "randomize" the seed itself before giving out the random number */
#define RANSEEDVAR 42
/*Maximum amount of chars the random number can be (+ 1 for \0)*/
#define MAXRANCHARS 3

#define strsame(A,B) (strcmp(A, B)==0)

typedef enum {FALSE, TRUE} bool;
typedef enum {NOTEXECUTED, EXECUTED} instr;
typedef enum {NUM, STR, ROTSTR} vartype;

/*These are only used for <IFCOND>*/
typedef enum {NOTEXEC, EXECPASS, EXECFAIL} cond; /*Similar to instr*/
typedef enum {NOCOMP, SMALLER, EQUAL, GREATER} comp;

typedef struct nalFile{
   char **words;
   int currWord;
   int totWords;
   char *name;
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

/*NOTE: In a lot of these functions, nl and vl are only passed so if an ERROR
occurs they can be appropriately freed*/

/*Check argv*/
void checkInput(int argc, char const *argv[]);

/*Testing*/
void test(void);
void testTokenization(void);
void testParsingFunctions(void);
void testInterpFunctions(void);
void testGetString(char const* word, char const* realStr);
void testROT(void);

/*ERROR functions*/
void nalERROR(nalFile *nf, vList *vl, char* const msg);
void indexERROR(nalFile *nf, vList *vl, char* const msg, int index);
void tokenizeERROR(nalFile *nf, vList *vl, char const *file, char const *msg);
void syntaxERROR(nalFile *nf, vList *vl, char const *prevWord, char const *expWord,  int index);
void variableERROR(nalFile *nf, vList *vl, char const *msg,char* const name, int index);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);
/*This function takes in a string and copies it into a correctly sized pointer,
it then returns that pointer (which will need freeing at some point)*/
char *allocString(const char *str);

/*These are used to read in and tokenize a file into a nalFile*/
FILE *getFile(nalFile *nf, vList *vl, char const file[]);
intList *getWordSizes(nalFile *nf, vList *vl, FILE *fp);
int wordLength(nalFile *nf, vList *vl, char const *currWord, FILE *fp, intList *wordLens);
void tokenizeFile(nalFile *nf, vList *vl, FILE *fp, intList *wordLengths);
void getWord(nalFile *nf, vList *vl, char *word, FILE* fp);
void strAppend(char *word, char c);
bool multiWordString(char const *word);
/*These are for an int linked intList used within getWordSizes*/
intNode *allocateNode(int i);
void freeList(intList **nf);

/*Functions used to play with the nalFile structures*/
nalFile *initNalFile(void);
void setupNalFile(nalFile *nf, vList *vl);
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

/*VARCON checks*/
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

/*INTERPRETING*/

/*General VARCON handling*/
/*Given a strcon word, returns the actual string (i.e. removes the extra "" or ##)*/
char *getString(char const* word);
char ROT(char c);
char ROTbase(char c, char base, int rotVal, int alphabet);
bool isnumber(char c);
/*Given a strvar or numvar name, returns it's actual str value or double value*/
char *extractStr(nalFile* nf, vList *vl, char *str);
double extractNum(nalFile* nf, vList *vl, char *name);

/*INPUT*/
void insertInputStrings(nalFile *nf, vList *vl, char **varnames);
void insertInputNum(nalFile *nf, vList *vl, char* name);
/*PRINT*/
void print(nalFile *nf, vList *vl, char *varcon);
/*RAND*/
void setRandom(vList *vl, char *name);
/*SET*/
void setVariable(nalFile *nf, vList *vl);
bool validSet(char *name, char *val);
/*INC*/
void incVar(nalFile *nf, vList *vl, char *name);
/*IFCOND*/
void skipToMatchingBracket(nalFile *nf, vList *vl);
comp compStrings(nalFile *nf, vList *vl, char *str1, char *str2);
comp compNums(nalFile *nf, vList *vl, char *num1, char *num2);
comp compDoubles(double n1, double n2);
/*IFEQUAL*/
bool condEqual(nalFile *nf, vList *vl, char *varcon1, char *varcon2);
/*IFGREATER*/
bool condGreater(nalFile *nf, vList *vl, char *varcon1, char *varcon2);
