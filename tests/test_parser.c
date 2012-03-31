#include "test_parser.h"

#include "src/gcc.h"

#include <sacc.h>

#include <string.h>
#include <assert.h>



static int property(void *userData SAC_UNUSED,
  const SAC_STRING propertyName SAC_UNUSED,
  const SAC_LexicalUnit *value SAC_UNUSED,
  SAC_Boolean important SAC_UNUSED)
{
  return 100;
}



static void test_parser_errno(void) {
  const char *css;
  SAC_Parser parser;
  int errno;

  css = "p { color: green }";
  parser = SAC_CreateParser();
  SAC_SetPropertyHandler(parser, property);


  errno = SAC_ParseStyleSheet(parser, css, strlen(css));
  assert(errno == 100);  

  SAC_DisposeParser(parser);
}



void test_parser(void) {
  test_parser_errno();
}
