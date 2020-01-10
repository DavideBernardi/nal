#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vList.h"


vList *vList_init(void)
{
   vList *vl;

   vl = (vList *)allocate(sizeof(vList), "Variables List");
   vl->head = NULL;
   vl->size = 0;

   return vl;
}
