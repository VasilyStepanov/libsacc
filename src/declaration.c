#include "declaration.h"



SAC_Declaration* declaration_alloc(mpool_t mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important)
{
  SAC_Declaration *declaration = (SAC_Declaration*)mpool_alloc(mpool,
    sizeof(SAC_Declaration));
  declaration->property = property;
  declaration->value = value;
  declaration->important = important;
  return declaration;
}
