#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"

#include <sacc.h>



SAC_LexicalUnit* lexical_unit_alloc(
  mpool_t mpool, SAC_LexicalUnitCode type);



#endif
