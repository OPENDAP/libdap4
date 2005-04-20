
dnl -*- m4 -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(libdap, 3.5, dods-tech@unidata.ucar.edu)
AC_CONFIG_HEADER([config_dap.h dods-datatypes.h])

DODS_DIRECTORY_VERSION

AC_DEFINE(CNAME, "OPeNDAP DAP++", [What sort of HTTP client is this?])
AC_DEFINE_UNQUOTED(CVER, "$VERSION", [Client version number])
DVR="DODS DAP++/$VERSION"
AC_DEFINE_UNQUOTED(DVR, "$DVR", [Client name and vesion combined])
AC_SUBST(DVR)

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

dnl Default to g++

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
AC_PROG_LEX
dnl DODS_PROG_BISON
dnl AC_CHECK_PROG(YACC,bison,bison)
AC_PROG_YACC
dnl Find where perl lives on the user's system
DODS_PROG_PERL
dnl Find where GNU tar lives on the user's system
DODS_PROG_GTAR

dnl The command to use to generate dependencies for Makefiles
depend=depend.sh
AC_SUBST(depend)


AC_CHECK_PROG(VALGRIND, valgrind, [valgrind --logfile=memcheck --tool=memcheck --leak-check=yes])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h limits.h unistd.h pthread.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
DODS_CHECK_SIZES

dnl Checks for specific libraries
AC_CHECK_LIB([curl], [curl_version], 
	[LIBS="$LIBS `curl-config --libs`"],
	[AC_MSG_ERROR([I could not find libcurl])])

AC_CHECK_LIB([xml2], [xmlInitGlobals],
	[LIBS="$LIBS `xml2-config --libs`"],
	[AC_MSG_ERROR([I could not find libxml2])])

AC_CHECK_LIB([pthread], [pthread_testcancel], 
	[LIBS="$LIBS -lpthread"],
	[AC_MSG_ERROR([I could not find pthreads])])

AC_MSG_CHECKING([libcurl version])
version_libcurl=`curl-config --version`
version_major=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
version_minor=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
if test $version_major -lt 7 || test $version_minor -lt 12
then
	AC_MSG_ERROR([must have libcurl 7.12.0 or greater, found $version_libcurl])
else
	AC_MSG_RESULT([$version_libcurl])
fi

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_CHECK_FUNCS(strdup strftime timegm mktime)

dnl temporary hack for the librx/regex code. 04/08/04 jhrg.
INCS="-Iregex-0.12 $INCS"
REGEX_C=regex-0.12/regex.c
AC_SUBST(REGEX_C)
echo "Setting $REGEX_C"
REGEX_O=regex-0.12/regex.o
AC_SUBST(REGEX_O)

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
