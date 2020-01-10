/*To do:
ADD UNIT TESTING FOR:


Possible Improvements:
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>


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

#define strsame(A,B) (strcmp(A, B)==0)

typedef enum {FALSE, TRUE} bool;
typedef enum {NOTEXECUTED, EXECUTED} instr;
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
void nalERROR(nalFile *nf, char* const msg);
void syntaxERROR(nalFile *nf, char const *prevWord, char const *expWord,  int index);
void ERROR(char* const msg);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);

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
void wordStep(nalFile *nf, int s);

/*Syntax*/
void program(nalFile *nf);
void instrs(nalFile *nf);
void instruct(nalFile *nf);
instr file(nalFile *nf);
instr nalAbort(nalFile *nf);
instr input(nalFile *nf);
instr in2str(nalFile *nf);
instr innum(nalFile *nf);
instr jump(nalFile *nf);
instr nalPrint(nalFile *nf);
instr nalRnd(nalFile *nf);
instr nalIfcond(nalFile *nf);
instr nalIfequal(nalFile *nf);
instr nalIfgreater(nalFile *nf);
instr nalInc(nalFile *nf);
instr nalSet(nalFile *nf);

bool isstrcon(char const *word);
bool isnumvar(char const *word);
bool isstrvar(char const *word);
bool isnumcon(char const *word);
bool isvar(char const *word);
bool iscon(char const *word);
bool isvarcon(char const *word);

bool validVar(char const *word, char c);

/*Interpreting*/
char *getString(char const* word);
char ROT(char c);
char ROTbase(char c, char base, int rotVal, int alphabet);
bool isnumber(char c);
