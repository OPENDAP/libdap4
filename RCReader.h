#ifndef _RCReader_h_
#define _RCReader_h_

#include <iostream>
#include <string>



class RCReader
{

    string lockstr;	        //  Lock file path
    string cifp;
    string cache_root;	//  Location of actual cache.
    string homedir;	        //  Cache init file path
    string tmpdir;		//  Fallback position for cache files.

    bool  _has_cache_file;
    bool  _can_create_cache_file;

    bool _dods_use_cache;      // 0- Disabled 1- Enabled
    int _dods_cache_max;	    // Max cache size in Mbytes
    int _dods_cached_obj;     // Max cache entry size in Mbytes
    int _dods_ign_expires;    // 0- Honor expires 1- Ignore them
  
    // NB: NEVER_DEFLATE: I added this (12/1/99 jhrg) because libwww 5.2.9 cannot
    // process compressed (i.e., deflated) documents in the cache. Users must be
    // able to choose whether they want compressed data that will always be
    // refreshed or uncompressed data that will be cached. When set this flag
    // overrides the value passed into the Connect object's constructor. This
    // gives users control over the value. Previously, this could only be set by
    // the program that called Connect(...). 
    // Note that I've now (4/6/2000 jhrg) fixed libwww so this parameter is no
    // longer needed.111
    bool _dods_never_deflate;   // 0- allow deflate, 1- disallow
  
    int _dods_default_expires; // 24 hours in seconds
    int _dods_always_validate; // Let libwww decide by default so set to 0
  
    //flags for PROXY_SERVER=<protocol>,<host url>
    string _dods_proxy_server_protocol;
    string _dods_proxy_server_host_url;

    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    string _dods_proxy_for_regexp;
    string _dods_proxy_for_proxy_host_url;
    int _dods_proxy_for_regexp_flags;

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    string _dods_no_proxy_for_protocol;
    string _dods_no_proxy_for_proxy_host;
    int _dods_no_proxy_for_port;

    static RCReader* _instance;

    RCReader();
    ~RCReader();
    static void clean();
  
    // File I/O methods
    void  save_cache_status();
    void load_cache_status();

public:
    static RCReader* instance();
  
    bool has_cache_file() {return  _has_cache_file;}
  
    // GET METHODS
    const string get_dods_cache_root() {return cache_root;}
    const bool get_use_cache() throw()      {return _dods_use_cache;}
    const int get_max_cache_size()  throw()  {return _dods_cache_max;}
    const int get_max_cached_obj() throw()   {return _dods_cached_obj;}
    const int get_ignore_expires() throw()  {return _dods_ign_expires;}
    const bool get_never_deflate() throw()   {return _dods_never_deflate;}
    const int get_default_expires() throw() {return _dods_default_expires;}
    const int get_always_validate() throw() {return _dods_always_validate;}
    const string get_proxy_server_protocol() throw() {return _dods_proxy_server_protocol;}
    const string get_proxy_server_host_url()  throw() {return _dods_proxy_server_host_url;}
    const string get_proxy_for_regexp() throw() {return _dods_proxy_for_regexp;}
    const string get_proxy_for_proxy_host_url() throw() {return _dods_proxy_for_proxy_host_url;}
    const int    get_proxy_for_regexp_flags() throw() {return _dods_proxy_for_regexp_flags;}
    const string get_no_proxy_for_protocol() throw() {return _dods_no_proxy_for_protocol;}
    const string get_no_proxy_for_host() throw() {return _dods_no_proxy_for_proxy_host;}
    const int    get_no_proxy_for_port() throw() {return _dods_no_proxy_for_port;}
  
    // SET METHODS
    void set_use_cache(bool b) throw() {_dods_use_cache=b;}
    void set_max_cache_size(int i) throw() {_dods_cache_max=i;}
    void set_max_cached_obj(int i) throw() {_dods_cached_obj=i;}
    void set_ignore_expires(int i) throw() {_dods_ign_expires=i;}
    void set_never_deflate(bool b) throw() {_dods_never_deflate=b;}
    void set_default_expires(int i) throw() { _dods_default_expires=i;}
    void set_always_validate(int i) throw() {_dods_always_validate=i;}
    void set_proxy_server_protocol(const string &s) throw() {_dods_proxy_server_protocol=s;}
    void set_proxy_server_host_url(const string &s) throw() {_dods_proxy_server_host_url=s;}
    void set_proxy_for_regexp(const string &s) throw() { _dods_proxy_for_regexp=s;}
    void set_proxy_for_proxy_host_url(const string &s) throw() {_dods_proxy_for_proxy_host_url=s;}
    void set_proxy_for_regexp_flags(int i) throw() {_dods_proxy_for_regexp_flags=i;}
    void set_no_proxy_for_protocol(const string &s) throw() {_dods_no_proxy_for_protocol=s;}
    void set_no_proxy_for_host(const string &s) throw() {_dods_no_proxy_for_proxy_host=s;}
    void set_no_proxy_for_port(int i) throw() {_dods_no_proxy_for_port=i;}

};

#endif // _RCReader_h_
