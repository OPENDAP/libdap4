
dnl -*- m4 -*-
dnl Process this file with autoconf to produce a configure script.

AC_REVISION($Id: configure.ac,v 1.1 2005/04/20 19:45:14 jimg Exp $)

AC_INIT(DAS.cc)
AC_CONFIG_HEADER(config_dap.h dods-datatypes.h)

AC_PREFIX_DEFAULT("\$(DODS_ROOT)")

dnl I think that the new dir naming scheme obviates this... 6/5/98 jhrg
dnl the variable `dir' is used as a basename for the directory tar file

dnl DODS_GET_DODS_ROOT

dnl DODS_FIND_PACKAGES_DIR

DODS_DIRECTORY_VERSION

AC_DEFINE(CNAME, "DODS DAP++", [What sort of HTTP client is this?])
AC_DEFINE_UNQUOTED(CVER, "$VERSION", [Client version number])
DVR="DODS DAP++/$VERSION"
AC_DEFINE_UNQUOTED(DVR, "$DVR", [Client name and vesion combined])
AC_SUBST(DVR)

dnl Find where perl lives on the user's system

DODS_PROG_PERL

dnl Find where GNU tar lives on the user's system

DODS_PROG_GTAR

dnl The command to use to generate dependencies for Makefiles

depend=depend.sh
AC_SUBST(depend)

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

# Until further notice OS X port doesn't get -gstabs because of an
# apparent bug in their gcc 3.3 clone (a beta).  If you see this after
# 1/1/2005, you can probably remove it.
case "${host_cpu}:${host_vendor}:${host_os}" in
     powerpc:apple:netbsd* | alphaev56:dec:osf5.*)
       CFLAGS=${CFLAGS-"-O2 $CFLAGS"}
       CXXFLAGS=${CXXFLAGS-"-O2 $CXXFLAGS"}
       break;;
    *:*:*)
       CFLAGS=${CFLAGS-"-gstabs -O2 $CFLAGS"}
       CXXFLAGS=${CXXFLAGS-"-gstabs -O2 $CXXFLAGS"}
       break;;
esac    


dnl You should have g++, but you can override that with an environment
dnl variable. 

CXX=${CXX-g++}
AC_MSG_RESULT(Setting CXX to $CXX)
AC_SUBST(CXX)

AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AC_PROG_INSTALL

DODS_PROG_LEX
DODS_PROG_BISON
DODS_GCC

AC_CHECK_PROG(VALGRIND, valgrind, [valgrind --logfile=memcheck --tool=memcheck --leak-check=yes])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h unistd.h pthread.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

dnl Checks for specific libraries

dnl Add '-DHAVE_PTHREAD_H' to the text returned by 'opendap-config --cflags'
dnl as a work-around for using that symbol in HTTPCache.h. Remove that symbol
dnl ASAP.
AC_CHECK_LIB(pthread, pthread_testcancel, 
	[LIBS="$LIBS -lpthread"
	 AC_DEFINE(_REENTRANT, ,
		   [Is this code reentrant (pthreads was found)?])
	 opendap_config_pthread=-DHAVE_PTHREAD_H
	 AC_SUBST(opendap_config_pthread)])

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_CHECK_FUNCS(strdup strftime timegm mktime)

DODS_CHECK_SIZES

DODS_RX_LIB
DODS_WWW_LIB
DODS_COMPRESSION_LIB
DODS_XML_LIB

dnl temporary hack for the librx/regex code. 04/08/04 jhrg.
INCS="-Iregex-0.12 $INCS"
REGEX_C=regex-0.12/regex.c
AC_SUBST(REGEX_C)
echo "Setting $REGEX_C"
REGEX_O=regex-0.12/regex.o
AC_SUBST(REGEX_O)

dnl DODS_EFENCE
dnl DODS_DBNEW
DODS_DEBUG_OPTION

dnl autoheader macros; tack some text at the top and bottom of config_dap.h.in

AH_TOP([#ifndef _config_dap_h
#define _config_dap_h])

AH_BOTTOM([/* Shorthand for gcc's unused attribute feature */
#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif /* __GNUG__ || __GNUC__ */

#endif /* _config_dap_h */])

AC_OUTPUT(Makefile tests/Makefile opendap-config)
