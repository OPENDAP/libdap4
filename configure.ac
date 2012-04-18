
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.62)
dnl Update version here and below at LIB_CURRENT, ..., if needed.
AC_INIT(libdap, 3.11.4, opendap-tech@opendap.org)
AC_DEFINE(DAP_PROTOCOL_VERSION, ["3.4"], [Highest DAP version implemented?])
AC_SUBST(DAP_PROTOCOL_VERSION)

AC_CONFIG_SRCDIR([Connect.cc])
AC_CONFIG_AUX_DIR(conf)
AM_CONFIG_HEADER([config.h dods-datatypes-config.h xdr-datatypes-config.h])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE
AC_CONFIG_TESTDIR(tests, [.])

dnl AC_DEFINE([FILE_METHODS], [1], [Define this to activate the old FILE * methods.])

AC_DEFINE(CNAME, "libdap", [What sort of HTTP client is this?])
AC_DEFINE_UNQUOTED(CVER, "$PACKAGE_VERSION", [Client version number])
AC_DEFINE_UNQUOTED(DVR, "libdap/$PACKAGE_VERSION", [Client name and version combined])
AC_SUBST(DVR)


PACKAGE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
PACKAGE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
PACKAGE_SUBMINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
AC_MSG_NOTICE(Package Major Version:     $PACKAGE_MAJOR_VERSION)
AC_MSG_NOTICE(Package Minor Version:     $PACKAGE_MINOR_VERSION)
AC_MSG_NOTICE(Package SubMinor Version:  $PACKAGE_SUBMINOR_VERSION)
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_SUBMINOR_VERSION)

AC_DEFINE(EVAL, 1, [Should all the classes run ConstraintEvaluator::eval()?])
AC_SUBST(EVAL)

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

dnl library version: Update these when the interface changes. Generally,
dnl assume that the interface tracks the major and minor release numbers.
DAPLIB_CURRENT=14
DAPLIB_AGE=3
DAPLIB_REVISION=0
AC_SUBST(DAPLIB_CURRENT)
AC_SUBST(DAPLIB_AGE)
AC_SUBST(DAPLIB_REVISION)

LIBDAP_VERSION="$DAPLIB_CURRENT:$DAPLIB_REVISION:$DAPLIB_AGE"
AC_SUBST(LIBDAP_VERSION)

CLIENTLIB_CURRENT=4
CLIENTLIB_AGE=1
CLIENTLIB_REVISION=0
AC_SUBST(CLIENTLIB_CURRENT)
AC_SUBST(CLIENTLIB_AGE)
AC_SUBST(CLIENTLIB_REVISION)

CLIENTLIB_VERSION="$CLIENTLIB_CURRENT:$CLIENTLIB_REVISION:$CLIENTLIB_AGE"
AC_SUBST(CLIENTLIB_VERSION)

SERVERLIB_CURRENT=9
SERVERLIB_AGE=2
SERVERLIB_REVISION=0
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
dnl Makefile will call bison -y which doesn't know how to make the parsers 
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

dnl echo "CC = $CC"
if test "$CC" = "gcc"
then
    AM_CONDITIONAL([COMPILER_IS_GCC],[true])
else
    AM_CONDITIONAL([COMPILER_IS_GCC],[false])
fi

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

AC_ARG_WITH(curl,[  --with-curl=PFX   Prefix where curl/libcurl is installed (optional). This will override pkgconfig, etc.],
            with_curl_prefix="$withval", with_curl_prefix="")

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.

curlprivatereq=
curlprivatelibs=
curl_set=

if test -n "$with_curl_prefix" -a -x $with_curl_prefix/bin/curl-config
then
    AC_MSG_NOTICE([Using $with_curl_prefix as the curl prefix directory.])
    CURL_LIBS="`$with_curl_prefix/bin/curl-config --libs`"
    CURL_STATIC_LIBS=$CURL_LIBS
    curlprivatelibs="`$with_curl_prefix/bin/curl-config --libs`"
    CURL_CFLAGS="`$with_curl_prefix/bin/curl-config --cflags`"
    curl_set="yes"
elif test -n "$with_curl_prefix"
then
    AC_MSG_ERROR([You set the curl-prefix directory to $with_curl_prefix, but curl-config is not there.])
fi

if test -z "$curl_set"
then
    # curlprivatereq=
    # curlprivatelibs=
    libdap_pkgconfig_libcurl=yes
    libdap_libcurl_module='libcurl >= 7.10.6'
    PKG_CHECK_MODULES([CURL],[$libdap_libcurl_module],,
        [libdap_pkgconfig_libcurl=no])
    AC_MSG_CHECKING([for libcurl])
    
    if test $libdap_pkgconfig_libcurl = 'yes'
    then
	   curlprivatereq=$libdap_libcurl_module
	   CURL_STATIC_LIBS="`$PKG_CONFIG --static --libs libcurl`"
	   AC_MSG_RESULT([yes; used pkg-config])
    elif curl-config --version > /dev/null 2>&1
    then
	   version_libcurl=`curl-config --version | sed 's@libcurl \(.*\)@\1@'`

	   version_M=`echo $version_libcurl | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	   version_m=`echo $version_libcurl | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	   version_m_m=`echo $version_libcurl | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	
       dnl Test for several different versions of libcurl. We can use 7.10.6
	   dnl or newer.
	
       if test $version_M -gt 7
	   then
		  libcurl_ok='yes'
	   elif test $version_M -eq 7 && test $version_m -gt 10
	   then
		  libcurl_ok='yes'
	   elif test $version_M -eq 7 && test $version_m -eq 10 && test $version_m_m -gt 5
	   then
		  libcurl_ok='yes'
	   else
		  libcurl_ok='no'
	   fi

	   dnl First test the minimum
	   if test $libcurl_ok = "no"
	   then
		  AC_MSG_ERROR([must have libcurl 7.10.6 or greater, found $version_libcurl])
	   fi

	   CURL_LIBS="`curl-config --libs`"
	   CURL_STATIC_LIBS=$CURL_LIBS
	   curlprivatelibs="`curl-config --libs`"
	   CURL_CFLAGS="`curl-config --cflags`"
	   AC_MSG_RESULT([yes; used curl-config])
    else
	   AC_MSG_ERROR([I could not find libcurl])
    fi
fi

AC_SUBST([curlprivatereq])
AC_SUBST([curlprivatelibs])
AC_SUBST([CURL_LIBS])
AC_SUBST([CURL_STATIC_LIBS])
AC_SUBST([CURL_CFLAGS])

AC_ARG_WITH(xml2,[  --with-xml2=PFX   Prefix where libxml2 is installed (optional). This will override pkgconfig, etc.],
            with_xml2_prefix="$withval", with_xml2_prefix="")

xmlprivatereq=
xmlprivatelibs=
xml_set=

if test -n "$with_xml2_prefix" -a -x $with_xml2_prefix/bin/xml2-config
then
    AC_MSG_NOTICE([Using $with_xml2_prefix as the libxml2 prefix directory.])
    XML2_LIBS="`$with_xml2_prefix/bin/xml2-config --libs`"
    XML2_STATIC_LIBS=$XML2_LIBS
    xmlprivatelibs="`$with_xml2_prefix/bin/xml2-config --libs`"
    XML2_CFLAGS="`$with_xml2_prefix/bin/xml2-config --cflags`"
    xml_set="yes"
elif test -n "$with_xml2_prefix"
then
    AC_MSG_ERROR([You set the libxml2 prefix directory to $with_xml2_prefix, but xml2-config is not there.])
fi

if test -z "$xml_set"
then
libdap_pkgconfig_libxml2=yes
libdap_libxml2_module='libxml-2.0 >= 2.6.16'
PKG_CHECK_MODULES([XML2],[$libdap_libxml2_module],,
  [libdap_pkgconfig_libxml2=no])
AC_MSG_CHECKING([for libxml2])
if test $libdap_pkgconfig_libxml2 = 'yes'
then
	xmlprivatereq=$libdap_libxml2_module
	XML2_STATIC_LIBS="`$PKG_CONFIG --static --libs libxml-2.0`"
	AC_MSG_RESULT([yes; used pkg-config])
elif xml2-config --version > /dev/null 2>&1
then
	version_libxml2=`xml2-config --version`
	version_M=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	version_m_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	
	if test $version_M -gt 2
	then
		libxml2_ok='yes'
	elif test $version_M -eq 2 && test $version_m -eq 6 && test $version_m_m -ge 16
	then
                libxml2_ok='yes'
	fi

	XML2_LIBS="`xml2-config --libs`"
	XML2_STATIC_LIBS=$XML2_LIBS
	XML2_CFLAGS="`xml2-config --cflags`"
	xmlprivatelibs="`xml2-config --libs`"
	AC_MSG_RESULT([yes; used xml2-config])
else
	AC_MSG_ERROR([I could not find libxml2 2.6.16 or newer])
fi
fi

AC_SUBST([xmlprivatereq])
AC_SUBST([xmlprivatelibs])
AC_SUBST([XML2_LIBS])
AC_SUBST([XML2_STATIC_LIBS])
AC_SUBST([XML2_CFLAGS])

dnl DAP_CHECK_ZLIB

dnl When pkgconfig support is added, use this
libdap_libgridfields_module='libgridfields >= 0.7.0'

libgf_ok="no"

AC_WITH([gridfields],
    [GF_LIBS="-L$withval/lib -lgridfields"
     GF_CFLAGS="-I$withval/include"
     AC_DEFINE([GRIDFIELDS],[1],[define if gridfields lib is present])
     libgf_ok="yes"],
    [])

AC_ARG_ENABLE([gridfields],
    AC_HELP_STRING([--enable-gridfields], [Enable libgridfields-based server functions (default is NO)]))

if test "$libgf_ok" = "no" -a "$enable_gridfields" = "yes"
then
     
AC_MSG_CHECKING([for libgridfields])
if gridfields-config --version > /dev/null 2>&1
then
	version_libgf=`gridfields-config --version | sed 's@libgridfields \(.*\)@\1@'`

	version_M=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_m=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	version_m_m=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	
	dnl Test for several different versions of libgridfields
	
	if test $version_M -gt 0
	then
		libgf_ok='yes'
	elif test $version_M -eq 0 && test $version_m -gt 7
	then
		libgf_ok='yes'
	elif test $version_M -eq 0 && test $version_m -eq 7 && test $version_m_m -gt 0
	then
		libgf_ok='yes'
	else
		libgf_ok='no'
	fi

	if test "$libgf_ok"="no"
	then
  	    AC_MSG_ERROR([must have libgf 0.7.0 or greater, found $version_libgf])
	fi

	GF_LIBS="`gridfields-config --libs`"
	GF_CFLAGS="`gridfields-config --cflags`"

	AC_DEFINE([GRDIFIELDS],[1],[define if gridfields lib is present])
	AC_MSG_RESULT([yes; used gridfields-config])
else
	AC_MSG_ERROR([I could not find libgridfields])
fi

fi

AC_SUBST([GF_LIBS])
AC_SUBST([GF_CFLAGS])

if test "$libgf_ok" = "yes"
then
    echo "Set gridfields CFLAGS to $GF_CFLAGS"
    echo "Set gridfields LIBS to $GF_LIBS"
    AM_CONDITIONAL([USING_GRIDFIELDS],[true])
else
    AM_CONDITIONAL([USING_GRIDFIELDS],[false])
fi

AC_CHECK_LIB([pthread], [pthread_kill], 
	[PTHREAD_LIBS="-lpthread"],
	[AC_MSG_ERROR([I could not find pthreads])])
AC_SUBST([PTHREAD_LIBS])

AC_CHECK_LIB([uuid], [uuid_generate], 
	[UUID_LIBS="-luuid"],
	[UUID_LIBS=""])
AC_SUBST([UUID_LIBS])

AM_PATH_CPPUNIT(1.12.0,
	[AM_CONDITIONAL([CPPUNIT], [true])],
	[AM_CONDITIONAL([CPPUNIT], [false])])

DODS_DEBUG_OPTION

dnl Temporary hack; find a home for the deflate program; set it to
dnl prefix/etc/deflate. See util.cc:dods_root()
if test "$prefix" = "NONE"
then
AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])
else
AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])
fi

AC_ARG_ENABLE([checksums], 
    AC_HELP_STRING([--enable-checksums], [Enable MD5 checksums (default is NO)]))

dnl echo "enable_checksums = $enable_checksums"
AS_IF([test "x$enable_checksums" = "xyes"], [
    dnl Do the stuff needed for enabling the feature
    dnl echo "Defining checksums! "
    AC_DEFINE([CHECKSUMS], 1, [Should checksums be generated])
])

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
                 libdap.pc
                 libdapclient.pc
                 libdapserver.pc
                 gl/Makefile
                 tests/Makefile
                 tests/atlocal
                 unit-tests/Makefile
                 unit-tests/cache-testsuite/Makefile
                 ugrid_project/bes.conf])
AC_CONFIG_FILES([dap-config], [chmod +x dap-config]) 
		 
AC_OUTPUT
