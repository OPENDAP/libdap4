prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libdap
Description: OPeNDAP Data Access Protocol library
Version: @LIBDAP_VERSION@
Requires:
Libs: -L${libdir} -ldap @LIBDAP_PKG_LIBS@
Cflags: -I${includedir} -I${includedir}/libdap @LIBDAP_PKG_CFLAGS@
