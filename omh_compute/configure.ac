AC_INIT([omh_compute], [0.0.1], [gmarcais@cs.cmu.edu])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([subdir-objects foreign parallel-tests color-tests])
AM_SILENT_RULES([yes])
#AC_CONFIG_SRCDIR([.])
#AC_CONFIG_HEADERS([config.h])
#PKG_PROG_PKG_CONFIG
AC_LIB_RPATH

# Change default compilation flags
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG(C++)

# Check for yaggo
AC_ARG_VAR([YAGGO], [Yaggo switch parser generator])
AS_IF([test "x$YAGGO" = "x"], [AC_PATH_PROG([YAGGO], [yaggo], [false])])

PKG_CHECK_MODULES([XXHASH], [libxxhash])
AC_LIB_LINKFLAGS_FROM_LIBS([XXHASH_LDFLAGS], [$XXHASH_LIBS])

PKG_CHECK_MODULES([GTEST], [gtest])
AC_LIB_LINKFLAGS_FROM_LIBS([GTEST_LDFLAGS], [$GTEST_LIBS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
