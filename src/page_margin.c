#include "page_margin.h"



SAC_PageMargin* SAC_page_margin(SAC_MPool mpool, SAC_PageMarginType type) {
  SAC_PageMargin *margin = (SAC_PageMargin*)SAC_mpool_alloc(
    mpool, sizeof(SAC_PageMargin)
  );

  if (margin == NULL) return NULL;

  margin->pageMarginType = type;

  return margin;
}
