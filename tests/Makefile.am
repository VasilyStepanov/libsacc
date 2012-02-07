noinst_PROGRAMS = test_suite \
                  sacc

AM_CFLAGS = -Wall -Wextra -Werror -ansi -pedantic -D_XOPEN_SOURCE=700 \
            -I$(top_srcdir)/include -I$(top_srcdir)/src

XML_FORMAT = xmllint --format


test_suite_SOURCES = test_mpool.c \
                     test_list.c \
                     test_vector.c \
                     test_lexical_unit.c \
                     test_condition.c \
                     test_selector.c \
                     test_media_query.c \
                     test_suite.c

test_suite_LDADD = $(top_srcdir)/src/libsacc.la



sacc_SOURCES = sacc.c \
               parser.c

sacc_LDADD = $(top_srcdir)/src/libsacc.la



test: test_suite sacc
	./test_suite
	@for css in data/*_*.css; do \
	  type=`expr match "$$css" 'data/[0-9]\+_\([a-z]\+\)'`; \
	  echo "$$css"; \
	  xml=$${css/%css/xml}; \
	  $(XML_FORMAT) $$xml > expected.xml; \
	  cat "$$css" | ./sacc -t $$type > received.xml; \
		$(XML_FORMAT) received.xml > received.fmt.xml; \
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
