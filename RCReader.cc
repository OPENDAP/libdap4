
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

#include "config.h"

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
	fpo << "# OPeNDAP client configuation file. See the OPeNDAP" << endl;
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
	int tokenlength;
	while (true) {
	    fpi.getline(tempstr, 1023);
	    if (!fpi.good())
		break;

	    value = strchr(tempstr, '=');
	    if (!value)
		continue;
	    tokenlength = value - tempstr;
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
	    }
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
}

RCReader::~RCReader()
{
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
