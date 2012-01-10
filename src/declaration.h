#ifndef DECLARATION_H
#define DECLARATION_H

#include "mpool.h"

#include <sacc.h>



struct declaration_s {
  SAC_STRING property;
  SAC_LexicalUnit *value;
  SAC_Boolean important;
};

typedef struct declaration_s *declaration_t;



declaration_t declaration_alloc(
  mpool_t mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important);

#endif
