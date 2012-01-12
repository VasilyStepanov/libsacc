#ifndef STYLE_UNIT_H
#define STYLE_UNIT_H

#include "mpool.h"
#include "vector.h"
#include "list.h"



struct _SAC_StyleUnit {
  SAC_Vector selectors;
  SAC_List declarations;
};

typedef struct _SAC_StyleUnit SAC_StyleUnit;



SAC_StyleUnit* SAC_style_unit_alloc(SAC_MPool mpool,
  SAC_Vector selectors, SAC_List declarations);



#endif
