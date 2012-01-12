#ifndef LIST_H
#define LIST_H

#include "mpool.h"



typedef void* SAC_List;

typedef void** list_iter_t;



SAC_List list_open(SAC_MPool mpool);

void list_close(SAC_MPool mpool, SAC_List list);

list_iter_t list_head(SAC_List list);

list_iter_t list_next(list_iter_t iter);

void list_push_front(SAC_List list, SAC_MPool mpool, void *obj);

void list_push_back(SAC_List list, SAC_MPool mpool, void *obj);

size_t list_size(SAC_List list);



#endif
