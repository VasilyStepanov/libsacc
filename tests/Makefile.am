noinst_PROGRAMS = test_suite

AM_CFLAGS = -Wall -Wextra -Werror -ansi -pedantic -D_XOPEN_SOURCE=700 \
            -I$(top_srcdir)/include -I$(top_srcdir)/src



test_suite_SOURCES = test_utils.c \
                     test_mpool.c \
                     test_list.c \
                     test_vector.c \
                     test_lexical_unit.c \
                     test_condition.c \
                     test_selector.c \
                     test_parser.c \
                     test_suite.c

test_suite_LDADD = $(top_srcdir)/src/libsacc.la



test: test_suite
	./test_suite

.PHONY: test
