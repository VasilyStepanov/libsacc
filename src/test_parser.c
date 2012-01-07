#include "test_parser.h"

#include "test_utils.h"

#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



#define USERDATA_FILE(userData) ((struct userdata_t*)(userData))->file
#define USERDATA_INDENT(userData) ((struct userdata_t*)(userData))->indent

struct userdata_t {
  FILE *file;
  char *indent;
};



void* userdata_open(char **data, size_t *size) {
  void *ret =
    (struct userdata_t*)malloc(sizeof(struct userdata_t));

  USERDATA_FILE(ret) = open_memstream(data, size);
  USERDATA_INDENT(ret) = (char*)malloc(sizeof(char));
  *USERDATA_INDENT(ret) = '\0';

  return ret;
}



void userdata_close(void *userdata) {
  free(USERDATA_INDENT(userdata));
  fclose(USERDATA_FILE(userdata));
  free(userdata);
}



void userdata_inc_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 + 2);
  indent[len + 0] = ' ';
  indent[len + 1] = ' ';
  indent[len + 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



void userdata_dec_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 - 2);
  indent[len - 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



void userdata_printf(void *userdata, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  fprintf(USERDATA_FILE(userdata), "%s", USERDATA_INDENT(userdata));
  vfprintf(USERDATA_FILE(userdata), fmt, args);
}



void start_document(void *userData) {
  userdata_printf(userData, "doc {\n");
  userdata_inc_indent(userData);
}



void end_document(void *userData) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "doc }\n");
}



void dump_lexical_unit(FILE *out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    fprintf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_INHERIT:
        fprintf(out, "inherit");
        break;
      case SAC_URI:
        fprintf(out, "u'%s'", value->desc.uri);
        break;
      case SAC_IDENT:
        fprintf(out, "i'%s'", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        fprintf(out, "s'%s'", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        fprintf(out, "r'%s'", value->desc.unicodeRange);
        break;
      case SAC_OPERATOR_COMMA:
      case SAC_OPERATOR_PLUS:
      case SAC_OPERATOR_MINUS:
      case SAC_OPERATOR_MULTIPLY:
      case SAC_OPERATOR_SLASH:
      case SAC_OPERATOR_MOD:
      case SAC_OPERATOR_EXP:
      case SAC_OPERATOR_LT:
      case SAC_OPERATOR_GT:
      case SAC_OPERATOR_LE:
      case SAC_OPERATOR_GE:
      case SAC_OPERATOR_TILDE:
      case SAC_INTEGER:
      case SAC_REAL:
      case SAC_LENGTH_EM:
      case SAC_LENGTH_EX:
      case SAC_LENGTH_PIXEL:
      case SAC_LENGTH_INCH:
      case SAC_LENGTH_CENTIMETER:
      case SAC_LENGTH_MILLIMETER:
      case SAC_LENGTH_POINT:
      case SAC_LENGTH_PICA:
      case SAC_PERCENTAGE:
      case SAC_COUNTER_FUNCTION:
      case SAC_COUNTERS_FUNCTION:
      case SAC_RGBCOLOR:
      case SAC_DEGREE:
      case SAC_GRADIAN:
      case SAC_RADIAN:
      case SAC_MILLISECOND:
      case SAC_SECOND:
      case SAC_HERTZ:
      case SAC_KILOHERTZ:
      case SAC_ATTR:
      case SAC_RECT_FUNCTION:
      case SAC_SUB_EXPRESSION:
      case SAC_FUNCTION:
      case SAC_DIMENSION:
        break;
    };
  }
}



void property(
  void *userData,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  userdata_printf(userData, "prp '%s' ", propertyName);
  dump_lexical_unit(USERDATA_FILE(userData), value);
  if (important == SAC_TRUE) fprintf(USERDATA_FILE(userData), " important");
  fprintf(USERDATA_FILE(userData), "\n");
}



SAC_Parser create_parser(char **data, size_t *size) {
  struct userdata_t *userData;
  SAC_Parser parser;
  
  userData = userdata_open(data, size);

  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, userData);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  userdata_close(SAC_GetUserData(parser));
  SAC_DisposeParser(parser);
}



void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void test_parser_basics() {
  char *data;
  size_t size;
  SAC_Parser parser = create_parser(&data, &size);

  parse_stylesheet(parser,
"selector {\n"
"  property-ident : ident;\n"
"  property-inherit : inherit;\n"
"  property-string1 : 'string';\n"
"  property-string2 : \"string\";\n"
"  property-uri1 : url( \t\r\n\f\"uri\"\f\n\r\t );\n"
"  property-uri2 : url( \t\r\n\fhttp://example.com/\f\n\r\t );\n"
"  property-unicode1 : U+A5;\n"
"  property-unicode2 : U+0-7F;\n"
"  property-unicode3 : U+590-5ff;\n"
"  property-unicode4 : U+4E00-9FFF;\n"
"  property-unicode5 : U+30??;\n"
"}\n"
  );

  dispose_parser(parser);

  assert_equals(
"doc {\n"
"  prp 'property-ident' i'ident'\n"
"  prp 'property-inherit' inherit\n"
"  prp 'property-string1' s'string'\n"
"  prp 'property-string2' s'string'\n"
"  prp 'property-uri1' u'uri'\n"
"  prp 'property-uri2' u'http://example.com/'\n"
"  prp 'property-unicode1' r'U+A5'\n"
"  prp 'property-unicode2' r'U+0-7F'\n"
"  prp 'property-unicode3' r'U+590-5ff'\n"
"  prp 'property-unicode4' r'U+4E00-9FFF'\n"
"  prp 'property-unicode5' r'U+30\?\?'\n"
"doc }\n",
  data);

  free(data);
}



void test_parser() {
  test_parser_basics();
}
