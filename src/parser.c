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
  mpool_t mpool;
  SAC_StartDocumentHandler start_document_handler;
  SAC_EndDocumentHandler end_document_handler;
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




mpool_t parser_mpool(SAC_Parser parser) {
  return PARSER(parser)->mpool;
}



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
  struct parser_s *ret = (struct parser_s*)malloc(sizeof(struct parser_s));
  memset(ret, 0, sizeof(struct parser_s));
  return ret;
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;

  PARSER(parser)->mpool = mpool_open(16384);
  yylex_init_extra(parser, &scanner);

  parser_start_document(parser);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  parser_end_document(parser);

  yylex_destroy(scanner);
  mpool_close(PARSER(parser)->mpool);
  return 0;
}
