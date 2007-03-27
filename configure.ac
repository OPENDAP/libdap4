
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
dnl Update version here and below at LIB_CURRENT, ..., if needed.
AC_INIT(libdap, 3.7.6, dods-tech@unidata.ucar.edu)
AC_DEFINE(DAP_PROTOCOL_VERSION, "3.1", [What DAP version is supported?])
AC_SUBST(DAP_PROTOCOL_VERSION)

AC_CONFIG_SRCDIR([AISConnect.cc])
AC_CONFIG_AUX_DIR(conf)
AM_CONFIG_HEADER([config.h dods-datatypes.h])

AM_INIT_AUTOMAKE

AC_DEFINE(CNAME, "libdap", [What sort of HTTP client is this?])
AC_DEFINE_UNQUOTED(CVER, "$PACKAGE_VERSION", [Client version number])
AC_DEFINE_UNQUOTED(DVR, "libdap/$PACKAGE_VERSION", [Client name and vesion combined])
AC_SUBST(DVR)

AC_DEFINE(EVAL, 0, [Should all the classes run ConstraintEvaluator::eval()?])
AC_SUBST(EVAL)

AC_DEFINE(COMPRESSION_FOR_SERVER3, 0, [Should the subprocess compressor for Server3 be includes?])
AC_SUBST(COMPRESSION_FOR_SERVER3)

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

dnl library versionning: Update these when the interface changes. Generally,
dnl assume that the interface tracks the major and minor release numbers.
DAPLIB_CURRENT=7
DAPLIB_AGE=1
DAPLIB_REVISION=1
AC_SUBST(DAPLIB_CURRENT)
AC_SUBST(DAPLIB_AGE)
AC_SUBST(DAPLIB_REVISION)

LIBDAP_VERSION="$DAPLIB_CURRENT:$DAPLIB_REVISION:$DAPLIB_AGE"
AC_SUBST(LIBDAP_VERSION)

CLIENTLIB_CURRENT=1
CLIENTLIB_AGE=0
CLIENTLIB_REVISION=3
AC_SUBST(CLIENTLIB_CURRENT)
AC_SUBST(CLIENTLIB_AGE)
AC_SUBST(CLIENTLIB_REVISION)

CLIENTLIB_VERSION="$CLIENTLIB_CURRENT:$CLIENTLIB_REVISION:$CLIENTLIB_AGE"
AC_SUBST(CLIENTLIB_VERSION)

SERVERLIB_CURRENT=3
SERVERLIB_AGE=0
SERVERLIB_REVISION=2
AC_SUBST(SERVERLIB_CURRENT)
AC_SUBST(SERVERLIB_AGE)
AC_SUBST(SERVERLIB_REVISION)

SERVERLIB_VERSION="$SERVERLIB_CURRENT:$SERVERLIB_REVISION:$SERVERLIB_AGE"
AC_SUBST(SERVERLIB_VERSION)

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

dnl Call this gnulib macro right after a working C Compiler is found
gl_EARLY

AM_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl We really need bison and not yacc. If you use AC_PROG_YACC, the resulting 
dnl Makfile will call bison -y which doesn't know how to make the parsers 
dnl we require. jhrg 6/15/05
AC_CHECK_PROG(YACC,[bison],[bison])

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h malloc.h memory.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/param.h sys/time.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
DODS_CHECK_SIZES

# Checks for library functions.

# These, at least some of them, cannot be used along with gnulib without
# breaking stuff. Since we are not bothering to handle the cases where these
# functions break, there's not much point in testing for them. However, it
# might be a good thing to use the gnulib versions since those fix various
# common problems found in many distributions.

dnl AC_FUNC_CLOSEDIR_VOID
dnl AC_FUNC_FORK
dnl AC_FUNC_STRFTIME
dnl AC_HEADER_STDBOOL
dnl AC_FUNC_ALLOCA
dnl AC_FUNC_ERROR_AT_LINE
dnl AC_FUNC_MALLOC
dnl AC_FUNC_MEMCMP
dnl AC_FUNC_REALLOC
dnl AC_FUNC_STAT
dnl AC_FUNC_STRTOD

dnl using AC_CHECK_FUNCS does not run macros from gnulib.
AC_CHECK_FUNCS([alarm atexit bzero dup2 getcwd getpagesize localtime_r memmove memset pow putenv setenv strchr strerror strtol strtoul timegm mktime])

gl_SOURCE_BASE(gl)
gl_M4_BASE(gl/m4)
gl_MODULES(regex)

gl_INIT

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.
AC_MSG_CHECKING([for libcurl])
if curl-config --version > /dev/null 2>&1
then
	version_libcurl=`curl-config --version`
	version_major=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_minor=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	if test $version_major -lt 7 || test $version_minor -lt 10 || test $version_minor -lt 6
	then
		AC_MSG_ERROR([must have libcurl 7.10.6 or greater, found $version_libcurl])
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
