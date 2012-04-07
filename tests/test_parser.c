#include "test_parser.h"

#include "src/gcc.h"

#include <sacc.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>



static int property_errno(void *userData SAC_UNUSED,
  const SAC_STRING propertyName SAC_UNUSED,
  const SAC_LexicalUnit *value SAC_UNUSED,
  SAC_Boolean important SAC_UNUSED)
{
  return 100;
}



static int property_break(void *userData,
  const SAC_STRING propertyName SAC_UNUSED,
  const SAC_LexicalUnit *value SAC_UNUSED,
  SAC_Boolean important SAC_UNUSED)
{
  size_t *count = (size_t*)userData;
  ++(*count);
  return 0;
}



static void test_parser_errno(void) {
  const char *css;
  SAC_Parser parser;
  int errno;

  css = "p { color: green }";
  parser = SAC_CreateParser();
  SAC_SetPropertyHandler(parser, property_errno);


  errno = SAC_ParseStyleSheet(parser, css, strlen(css));
  assert(errno == 100);  

  SAC_DisposeParser(parser);
}



static int error_break(void *userData SAC_UNUSED,
  const SAC_Error *error SAC_UNUSED)
{
  return 200;
}



static void test_parser_break(void) {
  const char *css;
  SAC_Parser parser;
  size_t count;
  int errno;

  css =
"p { color: green }\n"
"{error}\n"
"p { color: red }\n"
  ;
  count = 0;
  parser = SAC_CreateParser();
  SAC_SetUserData(parser, &count);
  SAC_SetErrorHandler(parser, error_break);
  SAC_SetPropertyHandler(parser, property_break);


  errno = SAC_ParseStyleSheet(parser, css, strlen(css));
  assert(count == 1);
  assert(errno == 200);  

  SAC_DisposeParser(parser);
}



void test_parser(void) {
  test_parser_errno();
  test_parser_break();
}
