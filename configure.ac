AC_INIT([libsacc], [0.0])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER([config.h])

AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC

LT_INIT

AC_CONFIG_MACRO_DIR([m4])



AC_ARG_ENABLE([tests], [AS_HELP_STRING([--enable-tests],
  [enable testing code])],
  [tests_enabled=$enableval],
  [tests_enabled='no'])
AM_CONDITIONAL([ENABLE_TESTS], [test "x$tests_enabled" != "xno"])



AC_CONFIG_FILES(
  [Makefile]
  [src/Makefile]
  [tests/Makefile]
  [src/sacc.pc]
)

AC_OUTPUT
