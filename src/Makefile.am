lib_LTLIBRARIES = libsacc.la

noinst_PROGRAMS = test_suite

AM_CFLAGS = -Wall -Wextra -ansi -Werror -pedantic -D_XOPEN_SOURCE=700 \
            -I$(top_srcdir)/include

libsacc_ladir = $(includedir)

libsacc_la_HEADERS = ../include/sacc.h

libsacc_la_SOURCES = parser.c \
                     css.yy.c \
                     css.tab.c

test_suite_SOURCES = test_suite.c

test_suite_LDADD = libsacc.la

parser.c: css.tab.c css.yy.c

css.yy.c: css.lex css.tab.c
	flex -Pcss --header-file=css.yy.h -o css.yy.c css.lex

css.tab.c: css.y
	bison -p css --defines=css.tab.h -o css.tab.c css.y



test: test_suite
	./test_suite

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = sacc.pc
 
clean-local:
	rm -rf css.yy.c css.yy.h
	rm -rf css.tab.c css.tab.h

.PHONY: test
