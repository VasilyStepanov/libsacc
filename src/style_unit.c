#include "style_unit.h"



SAC_StyleUnit* SAC_style_unit_alloc(SAC_MPool mpool, SAC_StyleUnitType type) {
  SAC_StyleUnit *style = (SAC_StyleUnit*)SAC_mpool_alloc(mpool,
    sizeof(SAC_StyleUnit));
  style->type = type;
  return style;
}
