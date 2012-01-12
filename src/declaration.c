#include "declaration.h"



SAC_Declaration* SAC_declaration_alloc(SAC_MPool mpool,
  SAC_STRING property, SAC_LexicalUnit *value, SAC_Boolean important)
{
  SAC_Declaration *declaration = (SAC_Declaration*)SAC_mpool_alloc(mpool,
    sizeof(SAC_Declaration));
  declaration->property = property;
  declaration->value = value;
  declaration->important = important;
  return declaration;
}
