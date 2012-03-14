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
   * Operators.
   *
   * Note that these operators have no meaning for the parser,
   * it's up to the user to interpret each operators.
   */
  
  /**
   * Example: ,
   */
  SAC_OPERATOR_COMMA,

  /**
   * Example: +
   */
  SAC_OPERATOR_PLUS,

  /**
   * Example: -
   */
  SAC_OPERATOR_MINUS,
  
  /**
   * Example: *
   */
  SAC_OPERATOR_MULTIPLY,
  
  /**
   * Example: /
   */
  SAC_OPERATOR_SLASH,
  
  /**
   * Example: %
   */
  SAC_OPERATOR_MOD,

  /**
   * Example: ^
   */
  SAC_OPERATOR_EXP,

  /**
   * Example: <
   */
  SAC_OPERATOR_LT,

  /**
   * Example: >
   */
  SAC_OPERATOR_GT,

  /**
   * Example: <=
   */
  SAC_OPERATOR_LE,

  /**
   * Example: >=
   */
  SAC_OPERATOR_GE,

  /**
   * Example: ~
   */
  SAC_OPERATOR_TILDE,



  /**
   * Identifier inherit.
   */
  SAC_INHERIT,



  /**
   * Integers.
   */
  SAC_INTEGER,

  /**
   * Reals.
   */
  SAC_REAL,



  /**
   * Relative lengths.
   */

  /**
   * Relative length em.
   */
  SAC_LENGTH_EM,

  /**
   * Relative length ex.
   */
  SAC_LENGTH_EX,

  /**
   * Relative length px.
   */
  SAC_LENGTH_PIXEL,



  /**
   * Absolute lengths.
   */

  /**
   * Absolute length in.
   */
  SAC_LENGTH_INCH,

  /**
   * Absolute length cm.
   */
  SAC_LENGTH_CENTIMETER,

  /**
   * Absolute length mm.
   */
  SAC_LENGTH_MILLIMETER,

  /**
   * Absolute length pt.
   */
  SAC_LENGTH_POINT,

  /**
   * Absolute length pc.
   */
  SAC_LENGTH_PICA,



  /**
   * Percentage.
   */
  SAC_PERCENTAGE,

  /**
   * URI.
   *
   * Example:
   *   uri(...)
   */
  SAC_URI,



  /**
   * Counters.
   */

  /**
   * Function counter.
   */
  SAC_COUNTER_FUNCTION,

  /**
   * Function counters.
   */
  SAC_COUNTERS_FUNCTION,



  /**
   * Nths.
   */

  /**
   * ntn-child function.
   */
  SAC_NTH_CHILD_FUNCTION,

  /**
   * nth-last-child function.
   */
  SAC_NTH_LAST_CHILD_FUNCTION,

  /**
   * nth-of-type function.
   */
  SAC_NTH_OF_TYPE_FUNCTION,

  /**
   * nth-last-of-type function.
   */
  SAC_NTH_LAST_OF_TYPE_FUNCTION,



  /**
   * RGB Colors.
   *
   * Examples:
   *   rgb(0, 0, 0)
   *   #000
   */
  SAC_RGBCOLOR,



  /**
   * Angles.
   */

  /**
   * Angle deg.
   */
  SAC_DEGREE,

  /**
   * Angle grad.
   */
  SAC_GRADIAN,

  /**
   * Angle rad.
   */
  SAC_RADIAN,



  /**
   * Times.
   */

  /**
   * Time ms.
   */
  SAC_MILLISECOND,

  /**
   * Time s.
   */
  SAC_SECOND,



  /**
   * Frequencies.
   */

  /**
   * Frequency Hz.
   */
  SAC_HERTZ,

  /**
   * Frequency kHz.
   */
  SAC_KILOHERTZ,



  /**
   * Resolutions.
   */

  /**
   * Resolution dpi.
   */
  SAC_DOTS_PER_INCH,

  /**
   * Resolution dpcm.
   */
  SAC_DOTS_PER_CENTIMETER,



  /**
   * Misc.
   */

  /**
   * Any identifier except inherit.
   */
  SAC_IDENT,

  /**
   * A string.
   */
  SAC_STRING_VALUE,

  /**
   * Attribute.
   * Example: attr(...)
   */
  SAC_ATTR_FUNCTION,

  /**
   * Function rect.
   */
  SAC_RECT_FUNCTION,

  /**
   * A unicode range.
   */
  SAC_UNICODERANGE,



  /**
   * Sub expressions.
   *
   * Example:
   *   (a) (a + b) (normal/none)
   */
  SAC_SUB_EXPRESSION,



  /**
   * Unknown values.
   */

  /**
   * Unknown function.
   */
  SAC_FUNCTION,

  /**
   * unknown dimension.
   */
  SAC_DIMENSION
} SAC_LexicalUnitCode;

typedef struct _SAC_LexicalUnit SAC_LexicalUnit;

struct _SAC_LexicalUnit {
  SAC_LexicalUnitCode lexicalUnitType;
  union {
    /**
     * SAC_OPERATOR_COMMA
     * SAC_OPERATOR_PLUS
     * SAC_OPERATOR_MINUS
     * SAC_OPERATOR_MULTIPLY
     * SAC_OPERATOR_SLASH
     * SAC_OPERATOR_MOD
     * SAC_OPERATOR_EXP
     * SAC_OPERATOR_LT
     * SAC_OPERATOR_GT
     * SAC_OPERATOR_LE
     * SAC_OPERATOR_GE
     * SAC_OPERATOR_TILDE
     */

    /* empty */



    /**
     * SAC_INHERIT
     */

    /* empty */



    /**
     * SAC_INTEGER
     */

    /**
     * The integer value.
     */
    signed long integer;



    /**
     * SAC_REAL
     */

    /**
     * Signed float value.
     */
    double real;



    /**
     * SAC_DIMENSION, etc.
     */

    struct {
      /**
       * The string representation of the unit.
       */
      SAC_STRING unit;

      union {
        /**
         * SAC_DIMENSION
         * SAC_LENGTH_EM
         * SAC_LENGTH_EX
         * SAC_LENGTH_PIXEL
         * SAC_LENGTH_INCH
         * SAC_LENGTH_CENTIMETER
         * SAC_LENGTH_MILLIMETER
         * SAC_LENGTH_POINT
         * SAC_LENGTH_PICA
         * SAC_DEGREE
         * SAC_GRADIAN
         * SAC_RADIAN
         * SAC_PERCENTAGE
         */

        /**
         * Signed float value.
         */
        double sreal;



        /**
         * SAC_MILLISECOND
         * SAC_SECOND
         * SAC_HERTZ
         * SAC_KILOHERTZ
         * SAC_DOTS_PER_INCH
         * SAC_DOTS_PER_CENTIMETER
         */

        /**
         * Unsigned float value.
         */
        double ureal;
      } value;
    } dimension;



    /**
     * SAC_URI
     */

    /**
     * URI.
     *
     * The value doesn't contain uri(...) or quotes.
     */
    SAC_STRING uri;



    /**
     * SAC_COUNTER_FUNCTION
     * SAC_COUNTERS_FUNCTION
     * SAC_RGBCOLOR
     * SAC_FUNCTION
     * SAC_RECT_FUNCTION
     * SAC_ATTR_FUNCTION
     * SAC_NTH_CHILD_FUNCTION
     * SAC_NTH_LAST_CHILD_FUNCTION
     * SAC_NTH_OF_TYPE_FUNCTION
     * SAC_NTH_LAST_OF_TYPE_FUNCTION
     */

    struct {
      /**
       * The name of the function.
       */
      SAC_STRING name;

      /**
       * The function parameters including operators (like the comma).
       * #000 is converted to rgb(0, 0, 0)
       * Can return null if SAC_FUNCTION.
       *
       * nth- function expressions parsing rules.
       * For knwon expressions there will be three lexical units always:
       * SAC_DIMENSION, SAC_OPERATOR_ AND SAC_INTEGER.
       *
       * Examples:
       *   -2n-1: SAC_DIMENSION (-2n) SAC_OPERATOR_MINUS SAC_INTEGER (1)
       *   2n   : SAC_DIMENSION (2n) SAC_OPERATOR_PLUS SAC_INTEGER (0)
       *   1    : SAC_DIMENSION (0n) SAC_OPERATOR_PLUS SAC_INTEGER (0)
       *   odd  : treated ass 2n+1
       *   even : treated ass 2n
       * but
       *   foo  : SAC_IDENT
       */
      SAC_LexicalUnit **parameters;
    } function;



    /**
     * SAC_IDENT
     */

    SAC_STRING ident;



    /**
     * SAC_STRING_VALUE
     */

    SAC_STRING stringValue;



    /**
     * SAC_UNICODERANGE
     */

    /**
     * TODO: TO BE DEFINED
     */
    SAC_STRING unicodeRange;



    /**
     * SAC_SUB_EXPRESSION
     */

    /**
     * A list of values inside the sub expression.
     */
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
  /**
   * Simple selectors.
   */

  /**
   * This is a conditional selector.
   *
   * Examples:
   *   simple[role="private"]
   *   .part1
   *   H1#myId
   *   P:lang(fr).p1
   */
  SAC_CONDITIONAL_SELECTOR,

  /**
   * This selector matches any node.
   */
  SAC_ANY_NODE_SELECTOR,

  /**
   * This selector matches only element node.
   *
   * Examples:
   *   H1
   *   animate
   */
  SAC_ELEMENT_NODE_SELECTOR,

  /**
   * This selector matches only text node.
   */
  SAC_TEXT_NODE_SELECTOR,

  /**
   * This selector matches only cdata node.
   */
  SAC_CDATA_SECTION_NODE_SELECTOR,

  /**
   * This selector matches only processing instruction node.
   */
  SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR,

  /**
   * This selector matches only comment node.
   */
  SAC_COMMENT_NODE_SELECTOR,



  /**
   * Descendant selectors.
   */

  /**
   * This selector matches an arbitrary descendant of some ancestor element.
   *
   * Examples:
   *   E F
   */
  SAC_DESCENDANT_SELECTOR,

  /**
   * This selector matches a childhood relationship between two elements.
   *
   * Examples:
   *   E > F
   */
  SAC_CHILD_SELECTOR,



  /**
   * Sibling selectors.
   */

  /**
   * The elements represented by the two sequences share the same parent in
   * the document tree and the element represented by the first sequence
   * immediately precedes the element represented by the second one.
   *
   * Examples:
   *   E + F
   */
  SAC_DIRECT_ADJACENT_SELECTOR,

  /**
   * The elements represented by the two sequences share the same parent in
   * the document tree and the element represented by the first sequence
   * precedes (not necessarily immediately) the element represented by the
   * second one.
   *
   * Examples:
   *   h1 ~ pre
   */
  SAC_GENERAL_ADJACENT_SELECTOR
} SAC_SelectorType;

typedef enum {
  /**
   * This condition checks exactly two conditions.
   *
   * Example:
   *   .part1:lang(fr)
   */
  SAC_AND_CONDITION,

  /**
   * This condition checks one of two conditions.
   */
  SAC_OR_CONDITION,

  /**
   * This condition checks that a condition can't be applied to a node.
   */
  SAC_NEGATIVE_CONDITION,

  /**
   * This condition checks a specified position.
   *
   * Example:
   *   :first-child
   */
  SAC_POSITIONAL_CONDITION,

  /**
   * This condition checks if the specified attribure contains at least one
   * instance of the value.
   *
   * Example:
   *   [att*=val]
   */
  SAC_PREFIX_ATTRIBUTE_CONDITION,

  /**
   * This condition checks if the specified attribure ends with the value.
   *
   * Example:
   *   [att$=val]
   */
  SAC_SUFFIX_ATTRIBUTE_CONDITION,

  /**
   * This condition checks if the specified attribure begins with the value.
   *
   * Example:
   *   [att^=val]
   */
  SAC_SUBSTRING_ATTRIBUTE_CONDITION,

  /**
   * This condition checks an attribute.
   *
   * Example:
   *   [simple]
   *   [restart="never"]
   */
  SAC_ATTRIBUTE_CONDITION,

  /**
   * This condition checks an id attribute.
   *
   * Example:
   *   #myId
   */
  SAC_ID_CONDITION,

  /**
   * This condition checks the language of the node.
   *
   * Example:
   *   :lang(fr)
   */
  SAC_LANG_CONDITION,

  /**
   * This condition checks for a value in a space-separated values in a
   * specified attribute.
   *
   * Example:
   *    [values~="10"]
   */
  SAC_ONE_OF_ATTRIBUTE_CONDITION,

  /**
   * This condition checks if the value is in a hypen-separated list of values
   * in a specified attribute.
   *
   * Example:
   *   [languages|="fr"]
   */
  SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION,

  /**
   * This condition checks for a specified class.
   *
   * Example:
   *    .example
   */
  SAC_CLASS_CONDITION,

  /**
   * This condition checks for the link pseudo class.
   *
   * Example:
   *   :link
   *   :visited
   *   :hover
   */
  SAC_PSEUDO_CLASS_CONDITION,

  /**
   * This selector matches the 'first line' pseudo element.
   *
   * Examples:
   *   :first-line
   */
  SAC_PSEUDO_ELEMENT_CONDITION,

  /**
   * This condition checks if a node is the only one in the node list.
   */
  SAC_ONLY_CHILD_CONDITION,

  /**
   * This condition checks if a node is the only one of his type.
   */
  SAC_ONLY_TYPE_CONDITION,

  /**
   * This condition checks the content of a node.
   */
  SAC_CONTENT_CONDITION
} SAC_ConditionType;

typedef struct _SAC_Selector SAC_Selector;
typedef struct _SAC_Condition SAC_Condition;

struct _SAC_Selector {
  /* the type of the selector */
  SAC_SelectorType selectorType;

  union {
    /**
     * SAC_CONDITIONAL_SELECTOR
     */

    struct {
      /**
       * The simple selector.
       *
       * Can't be a combinator or a conditional selector
       */
      SAC_Selector  *simpleSelector;

      /**
       * The condition to be applied on the simple selector.
       */
      SAC_Condition *condition;
    } conditional;



    /**
     * SAC_ANY_NODE_SELECTOR
     */

    /* empty */


    /**
     * SAC_DESCENDANT_SELECTOR
     * SAC_CHILD_SELECTOR
     */

    struct {
      /**
       * The parent selector.
       *
       * Not exactly a "parentSelector", "parentOrAdjacentSelector".
       */
      SAC_Selector *descendantSelector;

      /**
       * Can't be a descendant or sibling selector.
       */
      SAC_Selector *simpleSelector;
    } descendant;



    /**
     * SAC_DIRECT_ADJACENT_SELECTOR
     * SAC_GENERAL_ADJACENT_SELECTOR
     */

    struct {
      /**
       * The first selector.
       */
      SAC_Selector *firstSelector;

      /**
       * Can't be a descendant or sibling selector.
       */
      SAC_Selector *secondSelector;
    } sibling;



    /**
     * SAC_ELEMENT_NODE_SELECTOR
     */

    struct {
      /**
       * The namespace URI of this element selector.
       *
       * NULL if this element selector can match any namespace.
       *
       * [namespace URI](http://www.w3.org/TR/REC-xml-names/#dt-NSName)
       */
      SAC_STRING namespaceURI;

      /**
       * The local part of the qualified name of this element.
       *
       * NULL if this element selector can match any element.
       *
       * [local part](http://www.w3.org/TR/REC-xml-names/#NT-LocalPart)
       * [qualified name](http://www.w3.org/TR/REC-xml-names/#ns-qualnames)
       */
      SAC_STRING localName;
    } element;



    /**
     * SAC_TEXT_NODE_SELECTOR
     * SAC_CDATA_SECTION_NODE_SELECTOR
     * SAC_COMMENT_NODE_SELECTOR
     */

    /**
     * The character data.
     *
     * NULL if any.
     */
    SAC_STRING data;



    /**
     * SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR
     */

    struct {
      /**
       * The target of the processing instruction.
       *
       * NULL if any.
       *
       * [target](http://www.w3.org/TR/REC-xml#NT-PITarget)
       */
      SAC_STRING target;

      /**
       * The character data.
       *
       * NULL if any.
       */
      SAC_STRING data; /* NULL if no data */
    } pi;
  } desc;
};

struct _SAC_Condition {
  /* the type of the condition */
  SAC_ConditionType conditionType;

  union {
    /**
     * SAC_AND_CONDITION
     * SAC_OR_CONDITION
     */

    struct {
      /**
       * The first condition.
       */
      SAC_Condition *firstCondition;

      /**
       * The second condition.
       */
      SAC_Condition *secondCondition;
    } combinator;



    /**
     * SAC_NEGATIVE_CONDITION
     */

    /**
     * The selector.
     */
    SAC_Selector *selector;



    /**
     * SAC_POSITIONAL_CONDITION
     */

    struct {
      /**
       * The position in the tree.
       *
       * A negative value means from the end of child node list.
       * The child node list begins at 0.
       */
      signed int position;

      /**
       * SAC_TRUE if the child node list only shows nodes of the same type of
       * the selector (only elements, only PIS, ...)
       */
      SAC_Boolean typeNode;

      /**
       * SAC_TRUE if the node should have the same node type (for element, same
       * namespaceURI and same localName).
       */
      SAC_Boolean type;
    } position;



    /**
     * SAC_PREFIX_ATTRIBUTE_CONDITION
     * SAC_SUFFIX_ATTRIBUTE_CONDITION
     * SAC_SUBSTRING_ATTRIBUTE_CONDITION
     * SAC_ATTRIBUTE_CONDITION
     * SAC_ONE_OF_ATTRIBUTE_CONDITION
     * SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION
     * SAC_ID_CONDITION
     * SAC_CLASS_CONDITION
     */

    struct {
      /**
       * The namespace URI of this attribute condition.
       *
       * NULL if :
       *  - this attribute condition can match any namespace.
       *  - this attribute is an id attribute.
       * 
       * [namespace URI](http://www.w3.org/TR/REC-xml-names/#dt-NSName)
       *
       */
      SAC_STRING namespaceURI;

      /**
       * The local part of the qualified name of this attribute.
       *
       * NULL if:
       *  - this attribute condition can match any attribute.
       *  - this attribute is a class attribute.
       *  - this attribute is an id attribute.
       *  - this attribute is a pseudo-class attribute.
       *
       * [local part](http://www.w3.org/TR/REC-xml-names/#NT-LocalPart) of the
       * [qualified name](http://www.w3.org/TR/REC-xml-names/#ns-qualnames) of
       */
      SAC_STRING localName;

      /**
       * SAC_TRUE if the attribute must have an explicit value in the original
       * document, SAC_FALSE otherwise.
       */
      SAC_Boolean specified;

      /**
       * The value of the attribute.
       * If this attribute is a class or a pseudo class attribute, you'll get
       * the class name (or psedo class name) without the '.' or ':'.
       *
       * NULL if any.
       */
      SAC_STRING  value;
    } attribute;



    /**
     * SAC_PSEUDO_CLASS_CONDITION
     * SAC_PSEUDO_ELEMENT_CONDITION
     */

    /**
     * Ident lexical unit for the pseudo class and pseudo element condition.
     * Nth lexical unit for the nth functional pseudo condition.
     * Function lexical unit for the unknown functional pseudo condition.
     */
    SAC_LexicalUnit *pseudo;


    /**
     * SAC_LANG_CONDITION
     */

    /**
     * The language.
     *
     * NULL if any.
     */
    SAC_STRING lang;



    /**
     * SAC_ONLY_CHILD_CONDITION
     * SAC_ONLY_TYPE_CONDITION
     */

    /* empty */



    /**
     * SAC_CONTENT_CONDITION
     */

    /**
     * The content.
     *
     * Can't be NULL.
     */
    SAC_STRING data;
  } desc;
};



/**
 * Media queries.
 */

typedef enum {
  /**
   * Example:
   *   @media screen {...}
   */
  SAC_TYPE_MEDIA_QUERY,

  /**
   * Example:
   *   @media (min-width: 400px) {...}
   */
  SAC_FEATURE_MEDIA_QUERY,

  /**
   * Example:
   *   @media screen and (min-width: 400px) {...}
   *   @media screen and (min-width: 400px) and (max-width: 700px) {...}
   */
  SAC_AND_MEDIA_QUERY,

  /**
   * Example:
   *   @media only screen {...}
   *   @media only screen and (min-width: 400px) {...}
   */
  SAC_ONLY_MEDIA_QUERY,

  /**
   * Example:
   *   @media not screen {...}
   *   @media not screen and (min-width: 400px) {...}
   */
  SAC_NOT_MEDIA_QUERY
} SAC_MediaQueryType;

typedef struct _SAC_MediaQuery SAC_MediaQuery;

struct _SAC_MediaQuery {
  /* the type of the media query */
  SAC_MediaQueryType mediaQueryType;

  union {
    /* SAC_TYPE_MEDIA_QUERY */
    SAC_STRING type;

    /* SAC_FEATURE_MEDIA_QUERY */
    struct {
      SAC_STRING name;
      SAC_LexicalUnit *value;
    } feature;

    /* SAC_AND_MEDIA_QUERY */
    struct {
      /**
       * Can't be an 'only' media query or a 'not' media query.
       */
      SAC_MediaQuery *firstQuery;

      /**
       * Can't be a 'type' media query, an 'only' media query or a 'not' media
       * query.
       */
      SAC_MediaQuery *secondQuery;
    } combinator;

    /* SAC_ONLY_MEDIA_QUERY */
    /* SAC_NOT_MEDIA_QUERY */
    SAC_MediaQuery *subQuery;
  } desc;
};



/**
 * Page margins.
 */



typedef enum {
  SAC_TOP_LEFT_CORNER_PAGE_MARGIN,
  SAC_TOP_LEFT_PAGE_MARGIN,
  SAC_TOP_CENTER_PAGE_MARGIN,
  SAC_TOP_RIGHT_PAGE_MARGIN,
  SAC_TOP_RIGHT_CORNER_PAGE_MARGIN,
  SAC_BOTTOM_LEFT_CORNER_PAGE_MARGIN,
  SAC_BOTTOM_LEFT_PAGE_MARGIN,
  SAC_BOTTOM_CENTER_PAGE_MARGIN,
  SAC_BOTTOM_RIGHT_PAGE_MARGIN,
  SAC_BOTTOM_RIGHT_CORNER_PAGE_MARGIN,
  SAC_LEFT_TOP_PAGE_MARGIN,
  SAC_LEFT_MIDDLE_PAGE_MARGIN,
  SAC_LEFT_BOTTOM_PAGE_MARGIN,
  SAC_RIGHT_TOP_PAGE_MARGIN,
  SAC_RIGHT_MIDDLE_PAGE_MARGIN,
  SAC_RIGHT_BOTTOM_PAGE_MARGIN
} SAC_PageMarginType;

typedef struct _SAC_PageMargin SAC_PageMargin;

struct _SAC_PageMargin {
  /* The type of the page margin */
  SAC_PageMarginType pageMarginType;
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
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_StartDocumentHandler)(void *userData);

/**
 * Receive notification of the end of a document.
 * The SAC parser will invoke this method only once, and it will be the last
 * method invoked during the parse. The parser shall not invoke this method
 * until it has either abandoned parsing (because of an unrecoverable error) or
 * reached the end of input.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_EndDocumentHandler)(void *userData);

/**
 * Receive notification of an unknown rule at-rule not supported by this parser.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_IgnorableAtRuleHandler)(void *userData,
  const SAC_STRING atRule);

/**
 * Receive notification of a namespace declaration and the default namespace
 * if any (prefix will be NULL in that case). uri can never be NULL.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_NamespaceDeclarationHandler)(void *userData,
  const SAC_STRING prefix, const SAC_STRING uri);

/**
 * Receive notification of an import statement.
 * Nonzero return from callback aborts the parser.
 * base  - The base argument is whatever was set by SAC_SetBase.
 * uri   - The unresolved URI of the imported style sheet.
 *         It can never be NULL.
 * media - It is an array of all media for the imported style sheet.
 *         It terminated by NULL.
 * defaultNamepaceURI - The default namespace URI for the imported style sheet.
 */
typedef int (*SAC_ImportHandler)(void *userData,
  const SAC_STRING base,
  const SAC_STRING uri,
  const SAC_MediaQuery *media[],
  const SAC_STRING defaultNamepaceURI);

/**
 * Receive notification of the beginning of a media rule.
 *
 * The SAC Parser will invoke this method at the beginning of every media rule
 * in the CSS document. There will be a corresponding SAC_EndMediaHandler event
 * for every SAC_StartMediaHandler event. All rules inside the rule will be
 * reported, in order, before the corresponding SAC_EndMediaHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * media - It is an array of all media for the media rule.
 *         It terminated by NULL.
 */
typedef int (*SAC_StartMediaHandler)(void *userData,
  const SAC_MediaQuery *media[]);

/**
 * Receive notification of the end of a media rule.
 *
 * The SAC Parser will invoke this method at the end of every media rule in the
 * CSS document. There will be a corresponding SAC_StartMediaHandler event for
 * every SAC_EndMediaHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * media - It is an array of all media for the media rule.
 *         It terminated by NULL.
 */
typedef int (*SAC_EndMediaHandler)(void *userData,
  const SAC_MediaQuery *media[]);

/**
 * Receive notification of the beginning of a page rule.
 *
 * The SAC Parser will invoke this method at the beginning of every page rule in
 * the CSS document. There will be a corresponding SAC_EndPageHandler event for
 * every SAC_StartPageHandler event. All properties inside the rule will be
 * reported, in order, before the corresponding SAC_EndPageHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * name         - The name of the page if any.
 * pseudoPseudo - The pseudo page associated to the page.
 */
typedef int (*SAC_StartPageHandler)(void *userData,
  const SAC_STRING name, const SAC_STRING pseudoPage);

/**
 * Receive notification of the end of a page rule.
 *
 * The SAC Parser will invoke this method at the end of every page rule in the
 * CSS document. There will be a corresponding SAC_StartPageHandler event for
 * every SAC_EndPageHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * name       - The name of the page if any.
 * pseudoPage - The pseudo page associated to the page.
 */
typedef int (*SAC_EndPageHandler)(void *userData,
  const SAC_STRING name, const SAC_STRING pseudoPage);

/**
 * Receive notification of the beginning of a page margin rule.
 *
 * The SAC Parser will invoke this method at the beginning of every page margin
 * rule in the CSS document. There will be a corresponding
 * SAC_EndPageMarginHandler event for every SAC_StartPageMarginHandler event.
 * All properties inside the rule will be reported, in order, before the
 * corresponding SAC_EndPageMarginHandler event.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_StartPageMarginHandler)(void *userData,
  const SAC_PageMargin *margin);

/**
 * Receive notification of the end of a page margin rule.
 *
 * The SAC Parser will invoke this method at the end of every page rule in the
 * CSS document. There will be a corresponding SAC_StartPageHandler event for
 * every SAC_EndPageHandler event.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_EndPageMarginHandler)(void *userData,
  const SAC_PageMargin *margin);

/**
 * Receive notification of a beginning of font-face rule.
 *
 * The SAC Parser will invoke this method at the beginning of every font-face
 * rule in the CSS document. There will be a corresponding
 * SAC_EndFontFaceHandler event for every SAC_StartFontFaceHandler event. All
 * properties inside the rule will be reported, in order, before the
 * corresponding SAC_EndFontFaceHandler event.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_StartFontFaceHandler)(void *userData);

/**
 * Receive notification of the end of a font-face rule.
 *
 * The SAC Parser will invoke this method at the end of every font-face rule in
 * the CSS document. There will be a corresponding SAC_StartFontFaceHandler
 * event for every SAC_EndFontFaceHandler event.
 * Nonzero return from callback aborts the parser.
 */
typedef int (*SAC_EndFontFaceHandler)(void *userData);

/**
 * Receive notification of the beginning of a style rule.
 *
 * The SAC Parser will invoke this method at the beginning of every style rule
 * in the CSS document. There will be a corresponding SAC_EndStyleHandler event
 * for every SAC_StartStyleHandler event. All properties inside the rule will be
 * reported, in order, before the corresponding SAC_EndStyleHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * selectors - It is an array of all selectors for the style rule.
 */
typedef int (*SAC_StartStyleHandler)(void *userData,
  const SAC_Selector *selectors[]);

/**
 * Receive notification of the end of a style rule.
 *
 * The SAC Parser will invoke this method at the end of every style rule in the
 * CSS document. There will be a corresponding SAC_StartStyleHandler event for
 * every SAC_EndStyleHandler event.
 * Nonzero return from callback aborts the parser.
 *
 * selectors - It is an array of all selectors for the style rule.
 */
typedef int (*SAC_EndStyleHandler)(void *userData,
  const SAC_Selector *selectors[]);

/**
 * Receive notification of a property declaration.
 *
 * The SAC Parser will call this method to report each property inside a rule.
 * Note that shorthand properties will be reported as-is.
 * Nonzero return from callback aborts the parser.
 *
 * propertyName - The name of the property.
 * value - The value of the property.
 * important - SAC_TRUE if this property is important (i.e. "!important"),
 *             SAC_FALSE otherwise.
 */
typedef int (*SAC_PropertyHandler)(void *userData,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important);



/**
 * Methods to set handlers on the parser.
 */

void SAC_SetDocumentHandler(SAC_Parser parser,
  SAC_StartDocumentHandler start, SAC_EndDocumentHandler end);

void SAC_SetIgnorableAtRuleHandler(SAC_Parser parser,
  SAC_IgnorableAtRuleHandler handler);

void SAC_SetNamespaceDeclarationHandler(SAC_Parser parser,
  SAC_NamespaceDeclarationHandler handler);

void SAC_SetImportHandler(SAC_Parser parser, SAC_ImportHandler handler);

void SAC_SetPageHandler(SAC_Parser parser,
  SAC_StartPageHandler start, SAC_EndPageHandler end);

void SAC_SetPageMarginHandler(SAC_Parser parser,
  SAC_StartPageMarginHandler start, SAC_EndPageMarginHandler end);

void SAC_SetFontFaceHandler(SAC_Parser parser,
  SAC_StartFontFaceHandler start, SAC_EndFontFaceHandler end);

void SAC_SetMediaHandler(SAC_Parser parser,
  SAC_StartMediaHandler start, SAC_EndMediaHandler end);

void SAC_SetStyleHandler(SAC_Parser parser,
  SAC_StartStyleHandler start, SAC_EndStyleHandler end);

void SAC_SetPropertyHandler(SAC_Parser parser, SAC_PropertyHandler handler);

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

/**
 * Sets the base to be used for resolving relative URIs in system identifiers
 * in declarations.  Resolving relative identifiers is left to the application:
 * this value will be passed through as the base argument to the
 * SAC_ImportHandler. The base argument will be copied. Returns non-zero if
 * out of memory, zero otherwise.
 */
int SAC_SetBase(SAC_Parser parser, const SAC_STRING base);

const SAC_STRING SAC_GetBase(SAC_Parser parser);

int SAC_ParseStyleSheet(SAC_Parser parser, const char *buffer, int len);

int SAC_ParseStyleDeclaration(SAC_Parser parser, const char *buffer, int len);

int SAC_ParseRule(SAC_Parser parser, const char *buffer, int len);

/**
 * Parse selectors. Returns an array of selectors terminated by NULL.
 * Returns NULL in case of errors.
 *
 * Do NOT free the returned pointer and do not rely on its value after the next
 * SAC_Parse* or SAC_DisposeParser call.
 */
const SAC_Selector** SAC_ParseSelectors(SAC_Parser parser,
  const char *buffer, int len);

/**
 * Parse property value.
 * Returns NULL in case of errors.
 *
 * Do NOT free the returned pointer and do not rely on its value after the next
 * SAC_Parse* or SAC_DisposeParser call.
 */
const SAC_LexicalUnit* SAC_ParsePropertyValue(SAC_Parser parser, 
  const char *buffer, int len);

SAC_Boolean SAC_ParsePriority(SAC_Parser parser, const char *buffer, int len);

/**
 * Create the parser.
 * 
 * The caller is responsible for disposing the parser by calling
 * SAC_DisposeParser.
 *
 * Returns NULL if out of memory.
 */
SAC_Parser SAC_CreateParser();

/**
 * Dispose the parser.
 *
 * NULL argument is valid and will return immediately.
 */
void SAC_DisposeParser(SAC_Parser parser);

/**
 * Error handling.
 */

typedef enum _SAC_FatalErrorCode {
  SAC_FATAL_ERROR_NO_MEMORY = -100
  /* @@CONTINUEME */
} SAC_FatalErrorCode;

typedef struct _SAC_FatalError {
  signed int line;   /* -1 if unknown */
  signed int column; /* -1 if unknown */
  SAC_FatalErrorCode code;
} SAC_FatalError;

typedef void (*SAC_FatalErrorHandler)(void *userData,
  const SAC_FatalError *fatalError);

void SAC_SetFatalErrorHandler(SAC_Parser parser,
  SAC_FatalErrorHandler handler);


typedef enum _SAC_ErrorCode {
  SAC_ERROR_NOT_SUPPORTED = -1,
  SAC_ERROR_SYNTAX = -2
} SAC_ErrorCode;

typedef struct _SAC_Error {
  signed int line;   /* -1 if unknown */
  signed int column; /* -1 if unknown */
  SAC_ErrorCode code;
  const SAC_STRING data; /* NULL if it is not relevant */
} SAC_Error;

typedef void (*SAC_ErrorHandler)(void *userData,
  const SAC_Error *error);

void SAC_SetErrorHandler(SAC_Parser parser,
  SAC_ErrorHandler handler);

#ifdef __cplusplus
}
#endif

#endif
