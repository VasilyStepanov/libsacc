#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"

#include <sacc.h>

#include <stddef.h>



SAC_LexicalUnit* lexical_unit_alloc(
  mpool_t mpool, SAC_LexicalUnitCode type);

SAC_LexicalUnit** lexical_unit_array_alloc(mpool_t mpool, size_t size);

void lexical_unit_array_cpy(SAC_LexicalUnit **dest, SAC_LexicalUnit **src);

size_t lexical_unit_array_size(SAC_LexicalUnit **array);



#endif
