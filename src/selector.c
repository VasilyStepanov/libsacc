#include "selector.h"



SAC_Selector* SAC_selector_alloc(SAC_MPool mpool, SAC_SelectorType type) {
  SAC_Selector *selector = (SAC_Selector*)SAC_mpool_alloc(mpool,
    sizeof(SAC_Selector));

  if (selector == NULL) return NULL;

  selector->selectorType = type;
  return selector;
}
