#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vList.h"

void freeVar(nalVar *var);
void *vList_allocate(int size, char* const msg);
void *vList_callocate(int size1, int size2, char* const msg);


vList *vList_init(void)
{
   vList *vl;

   vl = (vList *)vList_allocate(sizeof(vList), "Variables List");
   vl->head = NULL;
   vl->size = 0;

   return vl;
}

void vList_insert(vList *vl, char *name, char *val)
{
   nalVar *curr, *newVar;
   char *allocName, *allocVal;
   int namelen, vallen;

   if (vl == NULL || name == NULL || val == NULL) {
      fprintf(stderr, "Passed a NULL pointer to Insert function, terminating . . .\n");
      exit(EXIT_FAILURE);
   }

   namelen = strlen(name);
   vallen = strlen(val);

   curr = vl->head;

   /*If variable is already in list, just update it*/
   while (curr!=NULL) {
      if (!strcmp(curr->name,name)) {
         free(curr->val);
         curr->val = (char *)vList_allocate((vallen+1)*sizeof(char),"Value of a Variable");
         strcpy(curr->val,val);
         return;
      }
      curr = curr->next;
   }

   newVar = (nalVar *)vList_allocate(sizeof(nalVar), "Variable");
   allocName = (char *)vList_allocate((namelen+1)*sizeof(char), "Name of a Variable");
   allocVal = (char *)vList_allocate((vallen+1)*sizeof(char), "Value of a Variable");

   strcpy(allocName, name);
   strcpy(allocVal, val);

   newVar->name = allocName;
   newVar->val = allocVal;
   newVar->next = vl->head;

   vl->head = newVar;
   vl->size++;
}

char *vList_search(vList *vl, char *name)
{
   nalVar *curr;

   if (vl == NULL || name == NULL) {
      fprintf(stderr, "Passed a NULL pointer to Search function, terminating . . .\n");
      exit(EXIT_FAILURE);
   }

   curr = vl->head;

   while (curr!=NULL) {
      if (!strcmp(curr->name,name)) {
         return curr->val;
      }
      curr = curr->next;
   }

   return NULL;
}

void vList_free(vList **p)
{
   vList *vl;
   nalVar *curr, *next;

   if (p==NULL) {
      return;
   }
   vl = *p;
   if (vl==NULL) {
      return;
   }

   curr = vl->head;
   while (curr!=NULL) {
      next = curr->next;
      freeVar(curr);
      curr = next;
   }

   free(vl);
   *p = NULL;
}

void freeVar(nalVar *var)
{
   if (var == NULL) {
      return;
   }
   free(var->name);
   free(var->val);
   free(var);
}

void *vList_allocate(int size, char* const msg)
{
   void *p;

   p = malloc(size);
   if (p == NULL) {
      fprintf(stderr,"Couldn't allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}

void *vList_callocate(int size1, int size2, char* const msg)
{
   void *p;

   p = calloc(size1,size2);
   if (p == NULL) {
      fprintf(stderr,"Couldn't vList_allocate space for %s, Terminating ...\n", msg);
      exit(EXIT_FAILURE);
   }

   return p;
}
