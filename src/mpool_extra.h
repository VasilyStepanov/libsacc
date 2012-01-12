#ifndef MPOOL_EXTRA_H
#define MPOOL_EXTRA_H

#include "mpool.h"



char* mpool_strdup(SAC_MPool mpool, const char *s);

char* mpool_strndup(SAC_MPool mpool, const char *s, size_t n);



#endif
