##### http://autoconf-archive.cryp.to/check_zlib.html
#
# SYNOPSIS
#
#   CHECK_ZLIB()
#
# DESCRIPTION
#
#   This macro searches for an installed zlib library. If nothing was
#   specified when calling configure, it searches first in /usr/local
#   and then in /usr. If the --with-zlib=DIR is specified, it will try
#   to find it in DIR/include/zlib.h and DIR/lib/libz.a. If
#   --without-zlib is specified, the library is not searched at all.
#
#   If either the header file (zlib.h) or the library (libz) is not
#   found, the configuration exits on error, asking for a valid zlib
#   installation directory or --without-zlib.
#
#   The macro defines the symbol HAVE_LIBZ if the library is found. You
#   should use autoheader to include a definition for this symbol in a
#   config.h file. Sample usage in a C/C++ source is as follows:
#
#     #ifdef HAVE_LIBZ
#     #include <zlib.h>
#     #endif /* HAVE_LIBZ */
#
# LAST MODIFICATION
#
#   2004-09-20
#
# COPYLEFT
#
#   Copyright (c) 2004 Loic Dachary <loic@senga.org>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
#   02111-1307, USA.
#
#   As a special exception, the respective Autoconf Macro's copyright
#   owner gives unlimited permission to copy, distribute and modify the
#   configure scripts that are the output of Autoconf when processing
#   the Macro. You need not follow the terms of the GNU General Public
#   License when using or distributing such scripts, even though
#   portions of the text of the Macro appear in them. The GNU General
#   Public License (GPL) does govern all other use of the material that
#   constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the
#   Autoconf Macro released by the Autoconf Macro Archive. When you
#   make and distribute a modified version of the Autoconf Macro, you
#   may extend this special exception to the GPL to apply to your
#   modified version as well.

AC_DEFUN([DAP_CHECK_ZLIB],
#
# Handle user hints
#
[
AC_ARG_WITH(zlib,
[  --with-zlib=DIR root directory path of zlib installation [defaults to
                    /usr/local or /usr if not found in /usr/local]
  --without-zlib to disable zlib usage completely],
[if test "$withval" != no ; then
  if test -d "$withval"
  then
    ZLIB_HOME="$withval"
  else
    AC_MSG_WARN([Sorry, $withval does not exist, checking usual places])
  fi
else
  AC_MSG_WARN([zlib is required for deflate, not disabled])
fi])

ZLIB_OLD_LDFLAGS=$LDFLAGS
ZLIB_OLD_CPPFLAGS=$CPPFLAGS

#
# Locate zlib, if wanted
#
if test -n "${ZLIB_HOME}"
then
        LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
        CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
fi
AC_LANG_SAVE
AC_LANG_C
AC_CHECK_LIB(z, inflateEnd, [zlib_cv_libz=yes], [zlib_cv_libz=no])
AC_CHECK_HEADER(zlib.h, [zlib_cv_zlib_h=yes], [zlib_cv_zlib_h=no])
AC_LANG_RESTORE

if test "$zlib_cv_libz" = "yes" -a "$zlib_cv_zlib_h" = "yes"
then
        #
        # If both library and header were found, use them
        #
        ZLIB_LIBS="-lz"
else
        #
        # If either header or library was not found, revert and bomb
        #
        AC_MSG_ERROR([zlib is required for deflate, specify a valid zlib installation with --with-zlib=DIR])
fi

LDFLAGS="$ZLIB_OLD_LDFLAGS"
CPPFLAGS="$ZLIB_OLD_CPPFLAGS"

AC_SUBST([ZLIB_LIBS])
AC_SUBST([ZLIB_LDFLAGS])
AC_SUBST([ZLIB_CFLAGS])

])
