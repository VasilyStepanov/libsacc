#include "selector.h"



SAC_Selector* selector_alloc(mpool_t mpool, SAC_SelectorType type) {
  SAC_Selector *ret = (SAC_Selector*)mpool_alloc(mpool, sizeof(SAC_Selector));
  ret->selectorType = type;
  return ret;
}
