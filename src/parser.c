#include <sacc.h>

#include "parser.h"
#include "grammar.h"

#include <stdlib.h>
#include <string.h>

extern int yy_scan_bytes();
extern void yylex_destroy();

extern void yyparse();



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  PARSER(parser)->startDocumentHandler = start;
  PARSER(parser)->endDocumentHandler = end;
}



void SAC_SetUserData(SAC_Parser parser, void *userData) {
  PARSER(parser)->userData = userData;
}



void* SAC_GetUserData(SAC_Parser parser) {
  return PARSER(parser)->userData;
}



SAC_Parser SAC_CreateParser() {
  SAC_Parser ret = malloc(sizeof(SAC_ParserImpl));
  memset(ret, 0, sizeof(SAC_ParserImpl));
  return ret;
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  if (PARSER(parser)->startDocumentHandler != NULL)
    PARSER(parser)->startDocumentHandler(PARSER(parser)->userData);

  yy_scan_bytes(buffer, len);
  yyparse();

  if (PARSER(parser)->endDocumentHandler != NULL)
    PARSER(parser)->endDocumentHandler(PARSER(parser)->userData);

  yylex_destroy();
  return 0;
}
