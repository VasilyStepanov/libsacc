#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024



static void usage() {
  printf("USAGE: sacc -t [declarations|selectors|property|rule|stylesheet]\n");
}



typedef enum {
  DECLARATIONS,
  SELECTORS,
  PROPERTY,
  RULE,
  STYLESHEET,
  UNKNOWN
} InputType;



static InputType parse_type_arg(const char *arg) {
  if (strcmp(arg, "declarations") == 0) return DECLARATIONS;
  if (strcmp(arg, "selectors") == 0) return SELECTORS;
  if (strcmp(arg, "property") == 0) return PROPERTY;
  if (strcmp(arg, "rule") == 0) return RULE;
  if (strcmp(arg, "stylesheet") == 0) return STYLESHEET;
  return UNKNOWN;
 }



static char* read_all(FILE *in) {
  FILE *out;
  char buf[BUF_SIZE];
  size_t rsize;
  char *data;
  size_t wsize;
  size_t data_size;

  out = open_memstream(&data, &data_size);
  data = NULL;
  data_size = 0;
  while (1) {
    rsize = fread(buf, 1, BUF_SIZE, in);
    if (rsize == 0) break;
    wsize = fwrite(buf, 1, rsize, out);
  }
  fclose(out);
  return data;
}



int main(int argc, char **argv) {
  char *type_arg;
  InputType type;
  char *css;
  SAC_Parser parser;
  int i;

  type_arg = NULL;
  for (i = 1; i != argc; ++i) {
    char *arg = argv[i];
    if (strcmp(arg, "-t") == 0 && i != argc - 1)
      type_arg = argv[++i];
  }

  if (type_arg == NULL) {
    usage();
    return EXIT_FAILURE;
  }

  type = parse_type_arg(type_arg);
  if (type == UNKNOWN) {
    usage();
    return EXIT_FAILURE;
  }
 
  css = read_all(stdin);
  parser = create_parser(stdout);
  switch (type) {
    case DECLARATIONS:
      parse_styledeclaration(parser, css);
      break;
    case SELECTORS:
      parse_selectors(parser, css);
      break;
    case PROPERTY:
      parse_property_value(parser, css);
      break;
    case RULE:
      parse_rule(parser, css);
      break;
    case STYLESHEET:
      parse_stylesheet(parser, css);
      break;
    case UNKNOWN:
      break;
  }
  dispose_parser(parser);
  free(css);


  return EXIT_SUCCESS;
}
