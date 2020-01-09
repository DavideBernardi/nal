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
#define ERRORLINELENGTH 50
#define SYNTAXERRORLENGTH 30

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

typedef struct nalVar{
   vartype type;
   char *varname;
   char *strval;
   struct nalVar *next;
}nalVar;

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

/*Base Functions*/
void checkInput(int argc, char const *argv[]);
FILE *getFile(char const file[]);
nalFile *initNalFile(void);
void terminateNalFile(nalFile **nf);
void terminateAllNalFiles(nalFile *nf);
void setupNalFile(nalFile *nf, char const file[]);
void wordStep(nalFile *nf, int s);

/*ERROR message functions*/
void nalERROR(nalFile *nf, char* const msg);
void syntaxERROR(nalFile *nf, char const *prevWord, char const *expWord,  int index);
void ERROR(char* const msg);

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

/*These are malloc and calloc but also give errors if they fail to allocate*/
void *allocate(int size, char* const msg);
void *callocate(int size1, int size2, char* const msg);


int main(int argc, char const *argv[])
{
   nalFile *nf;

   test();

   checkInput(argc, argv);
   nf = initNalFile();
   setupNalFile(nf, argv[1]);
   program(nf);
   #ifndef INTERP
   printf("Parsed OK\n");
   #endif
   terminateNalFile(&nf);
   return 0;
}

void test(void)
{
/* initNatFile
   terminateNalFile */

   nalFile *nf;

   nf = initNalFile();
   assert(nf!=NULL);
   assert(nf->words == NULL);
   assert(nf->currWord == 0);
   assert(nf->totWords == 0);

   terminateNalFile(&nf);
   assert(nf==NULL);

   testTokenization();
   testParsingFunctions();
   testInterpFunctions();
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

   /*Testing wordStep*/
   for (i = 1; i < WORDSINTEST1; i++) {
      wordStep(testNal, 1);
      assert(strsame(testNal->words[testNal->currWord],wordsInTest1[i]));
   }

   terminateNalFile(&testNal);
}

void testParsingFunctions(void)
{
   /*Test validVar*/
   assert(validVar("$C",'$'));
   assert(validVar("$VARNAME",'$'));
   assert(validVar("$VERYVERYVERYVERYLONGVARNAME",'$'));
   assert(!validVar("$",'$'));
   assert(!validVar("",'$'));
   assert(!validVar("$c",'$'));
   assert(!validVar("$varname",'$'));
   assert(!validVar("$veryveryveryverylongvarname",'$'));
   assert(!validVar("$1numbersarenotallowed",'$'));
   assert(!validVar("NOTAVAR",'$'));

   assert(validVar("%C",'%'));
   assert(validVar("%VARNAME",'%'));
   assert(validVar("%VERYVERYVERYVERYLONGVARNAME",'%'));
   assert(!validVar("%",'%'));
   assert(!validVar("",'%'));
   assert(!validVar("%c",'%'));
   assert(!validVar("%varname",'%'));
   assert(!validVar("%veryveryveryverylongvarname",'%'));
   assert(!validVar("%1numbersarenotallowed",'%'));
   assert(!validVar("NOTAVAR",'%'));

   /*Test isnumvar*/
   assert(isnumvar("%C"));
   assert(isnumvar("%VARNAME"));
   assert(isnumvar("%VERYVERYVERYVERYLONGVARNAME"));
   assert(!isnumvar("%"));
   assert(!isnumvar(""));
   assert(!isnumvar("%c"));
   assert(!isnumvar("%varname"));
   assert(!isnumvar("%veryveryveryverylongvarname"));
   assert(!isnumvar("%1numbersarenotallowed"));
   assert(!isnumvar("NOTAVAR"));

   /*Test isstrvar*/
   assert(isstrvar("$C"));
   assert(isstrvar("$VARNAME"));
   assert(isstrvar("$VERYVERYVERYVERYLONGVARNAME"));
   assert(!isstrvar("$"));
   assert(!isstrvar(""));
   assert(!isstrvar("$c"));
   assert(!isstrvar("$varname"));
   assert(!isstrvar("$veryveryveryverylongvarname"));
   assert(!isstrvar("$1numbersarenotallowed"));
   assert(!isstrvar("NOTAVAR"));

   /*Test isstrcon*/
   assert(isstrcon("\"\""));
   assert(isstrcon("\"Hello World!\""));
   assert(isstrcon("\"If I have #hashtags in a string they don't interrupt the syntax\""));
   assert(isstrcon("\"Even if I have #two#\""));
   assert(!isstrcon("\"If not at the end, \" is not a string"));
   assert(isstrcon("\"1413\""));
   assert(!isstrcon("Not a string"));
   assert(!isstrcon("SOMEVAR"));
   assert(!isstrcon("#SOMEVAR"));
   assert(isstrcon("\"\""));
   assert(isstrcon("#Hello World!#"));
   assert(isstrcon("#If I have \"quotes in a string they don't interrupt the syntax#"));
   assert(isstrcon("#Even if I have \"two\"#"));
   assert(!isstrcon("#If not at the end, # is not a string"));


   /*Test isnumcon*/
   assert(!isnumcon(""));
   assert(isnumcon("0"));
   assert(isnumcon("0.0"));
   assert(isnumcon("1"));
   assert(isnumcon("-1"));
   assert(isnumcon("652353.14132"));
   assert(isnumcon("-524243.15345345"));
   assert(!isnumcon("4,3"));
   assert(!isnumcon("SOMEWORD"));
   assert(!isnumcon("123F123"));

   /*No need to test isvar(), iscon(), isvarcon() because they are just unions of other functions*/
}

void testInterpFunctions(void)
{
   testROT();

   /*Test getString*/
   testGetString("\"\"","");
   testGetString("\"Hello World\"","Hello World");
   testGetString("\"HELLO.TXT\"","HELLO.TXT");
   testGetString("#URYYB.GKG#","HELLO.TXT");
   testGetString("\"There is the number 48 and 12 in here\"", "There is the number 48 and 12 in here");
   testGetString("#Gurer vf gur ahzore 93 naq 67 va urer#", "There is the number 48 and 12 in here");

}

void testROT(void)
{
   /*For some reason I need to add +1 to these arrays or I get overflow errors - doesn't the [] syntax automatically add 1 for the end of string char?*/
   char PlainUpper[ALPHABET+1];
   char RotUpper[ALPHABET+1];
   char PlainLower[ALPHABET+1];
   char RotLower[ALPHABET+1];
   char PlainNum[NUMBASE+1];
   char RotNum[NUMBASE+1];
   char PlainSyms[SYMSAMOUNT+1];
   int i;

   strcpy(PlainUpper,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
   strcpy(PlainLower,"abcdefghijklmnopqrstuvwxyz");
   strcpy(PlainNum,"0123456789");
   strcpy(PlainSyms,".,<>/?@':;~#|\\!\"£$%^&*()_-+={}]['");
   strcpy(RotUpper,"NOPQRSTUVWXYZABCDEFGHIJKLM");
   strcpy(RotLower,"nopqrstuvwxyzabcdefghijklm");
   strcpy(RotNum,"5678901234");

   for (i = 0; i < ALPHABET; i++) {
      assert(PlainUpper[i]==ROT(RotUpper[i]));
      assert(PlainLower[i]==ROT(RotLower[i]));
   }

   for (i = 0; i < SYMSAMOUNT; i++) {
      assert(PlainSyms[i]==ROT(PlainSyms[i]));
   }

   for (i = 0; i < NUMBASE; i++) {
      assert(PlainNum[i]==ROT(RotNum[i]));
   }
}

/*word is the strcon, realStr is what the output from getString should look like*/
void testGetString(char const* word, char const* realStr)
{
   char *str;
   str = getString(word);
   assert(strsame(str,realStr));
   free(str);
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
      fprintf(stderr, "Failed to open file \"%s\"\nTerminating . . .\n", file);
      exit(EXIT_FAILURE);
   }

   return ifp;
}

void setupNalFile(nalFile *nf, char const file[])
{
   FILE *fp;
   intList *wordLengths;

   fp = getFile(file);
   wordLengths = getWordSizes(fp);
   tokenizeFile(nf, fp, wordLengths);
   fclose(fp);
}

nalFile *initNalFile(void)
{
   nalFile *nf;

   nf = (nalFile *)allocate(sizeof(nalFile), "Nal File");

   nf->words = NULL;
   nf->currWord = 0;
   nf->totWords = 0;
   nf->prev = NULL;

   return nf;
}

void terminateNalFile(nalFile **nf)
{
   nalFile *q;
   int i;

   if (*nf == NULL) {
      return;
   }

   q = *nf;

   for (i = 0; i < q->totWords; i++) {
      free(q->words[i]);
   }
   free(q->words);
   free(q);
   *nf = NULL;
}

/* Return a intList containing the exact size of each word in the file,
   the intList structure also contains the total number of words. */
intList *getWordSizes(FILE *fp)
{
   char currWord[MAXWORDSIZE];
   intList *wordLens;
   intNode *curr;

   wordLens = (intList *)allocate(sizeof(intList), "List");
   wordLens->size = 0;

   if (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr = allocateNode(wordLength(currWord, fp, wordLens));
      wordLens->head = curr;
   } else {
      fclose(fp);
      free(wordLens);
      ERROR("File is Empty, Terminating ...\n");
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
void tokenizeFile(nalFile *nf, FILE *fp, intList *wordLengths)
{
   intNode *curr, *oldNode;
   int i;

   nf->words = (char **)callocate(sizeof(char *), wordLengths->size, "Words");

   curr = wordLengths->head;
   i = 0;
   rewind(fp);

   while (curr!=NULL ) {
      nf->words[i] = (char *)callocate(sizeof(char), curr->data+1, "Word");
      getWord(nf->words[i], fp);
      i++;
      nf->totWords++;
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

   n = (intNode *)allocate(sizeof(intNode), "Node");

   n->data = i;
   n->next = NULL;
   return n;
}

void wordStep(nalFile *nf, int s)
{
   nf->currWord += s;
   if (nf->currWord >= nf->totWords) {
      nalERROR(nf,"Word Index Out of Bounds, Terminating...\n");
   }
}

void program(nalFile *nf)
{
   if (!strsame(nf->words[nf->currWord], "{")) {
      nalERROR(nf, "No starting \'{\'\n");
   }
   wordStep(nf,1);
   instrs(nf);
}

void instrs(nalFile *nf)
{
   if (strsame(nf->words[nf->currWord], "}")) {
      return;
   }
   instruct(nf);
   instrs(nf);
}

void instruct(nalFile *nf)
{
   char errorLine[ERRORLINELENGTH];

   if (nalAbort(nf)==EXECUTED) {
      return;
   }
   if (file(nf)==EXECUTED) {
      return;
   }
   if (input(nf)==EXECUTED) {
      return;
   }
   if (jump(nf)==EXECUTED) {
      return;
   }
   if (nalPrint(nf)==EXECUTED) {
      return;
   }
   if (nalRnd(nf)==EXECUTED) {
      return;
   }
   if (nalIfcond(nf)==EXECUTED) {
      return;
   }
   if (nalInc(nf)==EXECUTED) {
      return;
   }
   if (nalSet(nf)==EXECUTED) {
      return;
   }

   sprintf(errorLine, "Word at index %d doesn't match any syntax rule\n", nf->currWord);
   nalERROR(nf, errorLine);
}

instr file(nalFile *nf)
{
   #ifdef INTERP
   nalFile *newFile;
   char *fileName;
   #endif

   if (strsame(nf->words[nf->currWord], "FILE")) {
      wordStep(nf,1);
      if (!isstrcon(nf->words[nf->currWord])) {
         syntaxERROR(nf, "FILE", "STRCON", nf->currWord);
      }
      #ifdef INTERP
      fileName = getString(nf->words[nf->currWord]);
      newFile = initNalFile();
      setupNalFile(newFile, fileName);
      free(fileName);
      /*Keep track of previous file in case of ABORT call*/
      newFile->prev = nf;
      program(newFile);
      terminateNalFile(&newFile);
      #endif
      wordStep(nf,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Given a word that returns TRUE from isstrcon, output the string*/
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
         str[i] = ROT(word[i+1]);
      }
      str[strSize] = '\0';
   }
   return str;
}

char ROT(char c)
{
   if (islower(c)) {
      c = ROTbase(c, 'a', ROTCHAR, ALPHABET);
   } else if (isupper(c)) {
      c = ROTbase(c, 'A', ROTCHAR, ALPHABET);
   } else if (isnumber(c)) {
      c = ROTbase(c, '0', ROTNUM, NUMBASE);
   }
   return c;
}

char ROTbase(char c, char base, int rotVal, int alphabet)
{
   c = c - base;
   c = (c-rotVal);
   if (c<0) {
      c+=alphabet;
   }
   c = c + base;
   return c;
}

bool isnumber(char c)
{
   if (c>='0' && c<='9') {
      return TRUE;
   }
   return FALSE;
}

/*This should successfully abort a program with multiple files opened*/
instr nalAbort(nalFile *nf)
{
   if (strsame(nf->words[nf->currWord], "ABORT")) {
      #ifdef INTERP
      terminateAllNalFiles(nf);
      exit(EXIT_SUCCESS);
      #endif
      wordStep(nf,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr input(nalFile *nf)
{

   if (in2str(nf) == EXECUTED) {
      return EXECUTED;
   }
   if (innum(nf) == EXECUTED) {
      return EXECUTED;
   }
   return NOTEXECUTED;

}

instr in2str(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IN2STR", "(", "STRVAR", ",", "STRVAR", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "STRVAR")) {
            if (!isstrvar(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {;
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr innum(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[INNUMWORDS][MAXWORDSIZE] = {"INNUM", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < INNUMWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Can Add index to ERROR message by printing to a string and passing that string*/
instr jump(nalFile *nf)
{
   #ifdef INTERP
   int n, err;
   #endif
   if (strsame(nf->words[nf->currWord], "JUMP")) {
      wordStep(nf,1);
      if (!isnumcon(nf->words[nf->currWord])) {
         syntaxERROR(nf, "JUMP", "NUMCON", nf->currWord);
      }
      #ifdef INTERP
      err = sscanf(nf->words[nf->currWord], "%d", &n);
      if (err!=1 || n<0 || n>nf->totWords-1) {
         nalERROR(nf, "Number after JUMP is not the index of a word in the file, terminating . . .\n");
      }
      nf->currWord = n;
      #else
      wordStep(nf,1);
      #endif
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalPrint(nalFile *nf)
{
   if (strsame(nf->words[nf->currWord], "PRINT") || strsame(nf->words[nf->currWord], "PRINTN")) {
      wordStep(nf,1);
      if (!isvarcon(nf->words[nf->currWord])) {
         syntaxERROR(nf, nf->words[nf->currWord-1], "VARCON",nf->currWord);
      }
      /*if currWord-1 is PRINTN, printf("\n")*/
      wordStep(nf,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalRnd(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[RANDWORDS][MAXWORDSIZE] = {"RND", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < RANDWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
            if (!strsame(nf->words[nf->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfcond(nalFile *nf)
{
   if (nalIfequal(nf) == EXECUTED || nalIfgreater(nf) == EXECUTED) {
      if (!strsame(nf->words[nf->currWord], "{")) {
         syntaxERROR(nf, "CONDITION", "{", nf->currWord);
      }
      wordStep(nf,1);
      instrs(nf);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfequal(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFEQUAL", "(", "VARCON", ",", "VARCON", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalIfgreater(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFGREATER", "(", "VARCON", ",", "VARCON", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalInc(nalFile *nf)
{
   int i;
   bool correct;
   char syntax[INCWORDS][MAXWORDSIZE] = {"INC", "(", "NUMVAR", ")"};

   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,1);
      for (i = 1; i < INCWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            }
         } else {
            if (!strsame(nf->words[nf->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            syntaxERROR(nf, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,1);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalSet(nalFile *nf)
{
   if (isvar(nf->words[nf->currWord]) && strsame(nf->words[nf->currWord+1],"=")) {
      wordStep(nf,2);
   if (!isvarcon(nf->words[nf->currWord])) {
      syntaxERROR(nf, "=", "VARCON", nf->currWord);
      }
      wordStep(nf,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*This function exists only for clarity, could just use validVar(nf, '#') everywhere instead*/
bool isnumvar(char const *word)
{
   return validVar(word, '%');
}

/*This function exists only for clarity, could just use validVar(nf, '$') everywhere instead*/
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

void terminateAllNalFiles(nalFile *nf)
{
   nalFile *prevFile;

   while (nf!=NULL) {
      prevFile = nf->prev;
      terminateNalFile(&nf);
      nf = prevFile;
   }
}

void nalERROR(nalFile *nf, char* const msg)
{
   terminateAllNalFiles(nf);
   ERROR(msg);
}

void syntaxERROR(nalFile *nf, char const *prevWord, char const *expWord,  int index)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = SYNTAXERRORLENGTH;
   errorLineLength += strlen(expWord);
   errorLineLength += strlen(prevWord);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "Expected %s after %s at index %d\n", expWord, prevWord, index);

   terminateAllNalFiles(nf);
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
