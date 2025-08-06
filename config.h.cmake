//
// Created by James Gallagher on 6/8/25.
//

#ifndef CONFIG_H
#define CONFIG_H

/* Libraries */
#cmakedefine HAVE_LIBXML2
#cmakedefine HAVE_CURL
#cmakedefine HAVE_TIRPC

/* Functions */
#cmakedefine HAVE_PTHREADS
#cmakedefine HAVE_STRFTIME
#cmakedefine HAVE_TIMEGM
#cmakedefine HAVE_MKTIME
#cmakedefine HAVE_ATEXIT 1

/* Headers */
#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_TM_IN_SYS_TIME
#cmakedefine HAVE_PTHREAD_H
#cmakedefine HAVE_UUID_UUID_H
#cmakedefine HAVE_UUID_H
#cmakedefine HAVE_SYS_STAT_H
#cmakedefine HAVE_SYS_TYPES_H_AND_SYS_STAT_H
#cmakedefine HAVE_STRING_H
#cmakedefine HAVE_STDLIB_H
#cmakedefine HAVE_REGEX_H 1

/* Build options */
#cmakedefine ENABLE_ASAN
#cmakedefine BUILD_DEVELOPER
#cmakedefine USE_CPP_11_REGEX 1

#ifdef HAVE_PTHREAD_H
#define USE_POSIX_THREADS 1
#endif

/* Platform-specific */
#cmakedefine SOLARIS
#cmakedefine _REENTRANT

/* Version information */
#define LIBDAP_VERSION "@LIBDAP_VERSION@"
#define PACKAGE_NAME "@PROJECT_NAME@"
#define PACKAGE_VERSION "@PROJECT_VERSION@"

#define DAP_PROTOCOL_VERSION "4.0"

/* What sort of HTTP client is this? */
#define CNAME "@PROJECT_NAME@"
/* Client version number */
#define CVER "@LIBDAP_VERSION@"
/* Client name and version combined */
#define DVR "@PROJECT_NAME@/LIBDAP_VERSION@"

/* Install prefix for libdap */
#define LIBDAP_ROOT "@LIBDAP_ROOT@"

#endif //CONFIG_H_IN_H
