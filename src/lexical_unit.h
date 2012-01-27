#ifndef LEXICAL_UNIT_H
#define LEXICAL_UNIT_H

#include "mpool.h"
#include "list.h"

#include <sacc.h>



SAC_LexicalUnit* SAC_lexical_unit_alloc(SAC_MPool mpool,
  SAC_LexicalUnitCode type);

SAC_LexicalUnit* SAC_lexical_unit_from_list(SAC_List list, SAC_MPool mpool);



#endif
