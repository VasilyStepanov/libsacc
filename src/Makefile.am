lib_LTLIBRARIES = libsacc.la

noinst_LTLIBRARIES = libgrammar.la

GENERIC_CFLAGS = -Wall -Wextra -ansi -pedantic -D_XOPEN_SOURCE=700 -Werror \
                 -I$(top_srcdir)/include

AM_CFLAGS = $(GENERIC_CFLAGS)

BUILT_SOURCES = grammar.h
AM_YFLAGS = -d --warnings=error
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c



libsacc_ladir = $(includedir)

libsacc_la_HEADERS = $(top_srcdir)/include/sacc.h

libsacc_la_SOURCES = parser.c \
                     mlex.c \
                     mpool.c \
                     mpool_extra.c \
                     list.c \
                     vector.c \
                     vector_extra.c \
                     string_extra.c \
                     lexical_unit.c \
                     condition.c \
                     selector.c \
                     page_margin.c \
                     media_query.c

libsacc_la_LIBADD = libgrammar.la



libgrammar_la_SOURCES = lexis.l \
                        grammar.y

libgrammar_la_CFLAGS = $(GENERIC_CFLAGS) -Wno-unused-parameter



pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = sacc.pc
 
clean-local:
	rm -rf lexis.c
	rm -rf grammar.c grammar.h



if ENABLE_TESTS
test: all
	make -C $(top_srcdir)/tests test

.PHONY: test
endif
