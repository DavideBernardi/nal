#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define MAXWORDSIZE 1000
#define strsame(A,B) (strcmp(A, B)==0)
#define ERROR(PHRASE) {fprintf(stderr, "Fatal Error %s occurred in %s, line %d\n", PHRASE, __FILE__, __LINE__); exit(EXIT_SUCCESS); }

typedef enum {FALSE, TRUE} bool;

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

/*These are used to play with the file*/
void checkInput(int argc, char const *argv[]);
FILE *getFile(char const file[]);
list *getWordSizes(FILE *fp);
int wordLength(char const *currWord, FILE *fp, list *wordLens);
void tokenizeFile(nalFile *p, FILE *fp, list *wordLengths);
void getWord(char *word, FILE* fp);
void strAppend(char *word, char c);
nalFile *initNalFile(void);
void terminateNalFile(nalFile **p);
bool multiWordString(char const *word);

/*These are used for the int linked list*/
node *allocateNode(int i);
void freeList(list **p);

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);

/*Syntax*/
void prog(nalFile *p);
void code(nalFile *p);
void statement(nalFile *p);

int main(int argc, char const *argv[])
{
   FILE *fp;
   nalFile *p;
   list *wordLengths;
   int i;

   checkInput(argc, argv);
   fp = getFile(argv[1]);

   wordLengths = getWordSizes(fp);
   p = initNalFile();
   tokenizeFile(p, fp, wordLengths);
   fclose(fp);

   for (i = 0; i < p->totWords; i++) {
      printf("%s\n", p->words[i]);
   }

   /*prog(&p);
   printf("Parsed OK\n");*/

   terminateNalFile(&p);
   return 0;
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
   p = NULL;
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
      ERROR("Empty file");
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
            ERROR("No matching string characters (\" or #)");
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
   p = NULL;
}

node *allocateNode(int i)
{
   node *n;

   n = allocate(sizeof(node), "Node");

   n->data = i;
   n->next = NULL;
   return n;
}

void prog(nalFile *p)
{
   if (!strsame(p->words[p->currWord], "BEGIN")) {
      ERROR("No BEGIN statement ?");
   }
   p->currWord++;
   code(p);
}

void code(nalFile *p)
{
   if (strsame(p->words[p->currWord], "END")) {
      return;
   }
   statement(p);
   p->currWord++;
   code(p);
}

void statement(nalFile *p)
{
   if (strsame(p->words[p->currWord], "ONE")) {
      return;
   }
   if (strsame(p->words[p->currWord], "NOUGHT")) {
      return;
   }
   ERROR("Expecting a ONE or NOUGHT ?");
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
