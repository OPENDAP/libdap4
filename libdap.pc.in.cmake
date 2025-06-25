prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include/libdap

Name: libdap
Description: OPeNDAP Data Access Protocol library
Version: @LIBDAP_VERSION@
Requires:
Libs: -L${libdir} -ldap @LIBDAP_PKG_LIBS@
Cflags: -I${includedir} @LIBDAP_PKG_CFLAGS@
