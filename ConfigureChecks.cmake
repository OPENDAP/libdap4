### make the config.h header

include(CheckIncludeFiles)
include(CheckSymbolExists)

# System headers
check_include_files("unistd.h" HAVE_UNISTD_H)
# Check if <sys/time.h> defines struct tm
# (This is a proxy for TM_IN_SYS_TIME)
check_include_files("sys/time.h" HAVE_TM_IN_SYS_TIME)
check_include_files("pthread.h" HAVE_PTHREAD_H)
check_include_files("uuid/uuid.h" HAVE_UUID_UUID_H)
check_include_files("uuid.h" HAVE_UUID_H)
check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
check_include_files("sys/types.h;sys/stat.h" HAVE_SYS_TYPES_H_AND_SYS_STAT_H)
check_include_files("string.h" HAVE_STRING_H)
check_include_files("stdlib.h" HAVE_STDLIB_H)
check_include_files("regex.h" HAVE_REGEX_H)

# Library headers
check_include_files("libxml/xmlwriter.h" HAVE_LIBXML2)
check_include_files("curl/curl.h" HAVE_CURL)
check_include_files("rpc/xdr.h" HAVE_TIRPC)

# Functions
check_symbol_exists(pthread_create pthread.h HAVE_PTHREADS)
check_symbol_exists(strftime "time.h" HAVE_STRFTIME)
check_symbol_exists(timegm "time.h" HAVE_TIMEGM)
check_symbol_exists(mktime "time.h" HAVE_MKTIME)
check_symbol_exists(atexit "stdlib.h" HAVE_ATEXIT)
