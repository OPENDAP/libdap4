# Configure macro for Libdap
# Patrice Dumas 2005 based on freetype2.m4 from Marcelo Magallon 2001-10-26, 
# based on gtk.m4 by Owen Taylor

# AC_CHECK_LIBDAP([MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
# Test for Libdap and define DAP_CFLAGS and DAP_LIBS.
# Check that the version is above MINIMUM-VERSION 

AC_DEFUN([AC_CHECK_LIBDAP],
[
  AC_PATH_PROG([DAP_CONFIG], [dap-config], [no])
  dap_min_version=m4_if([$1], [], [3.5.0], [$1])
  AC_MSG_CHECKING([for libdap version >= $dap_min_version])
  dap_no=""
  if test "$DAP_CONFIG" = "no" ; then
     dap_no=yes
  else
     dap_config_major_version=`$DAP_CONFIG --version | sed 's/^libdap \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\1/'`
     dap_config_minor_version=`$DAP_CONFIG --version | sed 's/^libdap \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\2/'`
     dap_config_micro_version=`$DAP_CONFIG --version | sed 's/^libdap \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$/\2/'`
     dap_min_major_version=`echo $dap_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
     dap_min_minor_version=`echo $dap_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
     dap_min_micro_version=`echo $dap_min_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

     dap_config_is_lt=""
     if test $dap_config_major_version -lt $dap_min_major_version ; then
       dap_config_is_lt=yes
     else
       if test $dap_config_major_version -eq $dap_min_major_version ; then
         if test $dap_config_minor_version -lt $dap_min_minor_version ; then
           dap_config_is_lt=yes
         else
           if test $dap_config_minor_version -eq $dap_min_minor_version ; then
             if test $dap_config_micro_version -lt $dap_min_micro_version ; then
               dap_config_is_lt=yes
             fi
           fi
         fi
       fi
     fi
     if test x$dap_config_is_lt = xyes ; then
       dap_no=yes
     else
       DAP_LIBS="`$DAP_CONFIG --libs` -lstdc++"
       DAP_CFLAGS="`$DAP_CONFIG --cflags`"
     fi
   fi
   if test x$dap_no = x ; then
     AC_MSG_RESULT([yes])
     m4_if([$2], [], [:], [$2])
   else
     AC_MSG_RESULT([no])
     if test "$DAP_CONFIG" = "no" ; then
     AC_MSG_WARN([The dap-config script could not be found.])
     else
       if test x$dap_config_is_lt = xyes ; then
         AC_MSG_WARN([the installed libdap library is too old.])
       fi
     fi
     DAP_LIBS=""
     DAP_CFLAGS=""
     m4_if([$3], [], [:], [$3])
     
   fi
   AC_SUBST([DAP_CFLAGS])
   AC_SUBST([DAP_LIBS])
]) 
