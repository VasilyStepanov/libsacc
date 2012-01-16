#ifndef PARSER_H
#define PARSER_H

#include <sacc.h>

#include <stdio.h>



SAC_Parser create_parser(FILE *out);

void dispose_parser(SAC_Parser parser);

void parse_stylesheet(SAC_Parser parser, const char *buffer);

void parse_styledeclaration(SAC_Parser parser, const char *buffer);

void parse_selectors(SAC_Parser parser, const char *buffer);

void parse_property_value(SAC_Parser parser, const char *buffer);

void parse_priority(SAC_Parser parser, const char *buffer);

void parse_rule(SAC_Parser parser, const char *buffer);

#endif
