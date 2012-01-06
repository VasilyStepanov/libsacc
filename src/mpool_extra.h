#ifndef MPOOL_EXTRA_H
#define MPOOL_EXTRA_H

#include "mpool.h"



char* mpool_strdup(mpool_t mpool, const char *s);

char* mpool_strndup(mpool_t mpool, const char *s, size_t n);



#endif
