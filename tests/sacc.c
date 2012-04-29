#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024



static void usage(void) {
  printf("USAGE: sacc "
    "-t [declarations|selectors|property|priorityrule|stylesheet|mediaquery]\n");
}



static ParserType parse_type_arg(const char *arg) {
  if (strcmp(arg, "declarations") == 0) return DECLARATIONS;
  if (strcmp(arg, "selectors") == 0) return SELECTORS;
  if (strcmp(arg, "property") == 0) return PROPERTY;
  if (strcmp(arg, "priority") == 0) return PRIORITY;
  if (strcmp(arg, "rule") == 0) return RULE;
  if (strcmp(arg, "stylesheet") == 0) return STYLESHEET;
  if (strcmp(arg, "mediaquery") == 0) return MEDIAQUERY;
  usage();
  exit(EXIT_FAILURE);
}



static char* read_all(FILE *in) {
  FILE *out;
  char buf[BUF_SIZE];
  size_t rsize;
  char *data;
  size_t wsize;
  size_t data_size;

  out = open_memstream(&data, &data_size);
  if (out == NULL) {
    printf("FATAL: open_memstream(): %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  data = NULL;
  data_size = 0;
  while (1) {
    rsize = fread(buf, 1, BUF_SIZE, in);
    if (ferror(in)) {
      printf("FATAL: fread(): %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (rsize == 0) break;
    wsize = fwrite(buf, 1, rsize, out);
    if (wsize != rsize) {
      printf("FATAL: fwrite(): %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (rsize != BUF_SIZE && feof(in)) break;
  }
  if (fclose(out)) {
    printf("FATAL: fclose(): %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return data;
}



int main(int argc, char **argv) {
  char *type_arg;
  ParserType type;
  char *css;
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
 
  css = read_all(stdin);
  parse(stdout, type, css);
  free(css);

  return EXIT_SUCCESS;
}
