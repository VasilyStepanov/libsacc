#include "lexical_unit.h"

#include "vector_extra.h"



SAC_LexicalUnit* SAC_lexical_unit_alloc(
  SAC_MPool mpool, SAC_LexicalUnitCode type)
{
  SAC_LexicalUnit *value = (SAC_LexicalUnit*)SAC_mpool_alloc(
    mpool, sizeof(SAC_LexicalUnit)
  );

  if (value == NULL) return NULL;

  value->lexicalUnitType = type;
  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_from_list(SAC_List list, SAC_MPool mpool) {
  SAC_LexicalUnit *value;

  if (SAC_list_size(list) == 1) {
    value = *SAC_list_head(list);
  } else {
    SAC_Vector vector;

    value = SAC_lexical_unit_alloc(mpool, SAC_SUB_EXPRESSION);

    if (value == NULL) return NULL;

    vector = SAC_vector_from_list(list, mpool);

    if (vector == NULL) return NULL;

    value->desc.subValues = vector;
  }

  return value;
}
