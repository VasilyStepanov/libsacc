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



/*
 * Lexical unit
 */

typedef enum _SAC_LexicalUnitCode {

  /* Operators */
  /*
    Note that these operators have no meaning for the parser,
    it's up to the user to interpret each operators.
  */
  SAC_OPERATOR_COMMA,     /* , */
  SAC_OPERATOR_PLUS,      /* + */
  SAC_OPERATOR_MINUS,     /* - */
  SAC_OPERATOR_MULTIPLY,  /* * */
  SAC_OPERATOR_SLASH,     /* / */
  SAC_OPERATOR_MOD,       /* % */
  SAC_OPERATOR_EXP,       /* ^ */
  SAC_OPERATOR_LT,        /* < */
  SAC_OPERATOR_GT,        /* > */
  SAC_OPERATOR_LE,        /* <= */
  SAC_OPERATOR_GE,        /* >= */
  SAC_OPERATOR_TILDE,     /* ~ */

  SAC_INHERIT,            /* identifier inherit */
  /* Integers and reals numbers */
  SAC_INTEGER,
  SAC_REAL,
  /* Relative lengths */
  SAC_LENGTH_EM,
  SAC_LENGTH_EX,
  SAC_LENGTH_PIXEL,
  /* Absolute lengths */
  SAC_LENGTH_INCH,
  SAC_LENGTH_CENTIMETER,
  SAC_LENGTH_MILLIMETER,
  SAC_LENGTH_POINT,
  SAC_LENGTH_PICA,
  /* Percentages */
  SAC_PERCENTAGE,
  /* URI */
  SAC_URI,
  /* Counters */
  SAC_COUNTER_FUNCTION,
  SAC_COUNTERS_FUNCTION,
  /* RGB Colors */
  SAC_RGBCOLOR,           /* rgb(0, 0, 0) and #000 */
  /* Angles */
  SAC_DEGREE,
  SAC_GRADIAN,
  SAC_RADIAN,
  /* Times */
  SAC_MILLISECOND,
  SAC_SECOND,
  /* Frequencies */
  SAC_HERTZ,
  SAC_KILOHERTZ,
  /* Misc */
  SAC_IDENT,              /* any ident except inherit ! */
  SAC_STRING_VALUE,
  SAC_ATTR,               /* attribute */
  SAC_RECT_FUNCTION,      /* rect shape function */
  SAC_UNICODERANGE,

  /* sub expressions */
  SAC_SUB_EXPRESSION,     /* (a) (a + b) (normal/none) */

  /* Unknown values */
  SAC_FUNCTION,           /* unknown function */
  SAC_DIMENSION           /* unknown dimension */
} SAC_LexicalUnitCode;

typedef struct _SAC_LexicalUnit SAC_LexicalUnit;

struct _SAC_LexicalUnit {
  SAC_LexicalUnitCode lexicalUnitType;
  union _SAC_unit {
    /* SAC_OPERATOR_COMMA */
    /* SAC_OPERATOR_PLUS */
    /* SAC_OPERATOR_MINUS */
    /* SAC_OPERATOR_MULTIPLY */
    /* SAC_OPERATOR_SLASH */
    /* SAC_OPERATOR_MOD */
    /* SAC_OPERATOR_EXP */
    /* SAC_OPERATOR_LT */
    /* SAC_OPERATOR_GT */
    /* SAC_OPERATOR_LE */
    /* SAC_OPERATOR_GE */
    /* SAC_OPERATOR_TILDE */
    /* empty */

    /* SAC_INHERIT */
    /* empty */

    /* SAC_INTEGER */
    signed long integer;

    /* SAC_REAL */
    /* signed */ double real;

    /* SAC_DIMENSION, etc. */
    struct _SAC_Dimension {
      SAC_STRING unit;    /* The string representation of the unit */
      union _SAC_DimensionValue {
        /* SAC_DIMENSION */
        /* SAC_LENGTH_EM */
        /* SAC_LENGTH_EX */
        /* SAC_LENGTH_PIXEL */
        /* SAC_LENGTH_INCH */
        /* SAC_LENGTH_CENTIMETER */
        /* SAC_LENGTH_MILLIMETER */
        /* SAC_LENGTH_POINT */
        /* SAC_LENGTH_PICA */
        /* SAC_PERCENTAGE */
        double sreal; /* signed */

        /* SAC_DEGREE */
        /* SAC_GRADIAN */
        /* SAC_RADIAN */
        /* SAC_MILLISECOND */
        /* SAC_SECOND */
        /* SAC_HERTZ */
        /* SAC_KILOHERTZ */
        double ureal; /* unsigned */
      } value;
    } dimension;

    /* SAC_URI */
    SAC_STRING uri;

    /* SAC_COUNTER_FUNCTION */
    /* SAC_COUNTERS_FUNCTION */
    /* SAC_RGBCOLOR */
    /* SAC_FUNCTION */
    /* SAC_RECT_FUNCTION */
    struct _SAC_Function {
      SAC_STRING name;
      /* including operators, like the comma
         #000 is converted to rgb(0, 0, 0)
       */
      SAC_LexicalUnit **parameters;
    } function;

    /* SAC_IDENT */
    SAC_STRING ident;
    /* SAC_STRING_VALUE */
    SAC_STRING stringValue;
    /* SAC_ATTR */
    SAC_STRING attrName;

    /* SAC_UNICODERANGE */
    SAC_STRING unicodeRange; /* @@TO BE DEFINED */

    /* SAC_SUB_EXPRESSION */
    SAC_LexicalUnit **subValues;
  } desc;
};



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
  const SAC_LexicalUnit *value,
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
