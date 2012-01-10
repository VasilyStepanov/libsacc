#ifndef PARSER_H
#define PARSER_H

#include "mpool.h"
#include "list.h"
#include "declaration.h"
#include "lexical_unit.h"

#include <sacc.h>

#define YY_EXTRA(yy_scanner) ((struct yy_extra_t*)yyget_extra(yy_scanner))

#define YY_SCANNER_MPOOL(yy_scanner) YY_EXTRA(yy_scanner)->mpool
#define YY_SCANNER_PARSER(yy_scanner) YY_EXTRA(yy_scanner)->parser

extern void* yyget_extra();



struct yy_extra_t {
  mpool_t mpool;
  SAC_Parser parser;
};



void parser_start_document(SAC_Parser parser);

void parser_end_document(SAC_Parser parser);

void parser_property_handler(SAC_Parser parser,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important);



#endif
