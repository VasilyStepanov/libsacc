#include "test_lexical_unit.h"

#include "lexical_unit.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_lexical_unit_alloc() {
  mpool_t mpool;
  SAC_LexicalUnit *value;

  mpool = mpool_open(256);
  value = lexical_unit_alloc(mpool, SAC_STRING_VALUE);

  assert(value->lexicalUnitType == SAC_STRING_VALUE);

  mpool_close(mpool);
}



static void test_lexical_unit_vector_from_list() {
  mpool_t mpool;
  SAC_LexicalUnit *a, *b, *c;
  SAC_List list;
  SAC_LexicalUnit **vector;

  mpool = mpool_open(256);

  a = lexical_unit_alloc(mpool, SAC_INTEGER);
  b = lexical_unit_alloc(mpool, SAC_INTEGER);
  c = lexical_unit_alloc(mpool, SAC_INTEGER);

  list = list_open(mpool);
  list_push_back(list, mpool, a);
  list_push_back(list, mpool, b);
  list_push_back(list, mpool, c);
  vector = lexical_unit_vector_from_list(list, mpool);

  assert(vector[0] == a);
  assert(vector[1] == b);
  assert(vector[2] == c);
  assert(vector[3] == NULL);

  list_close(list, mpool);

  list = list_open(mpool);
  list_push_back(list, mpool, a);
  vector = lexical_unit_vector_from_list(list, mpool);

  assert(vector[0] == a);
  assert(vector[1] == NULL);

  mpool_close(mpool);
}



static void test_lexical_unit_from_list() {
  mpool_t mpool;
  SAC_LexicalUnit *a, *b, *c;
  SAC_List list;
  SAC_LexicalUnit *value;

  mpool = mpool_open(256);

  a = lexical_unit_alloc(mpool, SAC_INTEGER);
  b = lexical_unit_alloc(mpool, SAC_INTEGER);
  c = lexical_unit_alloc(mpool, SAC_INTEGER);

  list = list_open(mpool);
  list_push_back(list, mpool, a);
  list_push_back(list, mpool, b);
  list_push_back(list, mpool, c);
  value = lexical_unit_from_list(list, mpool);

  assert(value->lexicalUnitType == SAC_SUB_EXPRESSION);
  assert(value->desc.subValues[0] == a);
  assert(value->desc.subValues[1] == b);
  assert(value->desc.subValues[2] == c);
  assert(value->desc.subValues[3] == NULL);

  list_close(list, mpool);

  list = list_open(mpool);
  list_push_back(list, mpool, a);
  value = lexical_unit_from_list(list, mpool);

  assert(value == a);

  mpool_close(mpool);
}



void test_lexical_unit() {
  test_lexical_unit_alloc();
  test_lexical_unit_vector_from_list();
  test_lexical_unit_from_list();
}
