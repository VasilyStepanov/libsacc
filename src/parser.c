#include <sacc.h>

#include "parser.h"
#include "grammar.h"

#include <stdlib.h>
#include <string.h>

extern int yylex_init_extra();
extern int yy_scan_bytes();
extern void yylex_destroy();

extern void yyparse();



#define PARSER(parser) ((struct _SAC_Parser*)(parser))

struct _SAC_Parser {
  SAC_MPool mpool;
  SAC_StartDocumentHandler start_document_handler;
  SAC_EndDocumentHandler end_document_handler;
  SAC_StartStyleHandler start_style_handler;
  SAC_EndStyleHandler end_style_handler;
  SAC_PropertyHandler property_handler;
  void *user_data;
};



void SAC_parser_start_document(SAC_Parser parser) {
  if (PARSER(parser)->start_document_handler != NULL)
    PARSER(parser)->start_document_handler(PARSER(parser)->user_data);
}



void SAC_parser_end_document(SAC_Parser parser) {
  if (PARSER(parser)->end_document_handler != NULL)
    PARSER(parser)->end_document_handler(PARSER(parser)->user_data);
}



void SAC_parser_start_style_handler(
  SAC_Parser parser,
  const SAC_Selector *selectors[])
{
  if (PARSER(parser)->start_style_handler != NULL)
    PARSER(parser)->start_style_handler(PARSER(parser)->user_data, selectors);
}



void SAC_parser_end_style_handler(
  SAC_Parser parser,
  const SAC_Selector *selectors[])
{
  if (PARSER(parser)->end_style_handler != NULL)
    PARSER(parser)->end_style_handler(PARSER(parser)->user_data, selectors);
}



void SAC_parser_property_handler(
  SAC_Parser parser,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  if (PARSER(parser)->property_handler != NULL)
    PARSER(parser)->property_handler(PARSER(parser)->user_data,
      propertyName, value, important);
}



static void SAC_parser_clear(SAC_Parser parser) {
  SAC_mpool_close(PARSER(parser)->mpool);
  PARSER(parser)->mpool = SAC_mpool_open(16384);
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
  struct _SAC_Parser *ret = (struct _SAC_Parser*)malloc(
    sizeof(struct _SAC_Parser));
  memset(ret, 0, sizeof(struct _SAC_Parser));
  return ret;
}



void SAC_DisposeParser(SAC_Parser parser) {
  SAC_mpool_close(PARSER(parser)->mpool);
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;
  SAC_YYExtra yy_extra;

  SAC_parser_clear(parser);
  yy_extra.mpool = PARSER(parser)->mpool;
  yy_extra.parser = parser;
  yy_extra.start_token = START_AS_STYLESHEET;
  yy_extra.output = NULL;

  yylex_init_extra(&yy_extra, &scanner);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  yylex_destroy(scanner);
  return 0;
}



int SAC_ParseStyleDeclaration(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;
  SAC_YYExtra yy_extra;

  SAC_parser_clear(parser);
  yy_extra.mpool = PARSER(parser)->mpool;
  yy_extra.parser = parser;
  yy_extra.start_token = START_AS_STYLE_DECLARATIONS;
  yy_extra.output = NULL;

  yylex_init_extra(&yy_extra, &scanner);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  yylex_destroy(scanner);
  return 0;
}



const SAC_Selector** SAC_ParseSelectors(SAC_Parser parser,
  const char *buffer, int len)
{
  void *scanner;
  SAC_YYExtra yy_extra;

  SAC_parser_clear(parser);
  yy_extra.mpool = PARSER(parser)->mpool;
  yy_extra.parser = parser;
  yy_extra.start_token = START_AS_SELECTORS;
  yy_extra.output = NULL;

  yylex_init_extra(&yy_extra, &scanner);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  yylex_destroy(scanner);
  return (const SAC_Selector**)yy_extra.output;
}



int SAC_ParseRule(SAC_Parser parser, const char *buffer, int len) {
  void *scanner;
  SAC_YYExtra yy_extra;

  SAC_parser_clear(parser);
  yy_extra.mpool = PARSER(parser)->mpool;
  yy_extra.parser = parser;
  yy_extra.start_token = START_AS_RULE;
  yy_extra.output = NULL;

  yylex_init_extra(&yy_extra, &scanner);

  yy_scan_bytes(buffer, len, scanner);
  yyparse(scanner);

  yylex_destroy(scanner);
  return 0;
}



void SAC_parser_style_unit_handler(SAC_Parser parser, SAC_StyleUnit *unit) {
  SAC_ListIter lit;

  switch (unit->type) {
    case SAC_RULESET:
      SAC_parser_start_style_handler(parser, unit->desc.ruleset.selectors);
      for (lit = SAC_list_head(unit->desc.ruleset.declarations);
           lit != NULL;
           lit = SAC_list_next(lit))
      {
        SAC_Declaration *declaration = *lit;
        SAC_parser_property_handler(parser,
          declaration->property, declaration->value, declaration->important);
      }
      SAC_parser_end_style_handler(parser, unit->desc.ruleset.selectors);
    break;
  }
}
