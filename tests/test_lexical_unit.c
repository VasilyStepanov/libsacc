#include "test_lexical_unit.h"

#include "lexical_unit.h"
#include "mpool.h"
#include "test_utils.h"

#include <sacc.h>

#include <assert.h>
#include <math.h>



static void test_lexical_unit_alloc() {
  SAC_MPool mpool;
  SAC_LexicalUnit *value;

  mpool = SAC_mpool_open(256);
  value = SAC_lexical_unit_alloc(mpool, SAC_STRING_VALUE);

  assert(value->lexicalUnitType == SAC_STRING_VALUE);

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



static void test_lexical_unit_factory() {
  SAC_MPool mpool;
  SAC_LexicalUnit *value;

  mpool = SAC_mpool_open(256);

  value = SAC_lexical_unit_int(mpool, 10);
  assert(value->lexicalUnitType == SAC_INTEGER);
  assert(value->desc.integer == 10);

  value = SAC_lexical_unit_real(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_REAL);
  ASSERT_EQUAL_REALS(3.14, value->desc.real);

  value = SAC_lexical_unit_percentage(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_PERCENTAGE);
  ASSERT_EQUAL_STRINGS("%", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_pixel(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_PIXEL);
  ASSERT_EQUAL_STRINGS("px", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_inch(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_INCH);
  ASSERT_EQUAL_STRINGS("in", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_centimeter(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_CENTIMETER);
  ASSERT_EQUAL_STRINGS("cm", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_millimeter(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_MILLIMETER);
  ASSERT_EQUAL_STRINGS("mm", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_point(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_POINT);
  ASSERT_EQUAL_STRINGS("pt", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_pica(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_PICA);
  ASSERT_EQUAL_STRINGS("pc", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_em(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_EM);
  ASSERT_EQUAL_STRINGS("em", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_ex(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_LENGTH_EX);
  ASSERT_EQUAL_STRINGS("ex", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_degree(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_DEGREE);
  ASSERT_EQUAL_STRINGS("deg", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_radian(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_RADIAN);
  ASSERT_EQUAL_STRINGS("rad", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_gradian(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_GRADIAN);
  ASSERT_EQUAL_STRINGS("grad", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.sreal);

  value = SAC_lexical_unit_millisecond(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_MILLISECOND);
  ASSERT_EQUAL_STRINGS("ms", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  value = SAC_lexical_unit_second(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_SECOND);
  ASSERT_EQUAL_STRINGS("s", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  value = SAC_lexical_unit_hertz(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_HERTZ);
  ASSERT_EQUAL_STRINGS("Hz", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  value = SAC_lexical_unit_kilohertz(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_KILOHERTZ);
  ASSERT_EQUAL_STRINGS("kHz", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  value = SAC_lexical_unit_dots_per_inch(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_DOTS_PER_INCH);
  ASSERT_EQUAL_STRINGS("dpi", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  value = SAC_lexical_unit_dots_per_centimeter(mpool, 3.14);
  assert(value->lexicalUnitType == SAC_DOTS_PER_CENTIMETER);
  ASSERT_EQUAL_STRINGS("dpcm", value->desc.dimension.unit);
  ASSERT_EQUAL_REALS(3.14, value->desc.dimension.value.ureal);

  SAC_mpool_close(mpool);
}



void test_lexical_unit() {
  test_lexical_unit_alloc();
  test_lexical_unit_from_list();
  test_lexical_unit_factory();
}
