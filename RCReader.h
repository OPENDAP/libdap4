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
  void  saveCacheStatus();
  void loadCacheStatus();

public:
  static RCReader* instance();
  
  bool hasCacheFile() {return  _has_cache_file;}
  
  // GET METHODS
  const bool getUseCache() throw()      {return _dods_use_cache;}
  const int getMaxCacheSize()  throw()  {return _dods_cache_max;}
  const int getMaxCachedObj() throw()   {return _dods_cached_obj;}
  const int getIgnoreExpires() throw()  {return _dods_ign_expires;}
  const bool getNeverDeflate() throw()   {return _dods_never_deflate;}
  const int getDefaultExpires() throw() {return _dods_default_expires;}
  const int getAlwaysValidate() throw() {return _dods_always_validate;}
  const string getProxyServerProtocol() throw() {return _dods_proxy_server_protocol;}
  const string getProxyServerHostUrl()  throw() {return _dods_proxy_server_host_url;}
  const string getProxyForRegExp() throw() {return _dods_proxy_for_regexp;}
  const string getProxyForProxyHostUrl() throw() {return _dods_proxy_for_proxy_host_url;}
  const int    getProxyForRegExpFlags() throw() {return _dods_proxy_for_regexp_flags;}
  const  string getNoProxyForProtocol() throw() {return _dods_no_proxy_for_protocol;}
  const string getNoProxyForHost() throw() {return _dods_no_proxy_for_proxy_host;}
  const int    getNoProxyForPort() throw() {return _dods_no_proxy_for_port;}
  
  // SET METHODS
  void setUseCache(bool b) throw() {_dods_use_cache=b;}
  void setMaxCacheSize(int i) throw() {_dods_cache_max=i;}
  void setMaxCachedObj(int i) throw() {_dods_cached_obj=i;}
  void setIgnoreExpires(int i) throw() {_dods_ign_expires=i;}
  void setNeverDeflate(bool b) throw() {_dods_never_deflate=b;}
  void setDefaultExpires(int i) throw() { _dods_default_expires=i;}
  void setAlwaysValidate(int i) throw() {_dods_always_validate=i;}
  void setProxyServerProtocol(const string &s) throw() {_dods_proxy_server_protocol=s;}
  void setProxyServerHostUrl(const string &s) throw() {_dods_proxy_server_host_url=s;}
  void setProxyForRegExp(const string &s) throw() { _dods_proxy_for_regexp=s;}
  void setProxyForProxyHostUrl(const string &s) throw() {_dods_proxy_for_proxy_host_url=s;}
  void setProxyForRegExpFlags(int i) throw() {_dods_proxy_for_regexp_flags=i;}
  void setNoProxyForProtocol(const string &s) throw() {_dods_no_proxy_for_protocol=s;}
  void setNoProxyForHost(const string &s) throw() {_dods_no_proxy_for_proxy_host=s;}
  void settNoProxyForPort(int i) throw() {_dods_no_proxy_for_port=i;}

};

#endif // _RCReader_h_
