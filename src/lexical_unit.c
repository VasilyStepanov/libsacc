#include "lexical_unit.h"



SAC_LexicalUnit* lexical_unit_alloc(
  mpool_t mpool, SAC_LexicalUnitCode type)
{
  SAC_LexicalUnit *value = (SAC_LexicalUnit*)mpool_alloc(
    mpool, sizeof(SAC_LexicalUnit)
  );
  value->lexicalUnitType = type;
  return value;
}



SAC_LexicalUnit** lexical_unit_array_alloc(mpool_t mpool, size_t size) {
  SAC_LexicalUnit **array = mpool_alloc(
    mpool, sizeof(SAC_LexicalUnit*) * (size + 1)
  );

  array[size] = NULL;

  return array;
}



void lexical_unit_array_cpy(SAC_LexicalUnit **dest, SAC_LexicalUnit **src) {
  SAC_LexicalUnit **lhs, **rhs;

  for (lhs = dest, rhs = src; *rhs != NULL; ++lhs, ++rhs)
    *lhs = *rhs;

  *(++rhs) = NULL;
}



size_t lexical_unit_array_size(SAC_LexicalUnit **array) {
  size_t size;
  SAC_LexicalUnit **it;

  for (size = 0, it = array; *it != NULL; ++size, ++it);
  
  return size;
}
