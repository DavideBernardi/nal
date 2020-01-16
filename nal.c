#include "nal.h"

int main(int argc, char const *argv[])
{
   nalFile *nf;
   vList *vl;
   time_t t;

   srand((unsigned) time(&t));
   test();

   checkInput(argc, argv);
   nf = initNalFile();
   vl = vList_init();
   nf->name = allocString(argv[1]);
   setupNalFile(nf, vl);
   program(nf, vl);
   #ifndef INTERP
   printf("Parsed OK\n");
   #endif
   /*vList_print(vl);*/
   terminateNalFile(&nf);
   vList_free(&vl);
   return 0;
}

void checkInput(int argc, char const *argv[])
{
   if (argc!=2) {
      fprintf(stderr, "Input must be of the form: %s <FILENAME>\n", argv[0]);
      exit(EXIT_FAILURE);
   }
}

void test(void)
{
/* initNatFile
   terminateNalFile
    */

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
   /*multiWordString
   allocateNode
   wordLength
   getWordSizes
   getWord
   tokenizeFile
   wordStep*/
   intNode *n, *curr, *oldNode;
   FILE *testFile;
   intList *wordLengths;
   char testWord[MAXWORDSIZE];
   nalFile *testNal;
   vList *testList;
   int i, strCheck;
   char wordsInTest1[WORDSINTEST1][TESTWORDSIZE] =
   {"{", "PRINT", "\"Hello World! From test1\"", "}"};

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
   testFile = getFile(NULL, NULL, "test1.nal");
   wordLengths = NULL;
   for (i = 0; i < WORDSINTEST1; i++) {
      assert(fscanf(testFile, "%s", testWord)==1);
      assert(wordLength(NULL,NULL,testWord, testFile, wordLengths)==
      (int)strlen(wordsInTest1[i]));
   }
   fclose(testFile);

   /*Testing getWordSizes*/
   testFile = getFile(NULL,NULL,"test1.nal");
   wordLengths = getWordSizes(NULL,NULL,testFile);
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
   testFile = getFile(NULL, NULL, "test1.nal");
   for (i = 0; i < WORDSINTEST1; i++) {
      getWord(NULL, NULL, testWord, testFile);
      strCheck = strsame(testWord,wordsInTest1[i]);
      assert(strCheck);
   }
   fclose(testFile);

   /*Testing tokenizeFile*/
   testFile = getFile(NULL, NULL, "test1.nal");
   testNal = initNalFile();
   wordLengths = getWordSizes(NULL,NULL,testFile);
   tokenizeFile(testNal, NULL, testFile, wordLengths);
   fclose(testFile);
   assert(testNal->totWords == WORDSINTEST1);
   for (i = 0; i < WORDSINTEST1; i++) {
      strCheck = strsame(testNal->words[i],wordsInTest1[i]);
      assert(strCheck);
   }

   /*Testing wordStep*/
   testList = vList_init();
   for (i = 1; i < WORDSINTEST1; i++) {
      wordStep(testNal,testList, 1);
      strCheck = strsame(testNal->words[testNal->currWord],wordsInTest1[i]);
      assert(strCheck);
   }
   vList_free(&testList);
   terminateNalFile(&testNal);
}

void testParsingFunctions(void)
{
   /*validVar
   isnumvar
   isstrvar
   isstrcon
   isnumcon
   No need to test isvar(), iscon(), isvarcon(), isstr() and isnum() because they are just unions of other functions*/

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
}

void testInterpFunctions(void)
{
   /*ROT()
   getString()*/

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
   /*+1 for \0*/
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
   int strCheck;

   str = getString(word);
   strCheck = strsame(str,realStr);
   assert(strCheck);
   free(str);
}

/*Error when something goes wrong durint the tokenization of a file (i.e. before the nalFile is even setup)*/
void tokenizeERROR(nalFile *nf, vList *vl, char const *file, char const *msg)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = TOKENIZEERRORLENGTH;
   errorLineLength += strlen(file);
   errorLineLength += strlen(msg);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "ERROR MESSAGE: %s\nError occurred while tokenizing file: \"%s\"\nTERMINATING . . .\n", msg, file);

   fprintf(stderr, "%s", errorLine);
   vList_free(&vl);
   terminateAllNalFiles(nf);
   free(errorLine);
   exit(EXIT_FAILURE);
}

/*Basic error when an error that has to do with a tokenized nalFile happens.*/
void nalERROR(nalFile *nf, vList *vl, char* const msg)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = NALERRORLENGTH;
   errorLineLength += strlen(msg);
   errorLineLength += strlen(nf->name);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "ERROR MESSAGE: %s\nError occurred in file \"%s\"\nTERMINATING . . .\n", msg, nf->name);

   vList_free(&vl);
   terminateAllNalFiles(nf);
   fprintf(stderr, "%s", errorLine);
   free(errorLine);
   exit(EXIT_FAILURE);
}

/*Error for when something goes wrong at a particular point in a tokenized nalFile*/
void indexERROR(nalFile *nf, vList *vl, char* const msg, int index)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = INDEXERRORLENGTH;
   errorLineLength += strlen(msg);
   errorLineLength += strlen(nf->name);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "ERROR MESSAGE: %s\nError occurred at index %d of file \"%s\"\nTERMINATING . . .\n", msg, index,nf->name);

   vList_free(&vl);
   terminateAllNalFiles(nf);
   fprintf(stderr, "%s", errorLine);
   free(errorLine);
   exit(EXIT_FAILURE);
}

/*Error for when something goes wrong at a particular point in a tokenized nalFile.
SPECIFICALLY: A syntax rule is broken*/
void syntaxERROR(nalFile *nf, vList *vl,char const *prevWord, char const *expWord,  int index)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = SYNTAXERRORLENGTH;
   errorLineLength += strlen(expWord);
   errorLineLength += strlen(prevWord);
   errorLineLength += strlen(nf->name);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "Expected %s after %s at index %d in file \"%s\"\n", expWord, prevWord, index,nf->name);

   vList_free(&vl);
   terminateAllNalFiles(nf);
   fprintf(stderr, "%s", errorLine);
   free(errorLine);
   exit(EXIT_FAILURE);
}

/*Error for when something goes wrong at a particular point in a tokenized nalFile.
SPECIFICALLY: A variable is used incorrectly*/
void variableERROR(nalFile *nf, vList *vl, char const *msg, char* const name, int index)
{
   char *errorLine;
   int errorLineLength;

   errorLineLength = VARIABLEERRORLENGTH;
   errorLineLength += strlen(msg);
   errorLineLength += strlen(name);
   errorLineLength += strlen(nf->name);

   errorLine = (char *)allocate(errorLineLength*sizeof(char),"Error Line");

   sprintf(errorLine, "ERROR MESSAGE: %s\nAbout Variable %s in file \"%s\" around index %d\nTERMINATING . . .\n",msg,name,nf->name,index);

   vList_free(&vl);
   terminateAllNalFiles(nf);
   fprintf(stderr, "%s", errorLine);
   free(errorLine);
   exit(EXIT_FAILURE);
}

/*this is malloc() with a built in error message if allocation fails*/
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

/*this is calloc() with a built in error message if allocation fails*/
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

/*Given a string, it allocates some new space to store it and copies it, returns a pointer to the new space*/
char *allocString(const char *str)
{
   char *newStr;

   if (str == NULL) {
      return NULL;
   }

   newStr = (char *)allocate(sizeof(char)*(strlen(str)+1),"String");
   strcpy(newStr,str);

   return newStr;
}

/*Given a file name file[], returns a pointer to the file structure for that file.
nf and vl are only passed here in case they need to be freed after an error occurs - NULL pointers can be passed instead and if the file is read correctly nothing would change*/
FILE *getFile(nalFile *nf, vList *vl, char const file[])
{
   FILE *ifp;

   ifp = fopen(file, "rb");
   if (ifp == NULL) {
      tokenizeERROR(nf,vl, file, "Couldn't open File");
   }

   return ifp;
}

/* Returns an integer List structure containing the exact size of each word in the file, and the total number of words.
(Again nf and vl are only passed to be freed in case of ERRORS)*/
intList *getWordSizes(nalFile *nf, vList *vl, FILE *fp)
{
   char currWord[MAXWORDSIZE];
   intList *wordLens;
   intNode *curr;

   wordLens = (intList *)allocate(sizeof(intList), "List");
   wordLens->size = 0;

   /*Code for filling head of list*/
   if (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr = allocateNode(wordLength(nf, vl, currWord, fp, wordLens));
      wordLens->head = curr;
   } else {
      fclose(fp);
      free(wordLens);
      tokenizeERROR(nf,vl,nf->name,"The File is Empty");
   }

   /*Code for filling rest of list*/
   while (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr->next = allocateNode(wordLength(nf, vl, currWord, fp, wordLens));
      curr = curr->next;
   }

   return wordLens;
}

/*Finds length of the input word, if the word is the start of a string,
it moves along the file until it finds the end of that string as strings count as one word
(nf and vl only passed to be freed in case of ERRORS)*/
int wordLength(nalFile *nf, vList *vl, char const *currWord, FILE *fp, intList *wordLens)
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
            /*nf can't be NULL*/
            tokenizeERROR(nf, vl, nf->name, "No matching string characters (\" or #)");
         }
         wordLen++;
      }
      wordLen++;
   }
   return wordLen;
}

/*Allocates the exact amount of space for the array of words and then fills it up
Note: Here nf is the file that needs to be tokenized, it needs to already have the pointer to the previous file stored in nf->prev (or NULL if it is the first file opened).
vl is passed to be freed in case of ERRORS*/
void tokenizeFile(nalFile *nf, vList* vl, FILE *fp, intList *wordLengths)
{
   intNode *curr, *oldNode;
   int i;

   nf->words = (char **)callocate(sizeof(char *), wordLengths->size, "Words");

   curr = wordLengths->head;
   i = 0;
   rewind(fp);

   while (curr!=NULL ) {
      nf->words[i] = (char *)callocate(sizeof(char), curr->data+1, "Word");
      getWord(nf,vl,nf->words[i], fp);
      i++;
      nf->totWords++;
      oldNode = curr;
      curr = curr->next;
      free(oldNode);
   }
   free(wordLengths);
}

/*Fills word (which is of the correct size thanks to getWordSizes)
with the correct string from the file.
(nf and vl only passed to be freed in case of ERRORS).*/
void getWord(nalFile *nf, vList *vl, char *word, FILE* fp)
{
   char c, charToMatch;
   char currWord[MAXWORDSIZE];

   if (fscanf(fp, "%s", currWord)==1) {
      strcpy(word, currWord);
      if (multiWordString(currWord)) {
         charToMatch = currWord[0];
         while ((c = getc(fp)) != charToMatch) {
            if (c == EOF) {
               tokenizeERROR(nf,vl,nf->name,"Mismatch between allocated space for a Word and its Size");
            }
            strAppend(word, c);
         }
         strAppend(word, c);
      }
   }else{
      tokenizeERROR(nf,vl,nf->name,"Error while scanning file");
   }

}

/*Puts char c at the end of string word*/
void strAppend(char *word, char c)
{
   int len;

   len = strlen(word);

   word[len] = c;
   word[len+1] = '\0';
}

/*Says wether 'word' opens a string without closing it (i.e. starts with a " or #
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

   if (p == NULL) {
      return;
   }

   l = *p;

   if (l == NULL) {
      return;
   }

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

nalFile *initNalFile(void)
{
   nalFile *nf;

   nf = (nalFile *)allocate(sizeof(nalFile), "Nal File");

   nf->words = NULL;
   nf->currWord = 0;
   nf->totWords = 0;
   nf->prev = NULL;
   nf->name=NULL;

   return nf;
}

/*This function gets the file which has the name stored in nf->name, tokenizes it and places it into nf.
NOTE: the attributes 'name' and 'prev' of the nalFile structure have to be set before this function is called*/
void setupNalFile(nalFile *nf, vList *vl)
{
   FILE *fp;
   intList *wordLengths;

   fp = getFile(nf,vl,nf->name);
   wordLengths = getWordSizes(nf,vl,fp);
   tokenizeFile(nf, vl, fp, wordLengths);
   fclose(fp);
}

void terminateNalFile(nalFile **p)
{
   nalFile *nf;
   int i;

   if (p == NULL) {
      return;
   }

   nf = *p;

   if (nf == NULL) {
      return;
   }

   for (i = 0; i < nf->totWords; i++) {
      free(nf->words[i]);
   }
   free(nf->words);
   free(nf->name);
   free(nf);
   *p = NULL;
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

/*Increases the nalFile word index by s, checks if it is out of bounds*/
void wordStep(nalFile *nf, vList *vl, int s)
{
   nf->currWord += s;
   if (nf->currWord >= nf->totWords) {
      indexERROR(nf, vl,"Word Index Out of Bounds (Maybe missing a closing '}'?)",nf->currWord);
   }
}

void program(nalFile *nf, vList *vl)
{
   if (!strsame(nf->words[nf->currWord], "{")) {
      nalERROR(nf, vl,"No starting \'{\'");
   }
   wordStep(nf,vl,1);
   instrs(nf, vl);
}

void instrs(nalFile *nf, vList *vl)
{
   if (strsame(nf->words[nf->currWord], "}")) {
      if (nf->currWord<nf->totWords-1) {
         wordStep(nf,vl,1);
      }
      return;
   }
   instruct(nf,vl);
   instrs(nf,vl);
}

void instruct(nalFile *nf, vList *vl)
{

   if (nalAbort(nf,vl)==EXECUTED) {
      return;
   }
   if (file(nf,vl)==EXECUTED) {
      return;
   }
   if (input(nf,vl)==EXECUTED) {
      return;
   }
   if (jump(nf,vl)==EXECUTED) {
      return;
   }
   if (nalPrint(nf,vl)==EXECUTED) {
      return;
   }
   if (nalRnd(nf,vl)==EXECUTED) {
      return;
   }
   if (nalIfcond(nf,vl)==EXECUTED) {
      return;
   }
   if (nalInc(nf,vl)==EXECUTED) {
      return;
   }
   if (nalSet(nf,vl)==EXECUTED) {
      return;
   }

   indexERROR(nf,vl, "Word doesn't match any syntax rule",nf->currWord);
}

/*For rules with more complex syntax, simply pass an array of strings containing all the words and VARCONs that need to be in the syntax and parse it through here. Then return an array of strings containing all the VARCONs that need to be used by the interpeter*/
char **checkSyntax(nalFile *nf, vList *vl, char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE], int syntaxSize, int varconsToBeFound)
{
   int i;
   char **varcons;
   #ifdef INTERP
   int varconsFound;
   #endif

   varcons = (char**)allocate(varconsToBeFound*sizeof(char *),"VARCON array");
   #ifdef INTERP
   varconsFound = 0;
   #else
   /*Have to do this weird thing otherwise when compiling without -DINTERP
   varconsToBeFound is a unused variable*/
   free(varcons);
   varcons = NULL;
   #endif

   for (i = 1; i < syntaxSize; i++) {
      if (correctVARCON(syntax[i],nf->words[nf->currWord])) {
         #ifdef INTERP
         varcons[varconsFound] = allocString(nf->words[nf->currWord]);
         varconsFound++;
         #endif
      } else if (!strsame(nf->words[nf->currWord], syntax[i])) {
         #ifdef INTERP
         freeArray(varcons,varconsToBeFound);
         #endif
         syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
      }
      wordStep(nf,vl,1);
   }

   return varcons;
}

bool correctVARCON(char *varconType, char *word)
{
   if (strsame(varconType,"STRVAR")) {
      return isstrvar(word);
   }
   if (strsame(varconType,"STRCON")) {
      return isstrcon(word);
   }
   if (strsame(varconType,"NUMVAR")) {
      return isnumvar(word);
   }
   if (strsame(varconType,"NUMCON")) {
      return isnumcon(word);
   }
   if (strsame(varconType,"VAR")) {
      return isvar(word);
   }
   if (strsame(varconType,"CON")) {
      return iscon(word);
   }
   if (strsame(varconType,"VARCON")) {
      return isvarcon(word);
   }
   if (strsame(varconType,"STR")) {
      return isstr(word);
   }
   if (strsame(varconType,"NUM")) {
      return isnum(word);
   }
   return FALSE;
}

void freeArray(char **array, int size)
{
   int i;

   if (array == NULL) {
      return;
   }

   for (i = 0; i < size; i++) {
      free(array[i]);
   }
   free(array);
}

instr file(nalFile *nf, vList *vl)
{
   #ifdef INTERP
   nalFile *newFile;
   #endif

   if (strsame(nf->words[nf->currWord], "FILE")) {
      wordStep(nf,vl,1);
      if (!isstrcon(nf->words[nf->currWord])) {
         syntaxERROR(nf,vl, "FILE", "STRCON", nf->currWord);
      }
      #ifdef INTERP
      newFile = initNalFile();
      newFile->name = getString(nf->words[nf->currWord]);
      /*Keep track of previous file in case of ABORT call*/
      newFile->prev = nf;
      setupNalFile(newFile, vl);
      program(newFile,vl);
      terminateNalFile(&newFile);
      #endif
      wordStep(nf,vl,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*This should successfully abort a program with multiple files opened*/
instr nalAbort(nalFile *nf, vList *vl)
{
   if (strsame(nf->words[nf->currWord], "ABORT")) {
      #ifdef INTERP
      vList_free(&vl);
      terminateAllNalFiles(nf);
      exit(EXIT_SUCCESS);
      #endif
      wordStep(nf,vl,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr input(nalFile *nf, vList *vl)
{

   if (in2str(nf,vl) == EXECUTED) {
      return EXECUTED;
   }
   if (innum(nf,vl) == EXECUTED) {
      return EXECUTED;
   }
   return NOTEXECUTED;

}

instr in2str(nalFile *nf, vList *vl)
{
   char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE] = {"IN2STR", "(", "STRVAR", ",", "STRVAR", ")"};
   char **varnames;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,vl,1);
      varnames = checkSyntax(nf, vl, syntax, IN2STRWORDS, VARSFROMIN2STR);
      #ifdef INTERP
      insertInputStrings(nf,vl,varnames);
      #endif
      freeArray(varnames,VARSFROMIN2STR);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Using scanf to place the input into an array of fixed size can cause overflow errors, please improve this.*/
void insertInputStrings(nalFile *nf, vList *vl, char **varnames)
{
   char **strs;

   strs = allocate(VARSFROMIN2STR*sizeof(char *),"Input Strings Array");
   /*Allocating instead of using str1[MAXINPUTSTRLEN]
   because too big for stack*/
   strs[0] = allocate(MAXINPUTSTRLEN*sizeof(char),"Input String");
   strs[1] = allocate(MAXINPUTSTRLEN*sizeof(char),"Input String");

   if (scanf("%s %s", strs[0], strs[1])!=VARSFROMIN2STR) {
      freeArray(strs,VARSFROMIN2STR);
      freeArray(varnames,VARSFROMIN2STR);
      indexERROR(nf, vl, "Error while receiving string input",nf->currWord-IN2STRWORDS);
   }

   vList_insert(vl, varnames[0],strs[0]);
   vList_insert(vl, varnames[1],strs[1]);
   freeArray(strs,VARSFROMIN2STR);
}

instr innum(nalFile *nf, vList *vl)
{
   char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE] = {"INNUM", "(", "NUMVAR", ")"};
   char **varname;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,vl,1);
      varname = checkSyntax(nf, vl, syntax, INNUMWORDS, VARSFROMINNUM);
      #ifdef INTERP
      insertInputNum(nf,vl,varname);
      #endif
      freeArray(varname,VARSFROMINNUM);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Using scanf to place the input into an array of fixed size can cause overflow errors, please improve this.*/
void insertInputNum(nalFile *nf, vList *vl, char** name)
{
   char num[MAXINPUTSTRLEN];

   if (scanf("%s", num)!=1) {
      freeArray(name,VARSFROMINNUM);
      indexERROR(nf, vl, "Error while receiving number input",nf->currWord-INNUMWORDS);
   }
   if (!isnumcon(num)) {
      freeArray(name,VARSFROMINNUM);
      indexERROR(nf, vl, "Error while receiving number input",nf->currWord-INNUMWORDS);
   }

   vList_insert(vl, name[0] ,num);
}

/*Can Add index to ERROR message by printing to a string and passing that string*/
instr jump(nalFile *nf, vList *vl)
{
   #ifdef INTERP
   int n, err;
   #endif
   if (strsame(nf->words[nf->currWord], "JUMP")) {
      wordStep(nf,vl,1);
      if (!isnumcon(nf->words[nf->currWord])) {
         syntaxERROR(nf,vl, "JUMP", "NUMCON", nf->currWord);
      }
      #ifdef INTERP
      err = sscanf(nf->words[nf->currWord], "%d", &n);
      if (err!=1 || n<0 || n>nf->totWords-1) {
         indexERROR(nf,vl, "Number after JUMP is not the index of a word in the file",nf->currWord);
      }
      nf->currWord = n;
      #else
      wordStep(nf,vl,1);
      #endif
      return EXECUTED;
   }
   return NOTEXECUTED;
}

instr nalPrint(nalFile *nf, vList *vl)
{
   if (strsame(nf->words[nf->currWord], "PRINT") || strsame(nf->words[nf->currWord], "PRINTN")) {
      wordStep(nf,vl,1);
      if (!isvarcon(nf->words[nf->currWord])) {
         syntaxERROR(nf,vl, nf->words[nf->currWord-1], "VARCON",nf->currWord);
      }
      #ifdef INTERP
      print(nf, vl, nf->words[nf->currWord]);
      if (strsame(nf->words[nf->currWord-1],"PRINT")) {
         printf("\n");
      }
      #endif
      wordStep(nf,vl,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void print(nalFile *nf, vList *vl, char *varcon)
{
   char *printThis, *var;
   if (isstrcon(varcon)) {
      printThis = getString(varcon);
   }
   if (isnumcon(varcon)) {
      printThis = allocString(varcon);
   }
   if (isvar(varcon)) {
      var = vList_search(vl, varcon);
      if (var == NULL) {
         printf("%s", varcon);
         variableERROR(nf, vl, "Trying to print uninitialized variable",varcon,nf->currWord);
      }
      printThis = allocString(var);
   }

   printf("%s", printThis);
   free(printThis);
}

instr nalRnd(nalFile *nf, vList *vl)
{
   char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE] = {"RND", "(", "NUMVAR", ")"};
   char **varname;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,vl,1);
      varname = checkSyntax(nf, vl, syntax, RANDWORDS, VARSFROMRAND);
      #ifdef INTERP
      setRandom(vl,varname[0]);
      #endif
      freeArray(varname,VARSFROMRAND);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void setRandom(vList *vl, char *name)
{
   char* val;
   int rnum;

   rnum = (rand()%(RANMAX+1))+RANMIN;
   val = (char *)allocate(sizeof(char)*MAXRANCHARS, "Random number");

   sprintf(val, "%d", rnum);

   vList_insert(vl, name, val);
   free(val);
}

instr nalIfcond(nalFile *nf, vList *vl)
{
   cond condition;

   condition = nalIf(nf,vl);

   if (condition != NOTEXEC) {
      if (!strsame(nf->words[nf->currWord], "{")) {
         syntaxERROR(nf,vl, "CONDITION", "{", nf->currWord);
      }
      wordStep(nf,vl,1);
      if (condition==EXECFAIL) {
         skipToMatchingBracket(nf,vl);
      } else {
         instrs(nf,vl);
      }
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void skipToMatchingBracket(nalFile *nf, vList *vl)
{
   int brackets;

   brackets = 1;
   while (brackets>0) {
      if (strsame(nf->words[nf->currWord],"{" )) {
         brackets++;
      }
      if (strsame(nf->words[nf->currWord],"}" )) {
         brackets--;
      }
      wordStep(nf,vl,1);
   }

}

/*This is awful: if a string variable and a number variable have the same value, ifequal will return true.
It should either return false or returns an error!!
Need to check whether the variable we are checking is strvar, strcon, numvar, numcon.
Based on that pull out the correct value and check it against the pulled out value of the other varcon IFF they are both either str or num*/
cond nalIf(nalFile *nf, vList *vl)
{
   char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE] = {"", "(", "VARCON", ",", "VARCON", ")"};
   cond condition;
   char **vNames;

   condition = NOTEXEC;

   if (condCalled(nf->words[nf->currWord])) {
      strcpy(syntax[0],nf->words[nf->currWord]);
      condition = EXECPASS;
      wordStep(nf,vl,1);
      vNames = checkSyntax(nf, vl, syntax, CONDWORDS, VARSFROMCOND);
      #ifdef INTERP
      if (!condCheck(nf,vl,syntax[0],vNames)) {
         condition =  EXECFAIL;
      }
      #endif
      freeArray(vNames, VARSFROMCOND);
   }
   return condition;
}

/*"Conditional Called"*/
bool condCalled(char const *word)
{
   if (strsame(word,"IFGREATER")||strsame(word,"IFEQUAL")) {
      return TRUE;
   }
   return FALSE;
}

bool condCheck(nalFile *nf, vList *vl, char *condition, char **varcons)
{
   comp comparison;

   comparison = NOCOMP;
   if (isstr(varcons[0]) && isstr(varcons[1])) {
      comparison = compStrings(nf, vl, varcons);
   }
   if (isnum(varcons[0]) && isnum(varcons[1])) {
      comparison = compNums(nf, vl, varcons);
   }

   if (comparison == NOCOMP) {
      freeArray(varcons,VARSFROMCOND);
      indexERROR(nf,vl,"Comparing two VARCONs of uncomparable types",nf->currWord-CONDWORDS);
   }

   if (comparison==EQUAL && strsame(condition,"IFEQUAL")) {
      return TRUE;
   }
   if (comparison==GREATER && strsame(condition,"IFGREATER")) {
      return TRUE;
   }
   return FALSE;
}

comp compStrings(nalFile *nf, vList *vl, char **strs)
{
   char *val1, *val2;
   int result;

   val1 = extractStr(vl,strs[0]);
   val2 = extractStr(vl,strs[1]);
   if (val1 == NULL || val2 == NULL) {
      free(val1);
      free(val2);
      freeArray(strs,VARSFROMCOND);
      indexERROR(nf, vl, "Trying to call uninitialized variable", nf->currWord-CONDWORDS);
   }

   result = strcmp(val1,val2);

   free(val1);
   free(val2);

   if (result==0) {
      return EQUAL;
   }
   if (result>0 ) {
      return GREATER;
   }
   return SMALLER;
}

comp compNums(nalFile *nf, vList *vl, char **nums)
{
   double val1, val2;
   bool error;

   error = FALSE;

   val1 = extractNum(vl,nums[0],&error);
   if (error) {
      freeArray(nums,VARSFROMCOND);
      handleExtractNumError(nf,vl,val1);
   }
   val2 = extractNum(vl,nums[1],&error);
   if (error) {
      freeArray(nums,VARSFROMCOND);
      handleExtractNumError(nf,vl,val2);
   }

   return compDoubles(val1,val2);
}

void handleExtractNumError(nalFile *nf, vList *vl, double errortype)
{
   if (compDoubles(errortype,READERROR)==EQUAL) {
      indexERROR(nf, vl, "Critical Error while reading number constant", nf->currWord-CONDWORDS);
   }else{
      indexERROR(nf, vl, "Trying to call uninitialized variable", nf->currWord-CONDWORDS);
   }
}

comp compDoubles(double n1, double n2)
{
   double diff;

   diff = n1 - n2;

   if (diff>EPSILON) {
      return GREATER;
   }
   if (diff<-EPSILON) {
      return SMALLER;
   }
   return EQUAL;
}

instr nalInc(nalFile *nf, vList *vl)
{
   char syntax[MAXSYNTAXWORDS][MAXSYNTAXWORDSIZE] = {"INC", "(", "NUMVAR", ")"};
   char **varname = NULL;


   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,vl,1);
      varname = checkSyntax(nf, vl, syntax, INCWORDS, VARSFROMINC);
      #ifdef INTERP
      incVar(nf, vl,varname);
      #endif
      freeArray(varname,VARSFROMINC);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void incVar(nalFile *nf, vList *vl, char **varname)
{
   char *val, *newVal;
   double num;

   val = vList_search(vl, varname[0]);
   if (sscanf(val,"%lf",&num)!=1) {
      freeArray(varname, VARSFROMINC);
      indexERROR(nf,vl,"Critical Error while reading stored number variable",nf->currWord-INCWORDS);
   }
   num++;
   newVal = (char *)allocate(sizeof(char)*MAXDOUBLESIZE,"Value of increased Variable");

   sprintf(newVal,"%lf",num);
   vList_insert(vl, varname[0], newVal);
   free(newVal);
}

/*Note: the parsing syntax seems to indicate that assigning a string to a number variable should be allowed, so I only check for that in the interpreter*/
instr nalSet(nalFile *nf, vList *vl)
{
   if (isvar(nf->words[nf->currWord]) && strsame(nf->words[nf->currWord+1],"=")) {

   wordStep(nf,vl,2);
   if (!isvarcon(nf->words[nf->currWord])) {
      syntaxERROR(nf,vl, "=", "VARCON", nf->currWord);
      }
      #ifdef INTERP
      setVariable(nf,vl);
      #endif
      wordStep(nf,vl,1);
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void setVariable(nalFile *nf, vList *vl)
{
   char *name, *valstr, *val;

   name = nf->words[nf->currWord-2];
   valstr = nf->words[nf->currWord];
   val = NULL;

   if (!validSet(name, valstr)) {
      indexERROR(nf,vl,"Setting a Variable to wrong Value Type",nf->currWord);
   }
   if (isstrcon(valstr)) {
      val = getString(valstr);
   }else if (isvar(valstr)) {
      val = allocString(vList_search(vl,valstr));
   } else if (isnumcon(valstr)) {
      val = allocString(valstr);
   }
   if (val==NULL) {
      variableERROR(nf, vl, "Setting Variable to something invalid (probably to an uninitialized variable)", name, nf->currWord);
   }
   vList_insert(vl,name,val);
   free(val);
}

bool validSet(char *name, char *val)
{
   if (isnumvar(name) && isstr(val) ) {
      return FALSE;
   }
   if (isstrvar(name) && isnum(val)) {
      return FALSE;
   }
   return TRUE;
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

bool isstr(char const *word)
{
   if (isstrcon(word)||isstrvar(word)) {
      return TRUE;
   }
   return FALSE;
}

bool isnum(char const *word)
{
   if (isnumcon(word)||isnumvar(word)) {
      return TRUE;
   }
   return FALSE;
}

/*Given a word that returns TRUE from isstrcon, output the string
(Applies ROT18 if the string requires it)*/
char *getString(char const* word)
{
   char *str;
   int strSize, i;

   /*-2 to get rid of "" or ##*/
   strSize = strlen(word)-2;

   /*+1 for \0*/
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

/*Applies ROT18 to c and returns it*/
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

/*mod (%) handled negatives in a weird way, used if statement instead.
Formula is c = (((c-base)-rotVal)%alphabet)+base*/
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

char *extractStr(vList *vl, char *name)
{
   char *val, *temp;

   val = NULL;

   if (isstrcon(name)) {
      val = getString(name);
   }else{
      temp = vList_search(vl, name);
      if (temp == NULL) {
         return NULL;

      }
      val = allocString(temp);
   }

   return val;
}

double extractNum(vList *vl, char *name, bool *error)
{
   double num;
   char *val;

   if (isnumcon(name)) {
      if (sscanf(name,"%lf",&num)!=1) {
         *error = TRUE;
         return READERROR;
      }
   }
   if (isnumvar(name)) {
      val = vList_search(vl, name);
      if (val == NULL) {
         *error = TRUE;
         return INITERROR;
      }
      if (sscanf(val,"%lf",&num)!=1) {
         *error = TRUE;
         return READERROR;
      }
   }

   return num;
}
