#ifndef PARSER_H
#define PARSER_H

#include "stream.h"

#include <sacc.h>



SAC_Parser create_parser(SAC_Stream stream);

void dispose_parser(SAC_Parser parser);

void dump_selectors(SAC_Stream out, const SAC_Selector **value);

void parse_stylesheet(SAC_Parser parser, const char *buffer);

void parse_styledeclaration(SAC_Parser parser, const char *buffer);

const SAC_Selector** parse_selectors(SAC_Parser parser,
  const char *buffer);

void parse_rule(SAC_Parser parser, const char *buffer);

#endif
