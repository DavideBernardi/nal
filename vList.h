typedef struct nalVar{
   char *name;
   char *val;
   struct nalVar *next;
}nalVar;

typedef struct vList{
   nalVar *head;
   int size;
} vList;

vList *vList_init(void);
/*If variable exists already, just updates the val*/
void vList_insert(vList *vl, char *name, char *val);
/*Returns the name string*/
char *vList_search(vList *vl, char *name);
/*Frees all the variable nodes and the list*/
void vList_free(vList **p);
