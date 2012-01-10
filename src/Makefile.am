lib_LTLIBRARIES = libsacc.la

noinst_PROGRAMS = test_suite

noinst_LTLIBRARIES = libgrammar.la

GENERIC_CFLAGS = -Wall -Wextra -ansi -pedantic -D_XOPEN_SOURCE=700 \
                 -I$(top_srcdir)/include

AM_CFLAGS = $(GENERIC_CFLAGS) -Werror

BUILT_SOURCES = grammar.h
AM_YFLAGS = -d
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c



libsacc_ladir = $(includedir)

libsacc_la_HEADERS = ../include/sacc.h

libsacc_la_SOURCES = parser.c \
                     mlex.c \
                     mpool.c \
                     list.c \
                     mpool_extra.c \
                     string_extra.c \
                     lexical_unit.c \
                     declaration.c

libsacc_la_LIBADD = libgrammar.la



libgrammar_la_SOURCES = lexis.l \
                        grammar.y

libgrammar_la_CFLAGS = $(GENERIC_CFLAGS) -Wno-unused-parameter



test_suite_SOURCES = test_utils.c \
                     test_mpool.c \
                     test_list.c \
                     test_lexical_unit.c \
                     test_parser.c \
                     test_suite.c

test_suite_LDADD = libsacc.la



test: test_suite
	./test_suite

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = sacc.pc
 
clean-local:
	rm -rf lexis.c
	rm -rf grammar.c grammar.h

.PHONY: test
