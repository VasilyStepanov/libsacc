#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



void debug_startDocument(void *userData) {
  fprintf((FILE*)userData, "doc {\n");
}



void debug_endDocument(void *userData) {
  fprintf((FILE*)userData, "doc }\n");
}



SAC_Parser createParser(char **data, size_t *size) {
  FILE *f = open_memstream(data, size);
  SAC_Parser parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, debug_startDocument, debug_endDocument);
  SAC_SetUserData(parser, f);
  return parser;
}



void disposeParser(SAC_Parser parser) {
  FILE *f = SAC_GetUserData(parser);

  fclose(f);
  SAC_DisposeParser(parser);
}



void assertEquals(const char *lhs, const char *rhs) {
  int eq = strcmp(lhs, rhs);
  if (eq != 0) {
    printf(
"Expected: %s\n"
"Received: %s\n",
    lhs, rhs);
  }
  assert(eq == 0);
}



void parseStyleSheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void test_basics() {
  char *data;
  size_t size;
  SAC_Parser parser = createParser(&data, &size);

  parseStyleSheet(parser,
"selector {\n"
"  property : value;\n"
"}\n"
  );

  disposeParser(parser);

  assertEquals(
"doc {\n"
"doc }\n",
  data);

  free(data);
}



int main() {
  test_basics();
  return EXIT_SUCCESS;
}
