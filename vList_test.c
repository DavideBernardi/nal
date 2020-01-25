#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vList.h"

#define VARNUM 30
#define VARSIZE 100

int main(void)
{
   vList *vl1;
   int j, i;
   char strnames[VARNUM][VARSIZE] = {"$A",  "$B",  "$C",  "$VAR", "$LONGVARNAME","$S",  "$E", "$D",  "$F", "$G", "$H",  "$I",  "$L",  "$OP", "$HGT", "$RY",  "$C",  "$RTYUI",  "$HGF", "$OI","$DFG",  "$ASFS",  "$NBV", "$ETHF", "$KJHD","$UYTR", "$CBVDG",  "$TGBF",  "$IUJHNTR", "$OKERTH"};
   char strvals[VARNUM][VARSIZE] = {"meow", "bark", "tweet", "neigh", "croak","meowa",
   "barka", "tweeta", "neigha", "croaka","meowb", "More than one word", "Numbers in string 12312", "", "croakb","meowc",
   "bark", "tweet", "neigh", "croak","meow", "bark", "tweetd", "neighd", "croakd",
   "meowe", "barke", "tweete", "neighe", "croake"};
   char numnames[VARNUM][VARSIZE] = {"%A",  "%B",  "%C",  "%VAR", "%LONGVARNAME","%S",  "%E", "%D",  "%F", "%G", "%H",  "%I",  "%L",  "%OP", "%HGT", "%RY",  "%LHKL",  "%C",  "%HGF", "%OI","%DFG",  "%ASFS",  "%NBV", "%ETHF", "%KJHD","%UYTR", "%CBVDG",  "%TGBF",  "%IUJHNTR", "%OKERTH"};
   char numvals[VARNUM][VARSIZE] = {"0", "0.1", "-0.1", "12453415", "2134.654","-1234.32",
   "98765", "0.2345", "-0.6543", "6543.2","7445.04", "756345.043", "6536", "435", "8765","2345",
   "8765.2345", "-8761234", "76", "867586","32542.756", "987.23444445", "0.999999", "-5", "354673.4",
   "67.76767", "1427457", "9", "10101", "876",};

   printf("Basic vList Tests ... Start\n");
   /* Set up empty array */
   vl1 = vList_init();
   assert(vl1 != NULL);

   /* Building and Searching */
   for(j=0; j<VARNUM; j++){
      vList_insert(vl1, strnames[j], strvals[j]);
      i = strcmp(vList_search(vl1, strnames[j]), strvals[j]);
      assert(i==0);
   }
   for(j=0; j<VARNUM; j++){
      vList_insert(vl1, numnames[j], numvals[j]);
      i = strcmp(vList_search(vl1, numnames[j]), numvals[j]);
      assert(i==0);
   }

   vList_free(&vl1);
   assert(vl1==NULL);
   printf("Basic vList Tests ... Stop\n");

   return 0;
}
