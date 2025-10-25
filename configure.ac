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
AC_PROG_LEX(yywrap)
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
		 utl/Makefile
                 lib/Makefile
                 lib/asl/Makefile
                 lib/atf/Makefile
                 lib/cat/Makefile
                 lib/cbd/Makefile
                 lib/chr/Makefile
                 lib/collate/Makefile
                 lib/expat/Makefile
                 lib/frm/Makefile
                 lib/gdl/Makefile
                 lib/gvl/Makefile
                 lib/inl/Makefile
                 lib/joxer/Makefile
                 lib/jsn/Makefile
		 lib/l3/Makefile
		 lib/l3/bigrams/Makefile
		 lib/l3/dsa/Makefile
		 lib/l3/ilem/Makefile
		 lib/l3/link/Makefile
		 lib/l3/ngram/Makefile
		 lib/l3/prop/Makefile
		 lib/l3/psa/Makefile
		 lib/l3/psus/Makefile
		 lib/l3/sigs/Makefile
		 lib/l3/word/Makefile
		 lib/l3/xcl2/Makefile
		 lib/l3/xli/Makefile
                 lib/libstemmer_c/Makefile
                 lib/lng/Makefile
                 lib/ofp/Makefile
                 lib/oid/Makefile
                 lib/rnc/Makefile
                 lib/rnv/Makefile
                 lib/sas/Makefile
                 lib/scan/Makefile
                 lib/sll/Makefile
                 lib/std/Makefile
                 lib/sys/Makefile
                 lib/tis/Makefile
                 lib/tok/Makefile
                 lib/tsv/Makefile
                 lib/xmd/Makefile
                 lib/xml/Makefile
                 lib/xpd/Makefile
		 bin/Makefile
		 bin/ax/Makefile
		 bin/bx/Makefile
		 bin/cx/Makefile
		 bin/dx/Makefile
		 bin/dx/lib/Makefile
		 bin/dx/dxx/Makefile
		 bin/dx/osh/Makefile
		 bin/dx/odo/Makefile
		 bin/gx/Makefile
		 bin/jx/Makefile
		 bin/lx/Makefile
		 bin/mx/Makefile
		 bin/nx/Makefile
		 bin/ox/Makefile
		 bin/px/Makefile
		 bin/px/ice/Makefile
		 bin/px/ice/ce/Makefile
		 bin/px/ice/pg/Makefile
		 bin/px/ice/wm/Makefile
		 bin/px/iso/Makefile
		 bin/px/isp/Makefile
		 bin/px/iss/Makefile
		 bin/px/pui/Makefile
		 bin/px/pxr/Makefile
		 bin/px/wht/Makefile
		 bin/qx/Makefile
		 bin/qx/cat/Makefile
		 bin/qx/cbd/Makefile
		 bin/qx/dbg/Makefile
		 bin/qx/gdf/Makefile
		 bin/qx/lem/Makefile
		 bin/qx/lib/Makefile
		 bin/qx/qxx/Makefile
		 bin/qx/tra/Makefile
		 bin/qx/txt/Makefile
		 bin/sx/Makefile
		 bin/sx/sxweb/Makefile
		 bin/tx/Makefile
		 bin/wx/Makefile
		 bin/wx/ccgi/Makefile
		 bin/xx/Makefile
		 bin/xx/cun/Makefile
		 bin/xx/fonts/Makefile
		 bin/xx/lem/Makefile
		 bin/xx/osort/Makefile
		 bin/xx/rnv/Makefile
		 bin/xx/tis/Makefile
		 bin/xx/wrapper/Makefile
		 bin/zx/Makefile
		 msc/Makefile
		 msc/adm/Makefile
		 msc/adm/skel/Makefile
		 msc/cmd/Makefile
		 msc/cmd/check/Makefile
		 msc/cmd/config/Makefile
		 msc/cmd/update/Makefile
		 msc/cov/Makefile
		 msc/ela/Makefile
		 msc/fonts/Makefile
		 msc/lex/Makefile
		 msc/ood/Makefile
		 msc/otf/Makefile
		 msc/prj/Makefile
		 msc/prj/cdli/Makefile
		 msc/prj/neo/Makefile
		 msc/prj/osl/Makefile
		 msc/prj/pcsl/Makefile
		 msc/saxon/Makefile
		 msc/schemas/Makefile
		 msc/sys/Makefile
		 msc/tex/Makefile
		 msc/tex/h2t/Makefile
		 msc/tex/o25/Makefile
                 ])
AC_OUTPUT
