#include "test_page_margin.h"

#include "src/page_margin.h"

#include <assert.h>



static void test_page_margin_alloc(void) {
  SAC_MPool mpool;
  SAC_PageMargin *margin;

  mpool = SAC_mpool_open(256);

  margin = SAC_page_margin(mpool, SAC_TOP_LEFT_CORNER_PAGE_MARGIN);
  assert(margin->pageMarginType == SAC_TOP_LEFT_CORNER_PAGE_MARGIN);

  SAC_mpool_close(mpool);
}



void test_page_margin(void) {
  test_page_margin_alloc();
}
