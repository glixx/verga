#! /bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during TkGate installation
dnl	to configure the system for the local environment.

AC_INIT([verga++],[0.0.1],[starling13@gmail.com],[verga++],[https://bitbucket.org/starling13/verga])
AM_INIT_AUTOMAKE([foreign])
AC_PREREQ(2.57)
AC_CONFIG_HEADER(config.h)
AC_LANG(C++)


#############################################################################
#
# Default prefix directory for installation by OS
#
AC_PREFIX_DEFAULT(/usr/local)

#############################################################################
#
# Top and bottom of config.h
#

AH_TOP([#ifndef __config_h
#define __config_h
#include "options.h"])
AH_BOTTOM([

#if TKGATE_NOYYRESTART
#define yyrestart(f) { extern FILE *yyin; yyin = f; }
#endif

#if TKGATE_SIGSET
#define signal(a,b)	sigset(a,b)
#endif

#if !HAVE_PW_GECOS
#define pe_gecos pw_name
#endif

#endif /* __config_h */
])

#------------------------------------------------------------------------
# Standard compiler checks
#------------------------------------------------------------------------

#
# If the user did not set CFLAGS, set it now to keep
# the AC_PROG_CC macro from adding "-g -O2".
#
# Set TKGATE_CFLAGS in options.h to set compiler options.
#
if test "${CFLAGS+set}" != "set" ; then
    CFLAGS=`grep "^#define.*TKGATE_CFLAGS" options.h | sed 's/#define.*TKGATE_CFLAGS//'`
fi

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_YACC
AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_CPP
AC_PROG_INSTALL

# Checks for header files.
AC_PATH_XTRA
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([limits.h malloc.h stdlib.h string.h sys/param.h sys/time.h unistd.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES(unsigned long long)

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getcwd gettimeofday memmove memset putenv select strcasecmp strchr strcspn strdup strncasecmp strrchr strspn strstr strcasestr])
AC_CHECK_LIB(m, floor)

#############################################################################
#
# Check for word size and Endianess
#
AC_C_BIGENDIAN
TKG_WORDSIZE

#############################################################################
#
# Header file paths we may need
#
AC_SUBST(COMMON_IPATH, ['-I$(top_srcdir)/src/common'])
AC_SUBST(TOP_IPATH, ['-I$(top_srcdir)'])
AC_SUBST(COMMON_LPATH, ['-L$(top_builddir)/src/common'])

#############################################################################
#
# Substitution variables
#
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/common/Makefile
                 src/verga/Makefile])

AC_OUTPUT
