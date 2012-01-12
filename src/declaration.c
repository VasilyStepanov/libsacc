#include "declaration.h"



declaration_t declaration_alloc(mpool_t mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important)
{
  declaration_t declaration = (declaration_t)mpool_alloc(mpool,
    sizeof(struct declaration_s));
  declaration->property = property;
  declaration->value = value;
  declaration->important = important;
  return declaration;
}
