#include <sacc.h>

#include "parser.h"
#include "grammar.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gcc.h>

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
  SAC_StartPageMarginHandler start_page_margin_handler;
  SAC_EndPageMarginHandler end_page_margin_handler;
  SAC_IgnorableAtRuleHandler ignorable_at_rule_handler;
  SAC_NamespaceDeclarationHandler namespace_declaration_handler;
  SAC_ImportHandler import_handler;
  SAC_StartMediaHandler start_media_handler;
  SAC_EndMediaHandler end_media_handler;
  SAC_StartFontFaceHandler start_font_face_handler;
  SAC_EndFontFaceHandler end_font_face_handler;
  SAC_StartStyleHandler start_style_handler;
  SAC_EndStyleHandler end_style_handler;
  SAC_PropertyHandler property_handler;
  SAC_FatalError fatal_error;
  SAC_FatalErrorHandler fatal_error_handler;
  SAC_ErrorHandler error_handler;
  void *user_data;
  int errno;
};



static void SAC_parser_print_error(
  signed int line, signed int column, const char *msg, const char *data)
{
  if (line != -1) {
    if (column != -1) {
      if  (data != NULL) {
        fprintf(stderr, "sacc:%d:%d: %s: %s\n",
          line + 1, column + 1, msg, data);
      } else {
        fprintf(stderr, "sacc:%d:%d: %s\n",
          line + 1, column + 1, msg);
      }
    } else {
      if  (data != NULL) {
        fprintf(stderr, "sacc:%d: %s: %s\n",
          line + 1, msg, data);
      } else {
        fprintf(stderr, "sacc:%d: %s\n",
          line + 1, msg);
      }
    }
  }
}



static void SAC_default_fatal_error_handler(void *userData SAC_UNUSED,
  const SAC_FatalError *error)
{
  switch (error->code) {
    case SAC_FATAL_ERROR_NO_MEMORY:
      SAC_parser_print_error(error->line, error->column, "out of memory", NULL);
      break;
  }
}



static void SAC_default_error_handler(void *userData SAC_UNUSED,
  const SAC_Error *error)
{
  switch (error->code) {
    case SAC_ERROR_NOT_SUPPORTED:
      SAC_parser_print_error(error->line, error->column,
        "unspecified error", error->data);
      break;
    case SAC_ERROR_SYNTAX:
      SAC_parser_print_error(error->line, error->column,
        "syntax error", error->data);
      break;
  }
}



int SAC_parser_start_document(SAC_Parser parser) {
  if (PARSER(parser)->start_document_handler == NULL) return 0;
  return PARSER(parser)->start_document_handler(PARSER(parser)->user_data);
}



int SAC_parser_end_document(SAC_Parser parser) {
  if (PARSER(parser)->end_document_handler == NULL) return 0;
  return PARSER(parser)->end_document_handler(PARSER(parser)->user_data);
}



int SAC_parser_start_style_handler(SAC_Parser parser,
  const SAC_Selector *selectors[])
{
  if (PARSER(parser)->start_style_handler == NULL) return 0;
  return PARSER(parser)->start_style_handler(PARSER(parser)->user_data,
    selectors);
}



int SAC_parser_end_style_handler(SAC_Parser parser,
  const SAC_Selector *selectors[])
{
  if (PARSER(parser)->end_style_handler == NULL) return 0;
  return PARSER(parser)->end_style_handler(PARSER(parser)->user_data,
    selectors);
}



int SAC_parser_property_handler(SAC_Parser parser,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  if (PARSER(parser)->property_handler == NULL) return 0;
  return PARSER(parser)->property_handler(PARSER(parser)->user_data,
    propertyName, value, important);
}



void SAC_parser_fatal_error_handler(SAC_Parser parser,
  signed int line, signed int column, SAC_FatalErrorCode code)
{
  PARSER(parser)->fatal_error.line = line - 1;
  PARSER(parser)->fatal_error.column = column - 1;
  PARSER(parser)->fatal_error.code = code;
  PARSER(parser)->errno = code;
  PARSER(parser)->fatal_error_handler(PARSER(parser)->user_data,
    &PARSER(parser)->fatal_error);
}



void SAC_parser_error_handler(SAC_Parser parser,
  signed int line, signed int column,
  SAC_ErrorCode code, const SAC_STRING data)
{
  SAC_Error error;
  
  error.line = line - 1;
  error.column = column - 1;
  error.code = code;
  error.data = data;
  PARSER(parser)->errno = code;
  PARSER(parser)->error_handler(PARSER(parser)->user_data, &error);
}



static SAC_Boolean SAC_parser_clear(SAC_Parser parser) {
  SAC_mpool_close(PARSER(parser)->mpool);
  PARSER(parser)->mpool = SAC_mpool_open(16384);
  PARSER(parser)->errno = 0;
  if (PARSER(parser)->mpool == NULL) {
    SAC_parser_fatal_error_handler(parser, 0, 0, SAC_FATAL_ERROR_NO_MEMORY);
    return SAC_FALSE;
  }
  return SAC_TRUE;
}



int SAC_parser_ignorable_at_rule_handler(SAC_Parser parser,
  const SAC_STRING atRule)
{
  if (PARSER(parser)->ignorable_at_rule_handler == NULL) return 0;
  return PARSER(parser)->ignorable_at_rule_handler(PARSER(parser)->user_data,
    atRule);
}




int SAC_parser_namespace_declaration_handler(SAC_Parser parser,
  const SAC_STRING prefix, const SAC_STRING uri)
{
  if (PARSER(parser)->namespace_declaration_handler == NULL) return 0;
  return PARSER(parser)->namespace_declaration_handler(
    PARSER(parser)->user_data, prefix, uri);
}



int SAC_parser_import_handler(SAC_Parser parser,
  const SAC_STRING uri,
  const SAC_MediaQuery *media[],
  const SAC_STRING defaultNamepaceURI)
{
  if (PARSER(parser)->import_handler == NULL) return 0;
  return PARSER(parser)->import_handler(PARSER(parser)->user_data,
      PARSER(parser)->base, uri, media, defaultNamepaceURI);
}



int SAC_parser_start_media_handler(SAC_Parser parser,
  const SAC_MediaQuery *media[])
{
  if (PARSER(parser)->start_media_handler == NULL) return 0;
  return PARSER(parser)->start_media_handler(PARSER(parser)->user_data, media);
}



int SAC_parser_end_media_handler(SAC_Parser parser,
  const SAC_MediaQuery *media[])
{
  if (PARSER(parser)->end_media_handler == NULL) return 0;
  return PARSER(parser)->end_media_handler(PARSER(parser)->user_data, media);
}



int SAC_parser_start_page_handler(SAC_Parser parser,
  const SAC_STRING name, const SAC_STRING pseudoPage)
{
  if (PARSER(parser)->start_page_handler == NULL) return 0;
  return PARSER(parser)->start_page_handler(PARSER(parser)->user_data,
      name, pseudoPage);
}



int SAC_parser_end_page_handler(SAC_Parser parser,
  const SAC_STRING name, const SAC_STRING pseudoPage)
{
  if (PARSER(parser)->end_page_handler == NULL) return 0;
  return PARSER(parser)->end_page_handler(PARSER(parser)->user_data,
      name, pseudoPage);
}



int SAC_parser_start_page_margin_handler(SAC_Parser parser,
  const SAC_PageMargin *margin)
{
  if (PARSER(parser)->start_page_margin_handler == NULL) return 0;
  return PARSER(parser)->start_page_margin_handler(PARSER(parser)->user_data,
    margin);
}



int SAC_parser_end_page_margin_handler(SAC_Parser parser,
  const SAC_PageMargin *margin)
{
  if (PARSER(parser)->end_page_margin_handler == NULL) return 0;
  return PARSER(parser)->end_page_margin_handler(PARSER(parser)->user_data,
    margin);
}



int SAC_parser_start_font_face_handler(SAC_Parser parser) {
  if (PARSER(parser)->start_font_face_handler == NULL) return 0;
  return PARSER(parser)->start_font_face_handler(PARSER(parser)->user_data);
}



int SAC_parser_end_font_face_handler(SAC_Parser parser) {
  if (PARSER(parser)->end_font_face_handler == NULL) return 0;
  return PARSER(parser)->end_font_face_handler(PARSER(parser)->user_data);
}



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  PARSER(parser)->start_document_handler = start;
  PARSER(parser)->end_document_handler = end;
}



void SAC_SetIgnorableAtRuleHandler(SAC_Parser parser,
  SAC_IgnorableAtRuleHandler handler)
{
  PARSER(parser)->ignorable_at_rule_handler = handler;
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



void SAC_SetPageMarginHandler(SAC_Parser parser,
  SAC_StartPageMarginHandler start, SAC_EndPageMarginHandler end)
{
  PARSER(parser)->start_page_margin_handler = start;
  PARSER(parser)->end_page_margin_handler = end;
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
  struct _SAC_Parser *parser = (struct _SAC_Parser*)malloc(
    sizeof(struct _SAC_Parser));

  if (parser == NULL) return NULL;

  memset(parser, 0, sizeof(struct _SAC_Parser));
  parser->fatal_error_handler = SAC_default_fatal_error_handler;
  parser->error_handler = SAC_default_error_handler;
  return parser;
}



void SAC_DisposeParser(SAC_Parser parser) {
  if (parser == NULL) return;

  SAC_mpool_close(PARSER(parser)->mpool);
  free(PARSER(parser)->base);
  free(parser);
}



static void* SAC_parse(SAC_Parser parser, int start_token,
  const char *buffer, int len)
{
  void *scanner;
  SAC_YYExtra yy_extra;

  if (SAC_parser_clear(parser) == SAC_FALSE) return NULL;

  yy_extra.mpool = PARSER(parser)->mpool;
  yy_extra.parser = parser;
  yy_extra.start_token = start_token;
  yy_extra.output = NULL;

  yylex_init_extra(&yy_extra, &scanner);

  yy_scan_bytes(buffer, len, scanner);

  yyparse(scanner);

  yylex_destroy(scanner);

  return yy_extra.output;
}



int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len) {
  SAC_parse(parser, START_AS_STYLESHEET, buffer, len);
  return PARSER(parser)->errno;
}



int SAC_ParseStyleDeclaration(SAC_Parser parser, const char *buffer, int len) {
  SAC_parse(parser, START_AS_STYLE_DECLARATIONS, buffer, len);
  return PARSER(parser)->errno;
}



const SAC_Selector** SAC_ParseSelectors(SAC_Parser parser,
  const char *buffer, int len)
{
  return (const SAC_Selector**)SAC_parse(parser,
    START_AS_SELECTORS, buffer, len);
}



const SAC_LexicalUnit* SAC_ParsePropertyValue(SAC_Parser parser,
  const char *buffer, int len)
{
  return (const SAC_LexicalUnit*)SAC_parse(parser,
    START_AS_PROPERTY_VALUE, buffer, len);
}



SAC_Boolean SAC_ParsePriority(SAC_Parser parser, const char *buffer, int len) {
  return (SAC_Boolean)SAC_parse(parser,
    START_AS_PRIORITY, buffer, len);
}



int SAC_ParseRule(SAC_Parser parser, const char *buffer, int len) {
  SAC_parse(parser, START_AS_RULE, buffer, len);
  return PARSER(parser)->errno;
}



int SAC_SetBase(SAC_Parser parser, const SAC_STRING base) {
  free(PARSER(parser)->base);
  PARSER(parser)->base = strdup(base);
  if (PARSER(parser)->base == NULL) return SAC_FATAL_ERROR_NO_MEMORY;
  return 0;
}



const SAC_STRING SAC_GetBase(SAC_Parser parser) {
  return PARSER(parser)->base;
}



void SAC_SetFatalErrorHandler(SAC_Parser parser,
  SAC_FatalErrorHandler handler)
{
  PARSER(parser)->fatal_error_handler = handler;
}



void SAC_SetErrorHandler(SAC_Parser parser,
  SAC_ErrorHandler handler)
{
  PARSER(parser)->error_handler = handler;
}
