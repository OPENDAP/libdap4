
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)

dnl AC_PREREQ([2.68]); Fails on CentOS 6. jhrg 3/26/19
dnl m4_pattern_forbid([^_?(gl_[A-Z]|AX_|BISON_)])
dnl m4_pattern_allow([^BISON_USE_NLS$])

dnl Set the version number here. Set the build number using --with-build
dnl The version info is put in the VERSION file. The build number is used
dnl in the RPM spec file. The build number is not part of the pkg config
dnl 'pc' files and tarball for the source. See
dnl libdap4/travis/travis_libdap_build_offset.sh for info about setting the
dnl base value for the build number. jhrg 3/22/21

AC_INIT(libdap, 3.20.11, opendap-tech@opendap.org)

dnl As an alternative, we can...
dnl Read the version number from the file VERSION. The file must contain
dnl only the version number in the form x.y.z or x.y.z-b. Newlines are
dnl ignored. Anything else and the build will fail.
dnl
dnl But, this requires that the NEWS file have the version number AND build
dnl number somewhere in the first 15 lines or the dist target will fail.
dnl jhrg 3/17/21
dnl https://stackoverflow.com/questions/8559456/read-a-version-number-from-a-file-in-configure-ac
dnl AC_INIT([libdap], m4_normalize(m4_include([VERSION])), [opendap-tech@opendap.org])

dnl Provide a way to pass in a build number. This is used by CI/CD systems to record
dnl each build. The version number is set by the value in AC_INIT (see above). For
dnl people building the source, it's not customary to supply a build number (i.e.,
dnl if you're building the code for your own use, a build number seems like overkill).
dnl jhrg 3/16/21
AC_ARG_WITH([build],
            [AC_HELP_STRING([--with-build=<number>],
             [Inject the integer build number (default is to not define a build number)])],
            [build_number=${withval}], [build_number=])

dnl If a build number is given and valid (it must be an integer), use it. If it's
dnl given but invalid, print a message and exit. jhrg 3/17/21
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

dnl This block of code can be removed since the DDS now only builds the v3.2 DDX.
dnl jhrg 2/4/22
dnl
dnl Use one of these two blocks to build a DAP2 or DAP 3.2 DDX
dnl AC_DEFINE(DAP2_DDX, 1, [Build the DAP 2 version of the DDX])
dnl AC_SUBST(DAP2_DDX)
dnl AC_DEFINE(DAP3_2_DDX, 1, [Build the DAP 3.2 version of the DDX])
dnl AC_SUBST(DAP3_2_DDX)

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

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

dnl How to set these SO variables:
dnl No interfaces changed, only implementations (good): ==> Increment REVISION.
dnl Interfaces added, none removed (good): ==> Increment CURRENT,
dnl increment AGE, set REVISION to 0.
dnl Interfaces removed or changed (BAD, breaks upward compatibility):
dnl ==> Increment CURRENT, set AGE and REVISION to 0.

DAPLIB_CURRENT=28
DAPLIB_REVISION=1
DAPLIB_AGE=1

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

dnl Suppress the default values automake supplies for these. You can still
dnl set them on the command line, however. (e.g., ./configure ... CXXFLAGS="-g -o3").
dnl jhrg 2/6/18
: ${CFLAGS=""}
: ${CXXFLAGS=""}

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC

dnl Call this gnulib macro right after a working C Compiler is found
gl_EARLY

dnl echo "CC = $CC"
AS_IF([test "x$CC" = xgcc],
      [AM_CONDITIONAL([COMPILER_IS_GCC],[true])],
      [AM_CONDITIONAL([COMPILER_IS_GCC],[false])])

dnl AC_PROG_YACC
dnl AC_PROG_LEX

dnl Here we test for both C++11 and 14, but our code nominally requires 14.
dnl We can use the C++11 test and CXX_STD_FLAG to build with 11 if we add a
dnl compatibility layer for the missing features. jhrg 1/30/23.

CXX_STD_FLAG=""

CXX_FLAGS_CHECK([--std=c++11], [CXX_STD_FLAG=--std=c++11], [])
CXX_FLAGS_CHECK([--std=c++14], [CXX_STD_FLAG=--std=c++14],
    [AC_MSG_WARN([C++14 is required but not found. Using C++11 compatability mode.])])

AS_IF([test -z "$CXX_STD_FLAG"],
      [AC_MSG_ERROR([Not using modern C++ (C++11 or 14)])],
      [AC_MSG_NOTICE([Using $CXX_STD_FLAG])
       cxx_debug_flags="$cxx_debug_flags --pedantic $CXX_STD_FLAG"])

AC_SUBST(CXX_STD_FLAG)

AM_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl We really need bison and not yacc. If you use AC_PROG_YACC, the resulting 
dnl Makefile will call bison -y which doesn't know how to make the parsers 
dnl we require. jhrg 6/15/05
AC_CHECK_PROG(YACC,[bison],[bison])

dnl We have found bison; get its version
bison_version=`bison --version | sed -n '/^bison.*/p' | sed 's@.* \(.*\)@\1@'`

AC_MSG_CHECKING([for bison 3.0])

AS_VERSION_COMPARE(["$bison_version"], ["3.0"], 
	[AC_MSG_ERROR([not found])],
	[ ],
	[ ])

AC_MSG_RESULT([found vesion $bison_version])

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS_ONCE([fcntl.h malloc.h memory.h stddef.h stdlib.h string.h strings.h unistd.h pthread.h])
AC_CHECK_HEADERS_ONCE([sys/param.h sys/time.h])
AC_CHECK_HEADERS_ONCE([netinet/in.h])

dnl Do this because we have had a number of problems with the UUID header/library
AC_CHECK_HEADERS([uuid/uuid.h],[found_uuid_uuid_h=true],[found_uuid_uuid_h=false])
AC_CHECK_HEADERS([uuid.h],[found_uuid_h=true],[found_uuid_h=false])

AS_IF([test $found_uuid_uuid_h = true -o $found_uuid_h = true], [], [AC_MSG_ERROR([Could not find uuid.h])])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_HEADER_TIME
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
    [AC_HELP_STRING([--enable-runtime-endian-check], 
                    [Enable runtime tests for big- or little-endian byte order (default is NO)])])

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

AC_ARG_WITH([curl], [AC_HELP_STRING([--with-curl=pfx], [curl/libcurl prefix; overrides other tests including pkgconfig])],
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

dnl ******** new version, breaks the bes package step ********
dnl jhrg 8/31/20
dnl
dnl AC_ARG_WITH(xml2,
dnl     [AC_HELP_STRING([--with-xml2=PFX],
dnl         [Prefix where libxml2 is installed (optional). This will override pkgconfig, etc.])],
dnl     [with_xml2_prefix="$withval"],
dnl     [with_xml2_prefix=""])
dnl
dnl AS_IF([test -n "$with_xml2_prefix" -a ! -x $with_xml2_prefix/bin/xml2-config],
dnl     [AC_MSG_ERROR([You set the libxml2 prefix directory to $with_xml2_prefix, but xml2-config is not there.])])
dnl 
dnl xmlprivatereq=
dnl xmlprivatelibs=
dnl xml_set=
dnl 
dnl # I changed this code so that it searches for libxml2 using xml2-config
dnl # first, then pkg-config. This works more reliably when working on OSX
dnl # given Apple's penchant for moving the lib. jhrg 8/25/20
dnl 
dnl AS_IF([test -n "$with_xml2_prefix" -a -x $with_xml2_prefix/bin/xml2-config],
dnl     [
dnl     AC_MSG_NOTICE([Using $with_xml2_prefix as the libxml2 prefix directory.])
dnl     XML2_LIBS="`$with_xml2_prefix/bin/xml2-config --libs`"
dnl     XML2_CFLAGS="`$with_xml2_prefix/bin/xml2-config --cflags`"
dnl     xml_set="yes"
dnl     ])
dnl 
dnl # Try using the xml2-config script.
dnl 
dnl AS_IF([test -z "$xml_set" & xml2-config --version > /dev/null 2>&1],
dnl     [
dnl     AC_MSG_CHECKING([for libxml2])
dnl    	version_libxml2=`xml2-config --version`
dnl 
dnl     AS_VERSION_COMPARE(["$version_libxml2"], ["2.7.0"],
dnl         [AC_MSG_ERROR([I could not find libxml2 2.7.0 or newer])])
dnl 
dnl    	XML2_LIBS="`xml2-config --libs`"
dnl    	XML2_CFLAGS="`xml2-config --cflags`"
dnl    	xmlprivatelibs="`xml2-config --libs `"
dnl 
dnl    	# If XML2_CFLAGS does not have -I that ends in /libxml2, append that to
dnl    	# the string bound to -I. Assume there is only on -I in XML2_CFLAGS. jhrg 8/25/20
dnl    	AS_IF([echo $XML2_CFLAGS | grep -v -e '-I.*/libxml2'],
dnl    	    [XML2_CFLAGS=`echo $XML2_CFLAGS | sed "s@\(-I.*\)@\1/libxml2/@g"`])
dnl 
dnl    	AC_MSG_RESULT([yes; used xml2-config and found version $version_libxml2])
dnl    	xml_set=yes
dnl    	])
dnl 
dnl # If not found, try pkg-config
dnl AS_IF([test -z "$xml_set"],
dnl     [
dnl     libdap_libxml2_module='libxml-2.0 >= 2.7.0'
dnl     PKG_CHECK_MODULES([XML2], [libdap_libxml2_module],
dnl         [libdap_pkgconfig_libxml2=yes],
dnl         [libdap_pkgconfig_libxml2=no])
dnl     AS_IF([test $libdap_pkgconfig_libxml2 = yes],
dnl         [
dnl         XML2_LIBS="`$PKG_CONFIG --libs libxml-2.0`"
dnl         XML2_CFLAGS="`$PKG_CONFIG --cflags libxml-2.0`"
dnl         AC_MSG_RESULT([yes; used pkg-config])
dnl         xmlprivatereq=$libdap_libxml2_module
dnl         xml_set=yes
dnl         ],
dnl         [
dnl 	    AC_MSG_ERROR([I could not find xml2-config])
dnl 	    ])
dnl     ])
dnl 
dnl AS_IF([test -z "xml_set"],
dnl     [
dnl     AC_MSG_ERROR([I could not find xml2-config])
dnl     ])
dnl 
dnl AC_SUBST([xmlprivatereq])
dnl AC_SUBST([xmlprivatelibs])
dnl AC_SUBST([XML2_LIBS])
dnl AC_SUBST([XML2_CFLAGS])
dnl 
dnl ******** end new, broken-for-the-bes version *********

dnl Version of XML2 configuration from git commit 8624abec8e3d510508c3c97ac60082700995af2c
dnl jhrg 8/31/20

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
dnl AC_SUBST([XML2_STATIC_LIBS])
AC_SUBST([XML2_CFLAGS])

dnl End old version of XML2 configuration. jhrg 8/31/20
dnl ********

dnl Check for the RHEL 8 requirement libtirpc and its headers.
dnl jhrg 6/23/22

OX_RHEL8_TIRPC

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

dnl See util.cc:dods_root()
AS_IF([test "$prefix" = "NONE"],
      [AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])],
	  [AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])])

AC_ARG_ENABLE([asan], 
    [AS_HELP_STRING([--enable-asan], [Build using the address sanitizer if available (default: no)])])

ASAN_OPTIONS="-fsanitize=address -fno-omit-frame-pointer"
CXX_FLAGS_CHECK([$ASAN_OPTIONS], [has_asan=yes], [has_asan=no])

AS_IF([test x$enable_asan = xyes -a x$has_asan = xyes],
      [AC_MSG_NOTICE([Building Address Sanitizer version])
       AM_CONDITIONAL([USE_ASAN], [true])],
      [AC_MSG_NOTICE([Not building Address Sanitizer version])
       AM_CONDITIONAL([USE_ASAN], [false])])

dnl An optional big array(>4GB) unit test. Since it is resource consuming, turn it off by default.
AC_ARG_ENABLE([batest], 
    [AS_HELP_STRING([--enable-batest], [The big array unit test is turned on.  (default: no)])])
           
AS_IF([test x$enable_batest = xyes ],
      [AC_MSG_NOTICE([Adding big array unit test])
       AM_CONDITIONAL([USE_BA], [true])],
      [AC_MSG_NOTICE([Not adding big array unit test])
       AM_CONDITIONAL([USE_BA], [false])])

AC_ARG_ENABLE([leaks],
    [AS_HELP_STRING([--enable-leaks], [Run unit tests on OSX using the 'leaks' if available (default: no)])])

# The 'leaks' tool on OSX can be used to test if a program leaks memory.
# Look for 'leaks' and set LEAKS to it if found, else set it to 'no'
AC_CHECK_PROG(LEAKS, [leaks], [leaks], [no])

AS_IF([test x$enable_leaks = xyes -a x$LEAKS != xno],
      [AC_MSG_NOTICE([Will run unit-tests using leaks])
       LEAKS_FLAGS="-atExit -- "
       AC_SUBST(LEAKS_FLAGS)
       AM_CONDITIONAL([USE_LEAKS], [true])],
      [AC_MSG_NOTICE([Will not run unit-tests using leaks])
       LEAKS_FLAGS=
       AC_SUBST(LEAKS_FLAGS)
       AM_CONDITIONAL([USE_LEAKS], [false])])

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

dnl autoheader macros; tack some text at the top and bottom of config_dap.h.in

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
