#ifndef RULE_H
#define RULE_H

#include "mpool.h"
#include "vector.h"
#include "list.h"



struct rule_s {
  vector_t selectors;
  list_t declarations;
};

typedef struct rule_s* rule_t;



rule_t rule_alloc(mpool_t mpool, vector_t selectors, list_t declarations);



#endif
