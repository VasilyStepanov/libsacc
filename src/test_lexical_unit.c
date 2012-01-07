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



void test_lexical_unit_array() {
  mpool_t mpool;
  SAC_LexicalUnit *value;
  SAC_LexicalUnit **array1;
  SAC_LexicalUnit **array2;
  size_t array_size;
  size_t i;

  mpool = mpool_open(256);
  value = lexical_unit_alloc(mpool, SAC_STRING_VALUE);
  array1 = lexical_unit_array_alloc(mpool, 10);
  for (i = 0; i < 10; ++i) array1[i] = value;
  array_size = lexical_unit_array_size(array1);

  assert(array_size == 10);

  array2 = lexical_unit_array_alloc(mpool, 10);
  lexical_unit_array_cpy(array2, array1);
  array_size = lexical_unit_array_size(array1);

  assert(array_size == 10);
  for (i = 0; i < 10; ++i) assert(array2[i] == value);

  mpool_close(mpool);
}



void test_lexical_unit() {
  test_lexical_unit_alloc();
  test_lexical_unit_array();
}
