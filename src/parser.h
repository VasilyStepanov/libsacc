#ifndef SACC_PARSER_H
#define SACC_PARSER_H

#define PARSER(parser) ((struct SAC_ParserImpl*)(parser))

struct SAC_ParserImpl {
  SAC_StartDocumentHandler startDocumentHandler;
  SAC_EndDocumentHandler endDocumentHandler;
  void *userData;
};

#endif
