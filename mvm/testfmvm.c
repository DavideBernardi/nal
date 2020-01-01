/*
   The main changes in this test file are:
   1) The addition of a second, larger map which I use to test mvm_insert
   (to make sure it works correctly even when the array needs to be resized
   multiple times)

   2) Since mvm_print() now prints in a different order which is really hard to
   predict and hence really hard to hardcode in the test, I have made it so that
   this file tests whether the required [key](value) pairs appear IN ANY ORDER
   in the printed string.
   I have done so using the function strstr().

   3) I have added a couple edge cases at the bottom which I believe are relevant
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fmvm.h"

#define NEILLPAIRNUM 5
#define NEILLWORDSIZE 10
#define EXTRACHARS 5
#define PAIRNUM 30
#define WORDSIZE 10

int main(void)
{
   int i, j;
   mvm *m, *m2;
   char* str, *tempStr;
   char** av;
   char animals[NEILLPAIRNUM][NEILLWORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog"};
   char  noises[NEILLPAIRNUM][NEILLWORDSIZE] = {"meow", "bark", "tweet", "neigh", "croak"};
   char keys[PAIRNUM][WORDSIZE] = {"cat",  "dog",  "bird",  "horse", "frog","cata",  "doga",
   "birda",  "horsea", "froga", "catb",  "dogb",  "birdb",  "horseb", "frogb",
   "catc",  "dogc",  "birdc",  "horsec", "frogc","catd",  "dogd",  "birdd",
   "horsed", "frogd","cate",  "doge",  "birde",  "horsee", "froge",};
   char values[PAIRNUM][WORDSIZE] = {"meow", "bark", "tweet", "neigh", "croak","meowa",
   "barka", "tweeta", "neigha", "croaka","meowb", "barkb", "tweetb", "neighb", "croakb","meowc",
   "barkc", "tweetc", "neighc", "croakc","meowd", "barkd", "tweetd", "neighd", "croakd",
   "meowe", "barke", "tweete", "neighe", "croake",};
   printf("Basic MVM Tests ... Start\n");

   /* Set up empty array */
   m = mvm_init();
   m2 = mvm_init();
   assert(m != NULL);
   assert(m2 != NULL);
   assert(mvm_size(m)==0);
   assert(mvm_size(m2)==0);

   /* Building and Searching */
   for(j=0; j<NEILLPAIRNUM; j++){
      mvm_insert(m, animals[j], noises[j]);
      assert(mvm_size(m)==j+1);
      i = strcmp(mvm_search(m, animals[j]), noises[j]);
      assert(i==0);
   }

   for (j = 0; j < PAIRNUM; j++) {
      mvm_insert(m2, keys[j], values[j]);
      assert(mvm_size(m2)==j+1);
      i = strcmp(mvm_search(m2, keys[j]), values[j]);
      assert(i==0);
   }

   /* Test building & printing */
   str = mvm_print(m);
   tempStr = (char *)calloc(1, (2*NEILLWORDSIZE+EXTRACHARS+1)*sizeof(char));
   for (j = 0; j < NEILLPAIRNUM; j++) {
      sprintf(tempStr,"[%s](%s) ",animals[j],noises[j]);
      /*This test fails if a key-value pair is NOT printed correctly at some point,
      it doesn't care about the order in which the pairs are printed*/
      assert(strstr(str, tempStr)!=NULL);
   }
   free(tempStr);
   free(str);

   str = mvm_print(m2);
   tempStr = (char *)calloc(1, (2*WORDSIZE+EXTRACHARS+1)*sizeof(char));
   for (j = 0; j < PAIRNUM; j++) {
      sprintf(tempStr,"[%s](%s) ",keys[j],values[j]);
      /*This test fails if a key-value pair is NOT printed correctly at some point,
      it doesn't care about the order in which the pairs are printed*/
      assert(strstr(str, tempStr)!=NULL);
   }
   free(tempStr);
   free(str);

   /* Search for non-existent key */
   /* No-one knows what the fox says ? */
   assert(mvm_search(m, "fox") == NULL);

   /* Deletions - middle, then front */
   mvm_delete(m, "dog");
   assert(mvm_size(m)==4);
   str = mvm_print(m);
   /*Test that the dog-bark pair is no longer in the map*/
   assert(strstr(str, "[dog](bark) ")==NULL);
   /*Check all other pairs are*/
   tempStr = (char *)calloc(1, (2*NEILLWORDSIZE+EXTRACHARS+1)*sizeof(char));
   for (j = 0; j < NEILLPAIRNUM; j++) {
      if (strcmp(animals[j],"dog")) {
         sprintf(tempStr,"[%s](%s) ",animals[j],noises[j]);
         assert(strstr(str, tempStr)!=NULL);
      }
   }
   free(tempStr);
   free(str);
   mvm_delete(m, "frog");
   assert(mvm_size(m)==3);
   str = mvm_print(m);
   /*Test that the frog-croak pair is no longer in the map*/
   assert(strstr(str, "[frog](croak) ")==NULL);
   /*Check all other pairs are*/
   tempStr = (char *)calloc(1, (2*NEILLWORDSIZE+EXTRACHARS+1)*sizeof(char));
   for (j = 0; j < NEILLPAIRNUM; j++) {
      if (strcmp(animals[j],"dog") && strcmp(animals[j],"frog") ) {
         sprintf(tempStr,"[%s](%s) ",animals[j],noises[j]);
         assert(strstr(str, tempStr)!=NULL);
      }
   }
   free(tempStr);
   free(str);

   /* Insert Multiple Keys */
   mvm_insert(m, "frog", "croak");
   mvm_insert(m, "frog", "ribbit");
   assert(mvm_size(m)==5);
   str = mvm_print(m);
   assert(strstr(str, "[frog](croak) ")!=NULL);
   assert(strstr(str, "[frog](ribbit) ")!=NULL);
   free(str);

   /* Search Multiple Keys */
   str = mvm_search(m, "frog");
   i = strcmp(str, "ribbit");
   assert(i==0);

   /* Multisearching */
   av = mvm_multisearch(m, "cat", &i);
   assert(i==1);
   i = strcmp(av[0], "meow");
   assert(i==0);
   free(av);
   av = mvm_multisearch(m, "horse", &i);
   assert(i==1);
   i = strcmp(av[0], "neigh");
   assert(i==0);
   free(av);
   av = mvm_multisearch(m, "frog", &i);
   assert(i==2);
   i = strcmp(av[0], "ribbit");
   j = strcmp(av[1], "croak");
   assert((i==0)&&(j==0));
   free(av);

   /* Delete Multiple Keys */
   mvm_delete(m, "frog");
   assert(mvm_size(m)==4);
   str = mvm_print(m);
   assert(strstr(str, "[frog](croak) ")!=NULL);
   assert(strstr(str, "[frog](ribbit) ")==NULL);
   free(str);
   mvm_delete(m, "frog");
   assert(mvm_size(m)==3);
   str = mvm_print(m);
   assert(strstr(str, "[frog](croak) ")==NULL);
   assert(strstr(str, "[frog](ribbit) ")==NULL);
   free(str);

   /* Weird NULL insert() edge cases */
   mvm_insert(m, NULL, "quack");
   assert(mvm_size(m)==3);
   mvm_insert(NULL, "duck", "quack");
   assert(mvm_size(m)==3);
   mvm_insert(m, "duck", NULL);
   assert(mvm_size(m)==3);

   /* Weird NULL delete() edge cases */
   mvm_delete(m, "");
   assert(mvm_size(m)==3);
   mvm_delete(m, NULL);
   assert(mvm_size(m)==3);
   mvm_delete(NULL, "frog");
   assert(mvm_size(m)==3);
   mvm_delete(m, "bird");
   assert(mvm_size(m)==2);
   str = mvm_print(m);
   assert(strstr(str, "[horse](neigh) ")!=NULL);
   assert(strstr(str, "[cat](meow) ")!=NULL);
   free(str);

   /*Other edge cases*/
   assert(mvm_size(NULL)==0);
   assert(mvm_print(NULL)==NULL);
   assert(mvm_search(NULL, "meow")==NULL);
   assert(mvm_search(m, NULL)==NULL);

   /* Freeing */
   mvm_free(&m);
   mvm_free(&m2);
   assert(m==NULL);
   assert(mvm_size(m)==0);

   printf("Basic MVM Tests ... Stop\n");
   return 0;
}
