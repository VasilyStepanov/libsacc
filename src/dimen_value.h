#ifndef DIMENVALUE_H
#define DIMENVALUE_H

#include "mpool.h"

#include <sacc.h>



struct _SAC_DimenValue {
  SAC_STRING unit;
  double value;
};

typedef struct _SAC_DimenValue SAC_DimenValue;



SAC_DimenValue* SAC_dimen_value_alloc(SAC_MPool mpool, const char *text);

void SAC_dimen_value_free(SAC_DimenValue *dimen, SAC_MPool mpool);



#endif
