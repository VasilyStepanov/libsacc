#ifndef PARSER_H
#define PARSER_H

#include <sacc.h>

#include <stdio.h>



typedef enum {
  DECLARATIONS,
  SELECTORS,
  PAGESELECTORS,
  PROPERTY,
  PRIORITY,
  RULE,
  STYLESHEET,
  MEDIAQUERY
} ParserType;



void parse(FILE *out, ParserType type, const char *buffer);



#endif
