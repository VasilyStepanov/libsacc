#ifndef CHECKS_H
#define CHECKS_H

#include "config.h"

#ifdef SAC_CHECKS
#  include <assert.h>
#  include <strings.h>
#endif



#ifndef SAC_CHECKS
#define SAC_NOP while(0)
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_UNSIGNED(ureal) assert(ureal >= 0)
#else
#  define SAC_CHECK_UNSIGNED(ureal) SAC_NOP
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_STRING_NOT_EQUALS(lhs, rhs) \
     do { \
       int eq = strcasecmp(lhs, rhs); \
       assert(eq != 0); \
     } while (0)
#else
#  define SAC_CHECK_STRING_NOT_EQUALS(lhs, rhs) SAC_NOP
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_SIMPLE_FOR_CONDITIONAL_SELECTOR(simpleSelector) \
     do { \
       switch (simpleSelector->selectorType) { \
         case SAC_ANY_NODE_SELECTOR: \
         case SAC_NEGATIVE_SELECTOR: \
         case SAC_ELEMENT_NODE_SELECTOR: \
         case SAC_PSEUDO_ELEMENT_SELECTOR: \
         case SAC_TEXT_NODE_SELECTOR: \
         case SAC_CDATA_SECTION_NODE_SELECTOR: \
         case SAC_COMMENT_NODE_SELECTOR: \
         case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR: \
           break; \
         default: \
           assert(0); \
       } \
     } while (0)
#else
#  define SAC_CHECK_SIMPLE_FOR_CONDITIONAL_SELECTOR(simpleSelector) SAC_NOP
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_SIMPLE_FOR_DESCENDANT_SELECTOR(simpleSelector) \
     do { \
       switch (simpleSelector->selectorType) { \
         case SAC_CONDITIONAL_SELECTOR: \
         case SAC_ANY_NODE_SELECTOR: \
         case SAC_NEGATIVE_SELECTOR: \
         case SAC_ELEMENT_NODE_SELECTOR: \
         case SAC_PSEUDO_ELEMENT_SELECTOR: \
         case SAC_TEXT_NODE_SELECTOR: \
         case SAC_CDATA_SECTION_NODE_SELECTOR: \
         case SAC_COMMENT_NODE_SELECTOR: \
         case SAC_PROCESSING_INSTRUCTION_NODE_SELECTOR: \
           break; \
         default: \
           assert(0); \
       } \
     } while (0)
#else
#  define SAC_CHECK_SIMPLE_FOR_DESCENDANT_SELECTOR(simpleSelector) SAC_NOP
#endif

#define SAC_CHECK_SIMPLE_FOR_CHILD_SELECTOR(simpleSelector) \
  SAC_CHECK_SIMPLE_FOR_DESCENDANT_SELECTOR(simpleSelector)

#define SAC_CHECK_SIMPLE_FOR_DIRECT_ADJACENT_SELECTOR(simpleSelector) \
  SAC_CHECK_SIMPLE_FOR_DESCENDANT_SELECTOR(simpleSelector)



#ifdef SAC_CHECKS
#  define SAC_CHECK_FIRST_QUERY_FOR_AND_MEDIA_QUERY(query) \
     do { \
       switch (query->mediaQueryType) { \
         case SAC_TYPE_MEDIA_QUERY: \
         case SAC_FEATURE_MEDIA_QUERY: \
         case SAC_AND_MEDIA_QUERY: \
           break; \
         default: \
           assert(0); \
       } \
     } while (0)
#else
#  define SAC_CHECK_SECOND_QUERY_FOR_AND_MEDIA_QUERY(query) SAC_NOP
#endif



#ifdef SAC_CHECKS
#  define SAC_CHECK_SECOND_QUERY_FOR_AND_MEDIA_QUERY(query) \
     do { \
       switch (query->mediaQueryType) { \
         case SAC_FEATURE_MEDIA_QUERY: \
         case SAC_AND_MEDIA_QUERY: \
           break; \
         default: \
           assert(0); \
       } \
     } while (0)
#else
#  define SAC_CHECK_SECOND_QUERY_FOR_AND_MEDIA_QUERY(query) SAC_NOP
#endif



#endif
