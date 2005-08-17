
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
dnl Update version here and below at LIB_CURRENT, ..., if needed.
AC_INIT(libdap, 3.5.2, dods-tech@unidata.ucar.edu)
AC_CONFIG_SRCDIR([AISConnect.cc])
AM_CONFIG_HEADER([config.h dods-datatypes.h])
AM_INIT_AUTOMAKE

AC_DEFINE(CNAME, "libdap", [What sort of HTTP client is this?])
AC_DEFINE_UNQUOTED(CVER, "$PACKAGE_VERSION", [Client version number])
AC_DEFINE_UNQUOTED(DVR, "${CNAME}/${CVER}", [Client name and vesion combined])
AC_SUBST(DVR)

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

dnl library versionning: Update these when the interface changes. Generally,
dnl assume that the interface tracks the major and minor release numbers.
LIB_CURRENT=3
LIB_AGE=0
LIB_REVISION=0
AC_SUBST(LIB_CURRENT)
AC_SUBST(LIB_AGE)
AC_SUBST(LIB_REVISION)

LIBDAP_VERSION="$LIB_CURRENT:$LIB_REVISION:$LIB_AGE"
AC_SUBST(LIBDAP_VERSION)

dnl Until further notice OS X port doesn't get -gstabs because of an
dnl apparent bug in their gcc 3.3 clone (a beta).  If you see this after
dnl 1/1/2005, you can probably remove it.
dnl case "${host_cpu}:${host_vendor}:${host_os}" in
dnl     powerpc:apple:netbsd* | alphaev56:dec:osf5.* | powerpc:apple:darwin*)
dnl       CFLAGS=${CFLAGS-"-O2 $CFLAGS"}
dnl       CXXFLAGS=${CXXFLAGS-"-O2 $CXXFLAGS"}
dnl       break;;
dnl    *:*:*)
dnl       CFLAGS=${CFLAGS-"-gstabs -O2 $CFLAGS"}
dnl       CXXFLAGS=${CXXFLAGS-"-gstabs -O2 $CXXFLAGS"}
dnl       break;;
dnl esac    

dnl Default to g++

dnl CXX=${CXX-g++}
dnl AC_MSG_RESULT(Setting CXX to $CXX)
dnl AC_SUBST(CXX)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

gl_EARLY

AM_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
dnl AC_PROG_RANLIB

dnl We really need bison and not yacc. If you use AC_PROG_YACC, the resulting 
dnl Makfile will call bison -y which doesn't know how to make the parsers 
dnl we require. jhrg 6/15/05
AC_CHECK_PROG(YACC,[bison],[bison])

dnl Find where perl lives on the user's system
dnl DODS_PROG_PERL
dnl Find where GNU tar lives on the user's system
dnl DODS_PROG_GTAR

dnl AC_CHECK_PROG(VALGRIND, valgrind, [valgrind --leak-check=yes])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h malloc.h memory.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/param.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
DODS_CHECK_SIZES

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([alarm bzero getcwd localtime_r pow strchr])
# Added 04/22/05 jhrg
AC_CHECK_FUNCS([timegm mktime])

dnl in gnulib
dnl AC_HEADER_STDBOOL
dnl AC_FUNC_ALLOCA
dnl AC_FUNC_ERROR_AT_LINE
dnl AC_FUNC_MALLOC
dnl AC_FUNC_MEMCMP
dnl AC_FUNC_REALLOC
dnl AC_FUNC_STAT
dnl AC_FUNC_STRTOD
dnl AC_CHECK_FUNCS([alarm atexit bzero dup2 getcwd getpagesize localtime_r memmove memset pow putenv setenv strchr strerror strstr strtol strtoul])

gl_SOURCE_BASE(gl)
gl_M4_BASE(gl/m4)

dnl Removed "timegm mktime" jhrg 7/11/05 And "error" jhrg 7/28/05
gl_MODULES(alloca stdbool malloc memcmp realloc strftime strtod atexit dup2 getpagesize memmove memset putenv setenv strerror strstr strtol strtoul regex)
gl_INIT

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.
AC_MSG_CHECKING([for libcurl])
if curl-config --version > /dev/null 2>&1
then
	version_libcurl=`curl-config --version`
	version_major=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_minor=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	if test $version_major -lt 7 || test $version_minor -lt 12
	then
		AC_MSG_ERROR([must have libcurl 7.12.0 or greater, found $version_libcurl])
	fi

	LIBS="$LIBS `curl-config --libs`"
	CPPFLAGS="$CPPFLAGS `curl-config --cflags`"
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([I could not find libcurl])
fi

AC_MSG_CHECKING([for libxml2])
if xml2-config --version > /dev/null 2>&1
then
	version_libxml2=`xml2-config --version`
	version_M=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	version_m_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	
	dnl Test for several different versions of libxml2. We can use 2.5.7 
	dnl or newer, but part of the SAX interface changes depending on the
	dnl version.
	
	dnl First test the minimum
	if test $version_M -lt 2 || test $version_m -lt 5 || test $version_m_m -lt 7
	then
		AC_MSG_ERROR([must have libxml2 2.5.7 or greater, found $version_libxml2])
	fi
	
	dnl Sort out the particular variant
	if test $version_M -eq 2 && test $version_m -eq 5 && test $version_m_m -ge 10
	then
		AC_DEFINE(LIBXML2_5_10, [1], [define if you have xml2 2.5.10 or greater])
	fi
	if test $version_M -eq 2 && test $version_m -eq 6 && test $version_m_m -ge 16
	then
		AC_DEFINE(LIBXML2_5_10, [1], [define if you have xml2 2.5.10 or greater])
		AC_DEFINE(LIBXML2_6_16, [1], [define if you have xml2 2.6.16 or greater])
	fi

	LIBS="$LIBS `xml2-config --libs`"
	CPPFLAGS="$CPPFLAGS `xml2-config --cflags`"
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([I could not find libxml2])
fi

AC_CHECK_LIB([pthread], [pthread_kill], 
	[LIBS="$LIBS -lpthread"],
	[AC_MSG_ERROR([I could not find pthreads])])

DODS_DEBUG_OPTION

dnl Temporary hack; find a home for the deflate program; set it to
dnl prefix/etc/deflate. See util.cc:dods_root()
if test "$prefix" = "NONE"
then
AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])
else
AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])
fi

dnl autoheader macros; tack some text at the top and bottom of config_dap.h.in

AH_TOP([#ifndef _config_h
#define _config_h])

AH_BOTTOM([/* Shorthand for gcc's unused attribute feature */
#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif /* __GNUG__ || __GNUC__ */

#endif /* _config_h */])

AC_CONFIG_FILES([Makefile
                 gl/Makefile
                 tests/Makefile
                 unit-tests/Makefile
                 unit-tests/cache-testsuite/Makefile])
AC_CONFIG_FILES([dap-config], [chmod +x dap-config]) 
		 
AC_OUTPUT
