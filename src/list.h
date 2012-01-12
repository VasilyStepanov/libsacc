#ifndef LIST_H
#define LIST_H

#include "mpool.h"



typedef void* SAC_List;

typedef void** list_iter_t;



SAC_List list_open(mpool_t mpool);

void list_close(mpool_t mpool, SAC_List list);

list_iter_t list_head(SAC_List list);

list_iter_t list_next(list_iter_t iter);

void list_push_front(SAC_List list, mpool_t mpool, void *obj);

void list_push_back(SAC_List list, mpool_t mpool, void *obj);

size_t list_size(SAC_List list);



#endif
