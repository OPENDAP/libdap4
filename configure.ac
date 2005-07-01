
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(libdap, 3.5.1, dods-tech@unidata.ucar.edu)
AC_CONFIG_SRCDIR([AISConnect.cc])
AM_CONFIG_HEADER([config_dap.h dods-datatypes.h regex-0.12/config.h])
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

# Until further notice OS X port doesn't get -gstabs because of an
# apparent bug in their gcc 3.3 clone (a beta).  If you see this after
# 1/1/2005, you can probably remove it.
case "${host_cpu}:${host_vendor}:${host_os}" in
     powerpc:apple:netbsd* | alphaev56:dec:osf5.* | powerpc:apple:darwin*)
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

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl We really need bison and not yacc. If you use AC_PROG_YACC, the resulting 
dnl Makfile will call bison -y which doesn't know how to make the parsers 
dnl we require. jhrg 6/15/05
AC_CHECK_PROG(YACC,[bison],[bison])

dnl Find where perl lives on the user's system
DODS_PROG_PERL
dnl Find where GNU tar lives on the user's system
DODS_PROG_GTAR

AC_CHECK_PROG(VALGRIND, valgrind, [valgrind --logfile=memcheck --tool=memcheck --leak-check=yes])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h malloc.h memory.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/param.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
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
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([alarm atexit bzero dup2 getcwd getpagesize localtime_r memmove memset pow putenv setenv strchr strerror strstr strtol strtoul])
# Added 04/22/05 jhrg
AC_CHECK_FUNCS([timegm mktime])

AC_CHECK_HEADERS([regex.h],[openndap_regex_H=yes],[openndap_regex_H=no])
AC_CHECK_FUNCS([re_match_2 re_compile_fastmap],[openndap_regex_funcs=yes],
  [openndap_regex_funcs=no])
AM_CONDITIONAL([USE_GNU_REGEX_FUNCS],
	       [test "z$openndap_regex_funcs" != "zyes" \
		     -o "z$openndap_regex_H" != "zyes"])

dnl if test "z$openndap_regex_funcs" = "zyes" \
dnl    -a "z$openndap_regex_H" = "zyes"; then
dnl     AC_MSG_NOTICE([Using system regex library])  
dnl else
dnl     CPPFLAGS="$CPPFLAGS -I\$(srcdir)/regex-0.12"
dnl     AC_MSG_NOTICE([Using package regex library])  
dnl     DODS_REGEX_CLEAN=regex-0.12/config.h
dnl     DODS_REGEX_SRCS=regex-0.12/regex.c
dnl     DODS_REGEX_OBJS=regex-0.12/regex.o
dnl fi
dnl AC_SUBST(DODS_REGEX_CLEAN)
dnl AC_SUBST(DODS_REGEX_SRCS)
dnl AC_SUBST(DODS_REGEX_OBJS)

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

dnl AC_CHECK_LIB([curl], [curl_version], 
dnl 	[LIBS="$LIBS `curl-config --libs`"
dnl 	 CPPFLAGS="$CPPFLAGS `curl-config --cflags`"],
dnl 	[AC_MSG_ERROR([I could not find libcurl])]
dnl 	[`curl-config --libs`])

AC_MSG_CHECKING([for libxml2])
if xml2-config --version > /dev/null 2>&1
then
	version_libxml2=`xml2-config --version`
	version_M=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	version_m_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	if test $version_M -lt 2 || test $version_m -lt 5 || test $version_m_m -lt 7
	then
		AC_MSG_ERROR([must have libxml2 2.5.7 or greater, found $version_libxml2])
	fi

	LIBS="$LIBS `xml2-config --libs`"
	CPPFLAGS="$CPPFLAGS `xml2-config --cflags`"
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([I could not find libxml2])
fi

dnl AC_CHECK_LIB([xml2], [xmlInitGlobals],
dnl 	[LIBS="$LIBS `xml2-config --libs`"
dnl 	 CPPFLAGS="$CPPFLAGS `xml2-config --cflags`"],
dnl 	[AC_MSG_ERROR([I could not find libxml2])]
dnl 	[`xml2-config --libs`])

AC_CHECK_LIB([pthread], [pthread_kill], 
	[LIBS="$LIBS -lpthread"],
	[AC_MSG_ERROR([I could not find pthreads])])

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

dnl Temporary hack; find a home for the deflate program; set it to
dnl prefix/etc/deflate. See util.cc:dods_root()
if test "$prefix" = "NONE"
then
AC_DEFINE([LIBDAP_ROOT], ["/usr/local"], [Set to the prefix directory])
else
AC_DEFINE_UNQUOTED([LIBDAP_ROOT], ["$prefix"], [Set to the prefix directory])
fi

AC_CONFIG_FILES([dap-config
		 Makefile
                 tests/Makefile] , [chmod +x dap-config])
AC_OUTPUT
