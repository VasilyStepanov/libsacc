#ifndef SACC_PARSER_H
#define SACC_PARSER_H

typedef struct {
  SAC_StartDocumentHandler startDocumentHandler;
  SAC_EndDocumentHandler endDocumentHandler;
} SAC_ParserImpl;

#endif
