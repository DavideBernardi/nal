/* Multi-Value Map ADT :

   Keys are stored in structures called mvmkey.
   Values (data) are stored in structures called mvmval.

   An array is used along with hashing to store pointers to these key structures, resizing during insertion if necessary.
   No resizing happens during deletion of keys (The assumption is, if the array reaches a certain size once, it will likely reach it again at some point).

   All of the values matching with a single key are stored as a linked list. the key structure mvmkey contains a pointer to the head of this linked list.
   When a new value for a key is found, it is inserted at the head of this list.

   Reasoning: Since the whole point of a mvm is to store MULTIPLE values for EACH key, it makes sense to have this kind of data structure.


   Here is an attempt at a graphical visualisation of what this data
   structure looks like.
   Note that here value(i,j) means "the j^th value matching key i"
________________________________________________________________________
|HASHED ARRAY:  LINKED LISTS OF VALUES:
|      [key1]-->[value(1,3)]-->[value(1,2)]-->[value(1,1)]-->[NULL]
|      [key2]-->[value(2,2)]-->[value(2,1)]-->[NULL]
|      [NULL]
|      [NULL]
|      [key3]-->[value(3,1)]-->[NULL]
|         .
|         .
|         .
|      [key4]-->[value(4,2)]-->[value(4,1)]-->[NULL]
|      [NULL]
|      [key5]-->[value(3,1)]-->[NULL]
|         .
|         .
|         .
|_______________________________________________________________________

   It is clear from this that an entry such as [key]-->[NULL] is not valid
   as it means we have a key matching with no values.

   For homophones.c, this implementation is actually noticeably slower since
   insertion time is so much larger here.

   Final note:
   This data structure causes the behaviour of mvm_print() to differ substantially from the one in mvm.h, as in this case the order of printing is pseudo-random since it depends on where the hashing stores each key in the array.
*/

/* Error that can't be ignored */
#define ON_ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)

struct mvmval {
   char* val;
   struct mvmval* vnext;
};
typedef struct mvmval mvmval;

struct mvmkey {
   char* key;
   struct mvmval* vhead;
};
typedef struct mvmkey mvmkey;

struct mvm {
   mvmkey** array;
   unsigned long arrSize;
   int numElems;
};
typedef struct mvm mvm;

mvm* mvm_init(void);
/* Number of key/value pairs stored */
int mvm_size(mvm* m);
/* Insert one key/value pair */
void mvm_insert(mvm* m, char* key, char* data);
/* Store list as a string "[key](value) [key](value) " etc.  */
char* mvm_print(mvm* m);
/* Remove one key/value */
void mvm_delete(mvm* m, char* key);
/* Return the corresponding value for a key */
char* mvm_search(mvm* m, char* key);
/* Return *argv[] list of pointers to all values stored with key, n is the number of values */
char** mvm_multisearch(mvm* m, char* key, int* n);
/* Free & set p to NULL */
void mvm_free(mvm** p);
