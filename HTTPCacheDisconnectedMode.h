
// -*- mode: c++; c-basic-offset:4 -*-

#ifndef _http_cache_disconnected_mode_h
#define _http_cache_disconnected_mode_h

/** Disconnected Operation

    The cache can be set to handle disconnected operation where it does not
    use the network to validate entries and does not attempt to load new
    documents. All requests that can not be fulfilled by the cache will be
    returned with a "504 Gateway Timeout" response. There are two operational
    disconnected modes:
  
    No network activity at all: Here it uses its own persistent cache to
    answer the request, if possible.
  
    Forward all disconnected requests to a proxy cache: Here it uses the
    HTTP/1.1 cache-control header to indicate that the proxy should operate in
    disconnected mode. */

typedef enum {
    DISCONNECT_NONE     = 0,
    DISCONNECT_NORMAL   = 1,
    DISCONNECT_EXTERNAL = 2
} CacheDisconnectedMode;

// $Log: HTTPCacheDisconnectedMode.h,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.3  2002/10/08 05:29:12  jimg
// Minor change: removed unnecessary enum name.
//
// Revision 1.1.2.2  2002/09/17 22:10:02  jimg
// Preprocessor guard has leading underscore like all the others in our code.
//
// Revision 1.1.2.1  2002/09/17 22:05:22  jimg
// Created using code from W3C's libwww distribution, (c) COPYRIGHT MIT 1995.
// See the file COPYRIGHT.libwww
//

#endif // _http_cache_disconnected_mode_h
