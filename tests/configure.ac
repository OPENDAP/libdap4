dnl -*- autoconf -*-
dnl Minimal configure.ac used only for generating the autotest support files
dnl needed by the CMake integration-test path.

AC_PREREQ([2.69])

AC_INIT([libdap],[3.21.1],[opendap-tech@opendap.org])
AC_CONFIG_SRCDIR([DASTest.at])
AC_CONFIG_TESTDIR([.], [.])

AC_PROG_CC
AC_C_BIGENDIAN

dnl Match the word-order substitution used by the full autotools build so the
dnl DMR autotest suites pick the correct baseline directory.
AS_IF([test "$ac_cv_c_bigendian" = yes],
      [ac_word_order=big-endian],
      [ac_word_order=little-endian])
AC_SUBST([ac_word_order])

AC_CONFIG_FILES([atlocal package.m4])

AC_OUTPUT
