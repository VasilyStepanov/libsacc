#ifndef MPOOL_H
#define MPOOL_H

#include <stddef.h>



typedef void* SAC_MPool;



SAC_MPool SAC_mpool_open(size_t page_size);

void SAC_mpool_close(SAC_MPool mpool);

void* SAC_mpool_alloc(SAC_MPool mpool, size_t size);

void* SAC_mpool_realloc(SAC_MPool mpool, void *ptr, size_t size);

void SAC_mpool_free(SAC_MPool mpool, void *ptr);

void SAC_mpool_stats(SAC_MPool mpool, size_t *pages);



#endif
