#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# This configure.ac is still being adapated from configure.scan
#
AC_PREREQ([2.69])
AC_INIT([oracc], [2.0], [stinney-at-upenn.edu])
AC_LANG(C)
AC_CONFIG_SRCDIR([lib/liboracc2.c])
AM_INIT_AUTOMAKE([-Wall -Wno-portability subdir-objects])
LT_INIT
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_CC
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_MKDIR_P

AM_MISSING_PROG([GPERF], [gperf])

# Checks for libraries.
AC_SEARCH_LIBS(pcre2,pcre2_exec)

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h inttypes.h libintl.h malloc.h stdint.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memmove memset mkdir setlocale strchr strcspn strdup strerror strpbrk strrchr strstr strtol strtoul])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 lib/asl/Makefile
                 lib/atf/Makefile
                 lib/cat/Makefile
                 lib/cbd/Makefile
                 lib/collate/Makefile
                 lib/expat/Makefile
                 lib/frm/Makefile
                 lib/gdl/Makefile
                 lib/gvl/Makefile
                 lib/jsn/Makefile
                 lib/lng/Makefile
                 lib/oid/Makefile
                 lib/rnc/Makefile
                 lib/rnv/Makefile
                 lib/sll/Makefile
                 lib/std/Makefile
                 lib/sys/Makefile
                 lib/xml/Makefile
                 lib/xpd/Makefile
		 bin/Makefile
		 bin/ax/Makefile
		 bin/cx/Makefile
		 bin/dx/Makefile
		 bin/dx/osh/Makefile
		 bin/dx/odo/Makefile
		 bin/gx/Makefile
		 bin/jx/Makefile
		 bin/ox/Makefile
		 bin/sx/Makefile
		 bin/sx/sxweb/Makefile
		 bin/tx/Makefile
		 bin/wx/Makefile
		 bin/wx/ccgi/Makefile
		 bin/xx/Makefile
		 bin/xx/osort/Makefile
		 bin/xx/rnv/Makefile
		 msc/Makefile
		 msc/adm/Makefile
		 msc/adm/skel/Makefile
		 msc/cmd/Makefile
		 msc/cmd/config/Makefile
		 msc/cmd/update/Makefile
		 msc/lex/Makefile
		 msc/sys/Makefile
                 ])
AC_OUTPUT
