#include "lexical_unit.h"



SAC_LexicalUnit* lexical_unit_alloc(
  SAC_MPool mpool, SAC_LexicalUnitCode type)
{
  SAC_LexicalUnit *value = (SAC_LexicalUnit*)mpool_alloc(
    mpool, sizeof(SAC_LexicalUnit)
  );
  value->lexicalUnitType = type;
  return value;
}



SAC_LexicalUnit** lexical_unit_vector_from_list(
  SAC_List list, SAC_MPool mpool)
{
  SAC_Vector vector;
  SAC_VectorIter vit;
  list_iter_t lit;

  vector = vector_open(mpool, list_size(list));
  for (lit = list_head(list), vit = vector_head(vector);
       lit != NULL;
       lit = list_next(lit), ++vit)
  {
    *vit = *lit;
  }

  return (SAC_LexicalUnit**)vector;
}



SAC_LexicalUnit* lexical_unit_from_list(SAC_List list, SAC_MPool mpool) {
  size_t size;
  SAC_LexicalUnit *value;

  size = list_size(list);
  if (size == 1) {
    value = *list_head(list);
  } else {
    SAC_Vector vector;
    SAC_VectorIter vit;
    list_iter_t lit;

    value = lexical_unit_alloc(mpool, SAC_SUB_EXPRESSION);
    vector = vector_open(mpool, size);
    for (lit = list_head(list), vit = vector_head(vector);
         lit != NULL;
         lit = list_next(lit), ++vit)
    {
      *vit = *lit;
    }
    value->desc.subValues = vector;
  }

  return value;
}
