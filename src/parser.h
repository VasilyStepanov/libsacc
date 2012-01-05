#ifndef SACC_PARSER_H
#define SACC_PARSER_H

#include <sacc.h>

#define PARSER(parser) ((struct parser_t*)(parser))

struct parser_t {
  SAC_StartDocumentHandler start_document_handler;
  SAC_EndDocumentHandler end_document_handler;
  void *user_data;
};

#endif
