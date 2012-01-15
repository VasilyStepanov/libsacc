noinst_PROGRAMS = test_suite \
                  sacc

AM_CFLAGS = -Wall -Wextra -Werror -ansi -pedantic -D_XOPEN_SOURCE=700 \
            -I$(top_srcdir)/include -I$(top_srcdir)/src



test_suite_SOURCES = stream.c \
                     parser.c \
                     test_utils.c \
                     test_mpool.c \
                     test_list.c \
                     test_vector.c \
                     test_lexical_unit.c \
                     test_condition.c \
                     test_selector.c \
                     test_suite.c

test_suite_LDADD = $(top_srcdir)/src/libsacc.la



sacc_SOURCES = sacc.c \
               stream.c \
               parser.c

sacc_LDADD = $(top_srcdir)/src/libsacc.la



test: test_suite sacc
	./test_suite
	@for css in data/*_*.css; do \
	  type=`expr match "$$css" 'data/.*_\(.*\)_'`; \
	  echo "$$css"; \
	  xml=$${css/%css/xml}; \
	  xmllint --format $$xml > expected.xml; \
	  cat "$$css" | ./sacc -t $$type > received.xml; \
		xmllint --format received.xml > received.fmt.xml; \
	  RES=$$?; \
	  if test $$RES != 0; then \
	    exit 1; \
	  fi; \
		mv received.fmt.xml received.xml; \
		diff expected.xml received.xml; \
	  RES=$$?; \
	  if test $$RES != 0; then \
	    exit 1; \
	  fi; \
	done;

.PHONY: test
