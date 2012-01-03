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



SAC_Parser SAC_CreateParser() {
  return malloc(sizeof(SAC_ParserImpl));
}



void SAC_DisposeParser(SAC_Parser parser) {
  free(parser);
}

