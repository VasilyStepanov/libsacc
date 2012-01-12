#include "test_selector.h"

#include "selector.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_selector_alloc() {
  SAC_MPool mpool;
  SAC_Selector *selector;

  mpool = SAC_mpool_open(256);
  selector = SAC_selector_alloc(mpool, SAC_ANY_NODE_SELECTOR);

  assert(selector->selectorType == SAC_ANY_NODE_SELECTOR);
  
  SAC_mpool_close(mpool);
}



void test_selector() {
  test_selector_alloc();
}
