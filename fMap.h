#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*Fraction of the array that needs to be filled before triggering the reallocation*/
#define MAXFRACTIONFILLED 0.6

/*To print each cell need 5 extra chars of space for "()[] "*/
#define EXTRACHARS 5
/*First prime used for the initialization of the array*/
#define INITPRIME 11

#define MAGICNUMBER1 5381
#define MAGICNUMBER2 33

/*In theory this should define bool only if it hasn't been defined before
(as long as the other definitions of bool have this same #include guard)*/
#ifndef BOOL
#define BOOL
typedef enum {FALSE, TRUE} bool;
#endif
typedef unsigned long ulong;

struct fMapCell {
   char* fname;
   int index;
   char **vars;
   int varTot;
};
typedef struct fMapCell fMapCell;

struct fMap {
   fMapCell** array;
   unsigned long arrSize;
   int numElems;
};
typedef struct fMap fMap;

fMap* fMap_init(void);
/* Number of fname/index Cells stored */
int fMap_size(fMap* m);
/* Insert one fname/index Cell
If two functions have the same name, the second one doesn't get stored */
void fMap_insert(fMap* m, char* fname, int index, char **vars, int varTot);
/* Return the corresponding index for a fname
Since we need this for the index of a word in a nalFile,
we use -1 as the value to be returned in case of error*/
fMapCell *fMap_search(fMap* m, char* fname);

void fMap_print(fMap* m);
/* Free & set p to NULL */
void fMap_free(fMap** p);

/*Functions use to play with Array*/
void resizeArray(fMap *m);
void reallocateCells(fMap *m, fMapCell** newArray, int newPrime);
void insertCell(fMap *m, ulong hash, char *fname, int index, char **vars, int varTot);
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
