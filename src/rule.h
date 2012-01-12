#ifndef RULE_H
#define RULE_H

#include "mpool.h"
#include "vector.h"
#include "list.h"



struct _SAC_Rule {
  vector_t selectors;
  list_t declarations;
};

typedef struct _SAC_Rule SAC_Rule;



SAC_Rule* rule_alloc(mpool_t mpool, vector_t selectors, list_t declarations);



#endif
