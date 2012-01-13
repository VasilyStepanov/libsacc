#ifndef PARSER_H
#define PARSER_H

#include "mpool.h"

#include <sacc.h>



#define YY_EXTRA(yy_scanner) ((SAC_YYExtra*)yyget_extra(yy_scanner))

#define YY_SCANNER_MPOOL(yy_scanner) YY_EXTRA(yy_scanner)->mpool
#define YY_SCANNER_PARSER(yy_scanner) YY_EXTRA(yy_scanner)->parser
#define YY_SCANNER_START_TOKEN(yy_scanner) YY_EXTRA(yy_scanner)->start_token
#define YY_SCANNER_OUTPUT(yy_scanner) YY_EXTRA(yy_scanner)->output

extern void* yyget_extra();

struct _SAC_YYExtra {
  SAC_MPool mpool;
  SAC_Parser parser;
  int start_token;
  void *output;
};

typedef struct _SAC_YYExtra SAC_YYExtra;



void SAC_parser_start_document(SAC_Parser parser);

void SAC_parser_end_document(SAC_Parser parser);

void SAC_parser_property_handler(SAC_Parser parser,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important);

void SAC_parser_start_style_handler(SAC_Parser parser,
  const SAC_Selector *selectors[]);

void SAC_parser_end_style_handler(SAC_Parser parser,
  const SAC_Selector *selectors[]);

void SAC_parser_import_handler(SAC_Parser parser,
  const SAC_STRING uri,
  const SAC_STRING media[],
  const SAC_STRING defaultNamepaceURI);


#endif
