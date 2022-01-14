
# -*- autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

# Set the version number here. Set the build number using --with-build
# The version info is put in the VERSION file. The build number is used
# in the RPM spec file. The build number is not part of the pkg config
# 'pc' files and tarball for the source. See
# libdap4/travis/travis_libdap_build_offset.sh for info about setting the
# base value for the build number. jhrg 3/22/21

AC_INIT([libdap],[3.20.9],[opendap-tech@opendap.org])

# As an alternative, we can...
# Read the version number from the file VERSION. The file must contain
# only the version number in the form x.y.z or x.y.z-b. Newlines are
# ignored. Anything else and the build will fail.
#
# But, this requires that the NEWS file have the version number AND build
# number somewhere in the first 15 lines or the dist target will fail.
# jhrg 3/17/21
# https://stackoverflow.com/questions/8559456/read-a-version-number-from-a-file-in-configure-ac
# AC_INIT([libdap],[m4_normalize(m4_include(VERSION))],[opendap-tech@opendap.org])

# Provide a way to pass in a build number. This is used by CI/CD systems to record
# each build. The version number is set by the value in AC_INIT (see above). For
# people building the source, it's not customary to supply a build number (i.e.,
# if you're building the code for your own use, a build number seems like overkill).
# jhrg 3/16/21
AC_ARG_WITH([build],
            [AS_HELP_STRING([--with-build=number],[Inject the integer build number (default is to not define a build number)])],
            [build_number=${withval}], [build_number=])

# If a build number is given and valid (it must be an integer), use it. If it's
# given but invalid, print a message and exit. jhrg 3/17/21
AS_IF([test -n "$with_build"],
    [AS_IF([echo $build_number | grep '^[[0-9]][[-0-9.]]*$' > /dev/null 2>&1 ],
     [PACKAGE_VERSION=$PACKAGE_VERSION-$build_number
      echo "PACKAGE_VERSION: $PACKAGE_VERSION"],
     [AC_MSG_ERROR([Invalid build number given (must be an integer): $build_number])])])

AC_DEFINE(DAP_PROTOCOL_VERSION, ["4.0"], [Highest DAP version implemented?])
AC_SUBST([DAP_PROTOCOL_VERSION])

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

# Use one of these two blocks to build a DAP2 or DAP 3.2 DDX
# AC_DEFINE(DAP2_DDX, 1, [Build the DAP 2 version of the DDX])
# AC_SUBST(DAP2_DDX)
AC_DEFINE(DAP3_2_DDX, 1, [Build the DAP 3.2 version of the DDX])
AC_SUBST(DAP3_2_DDX)

AS_IF([echo $PACKAGE_VERSION | grep '^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)-\([[0-9]]*\)$'],
      [PACKAGE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)-\([[0-9]]*\)$@\1@'`
       PACKAGE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)-\([[0-9]]*\)$@\2@'`
       PACKAGE_PATCH_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)-\([[0-9]]*\)$@\3@'`
       PACKAGE_BUILD_NUMBER=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)-\([[0-9]]*\)$@\4@'`],
      [AS_IF([echo $PACKAGE_VERSION | grep '^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$'],
       [PACKAGE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
        PACKAGE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
        PACKAGE_PATCH_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
        PACKAGE_BUILD_NUMBER=],
       [AC_MSG_ERROR([VERSION file does not contain a valid version number (x.y.z or x.y.z-b)])])])

AC_MSG_NOTICE(Package Version: $PACKAGE_VERSION)
AC_MSG_NOTICE(Package Minor Version: $PACKAGE_MINOR_VERSION)
AC_MSG_NOTICE(Package Patch Version: $PACKAGE_PATCH_VERSION)
AC_MSG_NOTICE(Package Build Number: $PACKAGE_BUILD_NUMBER)
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_PATCH_VERSION)
AC_SUBST(PACKAGE_BUILD_NUMBER)

# flags for the compilers and linkers - set these before locating the
# actual tools since some of the AC_PROG macros set these `flag variables'
# to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

# How to set these SO variables:
# No interfaces changed, only implementations (good): ==> Increment REVISION.
# Interfaces added, none removed (good): ==> Increment CURRENT,
# increment AGE, set REVISION to 0.
# Interfaces removed or changed (BAD, breaks upward compatibility):
# ==> Increment CURRENT, set AGE and REVISION to 0.

DAPLIB_CURRENT=27
DAPLIB_REVISION=5
DAPLIB_AGE=0

AC_SUBST(DAPLIB_CURRENT)
AC_SUBST(DAPLIB_REVISION)
AC_SUBST(DAPLIB_AGE)

LIBDAP_VERSION="$DAPLIB_CURRENT:$DAPLIB_REVISION:$DAPLIB_AGE"
AC_SUBST(LIBDAP_VERSION)

CLIENTLIB_CURRENT=7
CLIENTLIB_AGE=1
CLIENTLIB_REVISION=11
AC_SUBST(CLIENTLIB_CURRENT)
AC_SUBST(CLIENTLIB_AGE)
AC_SUBST(CLIENTLIB_REVISION)

CLIENTLIB_VERSION="$CLIENTLIB_CURRENT:$CLIENTLIB_REVISION:$CLIENTLIB_AGE"
AC_SUBST(CLIENTLIB_VERSION)

SERVERLIB_CURRENT=13
SERVERLIB_AGE=6
SERVERLIB_REVISION=11
AC_SUBST(SERVERLIB_CURRENT)
AC_SUBST(SERVERLIB_AGE)
AC_SUBST(SERVERLIB_REVISION)

SERVERLIB_VERSION="$SERVERLIB_CURRENT:$SERVERLIB_REVISION:$SERVERLIB_AGE"
AC_SUBST(SERVERLIB_VERSION)

# Suppress the default values automake supplies for CFLAGS and CXXFLAGS. You can still
# set them on the command line, however. (e.g., ./configure ... CXXFLAGS="-g -o3").
# jhrg 2/6/18
: ${CFLAGS=""}
: ${CXXFLAGS=""}

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

# Call this gnulib macro right after a working C Compiler is found.
#
# To update the gnulib code, cd to the 'gl' dir and look at the instructions
# in Makefile.am regarding gnulib-tool. Copy that command and run it in the
# libdap4 directory. jhrg 1/14/22

gl_EARLY

AS_IF([test "x$CC" = xgcc],
      [AM_CONDITIONAL([COMPILER_IS_GCC],[true])],
      [AM_CONDITIONAL([COMPILER_IS_GCC],[false])])

# NB: CentOS 6 does not support C++-11. We require C++-11

CXX11_FLAG=""

CXX_FLAGS_CHECK([--std=c++11], [CXX11_FLAG=--std=c++11], [])

AS_IF([test -z "$CXX11_FLAG"],
      [AC_MSG_ERROR([Not using C++11])],
      [AC_MSG_NOTICE([Using $CXX11_FLAG for C++11 support])
       cxx_debug_flags="$cxx_debug_flags --pedantic $CXX11_FLAG"])

AC_SUBST(CXX11_FLAG)

AM_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT

# We really need bison and not yacc. If you use AC_PROG_YACC, the resulting 
# Makefile will call bison -y which doesn't know how to make the parsers 
# we require. jhrg 6/15/05

AC_CHECK_PROG(YACC,[bison],[bison])

# We have found bison; get its version
bison_version=`bison --version | sed -n '/^bison.*/p' | sed 's@.* \(.*\)@\1@'`

AC_MSG_CHECKING([for bison 3.0])
AS_VERSION_COMPARE(["$bison_version"], ["3.0"], 
	[AC_MSG_ERROR([not found])],
	[ ],
	[ ])

AC_MSG_RESULT([found version $bison_version])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS_ONCE([fcntl.h malloc.h memory.h stddef.h stdlib.h string.h strings.h unistd.h pthread.h])
AC_CHECK_HEADERS_ONCE([sys/param.h sys/time.h])
AC_CHECK_HEADERS_ONCE([netinet/in.h])

# AC_CHECK_HEADERS_ONCE([uuid/uuid.h uuid.h])
# Do this because we have had a number of problems with the UUID header/library
AC_CHECK_HEADERS([uuid/uuid.h],[found_uuid_uuid_h=true],[found_uuid_uuid_h=false])
AC_CHECK_HEADERS([uuid.h],[found_uuid_h=true],[found_uuid_h=false])

AS_IF([test $found_uuid_uuid_h = true -o $found_uuid_h = true], [], [AC_MSG_ERROR([Could not find uuid.h])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])

AC_STRUCT_TM
AC_C_VOLATILE
AC_C_BIGENDIAN

# This is used by the DMR tests which must choose the correct set of baselines
# based on word order given that DAP4 uses 'reader make right' and thus the 
# DAP4 data responses are different for different word order machines. jhrg 9/29/15
AS_IF([test $ac_cv_c_bigendian = yes], [ac_word_order=big-endian], [ac_word_order=little-endian])
AC_SUBST([ac_word_order])
        
DODS_CHECK_SIZES

AC_ARG_ENABLE([runtime-endian-check],
    [AS_HELP_STRING([--enable-runtime-endian-check],[Enable runtime tests for big- or little-endian byte order (default is NO)])])

AS_IF([test "x$enable_runtime_endian_check" = "xyes"], [
    # Do the stuff needed for enabling the feature
    AC_DEFINE([COMPUTE_ENDIAN_AT_RUNTIME], 1, [Should a function (run-time) be used to determine the byte order?])
])

# Checks for library functions.

# using AC_CHECK_FUNCS does not run macros from gnulib.
AC_CHECK_FUNCS([alarm atexit bzero dup2 getcwd getpagesize localtime_r memmove memset pow putenv setenv strchr strerror strtol strtoul timegm mktime])

# To update the gnulib code, cd to the 'gl' dir and look at the instructions
# in Makefile.am regarding gnulib-tool. Copy that command and run it in the
# libdap4 directory. jhrg 1/14/22
gl_SOURCE_BASE(gl)
gl_M4_BASE(gl/m4)
gl_MODULES(regex btyeswap)

gl_INIT

AC_ARG_WITH([curl], [AS_HELP_STRING([--with-curl=pfx],[curl/libcurl prefix; overrides other tests including pkgconfig])],
            with_curl_prefix="$withval", with_curl_prefix="")

# I wrote these checks because we need the *-config scripts to build, so 
# the AC_CHECK_LIB macro is not needed.

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
    libdap_libcurl_module='libcurl >= 7.19.0'
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

       AS_VERSION_COMPARE(["$version_libcurl"], ["7.19.0"], 
                [AC_MSG_ERROR([I could not find libcurl 7.19.0 or newer, found $version_libcurl])])
        
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

# ******** new version, breaks the bes package step ********
# jhrg 8/31/20
#
# AC_ARG_WITH(xml2,
#     [AS_HELP_STRING([--with-xml2=PFX],[dnl         Prefix where libxml2 is installed (optional). This will override pkgconfig, etc.])],
#     [with_xml2_prefix="$withval"],
#     [with_xml2_prefix=""])
#
# AS_IF([test -n "$with_xml2_prefix" -a ! -x $with_xml2_prefix/bin/xml2-config],
#     [AC_MSG_ERROR([You set the libxml2 prefix directory to $with_xml2_prefix, but xml2-config is not there.])])
# 
# xmlprivatereq=
# xmlprivatelibs=
# xml_set=
# 
# # I changed this code so that it searches for libxml2 using xml2-config
# # first, then pkg-config. This works more reliably when working on OSX
# # given Apple's penchant for moving the lib. jhrg 8/25/20
# 
# AS_IF([test -n "$with_xml2_prefix" -a -x $with_xml2_prefix/bin/xml2-config],
#     [
#     AC_MSG_NOTICE([Using $with_xml2_prefix as the libxml2 prefix directory.])
#     XML2_LIBS="`$with_xml2_prefix/bin/xml2-config --libs`"
#     XML2_CFLAGS="`$with_xml2_prefix/bin/xml2-config --cflags`"
#     xml_set="yes"
#     ])
# 
# # Try using the xml2-config script.
# 
# AS_IF([test -z "$xml_set" & xml2-config --version > /dev/null 2>&1],
#     [
#     AC_MSG_CHECKING([for libxml2])
#    	version_libxml2=`xml2-config --version`
# 
#     AS_VERSION_COMPARE(["$version_libxml2"], ["2.7.0"],
#         [AC_MSG_ERROR([I could not find libxml2 2.7.0 or newer])])
# 
#    	XML2_LIBS="`xml2-config --libs`"
#    	XML2_CFLAGS="`xml2-config --cflags`"
#    	xmlprivatelibs="`xml2-config --libs `"
# 
#    	# If XML2_CFLAGS does not have -I that ends in /libxml2, append that to
#    	# the string bound to -I. Assume there is only on -I in XML2_CFLAGS. jhrg 8/25/20
#    	AS_IF([echo $XML2_CFLAGS | grep -v -e '-I.*/libxml2'],
#    	    [XML2_CFLAGS=`echo $XML2_CFLAGS | sed "s@\(-I.*\)@\1/libxml2/@g"`])
# 
#    	AC_MSG_RESULT([yes; used xml2-config and found version $version_libxml2])
#    	xml_set=yes
#    	])
# 
# # If not found, try pkg-config
# AS_IF([test -z "$xml_set"],
#     [
#     libdap_libxml2_module='libxml-2.0 >= 2.7.0'
#     PKG_CHECK_MODULES([XML2], [libdap_libxml2_module],
#         [libdap_pkgconfig_libxml2=yes],
#         [libdap_pkgconfig_libxml2=no])
#     AS_IF([test $libdap_pkgconfig_libxml2 = yes],
#         [
#         XML2_LIBS="`$PKG_CONFIG --libs libxml-2.0`"
#         XML2_CFLAGS="`$PKG_CONFIG --cflags libxml-2.0`"
#         AC_MSG_RESULT([yes; used pkg-config])
#         xmlprivatereq=$libdap_libxml2_module
#         xml_set=yes
#         ],
#         [
# 	    AC_MSG_ERROR([I could not find xml2-config])
# 	    ])
#     ])
# 
# AS_IF([test -z "xml_set"],
#     [
#     AC_MSG_ERROR([I could not find xml2-config])
#     ])
# 
# AC_SUBST([xmlprivatereq])
# AC_SUBST([xmlprivatelibs])
# AC_SUBST([XML2_LIBS])
# AC_SUBST([XML2_CFLAGS])
# 
# ******** end new, broken-for-the-bes version *********

# Version of XML2 configuration from git commit 8624abec8e3d510508c3c97ac60082700995af2c
# jhrg 8/31/20

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
libdap_libxml2_module='libxml-2.0 >= 2.7.0'
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

        AS_VERSION_COMPARE(["$version_libxml2"], ["2.7.0"],
                [AC_MSG_ERROR([I could not find libxml2 2.7.0 or newer])])

	XML2_LIBS="`xml2-config --libs`"
	dnl XML2_STATIC_LIBS=$XML2_LIBS
	XML2_CFLAGS="`xml2-config --cflags`"
	xmlprivatelibs="`xml2-config --libs `"
	dnl `
	AC_MSG_RESULT([yes; used xml2-config and found version $version_libxml2])
else
	AC_MSG_ERROR([I could not find xml2-config])
fi
fi

AC_SUBST([xmlprivatereq])
AC_SUBST([xmlprivatelibs])
AC_SUBST([XML2_LIBS])
# AC_SUBST([XML2_STATIC_LIBS])
AC_SUBST([XML2_CFLAGS])

# End old version of XML2 configuration. jhrg 8/31/20
# ********

AC_CHECK_LIB([pthread], [pthread_kill], 
	[PTHREAD_LIBS="-lpthread"],
	[AC_MSG_ERROR([I could not find pthreads])])
AC_SUBST([PTHREAD_LIBS])

AC_CHECK_LIB([uuid], [uuid_generate], 
	[UUID_LIBS="-luuid"],
	[UUID_LIBS=""])
AC_SUBST([UUID_LIBS])

AC_CHECK_LIB([crypto], [OpenSSL_add_all_algorithms], 
	[CRYPTO_LIBS="-lcrypto"],
	[CRYPTO_LIBS=""])
AC_SUBST([CRYPTO_LIBS])

AM_PATH_CPPUNIT(1.12.0,
	[AM_CONDITIONAL([CPPUNIT], [true])],
	[
	    PKG_CHECK_MODULES(CPPUNIT, [cppunit >= 1.12.0],
		[AM_CONDITIONAL([CPPUNIT], [true])],
		[AM_CONDITIONAL([CPPUNIT], [false])]
	    )
	]
)

DODS_DEBUG_OPTION

# See util.cc:dods_root()
AS_IF([test "$prefix" = "NONE"],
      [AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])],
	  [AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])])

# By default, DAP4 is enabled. Use this to disable. jhrg 5/12/15
AC_ARG_ENABLE([dap4], 
    [AS_HELP_STRING([--enable-dap4],[Enable DAP4 types and responses (default is Yes)])],
    [enable_dap4=${enableval}], [enable_dap4=yes])

AS_IF([test x$enable_dap4 = xyes], [
    dnl Do the stuff needed for enabling the feature
    dnl echo "Defining DAP4! "
    AC_DEFINE([DAP4], 1, [Should DAP4 support be enabled?])
    AM_CONDITIONAL([DAP4_DEFINED], [true]) ],
    [ dnl else enable_dap4 is false
    AM_CONDITIONAL([DAP4_DEFINED], [false])
])

AC_ARG_ENABLE([asan], 
    [AS_HELP_STRING([--enable-asan], [Build using the address sanitizer if available (default: no)])])

AS_IF([test x$enable_asan = xyes -a x$has_asan = xyes],
      [AC_MSG_NOTICE([Building Address Sanitizer version])
       AM_CONDITIONAL([USE_ASAN], [true])],
      [AC_MSG_NOTICE([Not building Address Sanitizer version])
       AM_CONDITIONAL([USE_ASAN], [false])])

AC_ARG_ENABLE([developer],
    [AS_HELP_STRING([--enable-developer], [Build for debug (-g3 -O0); include assert() calls (default: no)])])

AS_IF([test x$enable_developer = xyes],
      [AC_MSG_NOTICE([Building developer version])
       AM_CONDITIONAL([BUILD_DEVELOPER], [true])],
      [AC_MSG_NOTICE([Not building developer version])
       AM_CONDITIONAL([BUILD_DEVELOPER], [false])
       AC_DEFINE([NDEBUG], [1], [Define this to suppres assert() calls.])])

AC_ARG_ENABLE([coverage],
    [AS_HELP_STRING([--enable-coverage], [Build so tests emit coverage data and enable coverage target (default: no)])])

AC_CHECK_LIB([gcov], [gcov_open], [GCOV_LIBS="-lgcov"], [GCOV_LIBS=])

AS_IF([test x$enable_coverage = xyes && which gcov],
      [AC_MSG_NOTICE([Building coverage version])
       AM_CONDITIONAL([ENABLE_COVERAGE], [true])
       AS_IF([gcov -help | grep LLVM],
       	     [GCOVR_FLAGS=],
	         [GCOVR_FLAGS="-k -e '.*Test.cc' -e '.*T.cc' -e '.*-test.cc'"
	          LIBS="-lgcov $LIBS"])],
      [AC_MSG_NOTICE([Not building coverage version])
       AS_IF([test x$enable_coverage = xyes],
             [AC_MSG_NOTICE([Check that gcov is on your PATH])])
       AM_CONDITIONAL([ENABLE_COVERAGE], [false])])

AC_SUBST([GCOVR_FLAGS])

# autoheader macros; tack some text at the top and bottom of config_dap.h.in

AH_TOP([#ifndef _config_h
#define _config_h])

AH_BOTTOM([#endif /* _config_h */])

AC_CONFIG_FILES([Makefile
                 libdap_VERSION
                 libdap.spec
                 libdap.pc
                 libdapclient.pc
                 libdapserver.pc
                 main_page.doxygen
	             doxy.conf
	             abi_checker.xml

                 gl/Makefile
                 d4_ce/Makefile
                 d4_function/Makefile
                 
                 tests/Makefile
                 tests/atlocal
                 
                 unit-tests/Makefile
                 unit-tests/cache-testsuite/Makefile])

AC_CONFIG_FILES([dap-config], [chmod +x dap-config])
# AC_CONFIG_FILES([unit-tests/cache-testsuite/cleanup.sh], [chmod +x unit-tests/cache-testsuite/cleanup.sh])
	
AC_OUTPUT
