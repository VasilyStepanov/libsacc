#ifndef SACC_H
#define SACC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Defines
 */

#ifndef NULL
#  define NULL ((void *) 0)
#endif

#define SAC_STRING         char *

typedef enum SAC_Boolean {
    SAC_FALSE = 0,
    SAC_TRUE  = 1
} SAC_Boolean;




typedef void* SAC_Parser;

/*
 * Handlers definition
 * All SAC_STRING are NULL terminated.
 */

/*
 * Receive notification of the beginning of a document.
 * The SAC parser will invoke this method only once, before any other methods in
 * this interface.
 */
typedef void (*SAC_StartDocumentHandler)(void *userData);

/*
 * Receive notification of the end of a document.
 * The SAC parser will invoke this method only once, and it will be the last
 * method invoked during the parse. The parser shall not invoke this method
 * until it has either abandoned parsing (because of an unrecoverable error) or
 * reached the end of input.
 */
typedef void (*SAC_EndDocumentHandler)(void *userData);

/*
 * Receive notification of a property declaration.
 * 
 * The SAC Parser will call this method to report each property inside a rule.
 * Note that shorthand properties will be reported as-is.
 *
 * propertyName - The name of the property.
 * value - The value of the property.
 * important - true if this property is important (i.e. "!important"),
 *             false otherwise.
 */
typedef void (*SAC_PropertyHandler)(void *userData,
  const SAC_STRING propertyName,
  /* const SAC_LexicalUnit * value, */
  SAC_Boolean important);



/*
  Methods to set handlers on the parser.
 */

void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end);

void SAC_SetPropertyHandler(SAC_Parser parser,
  SAC_PropertyHandler handler);

/*
 * This value is passed as the userData argument to callbacks.
 *
 * Value is never touched by the parser.
 */
void SAC_SetUserData(SAC_Parser parser, void *userData);

/*
 * Returns the last value set by SAC_SetUserData or NULL.
 */
void* SAC_GetUserData(SAC_Parser parser);

SAC_Parser SAC_CreateParser();

void SAC_DisposeParser(SAC_Parser parser);

int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif
