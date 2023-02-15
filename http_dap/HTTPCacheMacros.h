/*
 * HTTPCacheMacros.h
 *
 *  Created on: Dec 28, 2011
 *      Author: jimg
 */

#ifndef HTTPCACHEMACROS_H_
#define HTTPCACHEMACROS_H_

#include <cerrno>
#include <cstring>
#include "InternalErr.h"

#define MKDIR(a,b) mkdir((a), (b))
#define UMASK(a) umask((a))

#define REMOVE_BOOL(a) remove((a))
#define REMOVE(a) ((void)remove((a)))

#define MKSTEMP(a) mkstemp((a))
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_STR "/"

#define LOCK(m) do { \
	int code = pthread_mutex_lock((m)); \
	if (code != 0) \
		throw InternalErr(__FILE__, __LINE__, string("Mutex lock: ") + strerror(code)); \
    } while(0);

#define UNLOCK(m) do { \
	int code = pthread_mutex_unlock((m)); \
	if (code != 0) \
		throw InternalErr(__FILE__, __LINE__, string("Mutex unlock: ") + strerror(code)); \
    } while(0);

#define TRYLOCK(m) pthread_mutex_trylock((m))
#define INIT(m) pthread_mutex_init((m), 0)
#define DESTROY(m) pthread_mutex_destroy((m))

#define CACHE_LOCATION "/tmp/"
#define CACHE_ROOT "dods-cache/"

#define CACHE_INDEX ".index"
#define CACHE_LOCK ".lock"
#define CACHE_META ".meta"
#define CACHE_EMPTY_ETAG "@cache@"


#endif /* HTTPCACHEMACROS_H_ */
