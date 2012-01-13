#include "test_parser.h"

#include "test_utils.h"
#include "gcc.h"

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
  stream_t stream;
  char *indent;
};



static void* userdata_open(stream_t stream) {
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

  stream_printf(USERDATA_STREAM(userdata), "%s", USERDATA_INDENT(userdata));
  va_start(args, fmt);
  stream_vprintf(USERDATA_STREAM(userdata), fmt, args);
  va_end(args);
}



static void dump_condition(stream_t out, const SAC_Condition *condition) {
  switch (condition->conditionType) {
    case SAC_ONE_OF_ATTRIBUTE_CONDITION:
    case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
    case SAC_ATTRIBUTE_CONDITION:
    case SAC_CLASS_CONDITION:
    case SAC_PSEUDO_CLASS_CONDITION:
    case SAC_ID_CONDITION:
      stream_printf(out, "cond_");
      switch (condition->conditionType) {
        case SAC_ID_CONDITION:
          stream_printf(out, "id");
          break;
        case SAC_CLASS_CONDITION:
          stream_printf(out, "class");
          break;
        case SAC_ATTRIBUTE_CONDITION:
          stream_printf(out, "attr");
          break;
        case SAC_ONE_OF_ATTRIBUTE_CONDITION:
          stream_printf(out, "one_of");
          break;
        case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
          stream_printf(out, "begin_hypen");
          break;
        case SAC_PSEUDO_CLASS_CONDITION:
          stream_printf(out, "pseudo_class");
          break;
        default:
          stream_printf(out, "unknown_%d", condition->conditionType);
          break;
      }
      stream_printf(out, "(");

      if (condition->desc.attribute.namespaceURI != NULL) {
        stream_printf(out, "'%s'", condition->desc.attribute.namespaceURI);
      } else {
        stream_printf(out, "NULL");
      }

      stream_printf(out, ", ");

      if (condition->desc.attribute.localName != NULL) {
        stream_printf(out, "'%s'", condition->desc.attribute.localName);
      } else {
        stream_printf(out, "NULL");
      }

      stream_printf(out, ", %s, ",
        condition->desc.attribute.specified == SAC_TRUE ? "true" : "false");

      if (condition->desc.attribute.value != NULL) {
        stream_printf(out, "'%s'", condition->desc.attribute.value);
      } else {
        stream_printf(out, "NULL");
      }

      stream_printf(out, ")");
      break;
    case SAC_AND_CONDITION:
      stream_printf(out, "cond_and(");
      dump_condition(out, condition->desc.combinator.firstCondition);
      stream_printf(out, ", ");
      dump_condition(out, condition->desc.combinator.secondCondition);
      stream_printf(out, ")");
      break;
    case SAC_OR_CONDITION:
    case SAC_NEGATIVE_CONDITION:
    case SAC_POSITIONAL_CONDITION:
    case SAC_LANG_CONDITION:
    case SAC_ONLY_CHILD_CONDITION:
    case SAC_ONLY_TYPE_CONDITION:
    case SAC_CONTENT_CONDITION:
      stream_printf(out, "cond(unknown %d)", condition->conditionType);
      break;
  }
}



static void dump_node_type(stream_t out, SAC_nodeType type) {
  switch (type) {
    case ELEMENT_NODE:
      stream_printf(out, "ELEMENT");
      break;
    case ATTRIBUTE_NODE:
      stream_printf(out, "ATTRIBUTE");
      break;
    case TEXT_NODE:
      stream_printf(out, "TEXT");
      break;
    case CDATA_NODE:
      stream_printf(out, "CDATA");
      break;
    case ENTITY_REFERENCE_NODE:
      stream_printf(out, "ENTITY_REFERENCE");
      break;
    case PROCESSING_INSTRUCTION_NODE:
      stream_printf(out, "PROCESSING_INSTRUCTION");
      break;
    case COMMENT_NODE:
      stream_printf(out, "COMMENT");
      break;
    case ANY_NODE:
      stream_printf(out, "ANY");
      break;
  }
}



static void dump_selector(stream_t out, const SAC_Selector *selector) {
  switch (selector->selectorType) {
    case SAC_CONDITIONAL_SELECTOR:
      stream_printf(out, "sel_cond(");
      dump_selector(out, selector->desc.conditional.simpleSelector);
      stream_printf(out, ", ");
      dump_condition(out, selector->desc.conditional.condition);
      stream_printf(out, ")");
      break;
    case SAC_ANY_NODE_SELECTOR:
      stream_printf(out, "sel_any");
      break;
    case SAC_ELEMENT_NODE_SELECTOR:
      stream_printf(out, "sel_el(");

      if (selector->desc.element.namespaceURI != NULL) {
        stream_printf(out, selector->desc.element.namespaceURI);
      } else {
        stream_printf(out, "NULL");
      }

      stream_printf(out, ", ");

      if (selector->desc.element.localName != NULL) {
        stream_printf(out, selector->desc.element.localName);
      } else {
        stream_printf(out, "NULL");
      }

      stream_printf(out, ")");
      break;
    case SAC_DESCENDANT_SELECTOR:
    case SAC_CHILD_SELECTOR:
      stream_printf(out, "sel_");
      switch (selector->selectorType) {
        case SAC_DESCENDANT_SELECTOR:
          stream_printf(out, "desc");
          break;
        case SAC_CHILD_SELECTOR:
          stream_printf(out, "child");
          break;
        default:
          stream_printf(out, "unknown_%d", selector->selectorType);
          break;
      }
      stream_printf(out, "(");
      dump_selector(out, selector->desc.descendant.descendantSelector);
      stream_printf(out, ", ");
      dump_selector(out, selector->desc.descendant.simpleSelector);
      stream_printf(out, ")");
      break;
    case SAC_DIRECT_ADJACENT_SELECTOR:
      stream_printf(out, "sel_sib(");
      dump_node_type(out, selector->desc.sibling.nodeType);
      stream_printf(out, ", ");
      dump_selector(out, selector->desc.sibling.firstSelector);
      stream_printf(out, ", ");
      dump_selector(out, selector->desc.sibling.secondSelector);
      stream_printf(out, ")");
      break;
    case SAC_PSEUDO_ELEMENT_SELECTOR:
    case SAC_ROOT_NODE_SELECTOR:
    case SAC_NEGATIVE_SELECTOR:
    case SAC_TEXT_NODE_SELECTOR:
    case SAC_CDATA_SECTION_NODE_SELECTOR:
    case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR:
    case SAC_COMMENT_NODE_SELECTOR:
      stream_printf(out, "sel(unknown %d)", selector->selectorType);
      break;
  }
}



static void dump_selectors(stream_t out, const SAC_Selector **value) {
  const SAC_Selector **it;

  if (value == NULL) {
    stream_printf(out, "NULL\n");
    return;
  }

  for (it = value; *it != NULL; ++it) {
    dump_selector(out, *it);
    stream_printf(out, "\n");
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



static void dump_lexical_unit(stream_t out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    stream_printf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_OPERATOR_COMMA:
        stream_printf(out, ",");
        break;
      case SAC_INHERIT:
        stream_printf(out, "inherit");
        break;
      case SAC_INTEGER:
        stream_printf(out, "int(%li)", value->desc.integer);
        break;
      case SAC_REAL:
        stream_printf(out, "real(%g)", value->desc.real);
        break;
      case SAC_LENGTH_EM:
        stream_printf(out, "em(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_EX:
        stream_printf(out, "ex(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PIXEL:
        stream_printf(out, "pixel(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_INCH:
        stream_printf(out, "inch(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_CENTIMETER:
        stream_printf(out, "centimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_MILLIMETER:
        stream_printf(out, "millimeter(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_POINT:
        stream_printf(out, "point(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_LENGTH_PICA:
        stream_printf(out, "pica(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_PERCENTAGE:
        stream_printf(out, "percentage(%g%s)",
          value->desc.dimension.value.sreal, value->desc.dimension.unit);
        break;
      case SAC_DEGREE:
        stream_printf(out, "degree(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_GRADIAN:
        stream_printf(out, "gradian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_RADIAN:
        stream_printf(out, "radian(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_MILLISECOND:
        stream_printf(out, "msecs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_SECOND:
        stream_printf(out, "secs(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_HERTZ:
        stream_printf(out, "hertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_KILOHERTZ:
        stream_printf(out, "khertz(%g%s)",
          value->desc.dimension.value.ureal, value->desc.dimension.unit);
        break;
      case SAC_URI:
        stream_printf(out, "uri('%s')", value->desc.uri);
        break;
      case SAC_RGBCOLOR:
      case SAC_FUNCTION:
        {
          SAC_LexicalUnit **arg;

          stream_printf(out, "func('%s')", value->desc.function.name);
          for (arg = value->desc.function.parameters; *arg != NULL; ++arg) {
            stream_printf(out, " arg(");
            dump_lexical_unit(out, *arg);
            stream_printf(out, ")");
          }
        }
        break;
      case SAC_IDENT:
        stream_printf(out, "ident('%s')", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        stream_printf(out, "str('%s')", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        stream_printf(out, "urange('%s')", value->desc.unicodeRange);
        break;
      case SAC_SUB_EXPRESSION:
        {
          SAC_LexicalUnit **sub;

          for (sub = value->desc.subValues; *sub != NULL; ++sub) {
            if (sub != value->desc.subValues) stream_printf(out, " ");
            stream_printf(out, "sub(");
            dump_lexical_unit(out, *sub);
            stream_printf(out, ")");
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
    stream_printf(USERDATA_STREAM(userData), " important");
  stream_printf(USERDATA_STREAM(userData), "\n");
}



void dump_media(stream_t out, const SAC_STRING media[]) {
  const SAC_STRING *m;

  for (m = media; *m != NULL; ++m) {
    if (m != media) stream_printf(out, ", ");
    stream_printf(out, "medium('%s')", *m);
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
    stream_printf(USERDATA_STREAM(userData), "'%s'", base);
  } else {
    stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  stream_printf(USERDATA_STREAM(userData), ", ");

  if (uri != NULL) {
    stream_printf(USERDATA_STREAM(userData), "'%s'", uri);
  } else {
    stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  stream_printf(USERDATA_STREAM(userData), ", ");

  if (defaultNamepaceURI != NULL) {
    stream_printf(USERDATA_STREAM(userData), "'%s'", defaultNamepaceURI);
  } else {
    stream_printf(USERDATA_STREAM(userData), "NULL");
  }

  stream_printf(USERDATA_STREAM(userData), ") ");

  dump_media(USERDATA_STREAM(userData), media);

  stream_printf(USERDATA_STREAM(userData), "\n");
}



void start_media(void *userData, const SAC_STRING media[]) {
  userdata_printf(userData, "media(");
  dump_media(USERDATA_STREAM(userData), media);
  stream_printf(USERDATA_STREAM(userData), ") {\n");
  userdata_inc_indent(userData);
}



void end_media(void *userData, const SAC_STRING media[]) {
  userdata_dec_indent(userData);
  userdata_printf(userData, "media(");
  dump_media(USERDATA_STREAM(userData), media);
  stream_printf(USERDATA_STREAM(userData), ") }\n");
}



static SAC_Parser create_parser(stream_t stream) {
  struct userdata_t *userData;
  SAC_Parser parser;
  
  userData = userdata_open(stream);

  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
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
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t match_stream = stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  stream_printf(css, "prop-ident : ident;\n");
  stream_printf(css, "prop-inherit : inherit;\n");
  stream_printf(css, "prop-string : 'string' \"string\";\n");
  stream_printf(css, "prop-uri1 : url( \t\r\n\f\"uri\"\f\n\r\t );\n");
  stream_printf(css, "prop-uri2 : "
    "url( \t\r\n\fhttp://example.com/\f\n\r\t );\n");
  stream_printf(css, "prop-unicode : "
    "U+A5, U+0-7F, U+590-5ff, U+4E00-9FFF, U+30??;\n");
  stream_printf(css, "prop-int : 149 -148;\n");
  stream_printf(css, "prop-real : 3.14 -2.71;\n");
  stream_printf(css, "prop-percentage : 149%% -148%%;\n");
  stream_printf(css, "prop-freq : 50.1hz 5.1khz;\n");
  stream_printf(css, "prop-time : 25.1S 50.2MS;\n");
  stream_printf(css, "prop-angle : 90.0DEG 1.57RAD 100.0GRAD;\n");
  stream_printf(css, "prop-em : 1.1EM;\n");
  stream_printf(css, "prop-ex : 1.2EX;\n");
  stream_printf(css, "prop-px : 1.3PX;\n");
  stream_printf(css, "prop-cm : 1.4CM;\n");
  stream_printf(css, "prop-mm : 1.5MM;\n");
  stream_printf(css, "prop-in : 1.6IN;\n");
  stream_printf(css, "prop-pt : 1.7PT;\n");
  stream_printf(css, "prop-pc : 1.8PC;\n");
  stream_printf(css, "prop-func : foo('arg') bar('arg1', 2);\n");
  stream_printf(css, "prop-color : #ab #aBc #abcde #aBCdef;\n");
  parse_styledeclaration(parser, stream_str(css));
  stream_close(css);

  dispose_parser(parser);

  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream, "  prop('prop-ident') ident('ident')\n");
  stream_printf(match_stream, "  prop('prop-inherit') inherit\n");
  stream_printf(match_stream, "  prop('prop-string') "
    "sub(str('string')) sub(str('string'))\n");
  stream_printf(match_stream, "  prop('prop-uri1') uri('uri')\n");
  stream_printf(match_stream, "  prop('prop-uri2') "
    "uri('http://example.com/')\n");
  stream_printf(match_stream, "  prop('prop-unicode') "
    "sub(urange('U+A5')) sub(,) "
    "sub(urange('U+0-7F')) sub(,) "
    "sub(urange('U+590-5ff')) sub(,) "
    "sub(urange('U+4E00-9FFF')) sub(,) "
    "sub(urange('U+30\?\?'))\n");
  stream_printf(match_stream, "  prop('prop-int') "
    "sub(int(149)) sub(int(-148))\n");
  stream_printf(match_stream, "  prop('prop-real') "
    "sub(real(3.14)) sub(real(-2.71))\n");
  stream_printf(match_stream, "  prop('prop-percentage') "
    "sub(percentage(149%%)) sub(percentage(-148%%))\n");
  stream_printf(match_stream, "  prop('prop-freq') "
    "sub(hertz(50.1Hz)) sub(khertz(5.1kHz))\n");
  stream_printf(match_stream, "  prop('prop-time') "
    "sub(secs(25.1s)) sub(msecs(50.2ms))\n");
  stream_printf(match_stream, "  prop('prop-angle') "
    "sub(degree(90deg)) sub(radian(1.57rad)) sub(gradian(100grad))\n");
  stream_printf(match_stream, "  prop('prop-em') em(1.1em)\n");
  stream_printf(match_stream, "  prop('prop-ex') ex(1.2ex)\n");
  stream_printf(match_stream, "  prop('prop-px') pixel(1.3px)\n");
  stream_printf(match_stream, "  prop('prop-cm') centimeter(1.4cm)\n");
  stream_printf(match_stream, "  prop('prop-mm') millimeter(1.5mm)\n");
  stream_printf(match_stream, "  prop('prop-in') inch(1.6in)\n");
  stream_printf(match_stream, "  prop('prop-pt') point(1.7pt)\n");
  stream_printf(match_stream, "  prop('prop-pc') pica(1.8pc)\n");
  stream_printf(match_stream, "  prop('prop-func') "
    "sub(func('foo') arg(str('arg'))) "
    "sub(func('bar') arg(str('arg1')) arg(,) arg(int(2)))\n");
  stream_printf(match_stream, "  prop('prop-color') "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(10)) arg(,) arg(int(11)) arg(,) arg(int(12))) "
    "sub(func('rgb')) "
    "sub(func('rgb') arg(int(171)) arg(,) "
                    "arg(int(205)) arg(,) "
                    "arg(int(239)))\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



static void test_parser_selector() {
  const SAC_Selector **selectors;
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t selector_stream;
  stream_t match_stream;
  SAC_Parser parser = create_parser(parser_stream);
  
  stream_printf(css, "#some-id\n");
  stream_printf(css, ", div.some-class\n");
  stream_printf(css, ", .some-class\n");
  stream_printf(css, ", .class1#some-id.class2\n");
  stream_printf(css, ", "
    "[attr][iattr=ident][sattr=\"str\"][oneof~=\"inc\"][hypen|=\"dash\"]\n");
  stream_printf(css, ", :pseudo-class\n");
  stream_printf(css, ", *.foo:bar\n");
  stream_printf(css, ", a b c\n");
  stream_printf(css, ", a + b + c\n");
  stream_printf(css, ", a > b > c\n");
  selectors = parse_selector(parser, stream_str(css));
  stream_close(css);

  selector_stream = stream_open();
  dump_selectors(selector_stream, selectors);

  match_stream = stream_open();
  stream_printf(match_stream,
    "sel_cond(sel_any, cond_id(NULL, 'id', true, 'some-id'))\n");
  stream_printf(match_stream,
    "sel_cond("
      "sel_el(NULL, div), "
      "cond_class(NULL, 'class', true, 'some-class'))\n");
  stream_printf(match_stream,
    "sel_cond(sel_any, cond_class(NULL, 'class', true, 'some-class'))\n");
  stream_printf(match_stream,
    "sel_cond("
      "sel_any, "
      "cond_and("
        "cond_and("
          "cond_class(NULL, 'class', true, 'class1'), "
          "cond_id(NULL, 'id', true, 'some-id')), "
        "cond_class(NULL, 'class', true, 'class2')))\n");
  stream_printf(match_stream,
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
  stream_printf(match_stream, 
    "sel_cond("
      "sel_any, "
      "cond_pseudo_class(NULL, 'pseudo-class', false, NULL))\n");
  stream_printf(match_stream, 
    "sel_cond("
      "sel_any, "
      "cond_and("
        "cond_class(NULL, 'class', true, 'foo'), "
        "cond_pseudo_class(NULL, 'bar', false, NULL)))\n");
  stream_printf(match_stream, 
    "sel_desc("
      "sel_desc("
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  stream_printf(match_stream, 
    "sel_sib("
      "ANY, "
      "sel_sib("
        "ANY, "
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  stream_printf(match_stream, 
    "sel_child("
      "sel_child("
        "sel_el(NULL, a), "
        "sel_el(NULL, b)), "
      "sel_el(NULL, c))\n");
  assert_equals(stream_str(match_stream), stream_str(selector_stream));
  stream_close(match_stream);
  stream_close(selector_stream);

  dispose_parser(parser);

  match_stream = stream_open();
  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



static void test_parser_rule() {
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t match_stream = stream_open();
  SAC_Parser parser = create_parser(parser_stream);
  
  stream_printf(css, "element {\n");
  stream_printf(css, "  prop : ident;\n");
  stream_printf(css, "}\n");
  parse_rule(parser, stream_str(css));
  stream_close(css);

  dispose_parser(parser);

  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream, "  style\n");
  stream_printf(match_stream, "sel_el(NULL, element)\n");
  stream_printf(match_stream, "  {\n");
  stream_printf(match_stream, "    prop('prop') ident('ident')\n");
  stream_printf(match_stream, "  style\n");
  stream_printf(match_stream, "sel_el(NULL, element)\n");
  stream_printf(match_stream, "  }\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



static void test_parser_stylesheet() {
  stream_t parser_stream = stream_open();
  stream_t css = stream_open();
  stream_t match_stream = stream_open();
  SAC_Parser parser = create_parser(parser_stream);

  assert(SAC_GetBase(parser) == NULL);
  SAC_SetBase(parser, "http://example.com/");
  assert_equals("http://example.com/", SAC_GetBase(parser));
  
  stream_printf(css, "@import url(\"bluish.css\") projection, tv;\n");
  stream_printf(css, "element {\n");
  stream_printf(css, "  prop : ident;\n");
  stream_printf(css, "}\n");
  stream_printf(css, "@media screen, print {\n");
  stream_printf(css, "  body {\n");
  stream_printf(css, "    line-height: 1.2\n");
  stream_printf(css, "  }\n");
  stream_printf(css, "}\n");
  parse_stylesheet(parser, stream_str(css));
  stream_close(css);

  dispose_parser(parser);

  stream_printf(match_stream, "doc {\n");
  stream_printf(match_stream,
    "  import('http://example.com/', 'bluish.css', NULL) "
      "medium('projection'), medium('tv')\n");
  stream_printf(match_stream, "  style\n");
  stream_printf(match_stream, "sel_el(NULL, element)\n");
  stream_printf(match_stream, "  {\n");
  stream_printf(match_stream, "    prop('prop') ident('ident')\n");
  stream_printf(match_stream, "  style\n");
  stream_printf(match_stream, "sel_el(NULL, element)\n");
  stream_printf(match_stream, "  }\n");
  stream_printf(match_stream, "  media(medium('screen'), medium('print')) {\n");
  stream_printf(match_stream, "    style\n");
  stream_printf(match_stream, "sel_el(NULL, body)\n");
  stream_printf(match_stream, "    {\n");
  stream_printf(match_stream, "      prop('line-height') real(1.2)\n");
  stream_printf(match_stream, "    style\n");
  stream_printf(match_stream, "sel_el(NULL, body)\n");
  stream_printf(match_stream, "    }\n");
  stream_printf(match_stream, "  media(medium('screen'), medium('print')) }\n");
  stream_printf(match_stream, "doc }\n");
  assert_equals(stream_str(match_stream), stream_str(parser_stream));
  stream_close(match_stream);

  stream_close(parser_stream);
}



void test_parser() {
  test_parser_styledeclaration();
  test_parser_selector();
  test_parser_rule();
  test_parser_stylesheet();
}
