#ifndef PARSER_H
#define PARSER_H

#include "mpool.h"

#include <sacc.h>

#define YY_EXTRA(yy_scanner) ((struct yy_extra_t*)yyget_extra(yy_scanner))

#define YY_SCANNER_MPOOL(yy_scanner) YY_EXTRA(yy_scanner)->mpool
#define YY_SCANNER_PARSER(yy_scanner) YY_EXTRA(yy_scanner)->parser

struct yy_extra_t {
  mpool_t mpool;
  SAC_Parser parser;
};



void parser_start_document(SAC_Parser parser);

void parser_end_document(SAC_Parser parser);



#endif
