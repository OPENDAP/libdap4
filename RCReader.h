
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Jose Garcia <jgarcia@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 2001-2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//         jose		Jose Garcia <jgarcia@ucar.edu>

#ifndef _rc_reader_h_
#define _rc_reader_h_

#include <iostream>
#include <string>

#include "Error.h"
#include "util.h"

using namespace std;

/** Read the .dodsrc file. By default the file ~/.dodsrc is read. If the
    environment variable DODS_CONF is set, use that value as the pathname to
    the configuration file. Else, if the environment variable DODS_CACHE_INIT
    is set, use that value. 

    NB: DODS_CACHE_INIT is deprecated and may be removed in the future. 

    @author Jose Garcia <jgarcia@ucar.edu> */
class RCReader {
private:
    string d_rc_file_path;
    string d_cache_root;

    bool _dods_use_cache;	// 0- Disabled 1- Enabled
    unsigned long _dods_cache_max; // Max cache size in Mbytes
    unsigned long _dods_cached_obj; // Max cache entry size in Mbytes
    int _dods_ign_expires;	// 0- Honor expires 1- Ignore them
  
    // NB: NEVER_DEFLATE: I added this (12/1/99 jhrg) because libwww 5.2.9
    // cannot process compressed (i.e., deflated) documents in the cache.
    // Users must be able to choose whether they want compressed data that
    // will always be refreshed or uncompressed data that will be cached.
    // When set this flag overrides the value passed into the Connect
    // object's constructor. This gives users control over the value.
    // Previously, this could only be set by the program that called
    // Connect(...). Note that I've now (4/6/2000 jhrg) fixed libwww so this
    // parameter is no longer needed.111
    //
    // Added back in, but with a better name (removed double negative).
    // 6/27/2002 jhrg
    bool _dods_deflate;		// 1- request comp responses, 0- don't
  
    int _dods_default_expires;	// 24 hours in seconds
    int _dods_always_validate;	// Let libwww decide by default so set to 0
  
    // flags for PROXY_SERVER=<protocol>,<host url>
    string d_dods_proxy_server_protocol;
    string d_dods_proxy_server_host;
    long d_dods_proxy_server_port;
    string d_dods_proxy_server_userpw;

    string _dods_proxy_server_host_url;	// deprecated

    // The proxy-for stuff is all deprecated. 06/17/04 jhrg
    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    bool _dods_proxy_for;	// true if proxy_for is used.
    string _dods_proxy_for_regexp;
    string _dods_proxy_for_proxy_host_url;
    int _dods_proxy_for_regexp_flags; // not used w/libcurl. 6/27/2002 jhrg

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    bool d_dods_no_proxy_for;	// true if no_proxy_for is used.
    string d_dods_no_proxy_for_protocol;
    string d_dods_no_proxy_for_host;
    int _dods_no_proxy_for_port; // not used w/libcurl. 6/27/2002 jhrg

    // Make this a vector of strings or support a PATH-style list. 02/26/03
    // jhrg 
    string d_ais_database;
    
    static RCReader* _instance;

    RCReader() throw(Error);
    ~RCReader();
  
    // File I/O methods
    bool write_rc_file(const string &pathname);
    bool read_rc_file(const string &pathname) throw(Error);

    // Look for the RC file
    string check_env_var(const string &variable_name);
    string check_string(string env_var);

    static void initialize_instance() throw(Error);
    static void delete_instance();

    friend class RCReaderTest;
    friend class HTTPConnectTest;

public:
    static RCReader* instance() throw(Error);
  
    // GET METHODS
    const string get_dods_cache_root() {return d_cache_root;}
    const bool get_use_cache() throw()      {return _dods_use_cache;}
    const int get_max_cache_size()  throw()  {return _dods_cache_max;}
    const unsigned long get_max_cached_obj() throw()   {return _dods_cached_obj;}
    const int get_ignore_expires() throw()  {return _dods_ign_expires;}
    const int get_default_expires() throw() {return _dods_default_expires;}
    const int get_always_validate() throw() {return _dods_always_validate;}

    const bool get_deflate() throw()   {return _dods_deflate;}

    /// Get the proxy server protocol
    const string get_proxy_server_protocol() throw() {return d_dods_proxy_server_protocol;}
    /// Get the proxy host
    const string get_proxy_server_host() throw() {return d_dods_proxy_server_host;}
    /// Get the proxy port
    const long get_proxy_server_port() throw() {return d_dods_proxy_server_port;}
    /// Get the proxy username and password
    const string get_proxy_server_userpw() throw() {return d_dods_proxy_server_userpw;}
    /// @deprecated
    const string get_proxy_server_host_url()  throw() {
	return (d_dods_proxy_server_userpw.empty() ? "" : d_dods_proxy_server_userpw + "@") 
	    + d_dods_proxy_server_host 
	    + ":" + long_to_string(d_dods_proxy_server_port);
    }

    // The whole regex/proxy-for implementation needs reworking. We really
    // need a vector of structs which hold the information on a set of regexs
    // and matching proxies. Then in the code that derefs a URL, we should
    // check to see if the URL matches any of the regexs, et cetera. I'm
    // going to disable the entire feature and see if anyone complains. If
    // they do, we can fix it. If not, one less thing to do... 06/17/04 jhrg
    /// @deprecated
    bool is_proxy_for_used() throw() {return _dods_proxy_for;}
    /// @deprecated
    const string get_proxy_for_regexp() throw() {return _dods_proxy_for_regexp;}
    /// @deprecated
    const string get_proxy_for_proxy_host_url() throw() {return _dods_proxy_for_proxy_host_url;}

    /// @deprecated
    const int get_proxy_for_regexp_flags() throw() {return _dods_proxy_for_regexp_flags;}

    // The whole no_proxy implementation also needs a rewrite. However, it is
    // useful as it is since the user can give a domain and there's often a
    // real need for suppressing proxy access for the local domain. The
    // ..._port() method is bogus, however, so it is deprecated. There's no
    // code that uses it. 06/17/04 jhrg
    bool is_no_proxy_for_used() throw() {return d_dods_no_proxy_for;}
    const string get_no_proxy_for_protocol() throw() {return d_dods_no_proxy_for_protocol;}
    const string get_no_proxy_for_host() throw() {return d_dods_no_proxy_for_host;}

    /// @deprecated
    const int    get_no_proxy_for_port() throw() {return _dods_no_proxy_for_port;}

    string get_ais_database() const throw() {return d_ais_database;}

    // SET METHODS
    void set_use_cache(bool b) throw() {_dods_use_cache=b;}
    void set_max_cache_size(int i) throw() {_dods_cache_max=i;}
    void set_max_cached_obj(int i) throw() {_dods_cached_obj=i;}
    void set_ignore_expires(int i) throw() {_dods_ign_expires=i;}
    void set_default_expires(int i) throw() { _dods_default_expires=i;}
    void set_always_validate(int i) throw() {_dods_always_validate=i;}

    void set_deflate(bool b) throw() {_dods_deflate=b;}

    void set_proxy_server_protocol(const string &s) throw() {d_dods_proxy_server_protocol=s;}
    void set_proxy_server_host(const string &s) throw() {d_dods_proxy_server_host=s;}
    void set_proxy_server_port(long l) throw() {d_dods_proxy_server_port=l;}
    void set_proxy_server_userpw(const string &s) throw() {d_dods_proxy_server_userpw=s;}

    /// @deprecated
    void set_proxy_server_host_url(const string &s) throw() {_dods_proxy_server_host_url=s;}

    /// @deprecated
    void set_proxy_for_regexp(const string &s) throw() { _dods_proxy_for_regexp=s;}
    /// @deprecated
    void set_proxy_for_proxy_host_url(const string &s) throw() {_dods_proxy_for_proxy_host_url=s;}
    /// @deprecated
    void set_proxy_for_regexp_flags(int i) throw() {_dods_proxy_for_regexp_flags=i;}

    void set_no_proxy_for_protocol(const string &s) throw() {d_dods_no_proxy_for_protocol=s;}
    void set_no_proxy_for_host(const string &s) throw() {d_dods_no_proxy_for_host=s;}

    /// @deprecated
    void set_no_proxy_for_port(int i) throw() {_dods_no_proxy_for_port=i;}

    void set_ais_database(const string &db) throw() {d_ais_database = db;}
};

// $Log: RCReader.h,v $
// Revision 1.11  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.8.2.4  2004/06/21 20:51:43  jimg
// Added new fields for the new proxy methods and made some fields deprecated.
//
// Revision 1.10  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.8.2.3  2004/02/11 17:10:48  jimg
// I made initialize_instance() and delete_instance() static void methods so
// they can be used in pthread_once() and atexit().
//
// Revision 1.8.2.2  2004/01/22 20:47:24  jimg
// Fix for bug 689. I added tests to make sure the cache size doesn't wind
// up being set to a negative number. I also changed the types of the cache
// size and entry size from int to unsigned long. Added information to
// the default .dodsrc file explaining the units of the CACHE_SIZE and
// MAX_ENTRY_SIZE parameters.
//
// Revision 1.9  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.8.2.1  2003/09/19 22:30:14  jimg
// Fixed the second part of bug #655 where DODS_CONF was not used properly.
// There was a mass of confusing (and broken) code in the ctor which I hope I've
// fixed. I added some unit tests (see RCReaderTest). I think it's probably a
// bad idea to have the code that looks for the RC file create it if it's not
// found, but the alternative is messier.
//
// Revision 1.8  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.5.2.2  2003/03/07 22:58:56  jimg
// Added class doc comment.
//
// Revision 1.7  2003/02/27 23:37:16  jimg
// Added get/set_ais_database() methods along with code to parse an
// AIS_DATABASE entry in the configuration file.
//
// Revision 1.6  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.5.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.5  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.4  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.3.4.6  2002/10/18 23:00:22  jimg
// Changed the declaration of clean(). Also cleaned up some detritus...
//
// Revision 1.3.4.5  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.3.4.4  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.3.4.3  2002/07/06 19:46:37  jimg
// Added this log.
// 

#endif // _RCReader_h_
