#include <sacc.h>

#include "parser.h"
#include "grammar.h"
#include "mpool.h"

#include <stdlib.h>
#include <string.h>

extern int yylex_init_extra();
extern int yy_scan_bytes();
extern void yylex_destroy();

extern void yyparse();



#define PARSER(parser) ((struct parser_s*)(parser))

struct parser_s {
  SAC_StartDocumentHandler start_document_handler;
  SAC_EndDocumentHandler end_document_handler;
  SAC_StartStyleHandler start_style_handler;
  SAC_EndStyleHandler end_style_handler;
  SAC_PropertyHandler property_handler;
  void *user_data;
};



void parser_start_document(SAC_Parser parser) {
  if (PARSER(parser)->start_document_handler != NULL)
    PARSER(parser)->start_document_handler(PARSER(parser)->user_data);
}



void parser_end_document(SAC_Parser parser) {
  if (PARSER(parser)->end_document_handler != NULL)
    PARSER(parser)->end_document_handler(PARSER(parser)->user_data);
}



void parser_property_handler(
  SAC_Parser parser,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  if (PARSER(parser)->property_handler != NULL)
    PARSER(parser)->property_handler(PARSER(parser)->user_data,
      propertyName, value, important);
}



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  PARSER(parser)->start_document_handler = start;
  PARSER(parser)->end_document_handler = end;
}



void SAC_SetStyleHandler(SAC_Parser parser,
  SAC_StartStyleHandler start,
  SAC_EndStyleHandler end)
{
  PARSER(parser)->start_style_handler = start;
  PARSER(parser)->end_style_handler = end;
}



void SAC_SetPropertyHandler(SAC_Parser parser, SAC_PropertyHandler handler) {
  PARSER(parser)->property_handler = handler;
}



void SAC_SetUserData(SAC_Parser parser, void *userData) {
  PARSER(parser)->user_data = userData;
}



void* SAC_GetUserData(SAC_Parser parser) {
  return PARSER(parser)->user_data;
}



SAC_Parser SAC_CreateParser() {
  struct parser_s *ret = (struct parser_s*)malloc(sizeof(struct parser_s));
  memset(ret, 0, sizeof(struct parser_s));
  return ret;
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;
  struct yy_extra_t yy_extra;

  yy_extra.mpool = mpool_open(16384);
  yy_extra.parser = parser;

  yylex_init_extra(&yy_extra, &scanner);

  parser_start_document(parser);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  parser_end_document(parser);

  yylex_destroy(scanner);
  mpool_close(yy_extra.mpool);
  return 0;
}
