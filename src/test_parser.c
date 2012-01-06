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



void property(
  void *userData,
  const SAC_STRING propertyName,
  SAC_Boolean important)
{
  if (important == SAC_FALSE) {
    userdata_printf(userData, "prp '%s'\n", propertyName);
  } else {
    userdata_printf(userData, "prp '%s' important\n", propertyName);
  }
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
"  property : value;\n"
"}\n"
  );

  dispose_parser(parser);

  assert_equals(
"doc {\n"
"  prp 'property'\n"
"doc }\n",
  data);

  free(data);
}



void test_parser() {
  test_parser_basics();
}
