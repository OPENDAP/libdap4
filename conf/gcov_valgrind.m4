# Support for coverage analysis via gcov:
# from http://fragglet.livejournal.com/14291.html
 
AC_DEFUN([DODS_GCOV_VALGRIND],
[
               
AC_ARG_ENABLE(coverage,
    [AS_HELP_STRING([--enable-coverage],
                    [Collect coverage data (default is no)]) ],
    [coverage=$enableval],
    [coverage=no])
 
AS_IF([test x$coverage = xyes], 
    [ AS_IF([test x$GCC = xyes],
            [ CFLAGS="-fprofile-arcs -ftest-coverage -pg $CFLAGS"
              CXXFLAGS="-fprofile-arcs -ftest-coverage -pg $CXXFLAGS"
              LDFLAGS="-pg $LDFLAGS"
              AC_CHECK_LIB([gcov], [gcov_open],[LIBS="$LIBS -lgcov"],[],[])
	          AC_MSG_NOTICE([Building coverage reporting.]) ],
            [ AC_MSG_ERROR([Can only enable coverage when using gcc.]) ]) ])
               
# Support for running test cases using valgrind:
               
use_valgrind=no
AC_ARG_ENABLE(valgrind,
    [AS_HELP_STRING([--enable-valgrind], 
    	            [Use valgrind when running unit tests. (default is no)])],
    [use_valgrind=$enableval],
    [use_valgrind=no])
               
AS_IF([test x$use_valgrind = xyes ],
      [ AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
        AS_IF([test x$HAVE_VALGRIND = xyes ],
	          [AC_MSG_NOTICE([Using valgrind with unit tests.])],
	          [AC_MSG_ERROR([Valgrind not found in PATH.])])])

AM_CONDITIONAL(USE_VALGRIND, [test x$use_valgrind = xyes])

])