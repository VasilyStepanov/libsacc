#include "lexical_unit.h"

#include "vector_extra.h"

#include "checks.h"



static SAC_LexicalUnit* SAC_lexical_unit_alloc(SAC_MPool mpool,
  SAC_LexicalUnitCode type)
{
  SAC_LexicalUnit *value = (SAC_LexicalUnit*)SAC_mpool_alloc(
    mpool, sizeof(SAC_LexicalUnit)
  );

  if (value == NULL) return NULL;

  value->lexicalUnitType = type;
  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_from_list(SAC_List list, SAC_MPool mpool) {
  SAC_LexicalUnit *value;

  if (SAC_list_size(list) == 1) {
    value = *SAC_list_head(list);
  } else {
    SAC_Vector vector;

    value = SAC_lexical_unit_alloc(mpool, SAC_SUB_EXPRESSION);

    if (value == NULL) return NULL;

    vector = SAC_vector_from_list(list, mpool);

    if (vector == NULL) return NULL;

    value->desc.subValues = vector;
  }

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_int(SAC_MPool mpool, int integer) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_INTEGER);
  if (value == NULL) return value;

  value->desc.integer = integer;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_real(SAC_MPool mpool, double real) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_REAL);
  if (value == NULL) return value;

  value->desc.real = real;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_percentage(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_PERCENTAGE);
  if (value == NULL) return value;

  value->desc.dimension.unit = "%";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_pixel(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_PIXEL);
  if (value == NULL) return value;

  value->desc.dimension.unit = "px";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_inch(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_INCH);
  if (value == NULL) return value;

  value->desc.dimension.unit = "in";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_centimeter(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_CENTIMETER);
  if (value == NULL) return value;

  value->desc.dimension.unit = "cm";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_millimeter(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_MILLIMETER);
  if (value == NULL) return value;

  value->desc.dimension.unit = "mm";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_point(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_POINT);
  if (value == NULL) return value;

  value->desc.dimension.unit = "pt";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_pica(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_PICA);
  if (value == NULL) return value;

  value->desc.dimension.unit = "pc";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_em(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_EM);
  if (value == NULL) return value;

  value->desc.dimension.unit = "em";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_ex(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_LENGTH_EX);
  if (value == NULL) return value;

  value->desc.dimension.unit = "ex";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_degree(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_DEGREE);
  if (value == NULL) return value;

  value->desc.dimension.unit = "deg";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_radian(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_RADIAN);
  if (value == NULL) return value;

  value->desc.dimension.unit = "rad";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_gradian(SAC_MPool mpool, double sreal) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_GRADIAN);
  if (value == NULL) return value;

  value->desc.dimension.unit = "grad";
  value->desc.dimension.value.sreal = sreal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_millisecond(SAC_MPool mpool, double ureal) {
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_MILLISECOND);
  if (value == NULL) return value;

  value->desc.dimension.unit = "ms";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_second(SAC_MPool mpool, double ureal) {
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_SECOND);
  if (value == NULL) return value;

  value->desc.dimension.unit = "s";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_hertz(SAC_MPool mpool, double ureal) {
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_HERTZ);
  if (value == NULL) return value;

  value->desc.dimension.unit = "Hz";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_kilohertz(SAC_MPool mpool, double ureal) {
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_KILOHERTZ);
  if (value == NULL) return value;

  value->desc.dimension.unit = "kHz";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_dots_per_inch(SAC_MPool mpool, double ureal) {
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_DOTS_PER_INCH);
  if (value == NULL) return value;

  value->desc.dimension.unit = "dpi";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_dots_per_centimeter(SAC_MPool mpool,
  double ureal)
{
  SAC_LexicalUnit *value;

  SAC_CHECK_UNSIGNED(ureal);

  value = SAC_lexical_unit_alloc(mpool, SAC_DOTS_PER_CENTIMETER);
  if (value == NULL) return value;

  value->desc.dimension.unit = "dpcm";
  value->desc.dimension.value.ureal = ureal;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_string(SAC_MPool mpool, SAC_STRING string) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_STRING_VALUE);
  if (value == NULL) return value;

  value->desc.stringValue = string;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_ident(SAC_MPool mpool, SAC_STRING ident) {
  SAC_LexicalUnit *value;

  SAC_CHECK_STRING_NOT_EQUALS(ident, "inherit");

  value = SAC_lexical_unit_alloc(mpool, SAC_IDENT);
  if (value == NULL) return value;

  value->desc.ident = ident;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_inherit(SAC_MPool mpool) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_INHERIT);
  if (value == NULL) return value;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_uri(SAC_MPool mpool, SAC_STRING uri) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_URI);
  if (value == NULL) return value;

  value->desc.uri = uri;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_unicode_range(SAC_MPool mpool,
  SAC_STRING unicode_range)
{
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_UNICODERANGE);
  if (value == NULL) return value;

  value->desc.unicodeRange = unicode_range;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_operator_slash(SAC_MPool mpool) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_OPERATOR_SLASH);
  if (value == NULL) return value;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_operator_comma(SAC_MPool mpool) {
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_OPERATOR_COMMA);
  if (value == NULL) return value;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_function(SAC_MPool mpool,
  SAC_STRING name, SAC_LexicalUnit **parameters)
{
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_FUNCTION);
  if (value == NULL) return value;

  value->desc.function.name = name;
  value->desc.function.parameters = parameters;

  return value;
}



SAC_LexicalUnit* SAC_lexical_unit_rgbcolor(SAC_MPool mpool,
  SAC_LexicalUnit **parameters)
{
  SAC_LexicalUnit *value;

  value = SAC_lexical_unit_alloc(mpool, SAC_RGBCOLOR);
  if (value == NULL) return value;

  value->desc.function.name = "rgb";
  value->desc.function.parameters = parameters;

  return value;
}
