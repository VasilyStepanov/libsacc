#include <sacc.h>

#include "parser.h"

#include <stdlib.h>



void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end)
{
  ((SAC_ParserImpl*)parser)->startDocumentHandler = start;
  ((SAC_ParserImpl*)parser)->endDocumentHandler = end;
}



void SAC_SetUserData(SAC_Parser parser, void *userData) {
  ((SAC_ParserImpl*)parser)->userData = userData;
}



void* SAC_GetUserData(SAC_Parser parser) {
  return ((SAC_ParserImpl*)parser)->userData;
}



SAC_Parser SAC_CreateParser() {
  return malloc(sizeof(SAC_ParserImpl));
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}



int SAC_ParseStyleSheet(SAC_Parser parser,
  const char *buffer __attribute__((unused)),
  int len __attribute__((unused)))
{
  void *userData = ((SAC_ParserImpl*)parser)->userData;
  ((SAC_ParserImpl*)parser)->startDocumentHandler(userData);
  ((SAC_ParserImpl*)parser)->endDocumentHandler(userData);
  return 0;
}
