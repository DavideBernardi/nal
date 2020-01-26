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
#define VARTOT 10

int main(void)
{
   int i, j, varTot;
   fMap *m, *m2;
   char smallnameset[NEILLPAIRNUM][NEILLWORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog"};
   int  smallnumset[NEILLPAIRNUM];
   char bignameset[PAIRNUM][WORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog","cata",  "doga",
   "birda",  "horsea", "froga", "catb",  "dogb",  "birdb",  "horseb", "frogb",
   "catc",  "dogc",  "birdc",  "horsec", "frogc","catd",  "dogd",  "birdd",
   "horsed", "frogd","cate",  "doge",  "birde",  "horsee", "froge",};
   char bignumset[PAIRNUM];
   char varnames[VARTOT][WORDSIZE] = {"$ASFS",  "$NBV", "$ETHF", "$KJHD","$UYTR", "$CBVDG",  "$TGBF",  "$IUJHNTR", "$OKERTH", "%BLEH"};
   char **vars;
   fMapCell *cell;

   for (i = 0; i < PAIRNUM; i++) {
      bignumset[i] = i+1;
   }
   for (i = 0; i < NEILLPAIRNUM; i++) {
      smallnumset[i] = i+1;
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
      fMap_insert(m, smallnameset[j], smallnumset[j], NULL, 0);
      assert(fMap_size(m)==j+1);
      assert(fMap_search(m, smallnameset[j])->index==smallnumset[j]);
   }

   for (j = 0; j < PAIRNUM; j++) {
      fMap_insert(m2, bignameset[j], bignumset[j], NULL, 0);
      assert(fMap_size(m2)==j+1);
      assert(fMap_search(m2, bignameset[j])->index==bignumset[j]);
   }

   /* Search for non-existent key */
   /* No-one knows what the fox function does */
   assert(fMap_search(m, "fox") == NULL);

   /* Weird NULL insert() edge cases */
   fMap_insert(m, NULL, 1, NULL, 0);
   assert(fMap_size(m)==NEILLPAIRNUM);
   fMap_insert(NULL, "duck", 1, NULL, 0);
   assert(fMap_size(m)==NEILLPAIRNUM);

   /*Other edge cases*/
   assert(fMap_size(NULL)==0);
   assert(fMap_search(NULL, "cat")==NULL);
   assert(fMap_search(m, NULL)==NULL);

   /*Testing vars and varTot*/
   varTot = VARTOT;
   vars = fMap_allocate(varTot*sizeof(char *), "Vars array");
   for (i = 0; i < varTot; i++) {
      vars[i] = fMap_allocate(WORDSIZE*sizeof(char), "VAR");
   }

   strcpy(vars[0],"$ASFS");
   strcpy(vars[1],"$NBV");
   strcpy(vars[2],"$ETHF");
   strcpy(vars[3],"$KJHD");
   strcpy(vars[4],"$UYTR");
   strcpy(vars[5],"$CBVDG");
   strcpy(vars[6],"$TGBF");
   strcpy(vars[7],"$IUJHNTR");
   strcpy(vars[8],"$OKERTH");
   strcpy(vars[9],"%BLEH");

   /*Reinsert a random map with this var array added*/
   fMap_insert(m, "NEWFUNC", smallnumset[4], vars, varTot);
   assert(fMap_size(m)==NEILLPAIRNUM+1);
   cell = fMap_search(m, "NEWFUNC");
   assert(cell->index==smallnumset[4]);
   assert(cell->varTot==varTot);
   for (i = 0; i < varTot; i++) {
      j = strcmp(cell->vars[i],varnames[i]);
      assert(j==0);
   }

   /* Freeing */
   fMap_free(&m);
   fMap_free(&m2);
   assert(m==NULL);
   assert(fMap_size(m)==0);

   printf("Basic fMap Tests ... Stop\n");
   return 0;
}
