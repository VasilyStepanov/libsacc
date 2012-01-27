#ifndef MEDIA_QUERY_H
#define MEDIA_QUERY_H

#include "mpool.h"

#include <sacc.h>

SAC_MediaQuery* SAC_media_query_alloc(SAC_MPool mpool, SAC_MediaQueryType type);

#endif
