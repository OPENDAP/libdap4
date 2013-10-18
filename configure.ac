
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
dnl Update version here and below at LIB_CURRENT, ..., if needed.
AC_INIT(libdap, 3.12.1, opendap-tech@opendap.org)
AC_DEFINE(DAP_PROTOCOL_VERSION, ["3.5"], [Highest DAP version implemented?])
AC_SUBST(DAP_PROTOCOL_VERSION)

AC_CONFIG_SRCDIR([Connect.cc])
AC_CONFIG_AUX_DIR(conf)
AC_CONFIG_HEADERS([config.h dods-datatypes-config.h xdr-datatypes-config.h])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE
AC_CONFIG_TESTDIR(tests, [.])

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

DAPLIB_CURRENT=16
DAPLIB_AGE=5
DAPLIB_REVISION=1
AC_SUBST(DAPLIB_CURRENT)
AC_SUBST(DAPLIB_AGE)
AC_SUBST(DAPLIB_REVISION)

LIBDAP_VERSION="$DAPLIB_CURRENT:$DAPLIB_REVISION:$DAPLIB_AGE"
AC_SUBST(LIBDAP_VERSION)

CLIENTLIB_CURRENT=5
CLIENTLIB_AGE=2
CLIENTLIB_REVISION=0
AC_SUBST(CLIENTLIB_CURRENT)
AC_SUBST(CLIENTLIB_AGE)
AC_SUBST(CLIENTLIB_REVISION)

CLIENTLIB_VERSION="$CLIENTLIB_CURRENT:$CLIENTLIB_REVISION:$CLIENTLIB_AGE"
AC_SUBST(CLIENTLIB_VERSION)

SERVERLIB_CURRENT=12
SERVERLIB_AGE=5
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

dnl echo "CC = $CC"
if test "$CC" = "gcc"
then
    AM_CONDITIONAL([COMPILER_IS_GCC],[true])
else
    AM_CONDITIONAL([COMPILER_IS_GCC],[false])
fi

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
AC_C_BIGENDIAN
        
DODS_CHECK_SIZES

AC_ARG_ENABLE([runtime-endian-check],
    AC_HELP_STRING([--enable-runtime-endian-check], [Enable runtime tests for big- or little-endian byte order (default is NO)])
)

AS_IF([test "x$enable_runtime_endian_check" = "xyes"], [
    dnl Do the stuff needed for enabling the feature
    AC_DEFINE([COMPUTE_ENDIAN_AT_RUNTIME], 1, [Should a function (run-time) be used to determine the byte order?])
])

# Checks for library functions.

dnl using AC_CHECK_FUNCS does not run macros from gnulib.
AC_CHECK_FUNCS([alarm atexit bzero dup2 getcwd getpagesize localtime_r memmove memset pow putenv setenv strchr strerror strtol strtoul timegm mktime])

gl_SOURCE_BASE(gl)
gl_M4_BASE(gl/m4)
gl_MODULES(regex btyeswap)

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

           AS_VERSION_COMPARE(["$version_libcurl"], ["7.10.6"], 
                [AC_MSG_ERROR([I could not find libcurl 7.10.6 or newer, found $version_libcurl])])
        
	   CURL_LIBS="`curl-config --libs`"
	   CURL_STATIC_LIBS=$CURL_LIBS
	   curlprivatelibs="`curl-config --libs`"
	   CURL_CFLAGS="`curl-config --cflags`"
	   AC_MSG_RESULT([yes; used curl-config and found version $version_libcurl])
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
    dnl XML2_STATIC_LIBS=$XML2_LIBS
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
PKG_CHECK_MODULES([XML2],[$libdap_libxml2_module], ,[libdap_pkgconfig_libxml2=no])
AC_MSG_CHECKING([for libxml2])
if test $libdap_pkgconfig_libxml2 = 'yes'
then
	xmlprivatereq=$libdap_libxml2_module
	dnl XML2_STATIC_LIBS="`$PKG_CONFIG --static --libs libxml-2.0`"
	XML2_LIBS="`$PKG_CONFIG --libs libxml-2.0`"
	AC_MSG_RESULT([yes; used pkg-config])
elif xml2-config --version > /dev/null 2>&1
then
	version_libxml2=`xml2-config --version`

        AS_VERSION_COMPARE(["$version_libxml2"], ["2.6.16"], 
                [AC_MSG_ERROR([I could not find libxml2 2.6.16 or newer])])
        
	XML2_LIBS="`xml2-config --libs`"
	dnl XML2_STATIC_LIBS=$XML2_LIBS
	XML2_CFLAGS="`xml2-config --cflags`"
	xmlprivatelibs="`xml2-config --libs`"
	AC_MSG_RESULT([yes; used xml2-config and found version $version_libxml2])
else
	AC_MSG_ERROR([I could not find ml2-config])
fi
fi

AC_SUBST([xmlprivatereq])
AC_SUBST([xmlprivatelibs])
AC_SUBST([XML2_LIBS])
dnl AC_SUBST([XML2_STATIC_LIBS])
AC_SUBST([XML2_CFLAGS])

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

dnl When pkgconfig support is added, use this
libdap_libgridfields_module='libgridfields >= 0.7.0'

libgf_ok="no"

dnl AC_ARG_WITH([gridfields],
dnl     AC_HELP_STRING([--with-gridfields=path], [Use the gridfields library at this location.]),
dnl     [if test "$withval" = "yes"
dnl      then
dnl          AC_MSG_ERROR([You must supply a path when using --with-gridfields; try --enable-gridfields.])
dnl      else
dnl          AC_MSG_NOTICE([Using $withval as the GridFields prefix directory.])
dnl          GF_LIBS="-L$withval/lib -lgridfields"
dnl          GF_CFLAGS="-I$withval/include"
dnl          AC_DEFINE([GRIDFIELDS],[1],[define if gridfields lib is present])
dnl          libgf_ok="yes"
dnl      fi],
dnl     [])

dnl AC_ARG_ENABLE([gridfields],
dnl     AC_HELP_STRING([--enable-gridfields], [Enable libgridfields-based server functions (default is NO)]))

dnl if test "$libgf_ok" = "no" -a "$enable_gridfields" = "yes"
dnl then
     
dnl AC_MSG_CHECKING([for libgridfields])
dnl if gridfields-config --version > /dev/null 2>&1
dnl then
dnl     version_libgf=`gridfields-config --version | sed 's@libgridfields \(.*\)@\1@'`

dnl     version_M=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
dnl     version_m=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
dnl     version_m_m=`echo $version_libgf | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
    
dnl     dnl Test for several different versions of libgridfields
    
dnl     if test $version_M -gt 0
dnl     then
dnl         libgf_ok='yes'
dnl     elif test $version_M -eq 0 && test $version_m -gt 7
dnl     then
dnl         libgf_ok='yes'
dnl     elif test $version_M -eq 0 && test $version_m -eq 7 && test $version_m_m -gt 0
dnl     then
dnl         libgf_ok='yes'
dnl     else
dnl         libgf_ok='no'
dnl     fi

dnl     if test "$libgf_ok"="no"
dnl     then
dnl         AC_MSG_ERROR([must have libgf 0.7.0 or greater, found $version_libgf])
dnl     fi

dnl     GF_LIBS="`gridfields-config --libs`"
dnl     GF_CFLAGS="`gridfields-config --cflags`"

dnl     AC_DEFINE([GRDIFIELDS],[1],[define if gridfields lib is present])
dnl     AC_MSG_RESULT([yes; used gridfields-config])
dnl else
dnl     AC_MSG_ERROR([I could not find libgridfields])
dnl fi

dnl fi

dnl AC_SUBST([GF_LIBS])
dnl AC_SUBST([GF_CFLAGS])

dnl if test "$libgf_ok" = "yes"
dnl then
dnl     echo "Set gridfields CFLAGS to $GF_CFLAGS"
dnl     echo "Set gridfields LIBS to $GF_LIBS"
dnl     AM_CONDITIONAL([USING_GRIDFIELDS],[true])
dnl else
dnl     AM_CONDITIONAL([USING_GRIDFIELDS],[false])
dnl fi

dnl # Which, if any, copy of GDAL should be used for the build?
dnl GDAL_FOUND=

dnl AC_ARG_WITH(gdal, AS_HELP_STRING([--with-gdal], [Use the copy of GDAL at this location]),
dnl             with_gdal_prefix="$withval", with_gdal_prefix="")

dnl if test -n "$with_gdal_prefix" -a -x $with_gdal_prefix/bin/gdal-config
dnl then
dnl     AC_MSG_NOTICE([Using $with_gdal_prefix as the GDAL prefix directory.])
dnl     GDAL_LIBS="`$with_gdal_prefix/bin/gdal-config --libs` `$with_gdal_prefix/bin/gdal-config --dep-libs`"
dnl     GDAL_CFLAGS="`$with_gdal_prefix/bin/gdal-config --cflags`"
dnl     GDAL_FOUND="yes"
dnl elif test -n "$with_gdal_prefix"
dnl then
dnl     AC_MSG_ERROR([You set the gdal-prefix directory to $with_gdal_prefix, but gdal-config is not there.])
dnl fi

dnl # Because of the run-time errors encountered when there are competing
dnl # versions of libs, only include GDAL when explictly told to. NB: see
dnl # the fileout_gdal handler for another place where GDAL can be added to
dnl # a running Hyrax server. jhrg 11/23/12

dnl # if test -z "$GDAL_FOUND"
dnl # then
dnl #    AX_LIB_GDAL([1.9.0])
dnl #    if test ! -z "$GDAL_CFLAGS" -a ! -z "$GDAL_LDFLAGS"; then
dnl #        GDAL_FOUND="yes"
dnl #    fi
dnl # fi

dnl AC_SUBST([GDAL_LIBS])
dnl AC_SUBST([GDAL_CFLAGS])


dnl if test -n "$GDAL_FOUND"
dnl then
dnl     echo "Set GDAL_CFLAGS to $GDAL_CFLAGS"
dnl     echo "Set GDAL_LIBS to $GF_LIBS"
dnl     AM_CONDITIONAL([USING_GDAL],[true])
dnl     AC_DEFINE([GDAL],[1],[define if gdal lib is present])
dnl else
dnl     AM_CONDITIONAL([USING_GDAL],[false])
dnl fi

DODS_DEBUG_OPTION

dnl See util.cc:dods_root()
if test "$prefix" = "NONE"
then
AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])
else
AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])
fi

AC_ARG_ENABLE([dap4], 
    AC_HELP_STRING([--enable-dap4], [Enable DAP4 types and responses (default is NO)]))

AS_IF([test "x$enable_dap4" = "xyes"], [
    dnl Do the stuff needed for enabling the feature
    dnl echo "Defining DAP4! "
    AC_DEFINE([DAP4], 1, [Should DAP4 support be enabled?])
    AM_CONDITIONAL([DAP4_DEFINED], [true]) ],
    [ dnl else enable_dap4 is false
    AM_CONDITIONAL([DAP4_DEFINED], [false])
])

AC_ARG_ENABLE([developer],
[AS_HELP_STRING([--enable-developer],
[Build a debug (-g3 -O0) version of this code and include assert() calls in the code (default is no)])],
[case "${enableval}" in
       yes) build_developer=true ;;
       no)  build_developer=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-developer]) ;;
     esac],
[build_developer=false])

if test "x$build_developer" = "xtrue"; then
   AC_MSG_NOTICE(["Building developer version"])
else
   AC_MSG_NOTICE(["Not building developer version"])
   AC_DEFINE([NDEBUG], [1], [Remove this to activate assert() calls.])
fi
AM_CONDITIONAL([BUILD_DEVELOPER], [test "x$build_developer" = "xtrue"])

dnl AC_ARG_ENABLE([checksums], 
dnl    AC_HELP_STRING([--enable-checksums], [Enable MD5 checksums for DAP2 (default is NO)]))
dnl
dnl echo "enable_checksums = $enable_checksums"
dnl AS_IF([test "x$enable_checksums" = "xyes"], [
dnl    dnl Do the stuff needed for enabling the feature
dnl    dnl echo "Defining checksums! "
dnl    AC_DEFINE([CHECKSUMS], 1, [Should checksums be generated])
dnl    AM_CONDITIONAL([CHECKSUMS_DEFINED], [true]) ],
dnl    [ dnl else enable_checksums is false
dnl    AM_CONDITIONAL([CHECKSUMS_DEFINED], [false])
dnl])

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
                 main_page.doxygen
	         doxy.conf
                 gl/Makefile
                 tests/Makefile
                 tests/atlocal
                 unit-tests/Makefile
                 unit-tests/cache-testsuite/Makefile])

dnl swath2grid/Makefile
dnl swath2grid/bes.conf
dnl ugridFunctions/Makefile

AC_CONFIG_FILES([dap-config], [chmod +x dap-config]) 
	
AC_OUTPUT
