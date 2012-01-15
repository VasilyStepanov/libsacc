#include "parser.h"

#include <string.h>

#define USERDATA_STREAM(userData) (SAC_Stream)(userData)



static void start_document(void *userData) {
  SAC_stream_printf(USERDATA_STREAM(userData), "<document>");
}



static void end_document(void *userData) {
  SAC_stream_printf(USERDATA_STREAM(userData), "</document>");
}



static void namespace_declaration(void *userData,
  const SAC_STRING prefix, const SAC_STRING uri)
{
  SAC_stream_printf(USERDATA_STREAM(userData), "<namespace");
  if (prefix != NULL)
    SAC_stream_printf(USERDATA_STREAM(userData), " prefix=\"%s\"", prefix);
  if (uri != NULL)
    SAC_stream_printf(USERDATA_STREAM(userData), " uri=\"%s\"", uri);
  SAC_stream_printf(USERDATA_STREAM(userData), "/>");
}



static void dump_media(SAC_Stream out, const SAC_STRING media[]) {
  const SAC_STRING *m;

  for (m = media; *m != NULL; ++m)
    SAC_stream_printf(out, "<medium>%s</medium>", *m);
}



static void import(void *userData,
  const SAC_STRING base,
  const SAC_STRING uri,
  const SAC_STRING media[],
  const SAC_STRING defaultNamepaceURI)
{
  SAC_stream_printf(USERDATA_STREAM(userData), "<import");
  if (base != NULL)
    SAC_stream_printf(USERDATA_STREAM(userData), " base=\"%s\"", base);
  if (uri != NULL)
    SAC_stream_printf(USERDATA_STREAM(userData), " uri=\"%s\"", uri);
  if (defaultNamepaceURI != NULL)
    SAC_stream_printf(USERDATA_STREAM(userData),
      " defaultNamepaceURI=\"%s\"", defaultNamepaceURI);
  SAC_stream_printf(USERDATA_STREAM(userData), ">");

  dump_media(USERDATA_STREAM(userData), media);

  SAC_stream_printf(USERDATA_STREAM(userData), "</import>");
}



static void start_media(void *userData, const SAC_STRING media[]) {
  SAC_stream_printf(USERDATA_STREAM(userData), "<media>");
  SAC_stream_printf(USERDATA_STREAM(userData), "<start_mediums>");
  dump_media(USERDATA_STREAM(userData), media);
  SAC_stream_printf(USERDATA_STREAM(userData), "</start_mediums>");
}



static void end_media(void *userData, const SAC_STRING media[]) {
  SAC_stream_printf(USERDATA_STREAM(userData), "<end_mediums>");
  dump_media(USERDATA_STREAM(userData), media);
  SAC_stream_printf(USERDATA_STREAM(userData), "</end_mediums>");
  SAC_stream_printf(USERDATA_STREAM(userData), "</media>");
}



static void dump_condition(SAC_Stream out, const SAC_Condition *condition) {
  switch (condition->conditionType) {
    case SAC_ONE_OF_ATTRIBUTE_CONDITION:
    case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
    case SAC_ATTRIBUTE_CONDITION:
    case SAC_CLASS_CONDITION:
    case SAC_PSEUDO_CLASS_CONDITION:
    case SAC_ID_CONDITION:
      SAC_stream_printf(out, "<condition type=");
      switch (condition->conditionType) {
        case SAC_ID_CONDITION:
          SAC_stream_printf(out, "\"id\"");
          break;
        case SAC_CLASS_CONDITION:
          SAC_stream_printf(out, "\"class\"");
          break;
        case SAC_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "\"attr\"");
          break;
        case SAC_ONE_OF_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "\"one_of\"");
          break;
        case SAC_BEGIN_HYPHEN_ATTRIBUTE_CONDITION:
          SAC_stream_printf(out, "\"begin_hypen\"");
          break;
        case SAC_PSEUDO_CLASS_CONDITION:
          SAC_stream_printf(out, "\"pseudo_class\"");
          break;
        default:
          SAC_stream_printf(out, "\"unknown_%d\"", condition->conditionType);
          break;
      }

      if (condition->desc.attribute.namespaceURI != NULL)
        SAC_stream_printf(out,
          " namespaceURI=\"%s\"", condition->desc.attribute.namespaceURI);

      if (condition->desc.attribute.localName != NULL)
        SAC_stream_printf(out,
          " localName=\"%s\"", condition->desc.attribute.localName);

      SAC_stream_printf(out, " specified=\"%s\"",
        condition->desc.attribute.specified == SAC_TRUE ? "true" : "false");

      if (condition->desc.attribute.value != NULL)
        SAC_stream_printf(out,
          " value=\"%s\"", condition->desc.attribute.value);

      SAC_stream_printf(out, "/>");
      break;
    case SAC_AND_CONDITION:
      SAC_stream_printf(out, "<condition type=\"and\">");
      dump_condition(out, condition->desc.combinator.firstCondition);
      dump_condition(out, condition->desc.combinator.secondCondition);
      SAC_stream_printf(out, "</condition>");
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



static const char* node_type(SAC_nodeType type) {
  switch (type) {
    case ELEMENT_NODE: return "ELEMENT";
    case ATTRIBUTE_NODE: return "ATTRIBUTE";
    case TEXT_NODE: return "TEXT";
    case CDATA_NODE: return "CDATA";
    case ENTITY_REFERENCE_NODE: return "ENTITY_REFERENCE";
    case PROCESSING_INSTRUCTION_NODE: return "PROCESSING_INSTRUCTION";
    case COMMENT_NODE: return "COMMENT";
    case ANY_NODE: return "ANY";
  }
  return "UNKNOWN";
}



static void dump_selector(SAC_Stream out, const SAC_Selector *selector) {
  switch (selector->selectorType) {
    case SAC_CONDITIONAL_SELECTOR:
      SAC_stream_printf(out, "<selector type=\"conditional\">");
      dump_selector(out, selector->desc.conditional.simpleSelector);
      dump_condition(out, selector->desc.conditional.condition);
      SAC_stream_printf(out, "</selector>");
      break;
    case SAC_ANY_NODE_SELECTOR:
      SAC_stream_printf(out, "<selector type=\"any_node\"/>");
      break;
    case SAC_ELEMENT_NODE_SELECTOR:
      SAC_stream_printf(out, "<selector type=\"element_node\"");

      if (selector->desc.element.namespaceURI != NULL)
        SAC_stream_printf(out,
          " namespaceURI=\"%s\"", selector->desc.element.namespaceURI);

      if (selector->desc.element.localName != NULL)
        SAC_stream_printf(out,
          " localName=\"%s\"", selector->desc.element.localName);

      SAC_stream_printf(out, "/>");
      break;
    case SAC_DESCENDANT_SELECTOR:
    case SAC_CHILD_SELECTOR:
      SAC_stream_printf(out, "<selector type=");
      switch (selector->selectorType) {
        case SAC_DESCENDANT_SELECTOR:
          SAC_stream_printf(out, "\"descendant\"");
          break;
        case SAC_CHILD_SELECTOR:
          SAC_stream_printf(out, "\"child\"");
          break;
        default:
          SAC_stream_printf(out, "\"unknown_%d\"", selector->selectorType);
          break;
      }
      SAC_stream_printf(out, ">");
      dump_selector(out, selector->desc.descendant.descendantSelector);
      dump_selector(out, selector->desc.descendant.simpleSelector);
      SAC_stream_printf(out, "</selector>");
      break;
    case SAC_DIRECT_ADJACENT_SELECTOR:
      SAC_stream_printf(out, "<selector type=\"sibling\" nodeType=\"%s\">",
        node_type(selector->desc.sibling.nodeType));
      dump_selector(out, selector->desc.sibling.firstSelector);
      dump_selector(out, selector->desc.sibling.secondSelector);
      SAC_stream_printf(out, "</selector>");
      break;
    case SAC_PSEUDO_ELEMENT_SELECTOR:
    case SAC_ROOT_NODE_SELECTOR:
    case SAC_NEGATIVE_SELECTOR:
    case SAC_TEXT_NODE_SELECTOR:
    case SAC_CDATA_SECTION_NODE_SELECTOR:
    case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR:
    case SAC_COMMENT_NODE_SELECTOR:
      SAC_stream_printf(out,
        "<selector type=\"unknown_%d\"/>", selector->selectorType);
      break;
  }
}



void dump_selectors(SAC_Stream out, const SAC_Selector **value) {
  const SAC_Selector **it;

  if (value == NULL) {
    SAC_stream_printf(out, "NULL");
    return;
  }

  for (it = value; *it != NULL; ++it)
    dump_selector(out, *it);
}



static void start_style(void *userData, const SAC_Selector *selectors[]) {
  SAC_stream_printf(USERDATA_STREAM(userData), "<style>");
  SAC_stream_printf(USERDATA_STREAM(userData), "<start_selectors>");
  dump_selectors(USERDATA_STREAM(userData), selectors);
  SAC_stream_printf(USERDATA_STREAM(userData), "</start_selectors>");
}



static void end_style(
  void *userData,
  const SAC_Selector *selectors[])
{
  SAC_stream_printf(USERDATA_STREAM(userData), "<end_selectors>");
  dump_selectors(USERDATA_STREAM(userData), selectors);
  SAC_stream_printf(USERDATA_STREAM(userData), "</end_selectors>");
  SAC_stream_printf(USERDATA_STREAM(userData), "</style>");
}



static void dump_lexical_unit(SAC_Stream out, const SAC_LexicalUnit *value) {
  if (value == NULL) {
    SAC_stream_printf(out, "NULL");
  } else {
    switch (value->lexicalUnitType) {
      case SAC_OPERATOR_COMMA:
        SAC_stream_printf(out, "<comma/>");
        break;
      case SAC_INHERIT:
        SAC_stream_printf(out, "<inherit/>");
        break;
      case SAC_INTEGER:
        SAC_stream_printf(out, "<int>%li</int>", value->desc.integer);
        break;
      case SAC_REAL:
        SAC_stream_printf(out, "<real>%g</real>", value->desc.real);
        break;
      case SAC_LENGTH_EM:
        SAC_stream_printf(out, "<em dimension=\"%s\">%g</em>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_EX:
        SAC_stream_printf(out, "<ex dimension=\"%s\">%g</ex>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_PIXEL:
        SAC_stream_printf(out, "<pixel dimension=\"%s\">%g</pixel>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_INCH:
        SAC_stream_printf(out, "<inch dimension=\"%s\">%g</inch>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_CENTIMETER:
        SAC_stream_printf(out, "<centimeter dimension=\"%s\">%g</centimeter>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_MILLIMETER:
        SAC_stream_printf(out, "<millimeter dimension=\"%s\">%g</millimeter>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_POINT:
        SAC_stream_printf(out, "<point dimension=\"%s\">%g</point>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_LENGTH_PICA:
        SAC_stream_printf(out, "<pica dimension=\"%s\">%g</pica>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_PERCENTAGE:
        SAC_stream_printf(out, "<percentage dimension=\"%s\">%g</percentage>",
          value->desc.dimension.unit, value->desc.dimension.value.sreal);
        break;
      case SAC_DEGREE:
        SAC_stream_printf(out, "<degree dimension=\"%s\">%g</degree>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_GRADIAN:
        SAC_stream_printf(out, "<gradian dimension=\"%s\">%g</gradian>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_RADIAN:
        SAC_stream_printf(out, "<radian dimension=\"%s\">%g</radian>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_MILLISECOND:
        SAC_stream_printf(out, "<msecs dimension=\"%s\">%g</msecs>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_SECOND:
        SAC_stream_printf(out, "<secs dimension=\"%s\">%g</secs>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_HERTZ:
        SAC_stream_printf(out, "<hertz dimension=\"%s\">%g</hertz>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_KILOHERTZ:
        SAC_stream_printf(out, "<khertz dimension=\"%s\">%g</khertz>",
          value->desc.dimension.unit, value->desc.dimension.value.ureal);
        break;
      case SAC_URI:
        SAC_stream_printf(out, "<uri>%s</uri>", value->desc.uri);
        break;
      case SAC_RGBCOLOR:
      case SAC_FUNCTION:
        {
          SAC_LexicalUnit **arg;

          SAC_stream_printf(out,
            "<func name=\"%s\">", value->desc.function.name);
          for (arg = value->desc.function.parameters; *arg != NULL; ++arg) {
            SAC_stream_printf(out, "<arg>");
            dump_lexical_unit(out, *arg);
            SAC_stream_printf(out, "</arg>");
          }
          SAC_stream_printf(out, "</func>");
        }
        break;
      case SAC_IDENT:
        SAC_stream_printf(out, "<ident>%s</ident>", value->desc.ident);
        break;
      case SAC_STRING_VALUE:
        SAC_stream_printf(out, "<str>%s</str>", value->desc.stringValue);
        break;
      case SAC_UNICODERANGE:
        SAC_stream_printf(out, "<urange>%s</urange>", value->desc.unicodeRange);
        break;
      case SAC_SUB_EXPRESSION:
        {
          SAC_LexicalUnit **sub;

          for (sub = value->desc.subValues; *sub != NULL; ++sub) {
            SAC_stream_printf(out, "<sub>");
            dump_lexical_unit(out, *sub);
            SAC_stream_printf(out, "</sub>");
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
  SAC_stream_printf(USERDATA_STREAM(userData),
    "<property name=\"%s\"", propertyName);
  if (important == SAC_TRUE)
    SAC_stream_printf(USERDATA_STREAM(userData), " important=\"important\"");
  SAC_stream_printf(USERDATA_STREAM(userData), ">");
  dump_lexical_unit(USERDATA_STREAM(userData), value);
  SAC_stream_printf(USERDATA_STREAM(userData), "</property>");
}



SAC_Parser create_parser(SAC_Stream stream) {
  SAC_Parser parser;
  
  parser = SAC_CreateParser();
  
  SAC_SetDocumentHandler(parser, start_document, end_document);
  SAC_SetNamespaceDeclarationHandler(parser, namespace_declaration);
  SAC_SetImportHandler(parser, import);
  SAC_SetMediaHandler(parser, start_media, end_media);
  SAC_SetStyleHandler(parser, start_style, end_style);
  SAC_SetPropertyHandler(parser, property);
  SAC_SetUserData(parser, stream);
  return parser;
}



void dispose_parser(SAC_Parser parser) {
  SAC_DisposeParser(parser);
}



void parse_stylesheet(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleSheet(parser, buffer, strlen(buffer));
}



void parse_styledeclaration(SAC_Parser parser, const char *buffer) {
  SAC_ParseStyleDeclaration(parser, buffer, strlen(buffer));
}



const SAC_Selector** parse_selectors(SAC_Parser parser,
  const char *buffer)
{
  return SAC_ParseSelectors(parser, buffer, strlen(buffer));
}



void parse_rule(SAC_Parser parser, const char *buffer) {
  SAC_ParseRule(parser, buffer, strlen(buffer));
}
