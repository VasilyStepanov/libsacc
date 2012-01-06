#ifndef SACC_H
#define SACC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Defines
 */
#define SAC_STRING         char *

typedef enum SAC_Boolean {
    SAC_FALSE = 0,
    SAC_TRUE  = 1
} SAC_Boolean;

typedef void* SAC_Parser;

typedef void (*SAC_StartDocumentHandler)(void *userData);

typedef void (*SAC_EndDocumentHandler)(void *userData);

typedef void (*SAC_PropertyHandler)(void *userData,
  const SAC_STRING propertyName,
  /* const SAC_LexicalUnit * value, */
  SAC_Boolean important);

void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end);

void SAC_SetPropertyHandler(SAC_Parser parser,
  SAC_PropertyHandler handler);

void SAC_SetUserData(SAC_Parser parser, void *userData);

void* SAC_GetUserData(SAC_Parser parser);

SAC_Parser SAC_CreateParser();

void SAC_DisposeParser(SAC_Parser parser);

int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif
