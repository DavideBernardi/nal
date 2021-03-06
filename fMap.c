#include "fMap.h"

fMap* fMap_init(void)
{
   fMap *m;
   ulong i;

   m = (fMap *)fMap_allocate(sizeof(fMap), "Map");

   i = INITPRIME;
   m->array = (fMapCell **)fMap_callocate(sizeof(fMapCell *),i, "Array");
   m->arrSize = i;
   m->numElems = 0;

   return m;
}

int fMap_size(fMap* m)
{
   if (m == NULL) {
      return 0;
   }
   return m->numElems;
}

void fMap_insert(fMap* m, char* fname, int index, char **vars, int varTot)
{
   ulong  hash, step;

   if (m == NULL || fname == NULL) {
      return;
   }

   hash = firstHash(fname, m->arrSize);
   step = secondHash(fname, m->arrSize);
   while (m->array[hash] != NULL) {
      /*If fname already exists, do not insert the second one*/
      if (!strcmp(m->array[hash]->fname,fname)) {
         return;
      }
      hash = takeStep(hash, step,m->arrSize);
   }
   /*Now we have found an empty space in the array where the fname can be placed*/

   /*If the array is too full, resize*/
   if (arrayTooFull(m)) {
      resizeArray(m);
      /*Since the array has been rebuilt, the hash for our current fname is going
      to be different so we need to compute it again*/
      hash = firstHash(fname, m->arrSize);
      step = secondHash(fname, m->arrSize);
      while (m->array[hash] != NULL) {
         hash = takeStep(hash, step,m->arrSize);
      }
   }

   insertCell(m, hash, fname,index,vars,varTot);
}

void insertCell(fMap *m, ulong hash, char *fname, int index, char **vars, int varTot)
{
   char *allocatedFname;
   fMapCell *Cell;

   Cell = (fMapCell *)fMap_allocate(sizeof(fMapCell), "Space for Functon Cell");
   allocatedFname = (char *)fMap_allocate((strlen(fname)+1)*sizeof(char), "Function Name");
   strcpy(allocatedFname, fname);
   Cell->fname = allocatedFname;
   Cell->index = index;
   Cell->vars = vars;
   Cell->varTot = varTot;
   m->array[hash] = Cell;
   m->numElems++;
}

fMapCell *fMap_search(fMap* m, char* fname)
{
   ulong hash, step;

   if (m == NULL || fname == NULL) {
      return NULL;
   }

   hash = firstHash(fname, m->arrSize);
   step = secondHash(fname, m->arrSize);
   while (m->array[hash]!=NULL) {
      if (!strcmp(m->array[hash]->fname,fname)) {
         return m->array[hash];
      }
      hash = takeStep(hash, step,m->arrSize);
   }

   return NULL;
}

void fMap_free(fMap** p)
{
   fMap *m;
   ulong i;
   int j;

   if (p==NULL) {
      return;
   }
   m = *p;
   if (m==NULL) {
      return;
   }

   for (i = 0; i < m->arrSize; i++) {
      if (m->array[i] != NULL) {
         free(m->array[i]->fname);
         for (j = 0; j < m->array[i]->varTot; j++) {
            free(m->array[i]->vars[j]);
         }
         free(m->array[i]->vars);
         free(m->array[i]);
         m->array[i]=NULL;
      }
   }

   free(m->array);
   free(m);
   *p = NULL;
}

void fMap_print(fMap *m)
{
   int i, j;
   for (i = 0; i < (int)m->arrSize; i++) {
      if (m->array[i] != NULL) {
         printf("Function: %s | Index: %d | Variables: ",m->array[i]->fname, m->array[i]->index);
         for (j = 0; j < m->array[i]->varTot; j++) {
            printf("%s, ", m->array[i]->vars[j]);
         }
         printf("\n");
      }
   }
}

/*Input is always a prime number, otherwise behaviour is undefined.
This is not checked simply because in my code this function is always
guaranteed to receive a prime number, and checking this can be extremely slow*/
ulong getDoubleSizedPrime(ulong oldPrime)
{
   ulong newPrime;

   newPrime = (2*oldPrime)+1;

   while (!isPrime(newPrime)) {
      newPrime+=2;
   }

   return newPrime;
}

/*This sieve is taken from the pseudocode found at
https://en.wikipedia.org/wiki/Primality_test
under "Simple methods"*/
bool isPrime(ulong num)
{
   ulong j;

   if (num == 1) {
      return FALSE;
   }
   if (num<=3) {
      return TRUE;
   }
   if (num%2==0 || num%3==0) {
      return FALSE;
   }


   for (j = 5; j*j <= num; j+=6) {
      if (num%j==0 || num%(j+2)==0) {
         return FALSE;
      }
   }
   return TRUE;
}

bool arrayTooFull(fMap *m)
{
   if ((double)m->arrSize*MAXFRACTIONFILLED < (double)m->numElems+1) {
      return TRUE;
   }
   return FALSE;
}

ulong firstHash(char* fname, ulong prime)
{
   ulong hash;
   int c, i;

   i = 0;

   hash = MAGICNUMBER1;

   while (fname[i]!='\0') {

      c = fname[i];
      hash = MAGICNUMBER2 * power(hash, c);
      i++;
   }

   return hash%prime;
}

ulong secondHash(char *fname, ulong prime)
{
   int c, i;
   ulong hash;

   i = 0;

   hash = MAGICNUMBER1;

   while (fname[i]!='\0') {

      c = fname[i];
      hash = MAGICNUMBER2 * power(hash, c);
      i++;
   }

   hash = (hash/prime)%prime;
   if (hash==0) {
      hash++;
   }
   return hash;
}

ulong takeStep(ulong hash, ulong step, ulong prime)
{
   if (hash<step) {
      hash+=prime;
   }
   hash = hash-step;
   return hash;
}

ulong power(ulong base, unsigned int exp)
{
   unsigned int i;
   ulong val;

   val = 1;

   for (i = 0; i < exp; i++) {
      val = val*base;
   }

   return val;
}

void reallocateCells(fMap *m, fMapCell** newArray, int newPrime)
{
   ulong hash, i, step;
   fMapCell *k;

   for (i = 0; i < m->arrSize; i++) {
      if (m->array[i]!=NULL) {
         k = m->array[i];
         hash = firstHash(k->fname, newPrime);
         step = secondHash(k->fname, newPrime);
         while (newArray[hash] != NULL) {
            hash = takeStep(hash, step, newPrime);
         }
         newArray[hash]=k;
      }
   }

   free(m->array);
   m->array = newArray;
   m->arrSize = newPrime;
}

void resizeArray(fMap *m)
{
   int newPrime;
   fMapCell **newArray;

   newPrime = getDoubleSizedPrime(m->arrSize);

   newArray = (fMapCell **)fMap_callocate(sizeof(fMapCell),newPrime, "Array");

   reallocateCells(m, newArray, newPrime);

}

void *fMap_allocate(int size, char* const msg)
{
   void *p;

   p = malloc(size);
   if (p == NULL) {
      fprintf(stderr,"Couldn't allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}

void *fMap_callocate(int size1, int size2, char* const msg)
{
   void *p;

   p = calloc(size1,size2);
   if (p == NULL) {
      fprintf(stderr,"Couldn't allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}
