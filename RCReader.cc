
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
 
// (c) COPYRIGHT URI/MIT 2001,2002
// Please read the full copyright statement in the file COPYRIGHT_URI.  
//
// Authors:
//	jose	Jose Garcia <jgarcia@ucar.edu>

/** A singleton which reads and parses the .dodsrc file. This code was
    extracted from Connect (which has since changed considerably). 

    @author: jose */

#include "config_dap.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#ifdef WIN32
#define FALSE 0
// Win32 does not define F_OK. 08/21/02 jhrg
#define F_OK 0
#define DIR_SEP_STRING "\\"
#define DIR_SEP_CHAR   '\\'
#include <direct.h>
#else
#define DIR_SEP_STRING "/"
#define DIR_SEP_CHAR   '/'
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <fstream>

#include "RCReader.h"

RCReader* RCReader::_instance = 0;

// This variable (instance_control) is used to ensure that in a MT
// environment _instance is correctly initialized. See the get_instance
// method. 08/07/02 jhrg
#if HAVE_PTHREAD_H
static pthread_once_t instance_control = PTHREAD_ONCE_INIT;
#endif

/** Using values from this instance of RCReader, write out values for a
    default .dodsrc file. Nominally this will use the defaults for each thing
    that might be read from the configuration file. */

void  
RCReader::write_rc_file()
{
    ofstream fpo(cifp.c_str());
  
    // If the  couldnt be created.  Nothing needs to be done here,
    // the program will simply use the defaults.

    if (fpo) {
	// This means we just created the file.  We will now save
	// the defaults in it for future use.       
	fpo << "# DODS client configuation file. See the DODS" << endl;
	fpo << "# users guide for information." << endl;
	fpo << "USE_CACHE=" << _dods_use_cache << endl;
	fpo << "MAX_CACHE_SIZE=" <<_dods_cache_max  << endl;
	fpo << "MAX_CACHED_OBJ=" << _dods_cached_obj << endl;
	fpo << "IGNORE_EXPIRES=" << _dods_ign_expires  << endl;
	fpo << "CACHE_ROOT=" << cache_root << endl;
	fpo << "DEFAULT_EXPIRES=" <<_dods_default_expires << endl;
	fpo << "ALWAYS_VALIDATE=" << _dods_always_validate << endl;
	fpo << "# Request servers compress responses if possible?" << endl;
	fpo << "# 1 (yes) or 0 (false)." << endl;
	fpo << "DEFLATE=" << _dods_deflate << endl;
	fpo << "# Proxy configuration:" << endl;
	fpo << "# PROXY_SERVER=<protocol>,<host[:port]>" << endl;
	fpo << "# PROXY_SERVER=" <<  _dods_proxy_server_protocol << ","
	    << _dods_proxy_server_host_url << endl;
	fpo << "# PROXY_FOR=<regex>,<host[:port]>" << endl;
	fpo << "# PROXY_FOR=" << _dods_proxy_for_regexp << ","
	    << _dods_proxy_for_proxy_host_url << endl;
	fpo << "# NO_PROXY_FOR=<protocol>,<host|domain>" << endl;
	fpo << "# NO_PROXY_FOR=" << _dods_no_proxy_for_protocol << ","
	    << _dods_no_proxy_for_proxy_host << endl;
	fpo.close();
    }
}

void 
RCReader::read_rc_file()
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
	    } else if ((strncmp(tempstr, "DEFLATE", 7) == 0)
		       && tokenlength == 7) {
		_dods_deflate= atoi(value) ? true : false;
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
			_dods_proxy_for = true;
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
			_dods_no_proxy_for = true;
		    }
		}
	    }
	}
    
	delete [] tempstr;
	tempstr = 0;
    
	fpi.close();	// Close the .dodsrc file. 12/14/99 jhrg
    }  // End of cache file parsing.

    // I think this is here to clean up if the previous access exited w/o
    // removing the lock. 6/27/2002 jhrg
    if (_dods_use_cache) {
	lockstr = cache_root + string(".lock");
	remove(lockstr.c_str());
    }
}

// This function deletes the object which calls the destructor. In this case
// it doesn't do anything, but ... 10/08/02 jhrg
void  
rcreader_clean()
{
    if (RCReader::_instance) {
	delete RCReader::_instance;
	RCReader::_instance = 0;
    }
}
  
RCReader::RCReader()
{
    atexit(rcreader_clean);
  
    lockstr = "";	        // Lock file path
    cifp = "";			// RC file path
    cache_root = "";		// Location of actual cache.
    homedir = "";	        // Cache init file path
    tmpdir = "";		// Fallback position for cache files.

    // Assume it will be able to get a cache file
    _has_rc_file=true;
    _can_create_rc_file=true;

    // ** Set default values **
    // Users must explicitly turn caching on.
    _dods_use_cache=false;
    _dods_cache_max=20;
    _dods_cached_obj=5;
    _dods_ign_expires=0;
    _dods_default_expires= 86400;
    _dods_always_validate=0;

    _dods_deflate=1;

    //flags for PROXY_SERVER=<protocol>,<host url>
    _dods_proxy_server_protocol = "";
    _dods_proxy_server_host_url = "";

    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    _dods_proxy_for = false;	// true if proxy_for is used.
    _dods_proxy_for_regexp = "";
    _dods_proxy_for_proxy_host_url = "";
    _dods_proxy_for_regexp_flags = 0;

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    _dods_no_proxy_for = false;
    _dods_no_proxy_for_protocol = "";
    _dods_no_proxy_for_proxy_host = "";
    // default to port 0 if not specified. This means all ports. Using 80
    // will fail when the URL does not contain the port number. That's
    // probably a bug in libwww. 10/23/2000 jhrg
    _dods_no_proxy_for_port=0;

	string cache_name = ".dods_cache";
    string rc_name = ".dodsrc";
  
    // The following code sets up the cache according to the data stored in
    // the following places, in this order. First the environment variable
    // DODS_CACHE_INIT is checked for a path to the data file. If this fails,
    // $HOME/.dodsrc is checked. Failing this, the compiled-in defaults are
    // used. However, if the path for the file exists and the file does not,
    // then the compiled-in defaults will be written to a file at the
    // location given. 8-1-99 cjm

//  Hard-code to C:\Dods to work around the space-in-pathnames problem for caching
//  under win32.
#ifdef WIN32
	homedir = string("C:") + string(DIR_SEP_STRING) + string("Dods");

	//  Normally, I'd prefer this for WinNT-based systems.
    //  if (getenv("APPDATA"))
	//  homedir = getenv("APPDATA");
    //  else if (getenv("TEMP"))
	//  homedir = getenv("TEMP");
    //  else if (getenv("TMP"))
	//  homedir = getenv("TMP");

#else
    //  Should be ok for Unix
    if (getenv("HOME"))
		homedir = getenv("HOME");
#endif // WIN32
  
    // If there is a leading '/' at the end of $HOME, remove it. 
    if (homedir.length() != 0)
		{
		if (homedir[homedir.length() - 1] == DIR_SEP_CHAR)
			homedir.erase(homedir.length() - 1);
    
		// set default cache root to $HOME/.dods_cache/
		cache_root = homedir + string(DIR_SEP_STRING) + cache_name
			+ string(DIR_SEP_STRING);
		}
#ifndef WIN32
    //  Otherwise set the default cache root to a temporary directory
    else
		{
		tmpdir = string(DIR_SEP_STRING) + string("tmp");
    
		// Otherwise set the default cache root the <tmpdir>/.dods_cache/
		cache_root = tmpdir + string(DIR_SEP_STRING) + cache_name
			+ string(DIR_SEP_STRING);
		}
#endif
  
    if (getenv("DODS_CACHE_INIT"))
	cifp = getenv("DODS_CACHE_INIT");
  
    // If the HOME environment variable wasn't set, and the users home
    // directory is indeterminable, we will neither read nor write a data
    // file and instead just use the compiled in defaults.

    if (cifp.length() == 0)
		{
		if (homedir.length() != 0)
			{
			// Environment variable was set, get data from $HOME/.dodsrc
			cifp = homedir + string(DIR_SEP_STRING) + rc_name;
			// test to get sure we can access the file
			if (access(cifp.c_str(), F_OK))
				{
				// The file does not exist, however we have a directory and a
				// file name so we can try to create it...
				_has_rc_file=false;
				_can_create_rc_file = true; 
				}     
			}
		}
  
    if (_has_rc_file) 
		read_rc_file();
    else if (_can_create_rc_file)
		write_rc_file();
}

RCReader::~RCReader()
{
#if 0
    // Don't always write the cache file. This was erasing values that
    // were set (and besides, it's confusing to see that the file's creation
    // date is always changing. Since this shouldn't be adding anything new,
    // DON'T write it out here. 6/27/2002 jhrg
    if(_instance)
	_instance->write_rc_file();
#endif
}

void
initialize_instance()
{
    // MT-Safe if called via pthread_once or similar
    RCReader::_instance = new RCReader;
}

RCReader* 
RCReader::instance()
{
#if HAVE_PTHREAD_H
    // The instance_control variable is defined at the top of this file.
    // 08/07/02 jhrg
    pthread_once(&instance_control, initialize_instance);
#else
    if (!_instance)
	initialize_instance();
#endif

    return _instance;
}

// $Log: RCReader.cc,v $
// Revision 1.6  2003/02/21 00:14:24  jimg
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
// Revision 1.3.4.10  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.3.4.9  2002/12/01 12:48:15  rmorris
// Bugs fixes for win32 port - numerous related to caching.
//
// Revision 1.3.4.8  2002/11/06 22:56:52  pwest
// Memory delete errors and uninitialized memory read errors corrected
//
// Revision 1.3.4.7  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.3.4.6  2002/10/18 22:56:15  jimg
// The ctor now arranges to call clean() using atexit() (as did the original). I made clean a function although I see that a static method is effectively a function... The class is a little cleaner WRT Win32 #ifdefs.
//
// Revision 1.3.4.5  2002/09/08 23:51:12  rmorris
// Minor tweeks to get it to complile for the 1st time in regards to
// multithreading under win32.
//
// Revision 1.3.4.4  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.3.4.3  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.3.4.2  2002/07/06 20:06:10  jimg
// I added support/use of new boolean fields that clients can check to see if
// proxy information was included in the RC file. I also changed the
// _dods_never_deflate and its use/sense to _dods_deflate (true is this client
// should ask for compressed responses). This parameter is useful because it
// provides users and testers with a way to control compression. I also changed
// the behavior of the class when the dtor is called; it no longer dumps the RC
// file out. This keeps the code from erasing changes users make while a program
// is running.
//
