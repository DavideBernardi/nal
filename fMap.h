#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*Important: default value returned if no function is found by fMap_search*/
#define NOFUNCTIONFOUND -1

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

struct fMapPair {
   char* fname;
   int index;
};
typedef struct fMapPair fMapPair;

struct fMap {
   fMapPair** array;
   unsigned long arrSize;
   int numElems;
};
typedef struct fMap fMap;

fMap* fMap_init(void);
/* Number of fname/index pairs stored */
int fMap_size(fMap* m);
/* Insert one fname/index pair */
void fMap_insert(fMap* m, char* fname, int data);
/* Return the corresponding index for a fname
Since we need this for the index of a word in a nalFile,
we use -1 as the value to be returned in case of error*/
int fMap_search(fMap* m, char* fname);
/* Free & set p to NULL */
void fMap_free(fMap** p);

/*Functions use to play with Array*/
void resizeArray(fMap *m);
void reallocatePairs(fMap *m, fMapPair** newArray, int newPrime);
void insertPair(fMap *m, ulong hash, char *fname, int index);
bool arrayTooFull(fMap *m);

/*Functions used for hashing*/
ulong getDoubleSizedPrime(ulong i);
bool isPrime(ulong num);
ulong firstHash(char* fname, ulong p);
ulong secondHash(char *fname, ulong prime);
ulong takeStep(ulong hash, ulong step, ulong prime);
ulong power(ulong base, unsigned int exp);

/*These functions are malloc and calloc but they give an error and
terminate the program if something goes wrong with the allocation*/
void *fMap_allocate(int size2, char* const msg);
void *fMap_callocate(int size1, int size2, char* const msg);
