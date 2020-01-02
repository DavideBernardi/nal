/*To do:
   - Change ERROR macro (make into a function?)
      1. Have it output some more useful information
      2. Maybe have it handle all the free() in case of unexpected nalAbort
      3. Maybe change exit() to something else which makes it possible to
         quit one file without stopping the whole execution
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>


/*Maximum size of a single %s pulled from the file,
Note: This is only used when initially reading in each space-separated string,
      actual words are then stored in properly allocated memory.*/
#define MAXWORDSIZE 1000
#define WORDSINTEST1 4
#define ERRORLINELENGTH 1000
#define strsame(A,B) (strcmp(A, B)==0)

typedef enum {FALSE, TRUE} bool;
typedef enum {NOTEXECUTED, EXECUTED} instr;

typedef struct nalFile{
   char **words;
   int currWord;
   int totWords;
} nalFile;

typedef struct node{
   int data;
   struct node *next;
}node;

typedef struct list{
   node *head;
   int size;
}list;

/*Testing*/
void test(void);
void testTokenization(void);

/*General*/
void checkInput(int argc, char const *argv[]);
FILE *getFile(char const file[]);
nalFile *initNalFile(void);
void terminateNalFile(nalFile **p);
void nalERROR(nalFile *p, char* const msg);
void ERROR(char* const msg);

/*Syntax*/
void program(nalFile *p);
void instrs(nalFile *p);
void instruct(nalFile *p);
instr file(nalFile *p);
instr nalAbort(nalFile *p);

bool isstrcon(nalFile *p);

/*These are used to read in and tokenize the file*/
list *getWordSizes(FILE *fp);
int wordLength(char const *currWord, FILE *fp, list *wordLens);
void tokenizeFile(nalFile *p, FILE *fp, list *wordLengths);
void getWord(char *word, FILE* fp);
void strAppend(char *word, char c);
bool multiWordString(char const *word);
/*These are for an int linked list used within getWordSizes*/
node *allocateNode(int i);
void freeList(list **p);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);


int main(int argc, char const *argv[])
{
   FILE *fp;
   nalFile *p;
   list *wordLengths;
   /*int i;*/

   test();

   checkInput(argc, argv);
   fp = getFile(argv[1]);

   wordLengths = getWordSizes(fp);
   p = initNalFile();
   tokenizeFile(p, fp, wordLengths);
   fclose(fp);

   /*for (i = 0; i < p->totWords; i++) {
      printf("%s\n", p->words[i]);
   }*/

   program(p);
   printf("Parsed OK\n");

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
   node *n, *curr, *oldNode;
   FILE *testFile;
   list *wordLengths;
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

/* Return a list containing the exact size of each word in the file,
   the list structure also contains the total number of words. */
list *getWordSizes(FILE *fp)
{
   char currWord[MAXWORDSIZE];
   list *wordLens;
   node *curr;

   wordLens = allocate(sizeof(list), "List");
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
int wordLength(char const *currWord, FILE *fp, list *wordLens)
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
void tokenizeFile(nalFile *p, FILE *fp, list *wordLengths)
{
   node *curr, *oldNode;
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

void freeList(list **p)
{
   node *curr, *oldNode;
   list *l;

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

node *allocateNode(int i)
{
   node *n;

   n = allocate(sizeof(node), "Node");

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

   sprintf(errorLine, "Word \"%s\" doesn't match any syntax rule, terminating\n",p->words[p->currWord]);
   nalERROR(p, errorLine);
}

instr file(nalFile *p)
{
   char errorLine[ERRORLINELENGTH];

   if (strsame(p->words[p->currWord], "FILE")) {
      p->currWord++;
      if (!isstrcon(p)) {
         sprintf(errorLine, "Expected STRCON at index %d\n",p->currWord);
         nalERROR(p, errorLine);
      }
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalAbort(nalFile *p)
{
   if (strsame(p->words[p->currWord], "ABORT")) {
      p->currWord++;
      return EXECUTED;
   }
   return NOTEXECUTED;
}

bool isstrcon(nalFile *p)
{
   char first, last;

   first = p->words[p->currWord][0];
   last = p->words[p->currWord][strlen(p->words[p->currWord]-1)];

   if (first == '\"' && last == '\"') {
      return TRUE;
   }
   if (first == '#' && last == '#') {
      return TRUE;
   }
   return FALSE;
}

void nalERROR(nalFile *p, char* const msg)
{
   terminateNalFile(&p);
   ERROR(msg);
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
