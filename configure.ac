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

if test "x$tests_enabled" != "xno"; then
  PKG_CHECK_MODULES(LIBXML2, libxml-2.0)
fi

AC_ARG_ENABLE([checks], [AS_HELP_STRING([--enable-checks],
  [enable extra checking code])],
  [checks_enabled=$enableval],
  [checks_enabled='no'])
if test "x$checks_enabled" != "xno"; then
  AC_DEFINE([SAC_CHECKS], 1, [Extra checking code])
fi



AC_CONFIG_FILES(
  [Makefile]
  [src/Makefile]
  [src/sacc.pc]
  [tests/Makefile]
)

AC_OUTPUT
