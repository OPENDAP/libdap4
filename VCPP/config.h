/*  config.h.  Generated by hand for MS VC++ 7.x.  ROM - 11/2006  */
#ifndef _config_h
#define _config_h

#define CNAME "libdap"
#define CVER "3.8.2"
#define DAP_PROTOCOL_VERSION "3.1"
#define DVR "libdap/3.8.2"
#define PACKAGE "libdap"
#define PACKAGE_NAME "libdap"
#define PACKAGE_STRING "libdap 3.8.2"
#define PACKAGE_VERSION "3.8.2"

#define HAVE_ALLOCA 1
#define HAVE_MKTIME 1
#define STDC_HEADERS 1
#define CNAME "DODS"
#define CVER "$VERSION"

#define SIZEOF_CHAR 1
#define SIZEOF_DOUBLE 8
#define SIZEOF_FLOAT 4
#define SIZEOF_INT 4
#define SIZEOF_LONG 4

#define HAVE_STRDUP 1

#define HAVE_FCNTL_H 1
#define HAVE_LIMITS_H 1

/*  Replaces dods-deployed stat.h defines only found on unixes  */
/*  Need of this reconfirmed 11/2006 - ROM.                     */
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)

#define XDR_FLOAT32 xdr_float
#define XDR_FLOAT64 xdr_double
#define XDR_INT16 xdr_short
#define XDR_INT32 xdr_long
#define XDR_UINT16 xdr_u_short
#define XDR_UINT32 xdr_u_long

#ifndef LIBDAP_ROOT
#define LIBDAP_ROOT "C:/opendap"
#endif

#undef LIBXML_ICONV_ENABLED

#define not_used

#endif


