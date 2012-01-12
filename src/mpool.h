#ifndef MPOOL_H
#define MPOOL_H

#include <stddef.h>



typedef void* SAC_MPool;



SAC_MPool mpool_open(size_t page_size);

void mpool_close(SAC_MPool mpool);

void* mpool_alloc(SAC_MPool mpool, size_t size);

void* mpool_realloc(SAC_MPool mpool, void *ptr, size_t size);

void mpool_free(SAC_MPool mpool, void *ptr);

void mpool_stats(SAC_MPool mpool, size_t *pages);



#endif
