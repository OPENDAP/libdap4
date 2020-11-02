# Support for coverage analysis via gcov:
# from http://fragglet.livejournal.com/14291.html
 
AC_DEFUN([DODS_GCOV_VALGRIND],
[
               
AC_ARG_ENABLE(coverage,
    [AS_HELP_STRING([--enable-coverage],
                    [Collect coverage data (default is no)]) ],
    [coverage=$enableval],
    [coverage=no])

AM_CONDITIONAL(ENABLE_COVERAGE, false)

AS_IF([test x$coverage = xyes], 
    [ AS_IF([test x$GCC = xyes],
            [ AC_CHECK_LIB([gcov], [gcov_open], [LIBS="$LIBS -lgcov"], [], [])
	          AC_MSG_NOTICE([Building coverage reporting.])
              AM_CONDITIONAL(ENABLE_COVERAGE, true)],
            [ AC_MSG_ERROR([Can only enable coverage when using gcc.]) ]) ],
    )
])
