#ifndef SACC_H
#define SACC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* SAC_Parser;

typedef void (*SAC_StartDocumentHandler)(void *userData);

typedef void (*SAC_EndDocumentHandler)(void *userData);

void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end);

SAC_Parser SAC_CreateParser();

void SAC_DisposeParser(SAC_Parser parser);

#ifdef __cplusplus
}
#endif

#endif
