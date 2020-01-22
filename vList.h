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
/*Insert new variable. If variable exists already, just updates the val*/
void vList_insert(vList *vl, char *name, char *val);
/*Returns the (original) val string, if no match returns NULL*/
char *vList_search(vList *vl, char *name);
/*Print all Variables to screen*/
void vList_print(vList *vl);
/*Frees all the variable nodes and the list*/
void vList_free(vList **p);
