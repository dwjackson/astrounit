#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([AstroUnit], [0.5.0], [dwjackson@users.noreply.github.com])
AC_CONFIG_SRCDIR([include/astrounit.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall foreign])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR

# Initialize libtool
LT_INIT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 lib/Makefile
		 tests/Makefile
		 man/Makefile
		 src/Makefile])
AC_OUTPUT
