#include <stdlib.h>
#include <unistd.h>


#include <fstream>

#include "RCReader.h"

#ifdef WIN32
#define DIR_SEP_STRING "\\"
#define DIR_SEP_CHAR   '\\'
#else
#define DIR_SEP_STRING "/"
#define DIR_SEP_CHAR   '/'
#endif



RCReader* RCReader::_instance =0;

inline void  RCReader::saveCacheStatus()
{
  ofstream fpo(cifp.c_str());
  
  if (!fpo) {
    // File couldnt be created.  Nothing needs to be done here,
    // the program will simply use the defaults.
  } else {
    // This means we just created the file.  We will now save
    // the defaults in it for future use.       
    fpo << "# DODS client configuation file. See the DODS" << endl;
    fpo << "# users guide for information." << endl;
    fpo << "USE_CACHE=" << _dods_use_cache << endl;
    fpo << "MAX_CACHE_SIZE=" <<_dods_cache_max  << endl;
    fpo << "MAX_CACHED_OBJ=" << _dods_cached_obj << endl;
    fpo << "IGNORE_EXPIRES=" << _dods_ign_expires  << endl;
#if 0
    fpo << "NEVER_DEFLATE=" << _dods_never_deflate << endl;
#endif
    fpo << "CACHE_ROOT=" << cache_root << endl;
    fpo << "DEFAULT_EXPIRES=" <<_dods_default_expires << endl;
    fpo << "ALWAYS_VALIDATE=" << _dods_always_validate << endl;
    fpo << "# PROXY_SERVER=<protocol>,<host url>" << endl;
    fpo << "# PROXY_FOR=<regex>,<proxy host url>,<flags>" << endl;
    fpo << "# NO_PROXY_FOR=<protocol>,<host>" << endl;
    fpo.close();
  }
}

inline void  RCReader::loadCacheStatus()
{
   
  ifstream fpi(cifp.c_str());
  if (fpi) {
    // The file exists and we may now begin to parse it.  
    // Defaults are already stored in the variables, if the correct
    // tokens are found in the file then those defaults will be 
    // overwritten. 
    char *value;
    char *tempstr;
    char *tempstr2;
    char *tempstr3;
    char *tempstr4;
    tempstr = new char[256];
    int tokenlength;
    while (1) {
      fpi.getline(tempstr, 128);
      if (!fpi.good())	//  Ok for unix also ??? Yes.
	break;	// Gets a line from the file.
      value = strchr(tempstr, '=');
      if (!value)
	continue;
      tokenlength = (int) value - (int) tempstr;
      value++;
      if ((strncmp(tempstr, "USE_CACHE", 9) == 0)
	  && tokenlength == 9) {
	_dods_use_cache= atoi(value) ? true : false;
      } else if ((strncmp(tempstr, "MAX_CACHE_SIZE", 14) == 0)
		 && tokenlength == 14) {
	_dods_cache_max = atoi(value);
      } else if ((strncmp(tempstr, "MAX_CACHED_OBJ", 14) == 0)
		 && tokenlength == 14) {
	_dods_cached_obj = atoi(value);
      } else if ((strncmp(tempstr, "IGNORE_EXPIRES", 14) == 0)
		 && tokenlength == 14) {
	_dods_ign_expires= atoi(value);
      } else if ((strncmp(tempstr, "NEVER_DEFLATE", 13) == 0)
		 && tokenlength == 13) {
	// (re)Set the member value iff the dodsrc file changes
	// te default. 12/1/99 jhrg
	_dods_never_deflate= atoi(value) ? false : true;
      } else if ((strncmp(tempstr, "CACHE_ROOT", 10) == 0)
		 && tokenlength == 10) {
	cache_root = value;
	if (cache_root[cache_root.length() - 1] !=
	    DIR_SEP_CHAR) cache_root += string(DIR_SEP_STRING);
      } else if ((strncmp(tempstr, "DEFAULT_EXPIRES", 15) == 0)
		 && tokenlength == 15) {
	_dods_default_expires= atoi(value);
      } else if ((strncmp(tempstr, "ALWAYS_VALIDATE", 15) == 0)
		 && tokenlength == 15) {
	_dods_always_validate = atoi(value);
      }
      // Check for tags relating to the proxy server
      // 8.20.2000 cjm
      else if ((strncmp(tempstr, "PROXY_SERVER", 12) == 0)
	       && tokenlength == 12) {
	// Setup a proxy server for all requests.
	tempstr2 = value;
	if (tempstr2 != NULL) {
	  tempstr3 = strchr(tempstr2, ',');
	  if (tempstr3 != NULL) {
	    tempstr3[0] = (char) 0;	//terminate access
	    // method string.
	    tempstr3++;	//advance pointer.
	    // Setup the proxy server.  tempstr2 is
	    // the access method (ftp, http, etc)
	    // and tempstr3 is a fully qualified 
	    // name which includes the access method.
	    _dods_proxy_server_protocol=string(tempstr2);
	    _dods_proxy_server_host_url=string(tempstr3);
	  }
	  
	}
      } 
      else if ((strncmp(tempstr, "PROXY_FOR", 9) == 0)
	       && tokenlength == 9) {
	// Setup a proxy server for any requests
	// matching the given regular expression.
	tempstr2 = value;
	if (tempstr2 != NULL) {
	  tempstr3 = strchr(tempstr2, ',');
	  if (tempstr3 != NULL) {
	    tempstr3[0] = (char) 0;	//terminate
	    //tempstr2
	    tempstr3++;	// point to proxy server;
	    tempstr4 = strchr(tempstr3, ',');
	    // tempstr4 will be !NULL only if the regex flags
	    // are given. But we're not going to support
	    // those unless requested. So handle the case
	    // where they are not given, too. 10/25/2000 jhrg
	    if (tempstr4 != NULL) {
	      tempstr4[0] = (char) 0;	//terminate
				//tempstr3
	      tempstr4++;
	      _dods_proxy_for_regexp_flags = atoi(tempstr4);
	      
	    }
	    _dods_proxy_for_regexp=string(tempstr2);
	    _dods_proxy_for_proxy_host_url=string(tempstr3);
	  }
	}
      } else if ((strncmp(tempstr, "NO_PROXY_FOR", 12) == 0)
		 && tokenlength == 12) {
	// Dont use a proxy server for the host
	// specified.  Multiple NO_PROXY_FOR entries
	// can be in the .dodsrc.  cjm
	tempstr2 = value;
	if (tempstr2 != NULL) {
	  tempstr3 = strchr(tempstr2, ',');
	  if (tempstr3 != NULL) {
	    tempstr3[0] = (char) 0;	//terminate
	    //tempstr2
	    tempstr3++;	// point to access method.
	    tempstr4 = strchr(tempstr3, ',');
	    if (tempstr4 != NULL) {
	      tempstr4[0] = (char) 0;	//terminate
				//tempstr3
	      tempstr4++;
	      _dods_no_proxy_for_port = atoi(tempstr4);
	    }
	    
	    // add proxy server for host 
	    // 'tempstr2' w/ access method 
	    // 'tempstr3' and port 'noproxy_host_port'.
	    // NB: Params reversed from above. 10/23/2000 jhrg
	    _dods_no_proxy_for_protocol=string(tempstr2);
	    _dods_no_proxy_for_proxy_host=string(tempstr3);
	    
	  }
	}
      }
    }
    
    delete tempstr;
    tempstr = 0;
    
    fpi.close();	// Close the .dodsrc file. 12/14/99 jhrg
  }  // End of cache file parsing.

}


RCReader::RCReader()
{
  // Register this method to clean up and save status upon exit event.
  atexit(RCReader::clean);
  
  // Assume it will be able to get a cache file
  _has_cache_file=true;
  _can_create_cache_file=false;

  _dods_use_cache=true;
  _dods_cache_max=20;
  _dods_cached_obj=5;
  _dods_ign_expires=0;
  _dods_never_deflate=0;
  _dods_default_expires= 86400;
  _dods_always_validate=0;

  _dods_proxy_for_regexp_flags=0;

  // default to port 0 if not specified. This means
  // all ports. Using 80 will fail when the URL
  // does not contain the port number. That's
  // probably a bug in libwww. 10/23/2000 jhrg
  _dods_no_proxy_for_port=0;

  lockstr = "";	        //  Lock file path
  cifp = "";
  cache_root = "";	//  Location of actual cache.
  homedir = "";	        //  Cache init file path
  tmpdir = "";		//  Fallback position for cache files.

  string cache_name = ".dods_cache";
  string src_name = ".dodsrc";
  
  // The following code sets up the cache according to the data stored
  // in the following places, in this order.  First the environment 
  // variable DODS_CACHE_INIT is checked for a path to the data file. 
  // If this fails, $HOME/.dodsrc is checked.  Failing this, the 
  // compiled-in defaults are used.    However, if the path for the 
  // file exists and the file does not, then the compiled-in defaults
  // will be written to a file at the location given. 8-1-99 cjm
  
  // Store the users home directory or for win-NT based systems, the user &
  // & application-specific directory.  Punt for win9x-based systems.
#ifdef WIN32
  
  //  Should be ok for WinNT and versions of Windows that are based upon it
  //  - such as Windows 2000. APPDATA not appropriate for Win9x-based
  //  systems, we'll have to default to using the temporary directory in
  //  that case because there is no user specific directory denoted by an
  //  env var.
  if (getenv("APPDATA"))
    homedir = getenv("APPDATA");
  else if (getenv("TEMP"))
    homedir = getenv("TEMP");
  else if (getenv("TMP"))
    homedir = getenv("TMP");
  //  One of the above _must_ have held true under win32 in the very
  //  unlikely situation where that wasn't the case - punt hard.
  else
    homedir = "C:" + string(DIR_SEP_STRING);
  
  //  Shouldn't happen, but double check
  if (homedir[homedir.length() - 1] == DIR_SEP_CHAR)
    homedir.erase(homedir.length() - 1);
  homedir += string(DIR_SEP_STRING) + string("Dods");
#else
  //  Should be ok for Unix
  if (getenv("HOME"))
    homedir = getenv("HOME");
#endif
  
  // If there is a leading '/' at the end of $HOME, remove it. 
  if (homedir.length() != 0) {
    if (homedir[homedir.length() - 1] == DIR_SEP_CHAR)
      homedir.erase(homedir.length() - 1);
    
    // set default cache root to $HOME/.dods_cache/
    cache_root =
      homedir + string(DIR_SEP_STRING) + cache_name +
      string(DIR_SEP_STRING);
  }
#ifndef WIN32
  //  Otherwise set the default cache root to a temporary directory
  else {
    tmpdir = string(DIR_SEP_STRING) + string("tmp");
    
    // Otherwise set the default cache root the <tmpdir>/.dods_cache/
    cache_root =
      tmpdir + string(DIR_SEP_STRING) + cache_name +
      string(DIR_SEP_STRING);
  }
#endif
  
  if (getenv("DODS_CACHE_INIT"))
    cifp = getenv("DODS_CACHE_INIT");
  
  if (cifp.length() == 0) 
    {
      if (homedir.length() != 0) 
	{
	  // Environment variable was set, get data from $HOME/.dodsrc
	  cifp = homedir + string(DIR_SEP_STRING) + src_name;
	  // test to get sure we can access the file
	  if (access(cifp.c_str(), F_OK))
	    {
	      // the file does not exist, however we have a directory and a file name so we can try to create it...
	      _has_cache_file=false;
	      _can_create_cache_file = true; 
	    }     
	}
      else
	{
	  // Environment variable wasn't set, and the users home directory
	  // is indeterminable, so we will neither read nor write a data 
	  // file and instead just use the compiled in defaults.
	}
    }

#define WIN32_CACHE_HACK
#if defined(WIN32) && defined(WIN32_CACHE_HACK)
  //  Temporary hack in lieu of a fix.  Caching doesn't work under
  //  Windows.  This lets us bypass the problem in the short-term
  //  by turning of client-side caching for windows systems.
  //  The problem lies with libwww and we expect or hope that
  //  when we update the Dods distribution with the newest libwww,
  //  the problem will go away.  The problem lies with the code
  //  that parses the .index file - it performs improperly on
  //  filenames containing spaces.  Using windows "short names"
  //  functionality for _cache_root doesn't solve the problem.
  //  Using a _cache_root beginning with file:/ with spaces
  //  escaped is also no help.
  //  rom - 07/17/2000.
  _has_cache_file = false;
  _dods_use_cache = false;
#endif
  
  if (_has_cache_file) 
    loadCacheStatus();

}

RCReader:: ~RCReader()
{
  if(_instance)
    _instance->saveCacheStatus();
}

RCReader* RCReader::instance()
{
  if (!_instance)
    _instance= new RCReader;
  return _instance;
}

void  RCReader::clean()
{
  if (_instance)
    delete _instance;
}
  

