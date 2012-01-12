#include "style_unit.h"



SAC_StyleUnit* SAC_style_unit_alloc(SAC_MPool mpool,
  SAC_Vector selectors, SAC_List declarations)
{
  SAC_StyleUnit *rule = (SAC_StyleUnit*)SAC_mpool_alloc(mpool,
    sizeof(SAC_StyleUnit));
  rule->selectors = selectors;
  rule->declarations = declarations;
  return rule;
}
