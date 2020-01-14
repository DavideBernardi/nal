#include "nal.h"

int main(int argc, char const *argv[])
{
   nalFile *nf;
   vList *vl;
   time_t t;
   nalVar *curr;

   srand((unsigned) time(&t));
   test();

   checkInput(argc, argv);
   nf = initNalFile();
   vl = vList_init();
   setupNalFile(nf, vl, argv[1]);
   program(nf, vl);
   #ifndef INTERP
   printf("Parsed OK\n");
   #endif
   curr = vl->head;
   while (curr!=NULL) {
      printf("Name: %s | Value: %s\n", curr->name, curr->val);
      curr = curr->next;
   }
   terminateNalFile(&nf);
   vList_free(&vl);
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
   vList *testList;
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
   testFile = getFile(NULL, NULL, "test1.nal");
   wordLengths = NULL;
   for (i = 0; i < WORDSINTEST1; i++) {
      assert(fscanf(testFile, "%s", testWord)==1);
      assert(wordLength(NULL,NULL,testWord, testFile, wordLengths)==(int)strlen(wordsInTest1[i]));
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
   /*IMPORTANT IF A TOKENIZED WORD IS LONGER THAN 1000 CHARS, THIS CODE WILL BREAK*/
   for (i = 0; i < WORDSINTEST1; i++) {
      getWord(NULL, NULL, testWord, testFile);
      assert(strsame(testWord,wordsInTest1[i]));
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
      assert(strsame(testNal->words[i],wordsInTest1[i]));
   }

   /*Testing wordStep*/
   testList = vList_init();
   for (i = 1; i < WORDSINTEST1; i++) {
      wordStep(testNal,testList, 1);
      assert(strsame(testNal->words[testNal->currWord],wordsInTest1[i]));
   }
   vList_free(&testList);
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

   /*No need to test isvar(), iscon(), isvarcon(), isstr() and isnum() because they are just unions of other functions*/
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

FILE *getFile(nalFile *nf, vList *vl, char const file[])
{
   FILE *ifp;

   ifp = fopen(file, "rb");
   if (ifp == NULL) {
      tokenizeERROR(nf,vl, file, "Couldn't open File");
   }

   return ifp;
}

/* Return a intList containing the exact size of each word in the file,
   the intList structure also contains the total number of words. */
intList *getWordSizes(nalFile *nf, vList *vl, FILE *fp)
{
   char currWord[MAXWORDSIZE];
   intList *wordLens;
   intNode *curr;

   wordLens = (intList *)allocate(sizeof(intList), "List");
   wordLens->size = 0;

   if (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr = allocateNode(wordLength(nf, vl, currWord, fp, wordLens));
      wordLens->head = curr;
   } else {
      fclose(fp);
      free(wordLens);
      tokenizeERROR(nf,vl,nf->name,"The File is Empty");
   }

   while (fscanf(fp, "%s", currWord)==1) {
      wordLens->size++;
      curr->next = allocateNode(wordLength(nf, vl, currWord, fp, wordLens));
      curr = curr->next;
   }

   return wordLens;
}

/*Finds length of the input word, if the word is the start of a string,
it moves along the file until it finds the end of that string as strings count
as one word*/
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

/*allocates the exact amount of space for the array of words and then fills it up*/
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

/*fills word (which is of the correct size thanks to getWordSizes)
with the correct string from the file*/
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

void setupNalFile(nalFile *nf, vList *vl, char const file[])
{
   FILE *fp;
   intList *wordLengths;

   nf->name = allocString(file);
   fp = getFile(nf,vl,file);
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

void wordStep(nalFile *nf, vList *vl, int s)
{
   nf->currWord += s;
   if (nf->currWord >= nf->totWords) {
      indexERROR(nf, vl,"Word Index Out of Bounds",nf->currWord);
   }
}

void program(nalFile *nf, vList *vl)
{
   if (!strsame(nf->words[nf->currWord], "{")) {
      nalERROR(nf, vl,"No starting \'{\'\n");
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

instr file(nalFile *nf, vList *vl)
{
   #ifdef INTERP
   nalFile *newFile;
   char *fileName;
   #endif

   if (strsame(nf->words[nf->currWord], "FILE")) {
      wordStep(nf,vl,1);
      if (!isstrcon(nf->words[nf->currWord])) {
         syntaxERROR(nf,vl, "FILE", "STRCON", nf->currWord);
      }
      #ifdef INTERP
      fileName = getString(nf->words[nf->currWord]);
      newFile = initNalFile();
      /*Keep track of previous file in case of ABORT call*/
      newFile->prev = nf;
      setupNalFile(newFile, vl, fileName);
      free(fileName);
      program(newFile,vl);
      terminateNalFile(&newFile);
      #endif
      wordStep(nf,vl,1);
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

char *extractStr(nalFile* nf, vList *vl, char *name)
{
   char *val, *temp;

   val = NULL;

   if (isstrcon(name)) {
      val = getString(name);
   }else{
      temp = vList_search(vl, name);
      if (temp == NULL) {
         variableERROR(nf, vl, "Trying to call uninitialized variable", name, nf->currWord);
      }
      val = allocString(temp);
   }

   return val;
}

double extractNum(nalFile* nf, vList *vl, char *name)
{
   double num;
   char *val;


   if (isnumcon(name)) {
      if (sscanf(name,"%lf",&num)!=1) {
         variableERROR(nf, vl, "Critical Error while reading number constant", name, nf->currWord);
      }
   }
   if (isnumvar(name)) {
      val = vList_search(vl, name);
      if (val == NULL) {
         variableERROR(nf, vl, "Trying to call uninitialized variable", name, nf->currWord);
      }
      if (sscanf(val,"%lf",&num)!=1) {
         variableERROR(nf, vl, "Critical Error while reading number constant", name, nf->currWord);
      }
   }

   return num;
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
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IN2STR", "(", "STRVAR", ",", "STRVAR", ")"};
   #ifdef INTERP
   int vFound;
   char **varnames;
   #endif

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      correct = TRUE;
      #ifdef INTERP
      varnames = (char**)allocate(2*sizeof(char *),"Variable names array");
      vFound = 0;
      #endif
      wordStep(nf,vl,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "STRVAR")) {
            if (!isstrvar(nf->words[nf->currWord])) {
               correct = FALSE;
            }else {
               #ifdef INTERP
               varnames[vFound] = allocString(nf->words[nf->currWord]);
               vFound++;
               #endif
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(varnames[0]);
            free(varnames[1]);
            free(varnames);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      insertInputStrings(nf,vl,varnames);
      free(varnames[0]);
      free(varnames[1]);
      free(varnames);
      #endif
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Using scanf to place the input into an array of fixed size can cause overflow errors, please improve this.*/
void insertInputStrings(nalFile *nf, vList *vl, char **varnames)
{
   char str1[MAXINPUTSTRLEN], str2[MAXINPUTSTRLEN];

   if (scanf("%s %s", str1, str2)!=2) {
      free(varnames[0]);
      free(varnames[1]);
      free(varnames);
      indexERROR(nf, vl, "Error while receiving string input",nf->currWord);
   }

   vList_insert(vl, varnames[0],str1);
   vList_insert(vl, varnames[1],str2);
}

instr innum(nalFile *nf, vList *vl)
{
   int i;
   bool correct;
   char syntax[INNUMWORDS][MAXWORDSIZE] = {"INNUM", "(", "NUMVAR", ")"};
   #ifdef INTERP
   char *name;
   #endif
   correct = TRUE;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      wordStep(nf,vl,1);
      for (i = 1; i < INNUMWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            } else {
               #ifdef INTERP
               name = allocString(nf->words[nf->currWord]);
               #endif
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(name);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      insertInputNum(nf,vl,name);
      free(name);
      #endif
      return EXECUTED;
   }
   return NOTEXECUTED;
}

/*Using scanf to place the input into an array of fixed size can cause overflow errors, please improve this.*/
void insertInputNum(nalFile *nf, vList *vl, char* name)
{
   char num[MAXINPUTSTRLEN];

   if (scanf("%s", num)!=1) {
      free(name);
      indexERROR(nf, vl, "Error while receiving number input",nf->currWord);
   }
   if (!isnumcon(num)) {
      free(name);
      indexERROR(nf, vl, "Error while receiving number input",nf->currWord);
   }

   vList_insert(vl, name ,num);
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
         printf("%s\n", varcon);
         variableERROR(nf, vl, "Trying to print uninitialized variable",varcon,nf->currWord);
      }
      printThis = allocString(var);
   }

   printf("%s", printThis);
   free(printThis);
}

instr nalRnd(nalFile *nf, vList *vl)
{
   int i;
   bool correct;
   char syntax[RANDWORDS][MAXWORDSIZE] = {"RND", "(", "NUMVAR", ")"};
   #ifdef INTERP
   char *name;
   #endif



   if (strsame(nf->words[nf->currWord], syntax[0])) {
      correct = TRUE;
      wordStep(nf,vl,1);
      for (i = 1; i < RANDWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            } else {
               #ifdef INTERP
               name = allocString(nf->words[nf->currWord]);
               #endif
            }
         } else {
            if (!strsame(nf->words[nf->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(name);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      setRandom(vl,name);
      free(name);
      #endif
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

   condition = nalIfequal(nf,vl);
   if (condition==NOTEXEC) {
      condition = nalIfgreater(nf,vl);
   }

   if (condition != NOTEXEC) {
      if (!strsame(nf->words[nf->currWord], "{")) {
         syntaxERROR(nf,vl, "CONDITION", "{", nf->currWord);
      }
      wordStep(nf,vl,1);
      if (condition==EXECFAIL) {
         skipToMatchingBracket(nf,vl);
         return EXECUTED;
      }
      instrs(nf,vl);
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
cond nalIfequal(nalFile *nf, vList *vl)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFEQUAL", "(", "VARCON", ",", "VARCON", ")"};
   cond condition;
   #ifdef INTERP
   char **vNames;
   int vFound;
   #endif

   condition = NOTEXEC;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      correct = TRUE;
      condition = EXECPASS;
      #ifdef INTERP
      vNames = (char**)allocate(2*sizeof(char *),"Values array");
      vFound = 0;
      #endif
      wordStep(nf,vl,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(nf->words[nf->currWord])) {
               correct = FALSE;
            }else{
               #ifdef INTERP
               vNames[vFound] = allocString(nf->words[nf->currWord]);
               vFound++;
               #endif
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(vNames[0]);
            free(vNames[1]);
            free(vNames);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      if (!condEqual(nf,vl,vNames[0],vNames[1])) {
         condition =  EXECFAIL;
      }
      free(vNames[0]);
      free(vNames[1]);
      free(vNames);
      #endif
   }
   return condition;
}

bool condEqual(nalFile *nf, vList *vl, char *varcon1, char *varcon2)
{
   comp comparison;

   comparison = NOCOMP;
   if (isstr(varcon1) && isstr(varcon2)) {
      comparison = compStrings(nf, vl, varcon1, varcon2);
   }
   if (isnum(varcon1) && isnum(varcon2)) {
      comparison = compNums(nf, vl, varcon1, varcon2);
   }

   if (comparison == NOCOMP) {
      indexERROR(nf,vl,"Comparing two VARCONs of uncomparable types",nf->currWord);
   }

   if (comparison==EQUAL) {
      return TRUE;
   }
   return FALSE;
}

comp compStrings(nalFile *nf, vList *vl, char *str1, char *str2)
{
   char *val1, *val2;
   int result;

   val1 = extractStr(nf,vl,str1);
   val2 = extractStr(nf,vl,str2);

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

comp compNums(nalFile *nf, vList *vl, char *num1, char *num2)
{
   double val1, val2;

   val1 = extractNum(nf,vl,num1);
   val2 = extractNum(nf,vl,num2);

   return compDoubles(val1,val2);
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

cond nalIfgreater(nalFile *nf, vList *vl)
{
   int i;
   bool correct;
   char syntax[IN2STRWORDS][MAXWORDSIZE] = {"IFGREATER", "(", "VARCON", ",", "VARCON", ")"};
   cond condition;
   #ifdef INTERP
   char **vNames;
   int vFound;
   #endif

   condition = NOTEXEC;

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      correct = TRUE;
      condition = EXECPASS;
      #ifdef INTERP
      vNames = (char**)allocate(2*sizeof(char *),"Values array");
      vFound = 0;
      #endif
      wordStep(nf,vl,1);
      for (i = 1; i < IN2STRWORDS; i++) {
         if (strsame(syntax[i], "VARCON")) {
            if (!isvarcon(nf->words[nf->currWord])) {
               correct = FALSE;
            } else {
               #ifdef INTERP
               vNames[vFound] = allocString(nf->words[nf->currWord]);
               vFound++;
               #endif
            }
         } else {
               if (!strsame(nf->words[nf->currWord], syntax[i])) {
               correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(vNames[0]);
            free(vNames[1]);
            free(vNames);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      if (!condGreater(nf,vl,vNames[0],vNames[1])) {
         condition =  EXECFAIL;
      }
      free(vNames[0]);
      free(vNames[1]);
      free(vNames);
      #endif
   }
   return condition;
}

bool condGreater(nalFile *nf, vList *vl, char *varcon1, char *varcon2)
{
   comp comparison;

   comparison = NOCOMP;
   if (isstr(varcon1) && isstr(varcon2)) {
      comparison = compStrings(nf, vl, varcon1, varcon2);
   }
   if (isnum(varcon1) && isnum(varcon2)) {
      comparison = compNums(nf, vl, varcon1, varcon2);
   }

   if (comparison == NOCOMP) {
      indexERROR(nf,vl,"Comparing two VARCONs of uncomparable types",nf->currWord);
   }

   if (comparison==GREATER) {
      return TRUE;
   }
   return FALSE;
}

instr nalInc(nalFile *nf, vList *vl)
{
   int i;
   bool correct;
   char syntax[INCWORDS][MAXWORDSIZE] = {"INC", "(", "NUMVAR", ")"};
   #ifdef INTERP
   char *name = NULL;
   #endif

   if (strsame(nf->words[nf->currWord], syntax[0])) {
      correct = TRUE;
      wordStep(nf,vl,1);
      for (i = 1; i < INCWORDS; i++) {
         if (strsame(syntax[i], "NUMVAR")) {
            if (!isnumvar(nf->words[nf->currWord])) {
               correct = FALSE;
            } else {
               #ifdef INTERP
               name = allocString(nf->words[nf->currWord]);
               #endif
            }
         } else {
            if (!strsame(nf->words[nf->currWord], syntax[i])) {
            correct = FALSE;
            }
         }
         if (!correct) {
            #ifdef INTERP
            free(name);
            #endif
            syntaxERROR(nf,vl, syntax[i-1], syntax[i], nf->currWord);
         }
         wordStep(nf,vl,1);
      }
      #ifdef INTERP
      incVar(nf, vl,name);
      free(name);
      #endif
      return EXECUTED;
   }
   return NOTEXECUTED;
}

void incVar(nalFile *nf, vList *vl, char *name)
{
   char *val, *newVal;
   double num;

   val = vList_search(vl, name);
   if (sscanf(val,"%lf",&num)!=1) {
      variableERROR(nf,vl,"Critical Error while reading stored number variable",name,nf->currWord);
   }
   num++;
   newVal = (char *)allocate(sizeof(char)*MAXDOUBLESIZE,"Value of increased Variable");

   sprintf(newVal,"%lf",num);
   vList_insert(vl, name, newVal);
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

void ERROR(char* const msg)
{

   fprintf(stderr, "%s", msg);
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

/*Given a string, it allocates some new space to store it and copies it, returns a pointer to the new space*/
char *allocString(const char *str)
{
   char *newStr;

   newStr = (char *)allocate(sizeof(char)*(strlen(str)+1),"String");
   strcpy(newStr,str);

   return newStr;
}
