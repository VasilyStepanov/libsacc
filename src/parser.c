#include "parser.h"

#include "grammar.h"

#include <sacc.h>
#include <stdlib.h>
#include <string.h>

extern int yylex_init();
extern int yy_scan_bytes();
extern void yylex_destroy();

extern void yyparse();



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  PARSER(parser)->start_document_handler = start;
  PARSER(parser)->end_document_handler = end;
}



void SAC_SetUserData(SAC_Parser parser, void *userData) {
  PARSER(parser)->user_data = userData;
}



void* SAC_GetUserData(SAC_Parser parser) {
  return PARSER(parser)->user_data;
}



SAC_Parser SAC_CreateParser() {
  SAC_Parser ret = malloc(sizeof(struct parser_t));
  memset(ret, 0, sizeof(struct parser_t));
  return ret;
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;

  yylex_init(&scanner);

  if (PARSER(parser)->start_document_handler != NULL)
    PARSER(parser)->start_document_handler(PARSER(parser)->user_data);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  if (PARSER(parser)->end_document_handler != NULL)
    PARSER(parser)->end_document_handler(PARSER(parser)->user_data);

  yylex_destroy(scanner);
  return 0;
}
