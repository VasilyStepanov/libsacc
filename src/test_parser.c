#include "test_parser.h"

#include "test_utils.h"

#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void start_document(void *userData) {
  fprintf((FILE*)userData, "doc {\n");
}



void end_document(void *userData) {
  fprintf((FILE*)userData, "doc }\n");
}



void property(
  void *userData,
  const SAC_STRING propertyName,
  SAC_Boolean important)
{
  if (important == SAC_FALSE) {
    fprintf((FILE*)userData, "prp '%s'\n", propertyName);
  } else {
    fprintf((FILE*)userData, "prp '%s' important\n", propertyName);
  }
}



SAC_Parser create_parser(char **data, size_t *size) {
  FILE *f = open_memstream(data, size);
  SAC_Parser parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, f);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  FILE *f = SAC_GetUserData(parser);

  fclose(f);
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
"prp 'property'\n"
"doc }\n",
  data);

  free(data);
}



void test_parser() {
  test_parser_basics();
}
