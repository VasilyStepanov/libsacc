#include "test_selector.h"

#include "selector.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



void test_selector_alloc() {
  mpool_t mpool;
  SAC_Selector *selector;

  mpool = mpool_open(256);
  selector = selector_alloc(mpool, SAC_ANY_NODE_SELECTOR);

  assert(selector->selectorType == SAC_ANY_NODE_SELECTOR);
  
  mpool_close(mpool);
}



void test_selector() {
  test_selector_alloc();
}