#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fMap.h"

#define NEILLPAIRNUM 5
#define NEILLWORDSIZE 10
#define EXTRACHARS 5
#define PAIRNUM 30
#define WORDSIZE 10

int main(void)
{
   int i, j;
   fMap *m, *m2;
   char animals[NEILLPAIRNUM][NEILLWORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog"};
   int  noises[NEILLPAIRNUM];
   char keys[PAIRNUM][WORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog","cata",  "doga",
   "birda",  "horsea", "froga", "catb",  "dogb",  "birdb",  "horseb", "frogb",
   "catc",  "dogc",  "birdc",  "horsec", "frogc","catd",  "dogd",  "birdd",
   "horsed", "frogd","cate",  "doge",  "birde",  "horsee", "froge",};
   char values[PAIRNUM];

   for (i = 0; i < PAIRNUM; i++) {
      values[i] = i+1;
   }
   for (i = 0; i < NEILLPAIRNUM; i++) {
      noises[i] = i+1;
   }
   printf("Basic fMap Tests ... Start\n");

   /* Set up empty array */
   m = fMap_init();
   m2 = fMap_init();
   assert(m != NULL);
   assert(m2 != NULL);
   assert(fMap_size(m)==0);
   assert(fMap_size(m2)==0);

   /* Building and Searching */
   for(j=0; j<NEILLPAIRNUM; j++){
      fMap_insert(m, animals[j], noises[j]);
      assert(fMap_size(m)==j+1);
      assert(fMap_search(m, animals[j])==noises[j]);
   }

   for (j = 0; j < PAIRNUM; j++) {
      fMap_insert(m2, keys[j], values[j]);
      assert(fMap_size(m2)==j+1);
      assert(fMap_search(m2, keys[j])==values[j]);
   }

   /* Search for non-existent key */
   /* No-one knows what the fox says ? */
   assert(fMap_search(m, "fox") == NOFUNCTIONFOUND);

   /* Weird NULL insert() edge cases */
   fMap_insert(m, NULL, 1);
   assert(fMap_size(m)==NEILLPAIRNUM);
   fMap_insert(NULL, "duck", 1);
   assert(fMap_size(m)==NEILLPAIRNUM);

   /*Other edge cases*/
   assert(fMap_size(NULL)==0);
   assert(fMap_search(NULL, "cat")==NOFUNCTIONFOUND);
   assert(fMap_search(m, NULL)==NOFUNCTIONFOUND);

   /* Freeing */
   fMap_free(&m);
   fMap_free(&m2);
   assert(m==NULL);
   assert(fMap_size(m)==0);

   printf("Basic fMap Tests ... Stop\n");
   return 0;
}
