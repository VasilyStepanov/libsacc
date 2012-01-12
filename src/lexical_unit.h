#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"
#include "list.h"
#include "vector.h"

#include <sacc.h>



SAC_LexicalUnit* lexical_unit_alloc(mpool_t mpool, SAC_LexicalUnitCode type);

SAC_LexicalUnit** lexical_unit_vector_from_list(list_t list, mpool_t mpool);

SAC_LexicalUnit* lexical_unit_from_list(list_t list, mpool_t mpool);



#endif
