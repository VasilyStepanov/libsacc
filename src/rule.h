#ifndef RULE_H
#define RULE_H

#include "mpool.h"
#include "vector.h"
#include "list.h"



struct _SAC_Rule {
  SAC_Vector selectors;
  SAC_List declarations;
};

typedef struct _SAC_Rule SAC_Rule;



SAC_Rule* SAC_rule_alloc(SAC_MPool mpool,
  SAC_Vector selectors, SAC_List declarations);



#endif
