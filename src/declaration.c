#include "declaration.h"



declaration_t declaration_alloc(
  mpool_t mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important)
{
  declaration_t ret = (declaration_t)mpool_alloc(
    mpool, sizeof(struct declaration_s));
  ret->property = property;
  ret->value = value;
  ret->important = important;
  return ret;
}
