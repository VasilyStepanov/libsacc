#include "test_lexical_unit.h"

#include "lexical_unit.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_lexical_unit_alloc() {
  SAC_MPool mpool;
  SAC_LexicalUnit *value;

  mpool = SAC_mpool_open(256);
  value = SAC_lexical_unit_alloc(mpool, SAC_STRING_VALUE);

  assert(value->lexicalUnitType == SAC_STRING_VALUE);

  SAC_mpool_close(mpool);
}



static void test_lexical_unit_vector_from_list() {
  SAC_MPool mpool;
  SAC_LexicalUnit *a, *b, *c;
  SAC_List list;
  SAC_LexicalUnit **vector;

  mpool = SAC_mpool_open(256);

  a = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);
  b = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);
  c = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);

  list = SAC_list_open(mpool);
  SAC_list_push_back(list, mpool, a);
  SAC_list_push_back(list, mpool, b);
  SAC_list_push_back(list, mpool, c);
  vector = SAC_lexical_unit_vector_from_list(list, mpool);

  assert(vector[0] == a);
  assert(vector[1] == b);
  assert(vector[2] == c);
  assert(vector[3] == NULL);

  SAC_list_close(list, mpool);

  list = SAC_list_open(mpool);
  SAC_list_push_back(list, mpool, a);
  vector = SAC_lexical_unit_vector_from_list(list, mpool);

  assert(vector[0] == a);
  assert(vector[1] == NULL);

  SAC_mpool_close(mpool);
}



static void test_lexical_unit_from_list() {
  SAC_MPool mpool;
  SAC_LexicalUnit *a, *b, *c;
  SAC_List list;
  SAC_LexicalUnit *value;

  mpool = SAC_mpool_open(256);

  a = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);
  b = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);
  c = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);

  list = SAC_list_open(mpool);
  SAC_list_push_back(list, mpool, a);
  SAC_list_push_back(list, mpool, b);
  SAC_list_push_back(list, mpool, c);
  value = SAC_lexical_unit_from_list(list, mpool);

  assert(value->lexicalUnitType == SAC_SUB_EXPRESSION);
  assert(value->desc.subValues[0] == a);
  assert(value->desc.subValues[1] == b);
  assert(value->desc.subValues[2] == c);
  assert(value->desc.subValues[3] == NULL);

  SAC_list_close(list, mpool);

  list = SAC_list_open(mpool);
  SAC_list_push_back(list, mpool, a);
  value = SAC_lexical_unit_from_list(list, mpool);

  assert(value == a);

  SAC_mpool_close(mpool);
}



void test_lexical_unit() {
  test_lexical_unit_alloc();
  test_lexical_unit_vector_from_list();
  test_lexical_unit_from_list();
}
