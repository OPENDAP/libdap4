#! /bin/sh
#
# Borrowed the idea for this script (and some code) from libcurl.
#

prefix="@CMAKE_INSTALL_PREFIX@"
exec_prefix="${prefix}"
includedir="${prefix}/include"
libdir="${prefix}/lib"
libdir64=${prefix}/lib64

cflags="-I${includedir} -I${includedir}/libdap @CURL_PKG_CFLAGS@ @XML2_PKG_CFLAGS@ @TIRPC_PKG_CFLAGS@"
libs="-L${libdir} -L${libdir64} -ldap -ldapclient -ldapserver @CURL_PKG_LIBS@ @XML2_PKG_LIBS@ @TIRPC_PKG_LIBS@"

# may need to add  @PTHREAD_LIBRARIES@ @UUID_LIB@ 6/25/25 jhrg

usage()
{
    cat <<EOF
Usage: dap-config [OPTION]

Available values for OPTION include:

  --help      	display this help message and exit
  --cc        	C compiler
  --cxx       	C++ compiler
  --cflags    	pre-processor and compiler flags
  --libs      	library linking information for libdap (both clients and servers)
  --server-libs libraries for servers
  --client-libs libraries for clients
  --prefix    	OPeNDAP install prefix
  --version   	Library version
EOF

    exit "$1"
}

if test $# -eq 0; then
    usage 1
fi

while test $# -gt 0; do
    case "$1" in
    # this deals with options in the style
    # --option=value and extracts the value part
    # [not currently used]
    -*=*) value=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
    *) value= ;;
    esac

    case "$1" in
    --help)
	usage 0
	;;

    --cc)
	echo "@CC@"
	;;

    --cxx)
	echo "@CXX@"
	;;

    # Added -I${includedir} so that code can use #include <libdap/Array.h>, ...
    # which avoids issues with IDE warnings and will help later when there are
    # two libdap libraries. jhrg 6/17/21
  --cflags)
    echo "${cflags}"
    ;;

  --libs)
    echo "${libs}"
    ;;

#
#   Changed CURL_STATIC_LIBS to CURL_LIBS because the former was including a
#   a boatload of crypto stuff that might or might not be present on a server.
#   Various handlers use this script to determine which libraries to link with.
#   jhrg 2/7/12

    --server-libs)
       	echo "${libs}"
       	;;

    --client-libs)
       	echo "${libs}"
       	;;

    --prefix)
       	echo "${prefix}"
       	;;

    --version)
		echo "libdap @LIBDAP_VERSION@"
		;;

    *)
        echo "unknown option: $1"
		usage
		exit 1
		;;
    esac
    shift
done

exit 0
