/*To do:
   - Change ERROR macro (make into a function?)
      1. Have it output some more useful information
      2. Maybe have it handle all the free() in case of unexpected nalAbort
      3. Maybe change exit() to something else which makes it possible to
         quit one file without stopping the whole execution


Possible Improvement:
   Instead of just having currword++, have a nextWord(p) function which checks
   whether we are currently on the last word
   (if currWord == totWords-1
      nalERROR("More words expected"))
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
#define WORDSINTEST1 4
#define ERRORLINELENGTH 1000
#define SYNTAXERRORLENGTH 30
#define IN2STRWORDS 6
#define INNUMWORDS 4
#define RANDWORDS 4
#define INCWORDS 4
#define SETWORDS 3

#define ROT13 13
#define ALPHABET 26
#define ROT18 18
#define NUMBASE 10

#define strsame(A,B) (strcmp(A, B)==0)

typedef enum {FALSE, TRUE} bool;
typedef enum {NOTEXECUTED, EXECUTED} instr;
typedef enum {NUM, STR, ROTSTR} vartype;

typedef struct nalFile{
   char **words;
   int currWord;
   int totWords;
} nalFile;

typedef struct fileNode{
   nalFile *file;
   struct fileNode *prev;
}fileNode;

/*Note: if type == NUM, strval == NULL - other way around if type == STR or ROTSTR*/
typedef struct nalVar{
   vartype type;
   char *varname;
   char *strval;
   double *numval;
}nalVar;

typedef struct nalProg{
   fileNode *curr;
   nalVar **vars;

}nalProg;

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

/*General*/
void checkInput(int argc, char const *argv[]);
FILE *getFile(char const file[]);
nalFile *initNalFile(void);
void terminateNalFile(nalFile **p);
void setupNalFile(nalFile *p, char const file[]);
void nalERROR(nalFile *p, char* const msg);
void syntaxERROR(nalFile *p, char const *prevWord, char const *expWord,  int index);
void ERROR(char* const msg);

/*These are used to read in and tokenize the file*/
intList *getWordSizes(FILE *fp);
int wordLength(char const *currWord, FILE *fp, intList *wordLens);
void tokenizeFile(nalFile *p, FILE *fp, intList *wordLengths);
void getWord(char *word, FILE* fp);
void strAppend(char *word, char c);
bool multiWordString(char const *word);
/*These are for an int linked intList used within getWordSizes*/
intNode *allocateNode(int i);
void freeList(intList **p);

/*Syntax*/
void program(nalFile *p);
void instrs(nalFile *p);
void instruct(nalFile *p);
instr file(nalFile *p);
instr nalAbort(nalFile *p);
instr input(nalFile *p);
instr in2str(nalFile *p);
instr innum(nalFile *p);
instr jump(nalFile *p);
instr nalPrint(nalFile *p);
instr nalRnd(nalFile *p);
instr nalIfcond(nalFile *p);
instr nalIfequal(nalFile *p);
instr nalIfgreater(nalFile *p);
instr nalInc(nalFile *p);
instr nalSet(nalFile *p);

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
char unROT(char c);
bool isnumber(char c);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);


int main(int argc, char const *argv[])
{
   nalFile *p;

   test();

   checkInput(argc, argv);
   p = initNalFile();
   setupNalFile(p, argv[1]);
   program(p);
   #ifndef INTERP
   printf("Parsed OK\n");
   #endif
   terminateNalFile(&p);
   return 0;
}

void test(void)
{
/* initNatFile
   terminateNalFile */

   nalFile *p;

   p = initNalFile();
   assert(p!=NULL);
   assert(p->words == NULL);
   assert(p->currWord == 0);
   assert(p->totWords == 0);

   terminateNalFile(&p);
   assert(p==NULL);

   testTokenization();
}

void testTokenization(void)
{
   intNode *n, *curr, *oldNode;
   FILE *testFile;
   intList *wordLengths;
   char testWord[MAXWORDSIZE];
   nalFile *testNal;
   int i;
   char wordsInTest1[WORDSINTEST1][MAXWORDSIZE] = {"{", "PRINT", "\"Hello World! From test1\"", "}"};

   /*Testing multiWordString*/
   assert(multiWordString("Hello")==FALSE);
   assert(multiWordString("\"Nope\"")==FALSE);
   assert(multiWordString("\"Yep")==TRUE);
   assert(multiWordString("#Nope#")==FALSE);
   assert(multiWordString("#Yep")==TRUE);
   assert(multiWordString("{")==FALSE);

   /*Testing allocateNode*/
   n = allocateNode(5);
   assert(n->data==5);
   assert(n->next==NULL);
   free(n);

   /*Testing wordLength*/
   testFile = getFile("test1.nal");
   wordLengths = NULL;
   for (i = 0; i < WORDSINTEST1; i++) {
      assert(fscanf(testFile, "%s", testWord)==1);
      assert(wordLength(testWord, testFile, wordLengths)==(int)strlen(wordsInTest1[i]));
   }
   fclose(testFile);

   /*Testing getWordSizes*/
   testFile = getFile("test1.nal");
   wordLengths = getWordSizes(testFile);
   assert(wordLengths->size==WORDSINTEST1);
   i = 0;
   curr = wordLengths->head;
   while (curr!=NULL) {
      assert(curr->data==(int)strlen(wordsInTest1[i]));
         i++;
      oldNode = curr;
      curr = curr->next;
      free(oldNode);
   }
   free(wordLengths);
   assert(i==WORDSINTEST1);
   fclose(testFile);

   /*Testing getWord*/
   testFile = getFile("test1.nal");
   /*IMPORTANT IF A TOKENIZED WORD IS LONGER THAN 1000 CHARS, THIS CODE WILL BREAK*/
   for (i = 0; i < WORDSINTEST1; i++) {
      getWord(testWord, testFile);
      assert(strsame(testWord,wordsInTest1[i]));
   }
   fclose(testFile);

   /*Testing tokenizeFile*/
   testFile = getFile("test1.nal");
   testNal = initNalFile();
   wordLengths = getWordSizes(testFile);
   tokenizeFile(testNal, testFile, wordLengths);
   fclose(testFile);
   assert(testNal->totWords == WORDSINTEST1);
   for (i = 0; i < WORDSINTEST1; i++) {
      assert(strsame(testNal->words[i],wordsInTest1[i]));
   }
   terminateNalFile(&testNal);
}

void checkInput(int argc, char const *argv[])
{
   if (argc!=2) {
      fprintf(stderr, "Input must be of the form: %s <FILENAME>\n", argv[0]);
      exit(EXIT_FAILURE);
   }
}

FILE *getFile(char const file[])
{
   FILE *ifp;

   ifp = fopen(file, "rb");
   if (ifp == NULL) {
      fprintf(stderr, "Failed to open file\n");
      exit(EXIT_FAILURE);
   }

   return ifp;
}

void setupNalFile(nalFile *p, char const file[])
{
   FILE *fp;
   intList *wordLengths;

   fp = getFile(file);
   wordLengths = getWordSizes(fp);
   tokenizeFile(p, fp, wordLengths);
   fclose(fp);
}

nalFile *initNalFile(void)
{
   nalFile *p;

   p = allocate(sizeof(nalFile), "Nal File");

   p->words = NULL;
   p->currWord = 0;
   p->totWords = 0;

   return p;
}

void terminateNalFile(nalFile **p)
{
   nalFile *q;
   int i;

   q = *p;

   for (i = 0; i < q->totWords; i++) {
      free(q->words[i]);
   }
   free(q->words);
   free(q);
   *p = NULL;
}

/* Return a intList containing the exact size of each word in the file,
   the intList structure also contains the total number of words. */
intList *getWordSizes(FILE *fp)
{
   char currWord[MAXWORDSIZE];
   intList *wordLens;
   intNode *curr;

   wordLens = allocate(sizeof(intList), "List");
   wordLens->size = 0;

   if (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr = allocateNode(wordLength(currWord, fp, wordLens));
      wordLens->head = curr;
   } else {
      fclose(fp);
      free(wordLens);
      ERROR("Empty file\n");
   }

   while (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr->next = allocateNode(wordLength(currWord, fp, wordLens));
      curr = curr->next;
   }

   return wordLens;
}

/*Finds length of the input word, if the word is the start of a string,
it moves along the file until it finds the end of that string as strings count
as one word*/
int wordLength(char const *currWord, FILE *fp, intList *wordLens)
{
   int wordLen;
   char c, charToMatch;

   wordLen = strlen(currWord);

   if (multiWordString(currWord)) {
      charToMatch = currWord[0];
      while ((c = getc(fp)) != charToMatch) {
         if (c == EOF) {
            freeList(&wordLens);
            fclose(fp);
            ERROR("No matching string characters (\" or #)\n");
         }
         wordLen++;
      }
      wordLen++;
   }
   return wordLen;
}

/*allocates the exact amount of space for the array of words and then fills it up*/
void tokenizeFile(nalFile *p, FILE *fp, intList *wordLengths)
{
   intNode *curr, *oldNode;
   int i;

   p->words = (char **)callocate(sizeof(char *), wordLengths->size, "Words");

   curr = wordLengths->head;
   i = 0;
   rewind(fp);

   while (curr!=NULL ) {
      p->words[i] = (char *)callocate(sizeof(char), curr->data+1, "Word");
      getWord(p->words[i], fp);
      i++;
      p->totWords++;
      oldNode = curr;
      curr = curr->next;
      free(oldNode);
   }
   free(wordLengths);
}

/*fills word (which is of the correct size thanks to getWordSizes)
with the correct string from the file*/
void getWord(char *word, FILE* fp)
{
   char c, charToMatch;
   char currWord[MAXWORDSIZE];

   if (fscanf(fp, "%s", currWord)==1) {
      strcpy(word, currWord);
      if (multiWordString(currWord)) {
         charToMatch = currWord[0];
         while ((c = getc(fp)) != charToMatch) {
            if (c == EOF) {
               ERROR("Mismatch between allocating space for a string and storing of it");
            }
            strAppend(word, c);
         }
         strAppend(word, c);
      }
   }else{
      ERROR("Mismatch between allocating space for a word and storing of it");
   }

}

/*puts char c at the end of string word*/
void strAppend(char *word, char c)
{
   int len;

   len = strlen(word);

   word[len] = c;
   word[len+1] = '\0';
}

/*Says wether word opens a string without closing it (i.e. starts with a " or #
but doesn't end with one)*/
bool multiWordString(char const *word)
{
   char firstChar, lastChar;

   firstChar = word[0];
   lastChar = word[strlen(word)-1];

   if (firstChar=='\"' && lastChar!='\"') {
      return TRUE;
   }

   if (firstChar=='#' && lastChar!='#') {
      return TRUE;
   }
   return FALSE;
}

void freeList(intList **p)
{
   intNode *curr, *oldNode;
   intList *l;

   l = *p;

   curr = l->head;

   while (curr!=NULL) {
      oldNode = curr;
      curr = curr->next;
      free(oldNode);
   }
   free(l);
   *p = NULL;
}

intNode *allocateNode(int i)
{
   intNode *n;

   n = allocate(sizeof(intNode), "Node");

   n->data = i;
   n->next = NULL;
   return n;
}

void program(nalFile *p)
{
   if (!strsame(p->words[p->currWord], "{")) {
      nalERROR(p, "No starting \'{\'\n");
   }
   p->currWord++;
   instrs(p);
}

void instrs(nalFile *p)
{
   if (strsame(p->words[p->currWord], "}")) {
      return;
   }
   instruct(p);
   instrs(p);
}

void instruct(nalFile *p)
{
   char errorLine[ERRORLINELENGTH];

   if (nalAbort(p)==EXECUTED) {
      return;
   }
   if (file(p)==EXECUTED) {
      return;
   }
   if (input(p)==EXECUTED) {
      return;
   }
   if (jump(p)==EXECUTED) {
      return;
   }
   if (nalPrint(p)==EXECUTED) {
      return;
   }
   if (nalRnd(p)==EXECUTED) {
      return;
   }
   if (nalIfcond(p)==EXECUTED) {
      return;
   }
   if (nalInc(p)==EXECUTED) {
      return;
   }
   if (nalSet(p)==EXECUTED) {
      return;
   }

   sprintf(errorLine, "Word \"%s\" (at index %d) doesn't match any syntax rule\n",p->words[p->currWord], p->currWord);
   nalERROR(p, errorLine);
}

instr file(nalFile *p)
{
   #ifdef INTERP
   nalFile *newFile;
   char *fileName;
   #endif

   if (strsame(p->words[p->currWord], "FILE")) {
      p->currWord++;
      if (!isstrcon(p->words[p->currWord])) {
         syntaxERROR(p, "FILE", "STRCON", p->currWord);
      }
      #ifdef INTERP
      fileName = getString(p->words[p->currWord]);
      newFile = initNalFile();
      setupNalFile(newFile, fileName);
      free(fileName);
      program(newFile);
      terminateNalFile(&newFile);
      #endif
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

char *getString(char const* word)
{
   char *str;
   int strSize, i;

   /*-2 spaces to ger rid of "" or ##*/
   strSize = strlen(word)-2;

   str = (char *)allocate(sizeof(char)*(strSize+1), "String");
   if (word[0]=='\"') {
      for (i = 0; i < strSize; i++) {
         str[i] = word[i+1];
      }
      str[strSize] = '\0';
   } else {
      for (i = 0; i < strSize; i++) {
         str[i] = unROT(word[i+1]);
      }
      str[strSize] = '\0';
   }
   return str;
}

char unROT(char c)
{
   if (islower(c)) {
      c = c - 'a';
      c = (c-ROT13)%ALPHABET;
   } else if (isupper(c)) {
      c = c - 'A';
      c = (c-ROT13)%ALPHABET;
   } else if (isnumber(c)) {
      c = c-'0';
      c = (c-ROT18)%NUMBASE;
   }
   return c;
}

bool isnumber(char c)
{
   if (c>=0 && c<=9) {
      return TRUE;
   }
   return FALSE;
}

instr nalAbort(nalFile *p)
{
   if (strsame(p->words[p->currWord], "ABORT")) {
      #ifdef INTERP
         terminateNalFile(&p);
         exit(EXIT_SUCCESS);
      #endif
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr input(nalFile *p)
{

   if (in2str(p) == EXECUTED) {
      return EXECUTED;
   }
   if (innum(p) == EXECUTED) {
      return EXECUTED;
   }
   return NOTEXECUTED;

}

instr in2str(nalFile *p)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IN2STR", "(", "STRVAR", ",", "STRVAR", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "STRVAR")) {
            if (!isstrvar(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(p->words[p->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {;
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr innum(nalFile *p)
{
   int i;
   bool correct;
   char syntax[INNUMWORDS][MAXWORDSIZE] = {"INNUM", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < INNUMWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(p->words[p->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr jump(nalFile *p)
{
   if (strsame(p->words[p->currWord], "JUMP")) {
      p->currWord++;
      if (!isnumcon(p->words[p->currWord])) {
         syntaxERROR(p, "JUMP", "NUMCON", p->currWord);
      }
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalPrint(nalFile *p)
{
   if (strsame(p->words[p->currWord], "PRINT") || strsame(p->words[p->currWord], "PRINTN")) {
      p->currWord++;
      if (!isvarcon(p->words[p->currWord])) {
         syntaxERROR(p, p->words[p->currWord-1], "VARCON",p->currWord);
      }
      /*if currWord-1 is PRINTN, printf("\n")*/
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalRnd(nalFile *p)
{
   int i;
   bool correct;
   char syntax[RANDWORDS][MAXWORDSIZE] = {"RND", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < RANDWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
            if (!strsame(p->words[p->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfcond(nalFile *p)
{
   if (nalIfequal(p) == EXECUTED || nalIfgreater(p) == EXECUTED) {
      if (!strsame(p->words[p->currWord], "{")) {
         syntaxERROR(p, "CONDITION", "{", p->currWord);
      }
      p->currWord++;
      instrs(p);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfequal(nalFile *p)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFEQUAL", "(", "VARCON", ",", "VARCON", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(p->words[p->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfgreater(nalFile *p)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFGREATER", "(", "VARCON", ",", "VARCON", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(p->words[p->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalInc(nalFile *p)
{
   int i;
   bool correct;
   char syntax[INCWORDS][MAXWORDSIZE] = {"INC", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(p->words[p->currWord], syntax[0])) {
      p->currWord++;
      for (i = 1; i < INCWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(p->words[p->currWord])) {
               correct = FALSE;
            }
         } else {
            if (!strsame(p->words[p->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(p, syntax[i-1], syntax[i], p->currWord);
         }
         p->currWord++;
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalSet(nalFile *p)
{
   if (isvar(p->words[p->currWord]) && strsame(p->words[p->currWord+1],"=")) {
      p->currWord+=2;
   if (!isvarcon(p->words[p->currWord])) {
      syntaxERROR(p, "=", "VARCON", p->currWord);
      }
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*This function exists only for clarity, could just use validVar(p, '#') everywhere instead*/
bool isnumvar(char const *word)
{
   return validVar(word, '%');
}

/*This function exists only for clarity, could just use validVar(p, '$') everywhere instead*/
bool isstrvar(char const *word)
{
   return validVar(word, '$');
}

bool validVar(char const *word, char c)
{
   int i;

   if (strlen(word)<2) {
      return FALSE;
   }

   if (word[0]!=c) {
      return FALSE;
   }

   for (i = 1; i < (int)strlen(word); i++) {
      if (!isupper(word[i])) {
         return FALSE;
      }
   }
   return TRUE;
}

bool isstrcon(char const *word)
{
   char first, last;

   if (strlen(word)<2) {
      return FALSE;
   }

   first = word[0];
   last = word[strlen(word)-1];

   if (first == '\"' && last == '\"') {
      return TRUE;
   }
   if (first == '#' && last == '#') {
      return TRUE;
   }
   return FALSE;
}

/*In this function strtod()'s output is unused, maybe somehow use it?*/
bool isnumcon(char const *word)
{
   char *lastchar;

   if (strlen(word)<1) {
      return FALSE;
   }

   strtod(word, &lastchar);
   if (*lastchar == '\0') {
      return TRUE;
   }
   return FALSE;
}

bool isvar(char const *word)
{
   if (isnumvar(word) || isstrvar(word)) {
      return TRUE;
   }
   return FALSE;
}

bool iscon(char const *word)
{
   if (isnumcon(word) || isstrcon(word)) {
      return TRUE;
   }
   return FALSE;
}

bool isvarcon(char const *word)
{
   if (isvar(word) || iscon(word)) {
      return TRUE;
   }
   return FALSE;
}

void nalERROR(nalFile *p, char* const msg)
{
   terminateNalFile(&p);
   ERROR(msg);
}

void syntaxERROR(nalFile *p, char const *prevWord, char const *expWord,  int index)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = SYNTAXERRORLENGTH;
   errorLineLength += strlen(expWord);
   errorLineLength += strlen(prevWord);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "Expected %s after %s at index %d\n", expWord, prevWord, index);

   terminateNalFile(&p);
   fprintf(stderr, "%s", errorLine);
   free(errorLine);
   exit(EXIT_FAILURE);
}

void *allocate(int size, char* const msg)
{
   void *p;

   p = malloc(size);
   if (p == NULL) {
      fprintf(stderr,"Couldn't allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}

void *callocate(int size1, int size2, char* const msg)
{
   void *p;

   p = calloc(size1,size2);
   if (p == NULL) {
      fprintf(stderr,"Couldn't allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}

void ERROR(char* const msg)
{
   fprintf(stderr, "%s", msg);
   exit(EXIT_FAILURE);
}
