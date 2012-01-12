#ifndef RULE_H
#define RULE_H

#include "mpool.h"
#include "vector.h"
#include "list.h"



struct _SAC_Rule {
  SAC_Vector selectors;
  list_t declarations;
};

typedef struct _SAC_Rule SAC_Rule;



SAC_Rule* rule_alloc(mpool_t mpool, SAC_Vector selectors, list_t declarations);



#endif
