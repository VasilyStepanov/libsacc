#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



void debug_start_document(void *userData) {
  fprintf((FILE*)userData, "doc {\n");
}



void debug_end_document(void *userData) {
  fprintf((FILE*)userData, "doc }\n");
}



SAC_Parser create_parser(char **data, size_t *size) {
  FILE *f = open_memstream(data, size);
  SAC_Parser parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, debug_start_document, debug_end_document);
  SAC_SetUserData(parser, f);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  FILE *f = SAC_GetUserData(parser);

  fclose(f);
  SAC_DisposeParser(parser);
}



void assert_equals(const char *lhs, const char *rhs) {
  int eq = strcmp(lhs, rhs);
  if (eq != 0) {
    printf(
"Expected: %s\n"
"Received: %s\n",
    lhs, rhs);
  }
  assert(eq == 0);
}



void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void test_basics() {
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
"doc }\n",
  data);

  free(data);
}



int main() {
  test_basics();
  return EXIT_SUCCESS;
}
