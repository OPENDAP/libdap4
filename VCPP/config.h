#ifndef _DODS_CONFIG_H_
#define _DODS_CONFIG_H_

#define HAVE_ALLOCA 1
#define HAVE_ALLOCA_H 1
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

#define sleep(n) Sleep(n * 1000)

/*  Replaces dods-deployed stat.h defines only found on unixes  */
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)

#define PACKAGE_VERSION "$VERSION"
#define PACKAGE_NAME "DODS"
#define DAP_PROTOCOL_VERSION "3.0"
#ifndef LIBDAP_ROOT
#define LIBDAP_ROOT "$DODS_ROOT/libdap"
#endif

#undef LIBXML_ICONV_ENABLED

/* For isatty() under MS VC++ */
#include <io.h>
#define isatty(n) _isatty(n)

#define DODS_BISON_VER 128

#define not_used 

#endif


