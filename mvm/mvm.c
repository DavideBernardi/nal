#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mvm.h"

#define MAXWORDSIZE 50
/*To print each cell need 5 extra chars of space for "()[] "*/
#define EXTRACHARS 5

typedef enum {FALSE, TRUE} bool;

typedef struct qnode {
   mvmcell *cell;
   struct qnode *qnext;
} qnode;

typedef struct queue {
   qnode *front;
   qnode *rear;
} queue;


void freeCell(mvmcell *cell);

/*Function used in mvm_print()*/
int findLength(mvm *m, int *maxLen);

/*These functions are malloc and calloc but they give an error and
terminate the program if something goes wrong with the allocation*/
void *allocate(int size2, char* const msg);
void *callocate(int size1, int size2, char* const msg);

/*Functions for queue used in mvm_multisearch()*/
queue *q_init(void);
void q_push(queue *q, mvmcell* cell);
mvmcell *q_pop(queue *q);
void q_free(queue *q);
int build_q(queue *q, mvm* m, char* key);

mvm* mvm_init(void)
{
   mvm *m;

   m = (mvm *)allocate(sizeof(mvm), "Map");

   m->head = NULL;
   m->numkeys = 0;

   return m;
}

int mvm_size(mvm* m)
{
   if (m == NULL) {
      return 0;
   }
   return m->numkeys;
}

void mvm_insert(mvm* m, char* key, char* data)
{
   mvmcell *cell;
   char *allocatedKey, *allocatedData;
   int keylen, datalen;

   if (m == NULL || key == NULL || data == NULL) {
      return;
   }

   keylen = strlen(key);
   datalen = strlen(data);

   /*Allocate memory for new cell*/
   cell = (mvmcell *)allocate(sizeof(mvmcell), "Cell");
   allocatedKey = (char *)allocate((keylen+1)*sizeof(char), "Key");
   allocatedData = (char *)allocate((datalen+1)*sizeof(char), "Value");

   strcpy(allocatedKey, key);
   strcpy(allocatedData, data);

   cell->key = allocatedKey;
   cell->data = allocatedData;
   cell->next = m->head;

   m->head = cell;
   m->numkeys++;
}

/*Note: This function goes through whole map twice: once to calloc the right
space and then again to actually build the string*/
char* mvm_print(mvm* m)
{
   char *str, *tempStr;
   mvmcell *cell;
   int maxLen, totLen;

   if (m == NULL) {
      return NULL;
   }

   /*This function explores the whole list only to find out the total char size
   the final string will need to be and the char size of the largest single
   cell output*/
   totLen = findLength(m, &maxLen);

   str = (char *)callocate(1,(totLen+1)*sizeof(char),"Map Printing");
   tempStr = (char *)callocate(1,(maxLen+1)*sizeof(char),"Map Printing");

   cell=m->head;

   while(cell!=NULL)
   {
      if (!sprintf(tempStr,"[%s](%s) ",cell->key,cell->data)) {
         ON_ERROR("Error while Printing Map, Terminating ...\n");
      }
      strcat(str,tempStr);
      cell=cell->next;
   }
   free(tempStr);
   return str;
}

void mvm_delete(mvm* m, char* key)
{
   mvmcell *cell, *cellNext;

   if (m == NULL || key == NULL) {
      return;
   }

   cell = m->head;
   if (cell==NULL) {
      return;
   }

   /*If we get a match for the head, remove it and set the next item as the
   new head*/
   if (!strcmp(cell->key,key)) {
      m->head = cell->next;
      m->numkeys--;
      freeCell(cell);
      return;
   }

   while (cell->next!=NULL) {
      if (!strcmp(cell->next->key,key)) {
         cellNext = cell->next;
         cell->next = cellNext->next;
         m->numkeys--;
         freeCell(cellNext);
         return;
      }
      cell = cell->next;
   }
}

char* mvm_search(mvm* m, char* key)
{
   mvmcell *cell;

   if (m == NULL || key == NULL) {
      return NULL;
   }

   cell = m->head;

   while (cell!=NULL) {
      if (!strcmp(cell->key,key)) {
         return cell->data;
      }
      cell = cell->next;
   }

   return NULL;
}

/*This function first goes through the whole list first to figure out how much
space it needs to calloc, while doing so it builds a queue with all the matching
Cells. It then goes through that queue to build the string array.
*/
char** mvm_multisearch(mvm* m, char* key, int* n)
{
   char **matches;
   int matchesAmount, j;
   mvmcell *cell;
   queue *q;

   if (m == NULL || key == NULL || n == NULL) {
      n = NULL;
      return NULL;
   }

   q = q_init();
   matchesAmount = build_q(q, m, key);

   j = 0;
   matches = (char **)callocate(matchesAmount,sizeof(char *),"Searching");

   cell = q_pop(q);
   while (cell!=NULL){
      matches[j] = cell->data;
      j++;
      cell = q_pop(q);
   }
   q_free(q);

   *n = matchesAmount;
   return matches;
}

int build_q(queue *q, mvm* m, char* key)
{
   int matchesAmount;
   mvmcell *cell;

   cell = m->head;
   matchesAmount = 0;

   while (cell!=NULL) {
      if (!strcmp(cell->key,key)) {
         q_push(q, cell);
         matchesAmount++;
      }
      cell=cell->next;
   }

   return matchesAmount;
}

void mvm_free(mvm** p)
{
   mvm *m;
   mvmcell *cell, *cellNext;

   if (p==NULL) {
      return;
   }
   m = *p;
   if (m==NULL) {
      return;
   }

   cell = m->head;
   while (cell!=NULL) {
      cellNext = cell->next;
      freeCell(cell);
      cell = cellNext;
   }

   free(m);
   *p = NULL;
}

void freeCell(mvmcell *cell)
{
   if (cell == NULL) {
      return;
   }
   free(cell->key);
   free(cell->data);
   free(cell);
}

int findLength(mvm *m, int *maxLen)
{
   int totLen, curr;
   mvmcell *cell;

   totLen = 0;
   *maxLen = 0;

   cell=m->head;

   while(cell!=NULL)
   {
      curr = strlen(cell->key)+strlen(cell->data)+EXTRACHARS;
      if (curr > *maxLen) {
         *maxLen = curr;
      }
      totLen += curr;
      cell=cell->next;
   }

   return totLen;
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

queue *q_init(void)
{
   queue *q;

   q = (queue *)allocate(sizeof(queue), "Queue");

   q->front = NULL;
   q->rear = NULL;

   return q;
}

void q_push(queue *q, mvmcell *cell)
{
   qnode *node;

   node = (qnode *)allocate(sizeof(qnode), "Queue Node");
   node->cell = cell;
   node->qnext = NULL;

   if (q->rear == NULL) {
      q->front = node;
   } else {
      q->rear->qnext = node;
   }
   q->rear = node;
}

mvmcell *q_pop(queue *q)
{
   mvmcell *cell;
   qnode *node;

   if (q->front == NULL) {
      return NULL;
   }

   cell = q->front->cell;

   node = q->front->qnext;

   if (q->front == q->rear) {
      q->rear = NULL;
   }
   free(q->front);
   q->front = node;

   return cell;

}

void q_free(queue *q)
{
   while (q->front!=NULL) {
      q_pop(q);
   }
   free(q);
}
