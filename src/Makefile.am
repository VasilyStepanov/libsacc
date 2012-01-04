lib_LTLIBRARIES = libsacc.la

noinst_PROGRAMS = test_suite

noinst_LTLIBRARIES = libgrammar.la

GENERIC_CFLAGS = -Wall -Wextra -ansi -pedantic -D_XOPEN_SOURCE=700

AM_CFLAGS = $(GENERIC_CFLAGS) -Werror -I$(top_srcdir)/include

BUILT_SOURCES = grammar.h
AM_YFLAGS = -d
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c



libsacc_ladir = $(includedir)

libsacc_la_HEADERS = ../include/sacc.h

libsacc_la_SOURCES = parser.c

libsacc_la_LIBADD = libgrammar.la


libgrammar_la_SOURCES = lexis.l \
                        grammar.y

libgrammar_la_CFLAGS = $(GENERIC_CFLAGS) 



test_suite_SOURCES = test_suite.c

test_suite_LDADD = libsacc.la



test: test_suite
	./test_suite

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = sacc.pc
 
clean-local:
	rm -rf lexis.c
	rm -rf grammar.c grammar.h

.PHONY: test
