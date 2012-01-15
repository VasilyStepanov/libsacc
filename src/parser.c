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
  SAC_STRING base;
  SAC_StartDocumentHandler start_document_handler;
  SAC_EndDocumentHandler end_document_handler;
  SAC_StartPageHandler start_page_handler;
  SAC_EndPageHandler end_page_handler;
  SAC_NamespaceDeclarationHandler namespace_declaration_handler;
  SAC_ImportHandler import_handler;
  SAC_StartMediaHandler start_media_handler;
  SAC_EndMediaHandler end_media_handler;
  SAC_StartFontFaceHandler start_font_face_handler;
  SAC_EndFontFaceHandler end_font_face_handler;
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



void SAC_parser_start_font_face_handler(SAC_Parser parser) {
  if (PARSER(parser)->start_font_face_handler != NULL)
    PARSER(parser)->start_font_face_handler(PARSER(parser)->user_data);
}



void SAC_parser_end_font_face_handler(SAC_Parser parser) {
  if (PARSER(parser)->end_font_face_handler != NULL)
    PARSER(parser)->end_font_face_handler(PARSER(parser)->user_data);
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



void SAC_parser_start_page_handler(SAC_Parser parser,
  const SAC_STRING name, const SAC_STRING pseudoPage)
{
  if (PARSER(parser)->start_page_handler != NULL)
    PARSER(parser)->start_page_handler(PARSER(parser)->user_data,
      name, pseudoPage);
}



void SAC_parser_end_page_handler(SAC_Parser parser,
  const SAC_STRING name, const SAC_STRING pseudoPage)
{
  if (PARSER(parser)->end_page_handler != NULL)
    PARSER(parser)->end_page_handler(PARSER(parser)->user_data,
      name, pseudoPage);
}



void SAC_parser_namespace_declaration_handler(SAC_Parser parser,
  const SAC_STRING prefix, const SAC_STRING uri)
{
  if (PARSER(parser)->namespace_declaration_handler != NULL)
    PARSER(parser)->namespace_declaration_handler(PARSER(parser)->user_data,
      prefix, uri);
}



void SAC_parser_import_handler(SAC_Parser parser,
  const SAC_STRING uri,
  const SAC_STRING media[],
  const SAC_STRING defaultNamepaceURI)
{
  if (PARSER(parser)->import_handler != NULL)
    PARSER(parser)->import_handler(PARSER(parser)->user_data,
      PARSER(parser)->base, uri, media, defaultNamepaceURI);
}



void SAC_parser_start_media_handler(SAC_Parser parser,
  const SAC_STRING media[])
{
  if (PARSER(parser)->start_media_handler != NULL)
    PARSER(parser)->start_media_handler(PARSER(parser)->user_data, media);
}



void SAC_parser_end_media_handler(SAC_Parser parser,
  const SAC_STRING media[])
{
  if (PARSER(parser)->end_media_handler != NULL)
    PARSER(parser)->end_media_handler(PARSER(parser)->user_data, media);
}



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  PARSER(parser)->start_document_handler = start;
  PARSER(parser)->end_document_handler = end;
}



void SAC_SetNamespaceDeclarationHandler(SAC_Parser parser,
  SAC_NamespaceDeclarationHandler handler)
{
  PARSER(parser)->namespace_declaration_handler = handler;
}



void SAC_SetPageHandler(SAC_Parser parser,
  SAC_StartPageHandler start, SAC_EndPageHandler end)
{
  PARSER(parser)->start_page_handler = start;
  PARSER(parser)->end_page_handler = end;
}



void SAC_SetImportHandler(SAC_Parser parser, SAC_ImportHandler handler) {
  PARSER(parser)->import_handler = handler;
}



void SAC_SetFontFaceHandler(SAC_Parser parser,
  SAC_StartFontFaceHandler start, SAC_EndFontFaceHandler end)
{
  PARSER(parser)->start_font_face_handler = start;
  PARSER(parser)->end_font_face_handler = end;
}



void SAC_SetMediaHandler(SAC_Parser parser,
  SAC_StartMediaHandler start, SAC_EndMediaHandler end)
{
  PARSER(parser)->start_media_handler = start;
  PARSER(parser)->end_media_handler = end;
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
  free(PARSER(parser)->base);
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



int SAC_SetBase(SAC_Parser parser, const SAC_STRING base) {
  free(PARSER(parser)->base);
  PARSER(parser)->base = strdup(base);
  return 1;
}



const SAC_STRING SAC_GetBase(SAC_Parser parser) {
  return PARSER(parser)->base;
}
