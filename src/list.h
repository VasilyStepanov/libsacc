#ifndef LIST_H
#define LIST_H

#include "mpool.h"



typedef struct _SAC_List* SAC_List;

typedef void** SAC_ListIter;



SAC_List SAC_list_open(SAC_MPool mpool);

void SAC_list_close(SAC_List list, SAC_MPool mpool);

SAC_ListIter SAC_list_head(SAC_List list);

SAC_ListIter SAC_list_next(SAC_ListIter iter);

void* SAC_list_push_front(SAC_List list, SAC_MPool mpool, void *obj);

void* SAC_list_push_back(SAC_List list, SAC_MPool mpool, void *obj);

size_t SAC_list_size(SAC_List list);



#endif
