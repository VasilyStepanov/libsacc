#include "lexical_unit.h"



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
  size_t size;
  SAC_LexicalUnit *value;

  size = SAC_list_size(list);
  if (size == 1) {
    value = *SAC_list_head(list);
  } else {
    SAC_Vector vector;
    SAC_VectorIter vit;
    SAC_ListIter lit;

    value = SAC_lexical_unit_alloc(mpool, SAC_SUB_EXPRESSION);

    if (value == NULL) return NULL;

    vector = SAC_vector_open(mpool, size);
    for (lit = SAC_list_head(list), vit = SAC_vector_head(vector);
         lit != NULL;
         lit = SAC_list_next(lit), ++vit)
    {
      *vit = *lit;
    }
    value->desc.subValues = vector;
  }

  return value;
}
