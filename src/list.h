#ifndef LIST_H
#define LIST_H

#include "mpool.h"



typedef void* list_t;

typedef void** list_iter_t;



list_t list_open(mpool_t mpool);

void list_close(mpool_t mpool, list_t list);

list_iter_t list_head(list_t list);

list_iter_t list_next(list_iter_t iter);

void list_push_back(list_t list, mpool_t mpool, void *obj);



#endif
