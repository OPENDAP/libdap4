
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 2001-2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//         jose		Jose Garcia <jgarcia@ucar.edu>

#ifndef _rc_reader_h_
#define _rc_reader_h_

#include <iostream>
#include <string>

using std::string;
using std::ofstream;
using std::ifstream;
using std::endl;

class RCReader
{
private:
    string lockstr;	        // Lock file path
    string cifp;		// Configuration file name.
    string cache_root;		// Location of actual cache.
    string homedir;	        // Cache init file path
    string tmpdir;		// Fallback position for cache files.

    bool  _has_rc_file;
    bool  _can_create_rc_file;

    bool _dods_use_cache;	// 0- Disabled 1- Enabled
    int _dods_cache_max;	// Max cache size in Mbytes
    int _dods_cached_obj;	// Max cache entry size in Mbytes
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
    string _dods_proxy_server_protocol;
    string _dods_proxy_server_host_url;

    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    bool _dods_proxy_for;	// true if proxy_for is used.
    string _dods_proxy_for_regexp;
    string _dods_proxy_for_proxy_host_url;
    int _dods_proxy_for_regexp_flags; // not used w/libcurl. 6/27/2002 jhrg

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    bool _dods_no_proxy_for;	// true if no_proxy_for is used.
    string _dods_no_proxy_for_protocol;
    string _dods_no_proxy_for_proxy_host;
    int _dods_no_proxy_for_port; // not used w/libcurl. 6/27/2002 jhrg

    static RCReader* _instance;

    RCReader();
    ~RCReader();
  
    // File I/O methods
    void write_rc_file();
    void read_rc_file();

    friend void initialize_instance();
    friend void rcreader_clean(); // This must be a function to be passed to
				// atexit (which is no longer used...)
				// 10/08/02 jhrg

public:
    static RCReader* instance();
  
    // GET METHODS
    const string get_dods_cache_root() {return cache_root;}
    const bool get_use_cache() throw()      {return _dods_use_cache;}
    const int get_max_cache_size()  throw()  {return _dods_cache_max;}
    const int get_max_cached_obj() throw()   {return _dods_cached_obj;}
    const int get_ignore_expires() throw()  {return _dods_ign_expires;}
    const int get_default_expires() throw() {return _dods_default_expires;}
    const int get_always_validate() throw() {return _dods_always_validate;}

    const bool get_deflate() throw()   {return _dods_deflate;}

    const string get_proxy_server_protocol() throw() {return _dods_proxy_server_protocol;}
    const string get_proxy_server_host_url()  throw() {return _dods_proxy_server_host_url;}
    bool is_proxy_for_used() throw() {return _dods_proxy_for;}
    const string get_proxy_for_regexp() throw() {return _dods_proxy_for_regexp;}
    const string get_proxy_for_proxy_host_url() throw() {return _dods_proxy_for_proxy_host_url;}
    // @deprecated
    const int get_proxy_for_regexp_flags() throw() {return _dods_proxy_for_regexp_flags;}
    bool is_no_proxy_for_used() throw() {return _dods_no_proxy_for;}
    const string get_no_proxy_for_protocol() throw() {return _dods_no_proxy_for_protocol;}
    const string get_no_proxy_for_host() throw() {return _dods_no_proxy_for_proxy_host;}
    const int    get_no_proxy_for_port() throw() {return _dods_no_proxy_for_port;}
  
    // SET METHODS
    void set_use_cache(bool b) throw() {_dods_use_cache=b;}
    void set_max_cache_size(int i) throw() {_dods_cache_max=i;}
    void set_max_cached_obj(int i) throw() {_dods_cached_obj=i;}
    void set_ignore_expires(int i) throw() {_dods_ign_expires=i;}
    void set_default_expires(int i) throw() { _dods_default_expires=i;}
    void set_always_validate(int i) throw() {_dods_always_validate=i;}

    void set_deflate(bool b) throw() {_dods_deflate=b;}

    void set_proxy_server_protocol(const string &s) throw() {_dods_proxy_server_protocol=s;}
    void set_proxy_server_host_url(const string &s) throw() {_dods_proxy_server_host_url=s;}
    void set_proxy_for_regexp(const string &s) throw() { _dods_proxy_for_regexp=s;}
    void set_proxy_for_proxy_host_url(const string &s) throw() {_dods_proxy_for_proxy_host_url=s;}
    void set_proxy_for_regexp_flags(int i) throw() {_dods_proxy_for_regexp_flags=i;}
    void set_no_proxy_for_protocol(const string &s) throw() {_dods_no_proxy_for_protocol=s;}
    void set_no_proxy_for_host(const string &s) throw() {_dods_no_proxy_for_proxy_host=s;}
    void set_no_proxy_for_port(int i) throw() {_dods_no_proxy_for_port=i;}

};

// $Log: RCReader.h,v $
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
