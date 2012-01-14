#include "test_parser.h"

#include "test_utils.h"
#include "stream.h"

#include <sacc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>



#define USERDATA(userData) ((struct userdata_t*)(userData))
#define USERDATA_STREAM(userData) USERDATA(userData)->stream
#define USERDATA_INDENT(userData) USERDATA(userData)->indent

struct userdata_t {
  SAC_Stream stream;
  char *indent;
};



static void* userdata_open(SAC_Stream stream) {
  void *ret =
    (struct userdata_t*)malloc(sizeof(struct userdata_t));

  USERDATA_STREAM(ret) = stream;
  USERDATA_INDENT(ret) = (char*)malloc(sizeof(char));
  *USERDATA_INDENT(ret) = '\0';

  return ret;
}



static void userdata_close(void *userdata) {
  free(USERDATA_INDENT(userdata));
  free(userdata);
}



static void userdata_inc_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 + 2);
  indent[len + 0] = ' ';
  indent[len + 1] = ' ';
  indent[len + 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



static void userdata_dec_indent(void *userdata) {
  size_t len;
  char *indent;

  len = strlen(USERDATA_INDENT(userdata));
  indent = realloc(USERDATA_INDENT(userdata), len + 1 - 2);
  indent[len - 2] = '\0';
  USERDATA_INDENT(userdata) = indent;
}



static void userdata_printf(void *userdata, const char *fmt, ...) {
  va_list args;

  SAC_stream_printf(USERDATA_STREAM(userdata), "%s", USERDATA_INDENT(userdata));
  va_start(args, fmt);
  SAC_stream_vprintf(USERDATA_STREAM(userdata), fmt, args);
  va_end(args);
}



static void dump_condition(SAC_Stream out, const SAC_Condition *condition) {
  switch (condition->conditionType) {
    case SAC_ONE_OF_ATTRIBUTE_CONDITION:
    case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
    case SAC_ATTRIBUTE_CONDITION:
    case SAC_CLASS_CONDITION:
    case SAC_PSEUDO_CLASS_CONDITION:
    case SAC_ID_CONDITION:
      SAC_stream_printf(out, "cond_");
      switch (condition->conditionType) {
        case SAC_ID_CONDITION:
          SAC_stream_printf(out, "id");
          break;
        case SAC_CLASS_CONDITION:
          SAC_stream_printf(out, "class");
          break;
        case SAC_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "attr");
          break;
        case SAC_ONE_OF_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "one_of");
          break;
        case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "begin_hypen");
          break;
        case SAC_PSEUDO_CLASS_CONDITION:
          SAC_stream_printf(out, "pseudo_class");
          break;
        default:
          SAC_stream_printf(out, "unknown_%d", condition->conditionType);
          break;
      }
      SAC_stream_printf(out, "(");

      if (condition->desc.attribute.namespaceURI != NULL) {
        SAC_stream_printf(out, "'%s'", condition->desc.attribute.namespaceURI);
      } else {
        SAC_stream_printf(out, "NULL");
      }

      SAC_stream_printf(out, ", ");

      if (condition->desc.attribute.localName != NULL) {
        SAC_stream_printf(out, "'%s'", condition->desc.attribute.localName);
      } else {
        SAC_stream_printf(out, "NULL");
      }

      SAC_stream_printf(out, ", %s, ",
        condition->desc.attribute.specified == SAC_TRUE ? "true" : "false");

      if (condition->desc.attribute.value != NULL) {
        SAC_stream_printf(out, "'%s'", condition->desc.attribute.value);
      } else {
        SAC_stream_printf(out, "NULL");
      }

      SAC_stream_printf(out, ")");
      break;
    case SAC_AND_CONDITION:
      SAC_stream_printf(out, "cond_and(");
      dump_condition(out, condition->desc.combinator.firstCondition);
      SAC_stream_printf(out, ", ");
      dump_condition(out, condition->desc.combinator.secondCondition);
      SAC_stream_printf(out, ")");
      break;
    case SAC_OR_CONDITION:
    case SAC_NEGATIVE_CONDITION:
    case SAC_POSITIONAL_CONDITION:
    case SAC_LANG_CONDITION:
    case SAC_ONLY_CHILD_CONDITION:
    case SAC_ONLY_TYPE_CONDITION:
    case SAC_CONTENT_CONDITION:
      SAC_stream_printf(out, "cond(unknown %d)", condition->conditionType);
      break;
  }
}



static void dump_node_type(SAC_Stream out, SAC_nodeType type) {
  switch (type) {
    case ELEMENT_NODE:
      SAC_stream_printf(out, "ELEMENT");
      break;
    case ATTRIBUTE_NODE:
      SAC_stream_printf(out, "ATTRIBUTE");
      break;
    case TEXT_NODE:
      SAC_stream_printf(out, "TEXT");
      break;
    case CDATA_NODE:
      SAC_stream_printf(out, "CDATA");
      break;
    case ENTITY_REFERENCE_NODE:
      SAC_stream_printf(out, "ENTITY_REFERENCE");
      break;
    case PROCESSING_INSTRUCTION_NODE:
      SAC_stream_printf(out, "PROCESSING_INSTRUCTION");
      break;
    case COMMENT_NODE:
      SAC_stream_printf(out, "COMMENT");
      break;
    case ANY_NODE:
      SAC_stream_printf(out, "ANY");
      break;
  }
}



static void dump_selector(SAC_Stream out, const SAC_Selector *selector) {
  switch (selector->selectorType) {
    case SAC_CONDITIONAL_SELECTOR:
      SAC_stream_printf(out, "sel_cond(");
      dump_selector(out, selector->desc.conditional.simpleSelector);
      SAC_stream_printf(out, ", ");
      dump_condition(out, selector->desc.conditional.condition);
      SAC_stream_printf(out, ")");
      break;
    case SAC_ANY_NODE_SELECTOR:
      SAC_stream_printf(out, "sel_any");
      break;
    case SAC_ELEMENT_NODE_SELECTOR:
      SAC_stream_printf(out, "sel_el(");

      if (selector->desc.element.namespaceURI != NULL) {
        SAC_stream_printf(out, selector->desc.element.namespaceURI);
      } else {
        SAC_stream_printf(out, "NULL");
      }

      SAC_stream_printf(out, ", ");

      if (selector->desc.element.localName != NULL) {
        SAC_stream_printf(out, selector->desc.element.localName);
      } else {
        SAC_stream_printf(out, "NULL");
      }

      SAC_stream_printf(out, ")");
      break;
    case SAC_DESCENDANT_SELECTOR:
    case SAC_CHILD_SELECTOR:
      SAC_stream_printf(out, "sel_");
      switch (selector->selectorType) {
        case SAC_DESCENDANT_SELECTOR:
          SAC_stream_printf(out, "desc");
          break;
        case SAC_CHILD_SELECTOR:
          SAC_stream_printf(out, "child");
          break;
        default:
          SAC_stream_printf(out, "unknown_%d", selector->selectorType);
          break;
      }
      SAC_stream_printf(out, "(");
      dump_selector(out, selector->desc.descendant.descendantSelector);
      SAC_stream_printf(out, ", ");
      dump_selector(out, selector->desc.descendant.simpleSelector);
      SAC_stream_printf(out, ")");
      break;
    case SAC_DIRECT_ADJACENT_SELECTOR:
      SAC_stream_printf(out, "sel_sib(");
      dump_node_type(out, selector->desc.sibling.nodeType);
      SAC_stream_printf(out, ", ");
      dump_selector(out, selector->desc.sibling.firstSelector);
      SAC_stream_printf(out, ", ");
      dump_selector(out, selector->desc.sibling.secondSelector);
      SAC_stream_printf(out, ")");
      break;
    case SAC_PSEUDO_ELEMENT_SELECTOR:
    case SAC_ROOT_NODE_SELECTOR:
    case SAC_NEGATIVE_SELECTOR:
    case SAC_TEXT_NODE_SELECTOR:
    case SAC_CDATA_SECTION_NODE_SELECTOR:
    case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR:
    case SAC_COMMENT_NODE_SELECTOR:
      SAC_stream_printf(out, "sel(unknown %d)", selector->selectorType);
      break;
  }
}



static void dump_selectors(SAC_Stream out, const SAC_Selector **value) {
  const SAC_Selector **it;

  if (value == NULL) {
    SAC_stream_printf(out, "NULL\n");
    return;
  }

  for (it = value; *it != NULL; ++it) {
    dump_selector(out, *it);
    SAC_stream_printf(out, "\n");
  }
}



static void start_document(void *userData) {
  userdata_printf(userData, "doc {\n");
  userdata_inc_indent(userData);
}



static void end_document(void *userData) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "doc }\n");
}



static void start_style(void *userData, const SAC_Selector *selectors[]) {
  userdata_printf(userData, "style\n");
  dump_selectors(USERDATA_STREAM(userData), selectors);
  userdata_printf(userData, "{\n");
  userdata_inc_indent(userData);
}



static void end_style(
  void *userData,
  const SAC_Selector *selectors[])
{
  userdata_dec_indent(userData);
  userdata_printf(userData, "style\n");
  dump_selectors(USERDATA_STREAM(userData), selectors);
  userdata_printf(userData, "}\n");
}



static void dump_lexical_unit(SAC_Stream out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    SAC_stream_printf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_OPERATOR_COMMA:
        SAC_stream_printf(out, ",");
        break;
      case SAC_INHERIT:
        SAC_stream_printf(out, "inherit");
        break;
      case SAC_INTEGER:
        SAC_stream_printf(out, "int(%li)", value->desc.integer);
        break;
      case SAC_REAL:
        SAC_stream_printf(out, "real(%g)", value->desc.real);
        break;
      case SAC_LENGTH_EM:
        SAC_stream_printf(out, "em(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_EX:
        SAC_stream_printf(out, "ex(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PIXEL:
        SAC_stream_printf(out, "pixel(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_INCH:
        SAC_stream_printf(out, "inch(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_CENTIMETER:
        SAC_stream_printf(out, "centimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_MILLIMETER:
        SAC_stream_printf(out, "millimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_POINT:
        SAC_stream_printf(out, "point(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PICA:
        SAC_stream_printf(out, "pica(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_PERCENTAGE:
        SAC_stream_printf(out, "percentage(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_DEGREE:
        SAC_stream_printf(out, "degree(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_GRADIAN:
        SAC_stream_printf(out, "gradian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_RADIAN:
        SAC_stream_printf(out, "radian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_MILLISECOND:
        SAC_stream_printf(out, "msecs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_SECOND:
        SAC_stream_printf(out, "secs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_HERTZ:
        SAC_stream_printf(out, "hertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_KILOHERTZ:
        SAC_stream_printf(out, "khertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_URI:
        SAC_stream_printf(out, "uri('%s')", value->desc.uri);
        break;
      case SAC_RGBCOLOR:
      case SAC_FUNCTION:
        {
          SAC_LexicalUnit **arg;

          SAC_stream_printf(out, "func('%s')", value->desc.function.name);
          for (arg = value->desc.function.parameters; *arg != NULL; ++arg) {
            SAC_stream_printf(out, " arg(");
            dump_lexical_unit(out, *arg);
            SAC_stream_printf(out, ")");
          }
        }
        break;
      case SAC_IDENT:
        SAC_stream_printf(out, "ident('%s')", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        SAC_stream_printf(out, "str('%s')", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        SAC_stream_printf(out, "urange('%s')", value->desc.unicodeRange);
        break;
      case SAC_SUB_EXPRESSION:
        {
          SAC_LexicalUnit **sub;

          for (sub = value->desc.subValues; *sub != NULL; ++sub) {
            if (sub != value->desc.subValues) SAC_stream_printf(out, " ");
            SAC_stream_printf(out, "sub(");
            dump_lexical_unit(out, *sub);
            SAC_stream_printf(out, ")");
          }
        }
        break;
      case SAC_OPERATOR_PLUS:
      case SAC_OPERATOR_MINUS:
      case SAC_OPERATOR_MULTIPLY:
      case SAC_OPERATOR_SLASH:
      case SAC_OPERATOR_MOD:
      case SAC_OPERATOR_EXP:
      case SAC_OPERATOR_LT:
      case SAC_OPERATOR_GT:
      case SAC_OPERATOR_LE:
      case SAC_OPERATOR_GE:
      case SAC_OPERATOR_TILDE:
      case SAC_COUNTER_FUNCTION:
      case SAC_COUNTERS_FUNCTION:
      case SAC_ATTR:
      case SAC_RECT_FUNCTION:
      case SAC_DIMENSION:
        break;
    };
  }
}



static void property(
  void *userData,
  const SAC_STRING propertyName,
  const SAC_LexicalUnit *value,
  SAC_Boolean important)
{
  userdata_printf(userData, "prop('%s') ", propertyName);
  dump_lexical_unit(USERDATA_STREAM(userData), value);
  if (important == SAC_TRUE)
    SAC_stream_printf(USERDATA_STREAM(userData), " important");
  SAC_stream_printf(USERDATA_STREAM(userData), "\n");
}



void dump_media(SAC_Stream out, const SAC_STRING media[]) {
  const SAC_STRING *m;

  for (m = media; *m != NULL; ++m) {
    if (m != media) SAC_stream_printf(out, ", ");
    SAC_stream_printf(out, "medium('%s')", *m);
  }
}



void import(void *userData,
  const SAC_STRING base,
  const SAC_STRING uri,
  const SAC_STRING media[],
  const SAC_STRING defaultNamepaceURI)
{
  userdata_printf(userData, "import(");

  if (base != NULL) {
    SAC_stream_printf(USERDATA_STREAM(userData), "'%s'", base);
  } else {
    SAC_stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  SAC_stream_printf(USERDATA_STREAM(userData), ", ");

  if (uri != NULL) {
    SAC_stream_printf(USERDATA_STREAM(userData), "'%s'", uri);
  } else {
    SAC_stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  SAC_stream_printf(USERDATA_STREAM(userData), ", ");

  if (defaultNamepaceURI != NULL) {
    SAC_stream_printf(USERDATA_STREAM(userData), "'%s'", defaultNamepaceURI);
  } else {
    SAC_stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  SAC_stream_printf(USERDATA_STREAM(userData), ") ");

  dump_media(USERDATA_STREAM(userData), media);

  SAC_stream_printf(USERDATA_STREAM(userData), "\n");
}



void namespace_declaration(void *userData,
  const SAC_STRING prefix, const SAC_STRING uri)
{
  userdata_printf(userData, "namespace(");

  if (prefix != NULL) {
    SAC_stream_printf(USERDATA_STREAM(userData), "'%s'", prefix);
  } else {
    SAC_stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  SAC_stream_printf(USERDATA_STREAM(userData), ", ");

  if (uri != NULL) {
    SAC_stream_printf(USERDATA_STREAM(userData), "'%s'", uri);
  } else {
    SAC_stream_printf(USERDATA_STREAM(userData), "NULL");
  }
  SAC_stream_printf(USERDATA_STREAM(userData), ")\n");
}



void start_media(void *userData, const SAC_STRING media[]) {
  userdata_printf(userData, "media(");
  dump_media(USERDATA_STREAM(userData), media);
  SAC_stream_printf(USERDATA_STREAM(userData), ") {\n");
  userdata_inc_indent(userData);
}



void end_media(void *userData, const SAC_STRING media[]) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "media(");
  dump_media(USERDATA_STREAM(userData), media);
  SAC_stream_printf(USERDATA_STREAM(userData), ") }\n");
}



static SAC_Parser create_parser(SAC_Stream stream) {
  struct userdata_t *userData;
  SAC_Parser parser;
  
  userData = userdata_open(stream);

  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetNamespaceDeclarationHandler(parser, namespace_declaration);
  SAC_SetImportHandler(parser, import);
  SAC_SetMediaHandler(parser, start_media, end_media);
  SAC_SetStyleHandler(parser, start_style, end_style);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, userData);
  return parser;
}



static void dispose_parser(SAC_Parser parser) {
  userdata_close(SAC_GetUserData(parser));
  SAC_DisposeParser(parser);
}



static void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



static void parse_styledeclaration(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleDeclaration(parser, buffer, strlen(buffer));
}



static const SAC_Selector** parse_selector(SAC_Parser parser,
  const char *buffer)
{
  return SAC_ParseSelectors(parser, buffer, strlen(buffer));
}



static void parse_rule(SAC_Parser parser, const char *buffer) {
  SAC_ParseRule(parser, buffer, strlen(buffer));
}



static void test_parser_styledeclaration() {
  SAC_Stream parser_stream = SAC_stream_open();
  SAC_Stream css = SAC_stream_open();
  SAC_Stream match_stream = SAC_stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  SAC_stream_printf(css, "prop-ident : ident;\n");
  SAC_stream_printf(css, "prop-inherit : inherit;\n");
  SAC_stream_printf(css, "prop-string : 'string' \"string\";\n");
  SAC_stream_printf(css, "prop-uri1 : url( \t\r\n\f\"uri\"\f\n\r\t );\n");
  SAC_stream_printf(css, "prop-uri2 : "
    "url( \t\r\n\fhttp://example.com/\f\n\r\t );\n");
  SAC_stream_printf(css, "prop-unicode : "
    "U+A5, U+0-7F, U+590-5ff, U+4E00-9FFF, U+30??;\n");
  SAC_stream_printf(css, "prop-int : 149 -148;\n");
  SAC_stream_printf(css, "prop-real : 3.14 -2.71;\n");
  SAC_stream_printf(css, "prop-percentage : 149%% -148%%;\n");
  SAC_stream_printf(css, "prop-freq : 50.1hz 5.1khz;\n");
  SAC_stream_printf(css, "prop-time : 25.1S 50.2MS;\n");
  SAC_stream_printf(css, "prop-angle : 90.0DEG 1.57RAD 100.0GRAD;\n");
  SAC_stream_printf(css, "prop-em : 1.1EM;\n");
  SAC_stream_printf(css, "prop-ex : 1.2EX;\n");
  SAC_stream_printf(css, "prop-px : 1.3PX;\n");
  SAC_stream_printf(css, "prop-cm : 1.4CM;\n");
  SAC_stream_printf(css, "prop-mm : 1.5MM;\n");
  SAC_stream_printf(css, "prop-in : 1.6IN;\n");
  SAC_stream_printf(css, "prop-pt : 1.7PT;\n");
  SAC_stream_printf(css, "prop-pc : 1.8PC;\n");
  SAC_stream_printf(css, "prop-func : foo('arg') bar('arg1', 2);\n");
  SAC_stream_printf(css, "prop-color : #ab #aBc #abcde #aBCdef;\n");
  parse_styledeclaration(parser, SAC_stream_str(css));
  SAC_stream_close(css);

  dispose_parser(parser);

  SAC_stream_printf(match_stream, "doc {\n");
  SAC_stream_printf(match_stream, "  prop('prop-ident') ident('ident')\n");
  SAC_stream_printf(match_stream, "  prop('prop-inherit') inherit\n");
  SAC_stream_printf(match_stream, "  prop('prop-string') "
    "sub(str('string')) sub(str('string'))\n");
  SAC_stream_printf(match_stream, "  prop('prop-uri1') uri('uri')\n");
  SAC_stream_printf(match_stream, "  prop('prop-uri2') "
    "uri('http://example.com/')\n");
  SAC_stream_printf(match_stream, "  prop('prop-unicode') "
    "sub(urange('U+A5')) sub(,) "
    "sub(urange('U+0-7F')) sub(,) "
    "sub(urange('U+590-5ff')) sub(,) "
    "sub(urange('U+4E00-9FFF')) sub(,) "
    "sub(urange('U+30\?\?'))\n");
  SAC_stream_printf(match_stream, "  prop('prop-int') "
    "sub(int(149)) sub(int(-148))\n");
  SAC_stream_printf(match_stream, "  prop('prop-real') "
    "sub(real(3.14)) sub(real(-2.71))\n");
  SAC_stream_printf(match_stream, "  prop('prop-percentage') "
    "sub(percentage(149%%)) sub(percentage(-148%%))\n");
  SAC_stream_printf(match_stream, "  prop('prop-freq') "
    "sub(hertz(50.1Hz)) sub(khertz(5.1kHz))\n");
  SAC_stream_printf(match_stream, "  prop('prop-time') "
    "sub(secs(25.1s)) sub(msecs(50.2ms))\n");
  SAC_stream_printf(match_stream, "  prop('prop-angle') "
    "sub(degree(90deg)) sub(radian(1.57rad)) sub(gradian(100grad))\n");
  SAC_stream_printf(match_stream, "  prop('prop-em') em(1.1em)\n");
  SAC_stream_printf(match_stream, "  prop('prop-ex') ex(1.2ex)\n");
  SAC_stream_printf(match_stream, "  prop('prop-px') pixel(1.3px)\n");
  SAC_stream_printf(match_stream, "  prop('prop-cm') centimeter(1.4cm)\n");
  SAC_stream_printf(match_stream, "  prop('prop-mm') millimeter(1.5mm)\n");
  SAC_stream_printf(match_stream, "  prop('prop-in') inch(1.6in)\n");
  SAC_stream_printf(match_stream, "  prop('prop-pt') point(1.7pt)\n");
  SAC_stream_printf(match_stream, "  prop('prop-pc') pica(1.8pc)\n");
  SAC_stream_printf(match_stream, "  prop('prop-func') "
    "sub(func('foo') arg(str('arg'))) "
    "sub(func('bar') arg(str('arg1')) arg(,) arg(int(2)))\n");
  SAC_stream_printf(match_stream, "  prop('prop-color') "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(10)) arg(,) arg(int(11)) arg(,) arg(int(12))) "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(171)) arg(,) "
                    "arg(int(205)) arg(,) "
                    "arg(int(239)))\n");
  SAC_stream_printf(match_stream, "doc }\n");
  assert_equals(SAC_stream_str(match_stream), SAC_stream_str(parser_stream));
  SAC_stream_close(match_stream);

  SAC_stream_close(parser_stream);
}



static void test_parser_selector() {
  const SAC_Selector **selectors;
  SAC_Stream parser_stream = SAC_stream_open();
  SAC_Stream css = SAC_stream_open();
  SAC_Stream selector_stream;
  SAC_Stream match_stream;
  SAC_Parser parser = create_parser(parser_stream);
  
  SAC_stream_printf(css, "#some-id\n");
  SAC_stream_printf(css, ", div.some-class\n");
  SAC_stream_printf(css, ", .some-class\n");
  SAC_stream_printf(css, ", .class1#some-id.class2\n");
  SAC_stream_printf(css, ", "
    "[attr][iattr=ident][sattr=\"str\"][oneof~=\"inc\"][hypen|=\"dash\"]\n");
  SAC_stream_printf(css, ", :pseudo-class\n");
  SAC_stream_printf(css, ", *.foo:bar\n");
  SAC_stream_printf(css, ", a b c\n");
  SAC_stream_printf(css, ", a + b + c\n");
  SAC_stream_printf(css, ", a > b > c\n");
  selectors = parse_selector(parser, SAC_stream_str(css));
  SAC_stream_close(css);

  selector_stream = SAC_stream_open();
  dump_selectors(selector_stream, selectors);

  match_stream = SAC_stream_open();
  SAC_stream_printf(match_stream,
    "sel_cond(sel_any, cond_id(NULL, 'id', true, 'some-id'))\n");
  SAC_stream_printf(match_stream,
    "sel_cond("
      "sel_el(NULL, div), "
      "cond_class(NULL, 'class', true, 'some-class'))\n");
  SAC_stream_printf(match_stream,
    "sel_cond(sel_any, cond_class(NULL, 'class', true, 'some-class'))\n");
  SAC_stream_printf(match_stream,
    "sel_cond("
      "sel_any, "
      "cond_and("
        "cond_and("
          "cond_class(NULL, 'class', true, 'class1'), "
          "cond_id(NULL, 'id', true, 'some-id')), "
        "cond_class(NULL, 'class', true, 'class2')))\n");
  SAC_stream_printf(match_stream,
    "sel_cond("
      "sel_any, "
      "cond_and("
        "cond_and("
          "cond_and("
            "cond_and("
              "cond_attr(NULL, 'attr', false, NULL), "
              "cond_attr(NULL, 'iattr', true, 'ident')), "
            "cond_attr(NULL, 'sattr', true, 'str')), "
          "cond_one_of(NULL, 'oneof', true, 'inc')), "
        "cond_begin_hypen(NULL, 'hypen', true, 'dash')))\n");
  SAC_stream_printf(match_stream, 
    "sel_cond("
      "sel_any, "
      "cond_pseudo_class(NULL, 'pseudo-class', false, NULL))\n");
  SAC_stream_printf(match_stream, 
    "sel_cond("
      "sel_any, "
      "cond_and("
        "cond_class(NULL, 'class', true, 'foo'), "
        "cond_pseudo_class(NULL, 'bar', false, NULL)))\n");
  SAC_stream_printf(match_stream, 
    "sel_desc("
      "sel_desc("
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  SAC_stream_printf(match_stream, 
    "sel_sib("
      "ANY, "
      "sel_sib("
        "ANY, "
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  SAC_stream_printf(match_stream, 
    "sel_child("
      "sel_child("
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  assert_equals(SAC_stream_str(match_stream), SAC_stream_str(selector_stream));
  SAC_stream_close(match_stream);
  SAC_stream_close(selector_stream);

  dispose_parser(parser);

  match_stream = SAC_stream_open();
  SAC_stream_printf(match_stream, "doc {\n");
  SAC_stream_printf(match_stream, "doc }\n");
  assert_equals(SAC_stream_str(match_stream), SAC_stream_str(parser_stream));
  SAC_stream_close(match_stream);

  SAC_stream_close(parser_stream);
}



static void test_parser_rule() {
  SAC_Stream parser_stream = SAC_stream_open();
  SAC_Stream css = SAC_stream_open();
  SAC_Stream match_stream = SAC_stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  SAC_stream_printf(css, "element {\n");
  SAC_stream_printf(css, "  prop : ident;\n");
  SAC_stream_printf(css, "}\n");
  parse_rule(parser, SAC_stream_str(css));
  SAC_stream_close(css);

  dispose_parser(parser);

  SAC_stream_printf(match_stream, "doc {\n");
  SAC_stream_printf(match_stream, "  style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, element)\n");
  SAC_stream_printf(match_stream, "  {\n");
  SAC_stream_printf(match_stream, "    prop('prop') ident('ident')\n");
  SAC_stream_printf(match_stream, "  style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, element)\n");
  SAC_stream_printf(match_stream, "  }\n");
  SAC_stream_printf(match_stream, "doc }\n");
  assert_equals(SAC_stream_str(match_stream), SAC_stream_str(parser_stream));
  SAC_stream_close(match_stream);

  SAC_stream_close(parser_stream);
}



static void test_parser_stylesheet() {
  SAC_Stream parser_stream = SAC_stream_open();
  SAC_Stream css = SAC_stream_open();
  SAC_Stream match_stream = SAC_stream_open();
  SAC_Parser parser = create_parser(parser_stream);

  assert(SAC_GetBase(parser) == NULL);
  SAC_SetBase(parser, "http://example.com/");
  assert_equals("http://example.com/", SAC_GetBase(parser));
  
  SAC_stream_printf(css, "@import url(\"all.css\");\n");
  SAC_stream_printf(css, "@import url(\"bluish.css\") projection, tv;\n");
  SAC_stream_printf(css, "@namespace \"\";\n");
  SAC_stream_printf(css, "@namespace svg url('http://www.w3.org/2000/svg');\n");
  SAC_stream_printf(css, "element {\n");
  SAC_stream_printf(css, "  prop : ident;\n");
  SAC_stream_printf(css, "}\n");
  SAC_stream_printf(css, "@media screen, print {\n");
  SAC_stream_printf(css, "  body {\n");
  SAC_stream_printf(css, "    line-height: 1.2\n");
  SAC_stream_printf(css, "  }\n");
  SAC_stream_printf(css, "}\n");
  parse_stylesheet(parser, SAC_stream_str(css));
  SAC_stream_close(css);

  dispose_parser(parser);

  SAC_stream_printf(match_stream, "doc {\n");
  SAC_stream_printf(match_stream,
    "  import('http://example.com/', 'all.css', NULL) \n");
  SAC_stream_printf(match_stream,
    "  import('http://example.com/', 'bluish.css', NULL) "
      "medium('projection'), medium('tv')\n");
  SAC_stream_printf(match_stream, "  namespace(NULL, '')\n");
  SAC_stream_printf(match_stream,
    "  namespace('svg', 'http://www.w3.org/2000/svg')\n");

  SAC_stream_printf(match_stream, "  style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, element)\n");
  SAC_stream_printf(match_stream, "  {\n");
  SAC_stream_printf(match_stream, "    prop('prop') ident('ident')\n");
  SAC_stream_printf(match_stream, "  style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, element)\n");
  SAC_stream_printf(match_stream, "  }\n");
  SAC_stream_printf(match_stream, "  media(medium('screen'), medium('print')) {\n");
  SAC_stream_printf(match_stream, "    style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, body)\n");
  SAC_stream_printf(match_stream, "    {\n");
  SAC_stream_printf(match_stream, "      prop('line-height') real(1.2)\n");
  SAC_stream_printf(match_stream, "    style\n");
  SAC_stream_printf(match_stream, "sel_el(NULL, body)\n");
  SAC_stream_printf(match_stream, "    }\n");
  SAC_stream_printf(match_stream, "  media(medium('screen'), medium('print')) }\n");
  SAC_stream_printf(match_stream, "doc }\n");
  assert_equals(SAC_stream_str(match_stream), SAC_stream_str(parser_stream));
  SAC_stream_close(match_stream);

  SAC_stream_close(parser_stream);
}



void test_parser() {
  test_parser_styledeclaration();
  test_parser_selector();
  test_parser_rule();
  test_parser_stylesheet();
}
