#ifndef SACC_H
#define SACC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
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



/**
 * Lexical unit
 */

typedef enum _SAC_LexicalUnitCode {

  /**
   * Operators
   *
   * Note that these operators have no meaning for the parser,
   * it's up to the user to interpret each operators.
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
      /**
       * including operators, like the comma
       * #000 is converted to rgb(0, 0, 0)
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



/**
 * Selector.
 *
 * This section defines the types of selectors and conditions
 * A condition can be applied to a selector.
 * Please note that this file defines more selectors and conditions
 * than the CSS syntax can do. This is only for a future usage, the CSS parser
 * is not supposed to support all of them.
 */

typedef enum {
  /* simple selectors */
  SAC_CONDITIONAL_SELECTOR,
  SAC_ANY_NODE_SELECTOR,
  SAC_ROOT_NODE_SELECTOR,
  SAC_NEGATIVE_SELECTOR,
  SAC_ELEMENT_NODE_SELECTOR,
  SAC_TEXT_NODE_SELECTOR,
  SAC_CDATA_SECTION_NODE_SELECTOR,
  SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR,
  SAC_COMMENT_NODE_SELECTOR,
  SAC_PSEUDO_ELEMENT_SELECTOR,

  /* descendant selectors */
  SAC_DESCENDANT_SELECTOR,
  SAC_CHILD_SELECTOR,

  /* sibling selectors */
  SAC_DIRECT_ADJACENT_SELECTOR
} SAC_SelectorType;

typedef enum {
  SAC_AND_CONDITION,
  SAC_OR_CONDITION,
  SAC_NEGATIVE_CONDITION,
  SAC_POSITIONAL_CONDITION,
  SAC_ATTRIBUTE_CONDITION,
  SAC_ID_CONDITION,
  SAC_LANG_CONDITION,
  SAC_ONE_OF_ATTRIBUTE_CONDITION,
  SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION,
  SAC_CLASS_CONDITION,
  SAC_PSEUDO_CLASS_CONDITION,
  SAC_ONLY_CHILD_CONDITION,
  SAC_ONLY_TYPE_CONDITION,
  SAC_CONTENT_CONDITION
} SAC_ConditionType;

typedef enum {
  ELEMENT_NODE,
  ATTRIBUTE_NODE,
  TEXT_NODE,
  CDATA_NODE,
  ENTITY_REFERENCE_NODE,
  PROCESSING_INSTRUCTION_NODE,
  COMMENT_NODE,
  ANY_NODE
} SAC_nodeType;

typedef struct _SAC_Selector SAC_Selector;
typedef struct _SAC_Condition SAC_Condition;

struct _SAC_Selector {
  /* the type of the selector */
  SAC_SelectorType selectorType;

  union _SAC_Sdesc {
    /* SAC_CONDITIONAL_SELECTOR */
    struct _SAC_Conditional {
      /* can't be a combinator or a conditional selector */
      SAC_Selector  *simpleSelector;
      SAC_Condition *condition;
    } conditional;

    /* SAC_ANY_NODE_SELECTOR */
    /* SAC_ROOT_NODE_SELECTOR */
    /* empty */

    /* SAC_NEGATIVE_SELECTOR */
    SAC_Selector *simpleSelector;

    /* SAC_DESCENDANT_SELECTOR */
    /* SAC_CHILD_SELECTOR */
    struct _SAC_Sdescendant {
      /* not exactly a "parentSelector", "parentOrAdjacentSelector" */
      SAC_Selector *descendantSelector;
      /* can't be a descendant or sibling selector */
      SAC_Selector *simpleSelector;
    } descendant;

    /* SAC_DIRECT_ADJACENT_SELECTOR */
    struct _SAC_Ssibling {
      SAC_nodeType nodeType;
      SAC_Selector *firstSelector;
      /* can't be a descendant or sibling selector */
      SAC_Selector *secondSelector;
    } sibling;

    /* SAC_ELEMENT_NODE_SELECTOR */
    /* SAC_PSEUDO_ELEMENT_SELECTOR */
    struct _SAC_Element {
      SAC_STRING namespaceURI; /* NULL if any */
      SAC_STRING localName; /* NULL if any */
    } element;

    /* SAC_TEXT_NODE_SELECTOR */
    /* SAC_CDATA_SECTION_NODE_SELECTOR */
    /* SAC_COMMENT_NODE_SELECTOR */
    SAC_STRING data; /* NULL if any */

    /* SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR */
    struct _SAC_Pi {
      SAC_STRING target; /* NULL if any */
      SAC_STRING data; /* NULL if no data */
    } pi;
  } desc;
};

struct _SAC_Condition {
  /* the type of the condition */
  SAC_ConditionType conditionType;

  union _SAC_Cdesc {
    /* SAC_AND_CONDITION */
    /* SAC_OR_CONDITION */
    struct _SAC_Ccombinator {
      SAC_Condition *firstCondition;
      SAC_Condition *secondCondition;
    } combinator;

    /* SAC_NEGATIVE_SELECTOR */
    SAC_Condition *condition;

    /* SAC_POSITIONAL_CONDITION */
    struct _SAC_CPositional {
      /* a negative value means from the end of child node list */
      signed int position;

      /**
       * SAC_TRUE if the child node list only shows nodes of the same
       *          type.
       */
      SAC_Boolean typeNode;
      /**
       * SAC_TRUE if the node should have the same node type.
       *          (for element, same namespaceURI and same localName)
       */
      SAC_Boolean type;
    } position;

    /* SAC_ATTRIBUTE_CONDITION */
    /* SAC_ONE_OF_ATTRIBUTE_CONDITION */
    /* SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION */
    /* SAC_ID_CONDITION */
    /* SAC_CLASS_CONDITION */
    /* SAC_PSEUDO_CLASS_CONDITION */
    struct _SAC_Attribute {
      /**
       * NULL if :
       *  - any
       *  - if conditionType == SAC_ID_CONDITION
      */
      SAC_STRING namespaceURI;
      /**
       * NULL if:
       *  - this attribute condition can match any attribute.
       *  - this attribute is a class or a pseudo class.
       */
      SAC_STRING localName;
      /**
       * If the attribute must have an explicit value in the original
       * document, 'specified' is SAC_TRUE; otherwise, it is SAC_FALSE.
       */
      SAC_Boolean specified;
      SAC_STRING  value; /* NULL if any */
    } attribute;

    /* SAC_LANG_CONDITION */
    SAC_STRING lang; /* NULL if any */

    /* SAC_ONLY_CHILD_CONDITION */
    /* SAC_ONLY_TYPE_CONDITION */
    /* empty */

    /* SAC_CONTENT_CONDITION */
    SAC_STRING data; /* can't be NULL */
  } desc;
};



/**
 * Parser.
 */

typedef void* SAC_Parser;

/**
 * Handlers definition
 * All SAC_STRING are NULL terminated.
 */

/**
 * Receive notification of the beginning of a document.
 * The SAC parser will invoke this method only once, before any other methods in
 * this interface.
 */
typedef void (*SAC_StartDocumentHandler)(void *userData);

/**
 * Receive notification of the end of a document.
 * The SAC parser will invoke this method only once, and it will be the last
 * method invoked during the parse. The parser shall not invoke this method
 * until it has either abandoned parsing (because of an unrecoverable error) or
 * reached the end of input.
 */
typedef void (*SAC_EndDocumentHandler)(void *userData);

/**
 * Receive notification of the beginning of a style rule.
 *
 * The SAC Parser will invoke this method at the beginning of every style rule
 * in the CSS document. There will be a corresponding SAC_EndStyleHandler event
 * for every SAC_StartStyleHandler event. All properties inside the rule will be
 * reported, in order, before the corresponding SAC_EndStyleHandler event.
 *
 * selectors - It is an array of all selectors for the style rule.
*/
typedef void (*SAC_StartStyleHandler)(void *userData,
  const SAC_Selector *selectors[]);

/**
 * Receive notification of the end of a style rule.
 *
 * The SAC Parser will invoke this method at the end of every style rule in the
 * CSS document. There will be a corresponding SAC_StartStyleHandler event for
 * every SAC_EndStyleHandler event.
 *
 * selectors - It is an array of all selectors for the style rule.
*/
typedef void (*SAC_EndStyleHandler)(void *userData,
  const SAC_Selector *selectors[]);

/**
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



/**
 * Methods to set handlers on the parser.
 */

void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start,
  SAC_EndDocumentHandler end);

void SAC_SetStyleHandler(SAC_Parser parser,
  SAC_StartStyleHandler start,
  SAC_EndStyleHandler end);

void SAC_SetPropertyHandler(SAC_Parser parser,
  SAC_PropertyHandler handler);

/**
 * This value is passed as the userData argument to callbacks.
 *
 * Value is never touched by the parser.
 */
void SAC_SetUserData(SAC_Parser parser, void *userData);

/**
 * Returns the last value set by SAC_SetUserData or NULL.
 */
void* SAC_GetUserData(SAC_Parser parser);

SAC_Parser SAC_CreateParser();

void SAC_DisposeParser(SAC_Parser parser);

int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len);

int SAC_ParseStyleDeclaration(SAC_Parser parser, const char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif
