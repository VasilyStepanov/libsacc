#include "test_lexical_unit.h"

#include "lexical_unit.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



void test_lexical_unit_alloc() {
  mpool_t mpool;
  SAC_LexicalUnit *value;

  mpool = mpool_open(256);
  value = lexical_unit_alloc(mpool, SAC_STRING_VALUE);

  assert(value->lexicalUnitType == SAC_STRING_VALUE);

  mpool_close(mpool);
}



void test_lexical_unit() {
  test_lexical_unit_alloc();
}
