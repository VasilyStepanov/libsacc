#ifndef SACC_PARSER_H
#define SACC_PARSER_H

#define PARSER(parser) ((SAC_ParserImpl*)(parser))

typedef struct {
  SAC_StartDocumentHandler startDocumentHandler;
  SAC_EndDocumentHandler endDocumentHandler;
  void *userData;
} SAC_ParserImpl;

#endif
