#ifndef MPOOL_H
#define MPOOL_H

#include <stddef.h>



typedef void* mpool_t;



mpool_t mpool_open(size_t page_size);

void mpool_close(mpool_t mpool);

void* mpool_alloc(mpool_t mpool, size_t size);

void* mpool_realloc(mpool_t mpool, void *ptr, size_t size);

char* mpool_strdup(mpool_t mpool, const char *s);

void mpool_free(mpool_t mpool, void *ptr);

void mpool_stats(mpool_t mpool, size_t *pages);



#endif
