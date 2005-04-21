
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

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

#include <pthread.h>

#include <fstream>

#include "debug.h"
#include "RCReader.h"
#include "Error.h"

using namespace std;

RCReader* RCReader::_instance = 0;

// This variable (instance_control) is used to ensure that in a MT
// environment _instance is correctly initialized. See the get_instance
// method. 08/07/02 jhrg
static pthread_once_t instance_control = PTHREAD_ONCE_INIT;

/** Using values from this instance of RCReader, write out values for a
    default .dodsrc file. Nominally this will use the defaults for each thing
    that might be read from the configuration file. */

bool
RCReader::write_rc_file(const string &pathname)
{
    DBG(cerr << "Writing the RC file to " << pathname << endl);
    ofstream fpo(pathname.c_str());

    // If the file couldn't be created.  Nothing needs to be done here,
    // the program will simply use the defaults.

    if (fpo) {
	// This means we just created the file.  We will now save
	// the defaults in it for future use.       
	fpo << "# DODS client configuation file. See the DODS" << endl;
	fpo << "# users guide for information." << endl;
	fpo << "USE_CACHE=" << _dods_use_cache << endl;
	fpo << "# Cache and object size are given in megabytes (20 ==> 20Mb)." 
	    << endl;
	fpo << "MAX_CACHE_SIZE=" <<_dods_cache_max  << endl;
	fpo << "MAX_CACHED_OBJ=" << _dods_cached_obj << endl;
	fpo << "IGNORE_EXPIRES=" << _dods_ign_expires  << endl;
	fpo << "CACHE_ROOT=" << d_cache_root << endl;
	fpo << "DEFAULT_EXPIRES=" <<_dods_default_expires << endl;
	fpo << "ALWAYS_VALIDATE=" << _dods_always_validate << endl;
	fpo << "# Request servers compress responses if possible?" << endl;
	fpo << "# 1 (yes) or 0 (false)." << endl;
	fpo << "DEFLATE=" << _dods_deflate << endl;

	fpo << "# Proxy configuration:" << endl;
	fpo << "# PROXY_SERVER=<protocol>,<[username:password@]host[:port]>"
	    << endl;
	if (!d_dods_proxy_server_host.empty()) {
	    fpo << "PROXY_SERVER=" <<  d_dods_proxy_server_protocol << ","
		<< (d_dods_proxy_server_userpw.empty()
		    ? "" 
		    : d_dods_proxy_server_userpw + "@")
		+ d_dods_proxy_server_host 
		+ ":" + long_to_string(d_dods_proxy_server_port) << endl;
	}
#if 0
	// Removed. See note in RCReader.h. 06/17/04 jhrg
	fpo << "# PROXY_FOR=<regex>,<[user:password@]host[:port]>" << endl;
	fpo << "# PROXY_FOR" << _dods_proxy_for_regexp << ","
	    << _dods_proxy_for_proxy_host_url << endl;
#endif

	fpo << "# NO_PROXY_FOR=<protocol>,<host|domain>" << endl;
	if (!d_dods_no_proxy_for_host.empty()) {
	    fpo << "NO_PROXY_FOR=" << d_dods_no_proxy_for_protocol << ","
		<< d_dods_no_proxy_for_host << endl;
	}

	fpo << "# AIS_DATABASE=<file or url>" << endl;
	fpo.close();

	return true;
    }
    
    return false;
}

bool
RCReader::read_rc_file(const string &pathname) throw(Error)
{
    DBG(cerr << "Reading the RC file from " << pathname << endl);

    ifstream fpi(pathname.c_str());
    if (fpi) {
	// The file exists and we may now begin to parse it.  
	// Defaults are already stored in the variables, if the correct
	// tokens are found in the file then those defaults will be 
	// overwritten. 
	char *value;
	char *tempstr = new char[1024];;
#if 0
	char *tempstr2;
	char *tempstr3;
	char *tempstr4;
#endif

	int tokenlength;
	while (true) {
	    fpi.getline(tempstr, 1023);
	    if (!fpi.good())
		break;

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
		d_cache_root = value;
		if (d_cache_root[d_cache_root.length() - 1] != DIR_SEP_CHAR) 
		    d_cache_root += string(DIR_SEP_STRING);
	    } else if ((strncmp(tempstr, "DEFAULT_EXPIRES", 15) == 0)
		       && tokenlength == 15) {
		_dods_default_expires= atoi(value);
	    } else if ((strncmp(tempstr, "ALWAYS_VALIDATE", 15) == 0)
		       && tokenlength == 15) {
		_dods_always_validate = atoi(value);
	    } else if (strncmp(tempstr, "AIS_DATABASE", 12) == 0 
		     && tokenlength == 12) {
		d_ais_database = value;
	    } else if ((strncmp(tempstr, "PROXY_SERVER", 12) == 0)
		     && tokenlength == 12) {
		// Setup a proxy server for all requests.
		string proxy = value;
		string::size_type comma = proxy.find(',');

		// Since the protocol is required, the comma *must* be
		// present. We could throw an Error on the malformed line...
		if (comma == string::npos)
		    continue;
		d_dods_proxy_server_protocol = proxy.substr(0, comma);
		proxy = proxy.substr(comma + 1);

		// Break apart into userpw, host and port.
		string::size_type at_sign = proxy.find('@');
		if (at_sign != string::npos) { // has userpw
		    d_dods_proxy_server_userpw = proxy.substr(0, at_sign);
		    proxy = proxy.substr(at_sign + 1);
		}
		else
		    d_dods_proxy_server_userpw = "";

		// Get host and look for a port number
		string::size_type colon = proxy.find(':');
		if (colon != string::npos) {
		    d_dods_proxy_server_host = proxy.substr(0, colon);
		    d_dods_proxy_server_port 
			= strtol(proxy.substr(colon + 1).c_str(), 0, 0);
		}
		else {
		    d_dods_proxy_server_host = proxy;
		    // No port given, look at protocol or throw
		    if (d_dods_proxy_server_protocol == "http")
			d_dods_proxy_server_port = 80;
		    else if (d_dods_proxy_server_protocol == "https")
			d_dods_proxy_server_port = 443;
		    else if (d_dods_proxy_server_protocol == "ftp")
			d_dods_proxy_server_port = 21;
		    else throw Error("Could not determine the port to use for proxy '"
				     + d_dods_proxy_server_host 
				     + ".' Please check your .dodsrc file.");
		}
	    } else if ((strncmp(tempstr, "NO_PROXY_FOR", 12) == 0)
		       && tokenlength == 12) {
		// Setup a proxy server for all requests.
		string no_proxy = value;
		string::size_type comma = no_proxy.find(',');

		// Since the protocol is required, the comma *must* be
		// present. We could throw an Error on the malformed line...
		if (comma == string::npos)
		    continue;
		d_dods_no_proxy_for_protocol = no_proxy.substr(0, comma);
		d_dods_no_proxy_for_host = no_proxy.substr(comma + 1);
		d_dods_no_proxy_for = true;
#if 0
		// Dont use a proxy server for the host
		// specified.  
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
#endif
	    }
#if 0
	    // Ignore this option. See comment in RCReader.h 06/17/04 jhrg
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
	    }
#endif
	}
    
	delete [] tempstr; tempstr = 0;
    
	fpi.close();	// Close the .dodsrc file. 12/14/99 jhrg
	
	return true;
    }  // End of cache file parsing.

    return false;
}

// Helper for check_env_var(). This is its main logic, separated out for the
// cases under WIN32 where we don't use an environment variable.  09/19/03
// jhrg 
string
RCReader::check_string(string env_var)
{
    struct stat stat_info;

    if (stat(env_var.c_str(), &stat_info) != 0)
	return "";		// ENV VAR not a file or dir, bail

    if (S_ISREG(stat_info.st_mode))
	return env_var;		// ENV VAR is a file, use it

    // ENV VAR is a directory, does it contain .dodsrc? Can we create
    // .dodsrc if it's not there?
    if (S_ISDIR(stat_info.st_mode)) {
	if (*env_var.rbegin() != DIR_SEP_CHAR)	// Add trailing / if missing
	    env_var += DIR_SEP_STRING;
	// Trick: set d_cache_root here in case we're going to create the
	// .dodsrc later on. If the .dodsrc file exists, its value will
	// overwrite this value, if not write_rc_file() will use the correct
	// value. 09/19/03 jhrg
	d_cache_root = env_var + string(".dods_cache") + DIR_SEP_STRING;
	env_var += ".dodsrc";
	if (stat(env_var.c_str(), &stat_info) == 0 &&
	    S_ISREG(stat_info.st_mode))
	    return env_var; // Found .dodsrc in ENV VAR

	// Didn't find .dodsrc in ENV VAR and ENV VAR is a directory; try to
	// create it. Note write_rc_file uses d_cache_root (set above) when
	// it creates the RC file's contents.
	if (write_rc_file(env_var))
	    return env_var;
    }

    // If we're here, then we've not found or created the RC file.
    return "";
}

/** Examine an environment variable. If the env variable is set, then If
    this is the name of a file, use that as the name of the RC file. If this
    is the name of a directory, look in that directory for a file called
    .dodsrc. If there's no such file, create it using default values for its
    parameters. In the last case, write the .dodsrc so that the .dods_cache
    directory is located in the directory named by DODS_CONF.

    @return The pathname to the RC file or "" if another variable/method
    should be used to find/create the RC file. */
string 
RCReader::check_env_var(const string &variable_name) 
{ 
    char *ev = getenv(variable_name.c_str());
    if (!ev || strlen(ev) == 0)
	return "";
    
    return check_string(ev);
}

RCReader::RCReader() throw(Error)
{
    d_rc_file_path = "";
    d_cache_root = "";

    // ** Set default values **
    // Users must explicitly turn caching on.
    _dods_use_cache=false;
    _dods_cache_max=20;
    _dods_cached_obj=5;
    _dods_ign_expires=0;
    _dods_default_expires= 86400;
    _dods_always_validate=0;

    _dods_deflate=0;

    //flags for PROXY_SERVER=<protocol>,<host url>
    d_dods_proxy_server_protocol = "";
    d_dods_proxy_server_host = "";
    d_dods_proxy_server_port = 0;
    d_dods_proxy_server_userpw = "";

    _dods_proxy_server_host_url = ""; // deprecated

    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    _dods_proxy_for = false;	// true if proxy_for is used.
    _dods_proxy_for_regexp = "";
    _dods_proxy_for_proxy_host_url = "";
    _dods_proxy_for_regexp_flags = 0;

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    d_dods_no_proxy_for = false;
    d_dods_no_proxy_for_protocol = "";
    d_dods_no_proxy_for_host = "";
    // default to port 0 if not specified. This means all ports. Using 80
    // will fail when the URL does not contain the port number. That's
    // probably a bug in libwww. 10/23/2000 jhrg
    _dods_no_proxy_for_port=0;

#ifdef WIN32
    string homedir = string("C:") + string(DIR_SEP_STRING) + string("Dods");
    d_rc_file_path = check_string(homedir);
    //  Normally, I'd prefer this for WinNT-based systems.
    if (d_rc_file_path.empty())
	d_rc_file_path = check_env_var("APPDATA");
    if (d_rc_file_path.empty())
	d_rc_file_path = check_env_var("TEMP");
    if (d_rc_file_path.empty())
	d_rc_file_path = check_env_var("TMP");
#else
    d_rc_file_path = check_env_var("DODS_CONF");
    if (d_rc_file_path.empty())
	d_rc_file_path = check_env_var("HOME");
#endif

    if (!d_rc_file_path.empty())
	read_rc_file(d_rc_file_path);

#if 0
    // Hmmm. This seems odd. Let HTTPCache manage the cache. 02/11/04 jhrg
    if (_dods_use_cache) {
	string lockstr = d_cache_root + string(".lock");
	remove(lockstr.c_str());
    }
#endif
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

/** Static void private method. */
void  
RCReader::delete_instance()
{
    if (RCReader::_instance) {
	delete RCReader::_instance; 
	RCReader::_instance = 0;
    }
}

/** Static void private method. */
void
RCReader::initialize_instance() throw(Error)
{
    DBG(cerr << "RCReader::initialize_instance() ... ");

    RCReader::_instance = new RCReader;
    atexit(RCReader::delete_instance);

    DBGN(cerr << "exiting." << endl);
}

RCReader* 
RCReader::instance() throw(Error)
{
    // The instance_control variable is defined at the top of this file.
    // 08/07/02 jhrg
    pthread_once(&instance_control, initialize_instance);

    return _instance;
}

// $Log: RCReader.cc,v $
// Revision 1.15  2005/04/21 17:48:59  jimg
// Removed PTHREADS compile-time switch. Also, checkpoint for the build
// work.
//
// Revision 1.14  2004/11/16 22:50:20  jimg
// Fixed tests. Also fixed a bug intorduced in Vector where a template
// with no name caused some software (any code which depends on the
// template having the same name as the array) to fail.
//
// Revision 1.13  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.12  2004/06/28 16:57:53  pwest
// unix compiler issues
//
// Revision 1.9.2.20  2004/06/21 20:50:43  jimg
// Changes to the proxy options parsing: I added new methods that parse the
// information in the PROXY_SERVER parameter and removed parsing of the
// PROXY_FOR parameter. Also, I've changed the text written to a default
// .dodsrc file so that a PROXY_SERVER line is output only if a proxy server
// is configured (the same/documentation comment line is always output). The
// PROXY_FOR lines are never written.
//
// Revision 1.9.2.19  2004/03/11 18:12:35  jimg
// Ripped out the code in delete_instance that (tries to) reset(s) the
// pthread_once_t mutex. We cannot do this in a portable way and it's needed
// only for the unit tests, which I claim to have fixed so they don't require
// it anymore.
//
// Revision 1.9.2.18  2004/03/07 22:27:47  rmorris
// Make PTHREAD_ONCE_INIT static initialization compatible across platforms.
//
// Revision 1.9.2.17  2004/02/27 17:25:20  edavis
//  adding { } for PTHREAD_ONCE_INIT parm
//
// Revision 1.9.2.16  2004/02/26 22:43:02  edavis
// remove the platform dependent parm PTHREAD_ONCE_INIT
//
// Revision 1.9.2.15  2004/02/22 23:35:03  rmorris
// Solved some problems regarding the differences in the pthread
// implementation across OSX, Win32 and non-OSX unixes. Either we are using
// pthreads in a manner that was not intended by the pthread 'standard' (??)
// or pthread implementations vary across platform or (finally) perhaps we
// are encountering different implementations of pthreads as a result of its
// development over time. Regardless our pthread code is starting to become
// less portable. See how pthread_once_t varies across the above-mentioned
// platforms. These changes get it to compile. I'm crossing my fingers that
// it will run correctly everywhere.
//
// Revision 1.11  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.9.2.14  2004/02/16 11:55:28  rmorris
// Minor #endif missing.
//
// Revision 1.9.2.13  2004/02/16 10:51:01  rmorris
// We may be peeking under the hook of the pthread library more than was
// intended (??) when we choose to assign PTHREAD_ONCE_INIT to pthread_once_t
// vars using "=".  Under unix such things are integer values.  Under win32,
// it is a structure.  These under-the-hood differences in the pthread library
// cross-platform may be moot points if we stuck with the agreed upon API to
// pthreads which (fingers crossed) doesn't vary across platforms.  Such an
// assignment was done in a single place (see delete_instance()) only.  Something
// about the recent extended use of the pthread library has me worried.
//
// Revision 1.9.2.12  2004/02/11 22:26:46  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.9.2.11  2004/02/11 17:07:34  jimg
// I moved 'using namespace std;' back down to where it is in all the files. I
// don't know why I moved it up in the first place. I think I was stabbing in the
// dark... Also, (more importantly) I modified the initialize_instance and
// delete_instance static methods so that they work like the SignalHandler and
// HTTPCache classes. It now works to call RCReader::delete_instance() and then
// get a brand new pointer using RCReader::instance(). This is important for
// various tests (and maybe elsewhere).
//
// Revision 1.9.2.10  2004/01/25 08:03:02  rmorris
// Moved using namespace std down, yet still above debug.h.  VC++ 7.x was
// choking and I cross tested (compiled) _everywhere_.
//
// Revision 1.9.2.9  2004/01/23 00:23:11  jimg
// Moved "using namespace std;" before include of debug.h.
//
// Revision 1.9.2.8  2004/01/22 20:47:24  jimg
// Fix for bug 689. I added tests to make sure the cache size doesn't wind
// up being set to a negative number. I also changed the types of the cache
// size and entry size from int to unsigned long. Added information to
// the default .dodsrc file explaining the units of the CACHE_SIZE and
// MAX_ENTRY_SIZE parameters.
//
// Revision 1.9.2.7  2004/01/22 17:09:52  jimg
// Added std namespace declarations since the DBG() macro uses cerr.
//
// Revision 1.10  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.9.2.6  2003/11/25 18:20:13  jimg
// Comments..
//
// Revision 1.9.2.5  2003/10/04 08:00:08  rmorris
// Patched a couple of erronous path separators - /.dods_cache/ needs
// to use \ under win32.  Was breaking caching sometimes.
//
// Revision 1.9.2.4  2003/09/23 14:44:26  jimg
// Fix from Kevin O'Brien: In the ctor _dods_use_cache was set true regardless
// of the value of USE_CACHE in the .dodsrc file.
//
// Revision 1.9.2.3  2003/09/22 21:22:17  jimg
// Fixed check_env_var(). It was assigning a null to a string when an
// environment was not defined. The string class crashes in this case!
//
// Revision 1.9.2.2  2003/09/19 22:29:52  jimg
// Fixed the second part of bug #655 where DODS_CONF was not used properly.
// There was a mass of confusing (and broken) code in the ctor which I hope I've
// fixed. I added some unit tests (see RCReaderTest). I think it's probably a
// bad idea to have the code that looks for the RC file create it if it's not
// found, but the alternative is messier.
//
// Revision 1.9.2.1  2003/05/08 00:05:28  jimg
// Modified so that compression is no longer requested by default. This is
// probably the best default since users can turn on compression in the
// .dodsrc file.
//
// Revision 1.9  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.8  2003/03/13 23:49:32  jimg
// Added DGB lines to track down a bug. These seem useful when you cannot use a
// debugger.
//
// Revision 1.5.2.2  2003/03/07 23:00:01  jimg
// Fixed up code indentation... Added use of env var DODS_CONF. DODS_CACHE_INIT
// should be deprecated.
//
// Revision 1.7  2003/02/27 23:37:15  jimg
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
// The ctor now arranges to call clean() using atexit() (as did the
// original). I made clean a function although I see that a static method is
// effectively a function... The class is a little cleaner WRT Win32 #ifdefs.
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
