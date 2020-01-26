typedef struct nalVar{
   char *name;
   char *val;
   struct nalVar *next;
}nalVar;

typedef struct vList{
   nalVar *head;
   int size;
   struct vList *prev;
} vList;

vList *vList_init(void);
/*Insert new variable. If variable exists already, just updates the val*/
void vList_insert(vList *vl, char *name, char *val);
/*Returns the (original) val string, if no match returns NULL*/
char *vList_search(vList *vl, char *name);
/*In the extension variables are kind of local, when a variable is searched we first look for it locally, if it is not within the scope of the function, we look at the previous scope and so forth.*/
char *vList_scopedSearch(vList *vl, char *name);
/*Print all Variables to screen*/
void vList_print(vList *vl);
/*Frees all the variable nodes and the list*/
void vList_free(vList **p);

void vList_freeAll(vList *vl);
