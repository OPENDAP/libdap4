/*
 * HTTPCacheMacros.h
 *
 *  Created on: Dec 28, 2011
 *      Author: jimg
 */

#ifndef HTTPCACHEMACROS_H_
#define HTTPCACHEMACROS_H_

#ifdef WIN32
#include <direct.h>
#include <time.h>
#include <fcntl.h>
#define MKDIR(a,b) _mkdir((a))
#define UMASK(a) _umask((a))

#if 0
#define REMOVE(a) do { \
        int s = remove((a)); \
        if (s != 0) \
            throw InternalErr(__FILE__, __LINE__, "Cache error; could not remove file: " + long_to_string(s)); \
    } while(0)
#endif

#define REMOVE_BOOL(a) remove((a))
#define REMOVE(a) ((void)remove((a)))
#define MKSTEMP(a) _open(_mktemp((a)),_O_CREAT,_S_IREAD|_S_IWRITE)
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_STR "\\"

#else
#include <cerrno>
#define MKDIR(a,b) mkdir((a), (b))
#define UMASK(a) umask((a))

#if 0
// Replaced this with something that signals errors. jhrg 12/28/2011
// A great idea, but it breaks things in ways that complicate
// testing. Push this change forward to H 1.9. jhrg 12/28/2011
#define REMOVE(a) do { \
        errno = 0; \
        int s = remove((a)); \
        if (s != 0) \
            throw InternalErr(__FILE__, __LINE__, "Cache error; could not remove file: " + long_to_string(errno)); \
    } while(0)
#endif

#define REMOVE_BOOL(a) remove((a))
#define REMOVE(a) ((void)remove((a)))

#define MKSTEMP(a) mkstemp((a))
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"
#endif

#ifdef WIN32
#define CACHE_LOCATION "\\tmp\\"
#define CACHE_ROOT "dods-cache\\"
#else
#define CACHE_LOCATION "/tmp/"
#define CACHE_ROOT "dods-cache/"
#endif

#define CACHE_INDEX ".index"
#define CACHE_LOCK ".lock"
#define CACHE_META ".meta"
#define CACHE_EMPTY_ETAG "@cache@"


#endif /* HTTPCACHEMACROS_H_ */
