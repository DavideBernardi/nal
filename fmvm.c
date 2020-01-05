/* Note: My functions firsthash(), secondhash() are not
particularly fast. I understand that the goal of this part of the assignment is
speed, but finding more efficient hashing functions feels a lot
like an exercise in Googling.
The underlying structure is sound and the functions I used are the ones we were
given either in the notes or during the lectures.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fmvm.h"

/*Fraction of the array that needs to be filled before triggering the reallocation*/
#define MAXFRACTIONFILLED 0.6
#define MAXWORDSIZE 50
/*To print each cell need 5 extra chars of space for "()[] "*/
#define EXTRACHARS 5
/*First prime used for the initialization of the array*/
#define INITPRIME 11

#define MAGICNUMBER1 5381
#define MAGICNUMBER2 33

typedef enum {FALSE, TRUE} bool;
typedef unsigned long ulong;

void resizeArray(mvm *m);
void reallocateKeys(mvm *m, mvmkey** newArray, int newPrime);
void insertKey(mvm *m, ulong hash, char *key);
void insertValue(mvm *m, ulong hash, char* value);
void freeKey(mvmkey *p);
void freeVal(mvmval *p);
int findLength(mvm *m, int *maxLen);
void printMap(mvm *m, char *str, char *tempStr);
int getNumberOfMatches(mvmkey *k);
void buildValuesArray(mvmkey* k, char **matches);
bool arrayTooFull(mvm *m);

/*Functions used for hashing*/
ulong getDoubleSizedPrime(ulong i);
bool isPrime(ulong num);
ulong firstHash(char* key, ulong p);
ulong secondHash(char *key, ulong prime);
ulong takeStep(ulong hash, ulong step, ulong prime);
ulong power(ulong base, unsigned int exp);
mvmkey *searchHashTable(mvm *m, char *key);


/*These functions are malloc and calloc but they give an error and
terminate the program if something goes wrong with the allocation*/
void *allocate(int size2, char* const msg);
void *callocate(int size1, int size2, char* const msg);

mvm* mvm_init(void)
{
   mvm *m;
   ulong i;

   m = (mvm *)allocate(sizeof(mvm), "Map");

   i = INITPRIME;
   m->array = (mvmkey **)callocate(sizeof(mvmkey *),i, "Array");
   m->arrSize = i;
   m->numElems = 0;

   return m;
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

int mvm_size(mvm* m)
{
   if (m == NULL) {
      return 0;
   }
   return m->numElems;
}

void mvm_insert(mvm* m, char* key, char* value)
{
   ulong  hash, step;

   if (m == NULL || key == NULL || value == NULL) {
      return;
   }

   hash = firstHash(key, m->arrSize);
   step = secondHash(key, m->arrSize);
   while (m->array[hash] != NULL) {
      /*If key already exists, simply insert the a new Value node*/
      if (!strcmp(m->array[hash]->key,key)) {
         insertValue(m,hash, value);
         return;
      }
      hash = takeStep(hash, step,m->arrSize);
   }
   /*Now we have found an empty space in the array where the key can be placed*/

   /*If the array is too full, resize*/
   if (arrayTooFull(m)) {
      resizeArray(m);
      /*Since the array has been rebuilt, the hash for our current key is going
      to be different so we need to compute it again*/
      hash = firstHash(key, m->arrSize);
      step = secondHash(key, m->arrSize);
      while (m->array[hash] != NULL) {
         hash = takeStep(hash, step,m->arrSize);
      }
   }

   insertKey(m, hash, key);
   insertValue(m, hash, value);
}

bool arrayTooFull(mvm *m)
{
   if ((double)m->arrSize*MAXFRACTIONFILLED < (double)m->numElems+1) {
      return TRUE;
   }
   return FALSE;
}

void insertKey(mvm *m, ulong hash, char *key)
{
   char *allocatedKey;
   mvmkey *keyCell;

   keyCell = (mvmkey *)callocate(1,sizeof(mvmkey), "Space for Key");
   allocatedKey = (char *)callocate(1,(strlen(key)+1)*sizeof(char), "Key");
   strcpy(allocatedKey, key);
   keyCell->key = allocatedKey;
   m->array[hash] = keyCell;
}

void insertValue(mvm *m, ulong hash, char* value)
{

   mvmval *valueNode;
   char *allocatedValue;

   valueNode = (mvmval *)callocate(1,sizeof(mvmval), "Space for Value");
   allocatedValue = (char *)callocate(1,(strlen(value)+1)*sizeof(char), "Value");
   strcpy(allocatedValue, value);

   valueNode->val = allocatedValue;
   valueNode->vnext = m->array[hash]->vhead;
   m->array[hash]->vhead = valueNode;
   m->numElems++;
}

ulong firstHash(char* key, ulong prime)
{
   ulong hash;
   int c, i;

   i = 0;

   hash = MAGICNUMBER1;

   while (key[i]!='\0') {

      c = key[i];
      hash = MAGICNUMBER2 * power(hash, c);
      i++;
   }

   return hash%prime;
}

ulong secondHash(char *key, ulong prime)
{
   int c, i;
   ulong hash;

   i = 0;

   hash = MAGICNUMBER1;

   while (key[i]!='\0') {

      c = key[i];
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

void reallocateKeys(mvm *m, mvmkey** newArray, int newPrime)
{
   ulong hash, i, step;
   mvmkey *k;

   for (i = 0; i < m->arrSize; i++) {
      if (m->array[i]!=NULL) {
         k = m->array[i];
         hash = firstHash(k->key, newPrime);
         step = secondHash(k->key, newPrime);
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

void resizeArray(mvm *m)
{
   int newPrime;
   mvmkey **newArray;

   newPrime = getDoubleSizedPrime(m->arrSize);

   newArray = (mvmkey **)callocate(sizeof(mvmkey),newPrime, "Array");

   reallocateKeys(m, newArray, newPrime);

}

char* mvm_print(mvm* m)
{
   char *str, *tempStr;
   int maxLen, totLen;

   if (m == NULL) {
      return NULL;
   }

   totLen = findLength(m, &maxLen);

   str = (char *)callocate(1,(totLen+1)*sizeof(char),"Map Printing");
   tempStr = (char *)callocate(1,(maxLen+1)*sizeof(char),"Map Printing");

   printMap(m, str, tempStr);

   free(tempStr);
   return str;
}

/*This functions explores the whole list only to find out the total char
size the final string will need to be and the char size of the largest
single cell output*/
int findLength(mvm *m, int *maxLen)
{
   int i, curr;
   mvmkey *k;
   mvmval *v;
   ulong j;

   i = 0;
   *maxLen = 0;

   for (j = 0; j < m->arrSize; j++) {
      if (m->array[j]!=NULL) {
         k = m->array[j];
         v=k->vhead;
         while (v!=NULL) {
            curr = strlen(k->key)+strlen(v->val)+EXTRACHARS;
            if (curr > *maxLen) {
               *maxLen = curr;
            }
            i += curr;
            v = v->vnext;
         }
      }

   }

   return i;
}

/*This function does the actual printing*/
void printMap(mvm *m, char *str, char *tempStr)
{
   mvmkey *k;
   mvmval *v;
   ulong i;

   for (i = 0; i < m->arrSize; i++) {
      if (m->array[i]!=NULL) {
         k = m->array[i];
         v=k->vhead;
         while (v!=NULL) {
            if (!sprintf(tempStr,"[%s](%s) ",k->key,v->val)) {
               ON_ERROR("Error while Printing Map, Terminating ...\n");
            }
            /*Note: function strcat() should have a pointer to the final
            string as output, that should be the same as the pointer to str,
            if it is not, something has gone wrong*/
            if (str != strcat(str,tempStr)) {
               ON_ERROR("Error while Printing Map, Terminating ...\n");
            }
            v = v->vnext;
         }
      }
   }
}

void mvm_delete(mvm* m, char* key)
{
   mvmkey *k;
   mvmval *v;
   ulong hash, step;

   if (m == NULL || key == NULL) {
      return;
   }

   hash = firstHash(key, m->arrSize);
   step = secondHash(key, m->arrSize);
   while (m->array[hash]!=NULL) {
      if (!strcmp(m->array[hash]->key,key)) {
         k = m->array[hash];
         v = k->vhead;
         /*The second value is set as the new head, and the first value is freed*/
         k->vhead = v->vnext;
         freeVal(v);
         /*If there is no second value, the key itself is also freed*/
         if (k->vhead==NULL) {
            freeKey(k);
            m->array[hash]=NULL;
         }
         m->numElems--;
         return;
      }
      hash = takeStep(hash, step,m->arrSize);
   }
}

char* mvm_search(mvm* m, char* key)
{

   ulong hash, step;

   if (m == NULL || key == NULL) {
      return NULL;
   }

   hash = firstHash(key, m->arrSize);
   step = secondHash(key, m->arrSize);
   while (m->array[hash]!=NULL) {
      if (!strcmp(m->array[hash]->key,key)) {
         return m->array[hash]->vhead->val;
      }
      hash = takeStep(hash, step,m->arrSize);
   }

   return NULL;
}

char** mvm_multisearch(mvm* m, char* key, int* n)
{
   char **matches;
   int  matchesAmount;
   mvmkey *k;

   if (m == NULL || key == NULL || n == NULL) {
      n = NULL;
      return NULL;
   }

   k = searchHashTable(m, key);
   if (k==NULL) {
      *n = 0;
      return NULL;
   }

   matchesAmount = getNumberOfMatches(k);
   matches = (char **)callocate(matchesAmount,sizeof(char *),"Searching");

   buildValuesArray(k, matches);

   *n = matchesAmount;
   return matches;
}

/*This function searches the hashed array for the pointer to the requested key*/
mvmkey *searchHashTable(mvm *m, char *key)
{
   ulong hash, step;

   hash = firstHash(key, m->arrSize);
   step = secondHash(key, m->arrSize);
   while (m->array[hash]!=NULL) {
      if (!strcmp(m->array[hash]->key,key)) {
         return m->array[hash];
      }
      hash = takeStep(hash, step,m->arrSize);
   }

   return NULL;
}

/*This function return the amount of values assigned to the given key*/
int getNumberOfMatches(mvmkey *k)
{
   int matchesAmount;
   mvmval *v;

   matchesAmount = 0;
   v = k->vhead;
   while (v!=NULL) {
      matchesAmount++;
      v = v->vnext;
   }

   return matchesAmount;
}

void buildValuesArray(mvmkey* k, char **matches)
{
   mvmval *v;
   int j;

   j = 0;
   v = k->vhead;
   while (v!=NULL){
      matches[j] = v->val;
      v = v->vnext;
      j++;
   }
}

void mvm_free(mvm** p)
{
   mvm *m;
   mvmkey *k;
   mvmval *v, *vnext;
   ulong i;

   if (p==NULL) {
      return;
   }
   m = *p;
   if (m==NULL) {
      return;
   }

   for (i = 0; i < m->arrSize; i++) {
      if (m->array[i] != NULL) {
         k = m->array[i];
         v = k->vhead;
         while (v!=NULL) {
            vnext = v->vnext;
            freeVal(v);
            v = vnext;
         }
         freeKey(k);
         m->array[i]=NULL;
      }
   }

   free(m->array);
   free(m);
   *p = NULL;
}

void freeKey(mvmkey *p)
{
   if (p == NULL) {
      return;
   }
   free(p->key);
   free(p);
   p = NULL;
}

void freeVal(mvmval *p)
{
   if (p == NULL) {
      return;
   }
   free(p->val);
   free(p);
   p = NULL;
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
