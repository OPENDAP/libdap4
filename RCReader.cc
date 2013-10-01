
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 2001,2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
// jose Jose Garcia <jgarcia@ucar.edu>

/** A singleton which reads and parses the .dodsrc file. This code was
    extracted from Connect (which has since changed considerably).

    @author: jose */

// #define DODS_DEBUG
#include "config.h"

#include <cstring>
#include <cstdlib>

#include <unistd.h>  // for stat
#include <sys/types.h>
#include <sys/stat.h>

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

namespace libdap {

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
        fpo << "# OPeNDAP client configuration file. See the OPeNDAP" << endl;
        fpo << "# users guide for information." << endl;
        fpo << "USE_CACHE=" << _dods_use_cache << endl;
        fpo << "# Cache and object size are given in megabytes (20 ==> 20Mb)."
        << endl;
        fpo << "MAX_CACHE_SIZE=" << _dods_cache_max  << endl;
        fpo << "MAX_CACHED_OBJ=" << _dods_cached_obj << endl;
        fpo << "IGNORE_EXPIRES=" << _dods_ign_expires  << endl;
        fpo << "CACHE_ROOT=" << d_cache_root << endl;
        fpo << "DEFAULT_EXPIRES=" << _dods_default_expires << endl;
        fpo << "ALWAYS_VALIDATE=" << _dods_always_validate << endl;
        fpo << "# Request servers compress responses if possible?" << endl;
        fpo << "# 1 (yes) or 0 (false)." << endl;
        fpo << "DEFLATE=" << _dods_deflate << endl;

        fpo << "# Should SSL certificates and hosts be validated? SSL" << endl;
        fpo << "# will only work with signed certificates." << endl;
        fpo << "VALIDATE_SSL=" << d_validate_ssl << endl;

        fpo << "# Proxy configuration (optional parts in []s)." << endl;
	fpo << "# You may also use the 'http_proxy' environment variable"
	    << endl;
	fpo << "# but a value in this file will override that env variable."
	    << endl;
        fpo << "# PROXY_SERVER=[http://][username:password@]host[:port]"
        << endl;
        if (!d_dods_proxy_server_host.empty()) {
            fpo << "PROXY_SERVER=" <<  d_dods_proxy_server_protocol << "://"
            << (d_dods_proxy_server_userpw.empty()
                ? ""
                : d_dods_proxy_server_userpw + "@")
            + d_dods_proxy_server_host
            + ":" + long_to_string(d_dods_proxy_server_port) << endl;
        }

        fpo << "# NO_PROXY_FOR=<host|domain>" << endl;
        if (!d_dods_no_proxy_for_host.empty()) {
            fpo << "NO_PROXY_FOR=" << d_dods_no_proxy_for_host << endl;
        }

        fpo << "# AIS_DATABASE=<file or url>" << endl;

	fpo << "# COOKIE_JAR=.dods_cookies" << endl;
	fpo << "# The cookie jar is a file that holds cookies sent from"
	    << endl;
	fpo << "# servers such as single signon systems. Uncomment this"
	    << endl;
	fpo << "# option and provide a file name to activate this feature."
	    << endl;
	fpo << "# If the value is a filename, it will be created in this"
	    << endl;
	fpo << "# directory; a full pathname can be used to force a specific"
	    << endl;
	fpo << "# location." << endl;

        fpo.close();
        return true;
    }

    return false;
}

bool
RCReader::read_rc_file(const string &pathname)
{
    DBG(cerr << "Reading the RC file from " << pathname << endl);

    ifstream fpi(pathname.c_str());
    if (fpi) {
        // The file exists and we may now begin to parse it.
        // Defaults are already stored in the variables, if the correct
        // tokens are found in the file then those defaults will be
        // overwritten.
        char *value;
        // TODO Replace with a vector<char>
        //char *tempstr = new char[1024];
        vector<char> tempstr(1024);
        int tokenlength;
        while (true) {
            fpi.getline(&tempstr[0], 1023);
            if (!fpi.good())
                break;

            value = strchr(&tempstr[0], '=');
            if (!value)
                continue;
            tokenlength = value - &tempstr[0];
            value++;

            if ((strncmp(&tempstr[0], "USE_CACHE", 9) == 0)
                && tokenlength == 9) {
                _dods_use_cache = atoi(value) ? true : false;
            }
            else if ((strncmp(&tempstr[0], "MAX_CACHE_SIZE", 14) == 0)
                     && tokenlength == 14) {
                _dods_cache_max = atoi(value);
            }
            else if ((strncmp(&tempstr[0], "MAX_CACHED_OBJ", 14) == 0)
                     && tokenlength == 14) {
                _dods_cached_obj = atoi(value);
            }
            else if ((strncmp(&tempstr[0], "IGNORE_EXPIRES", 14) == 0)
                     && tokenlength == 14) {
                _dods_ign_expires = atoi(value);
            }
            else if ((strncmp(&tempstr[0], "DEFLATE", 7) == 0)
                     && tokenlength == 7) {
                _dods_deflate = atoi(value) ? true : false;
            }
            else if ((strncmp(&tempstr[0], "CACHE_ROOT", 10) == 0)
                     && tokenlength == 10) {
                d_cache_root = value;
                if (d_cache_root[d_cache_root.length() - 1] != DIR_SEP_CHAR)
                    d_cache_root += string(DIR_SEP_STRING);
            }
            else if ((strncmp(&tempstr[0], "DEFAULT_EXPIRES", 15) == 0)
                     && tokenlength == 15) {
                _dods_default_expires = atoi(value);
            }
            else if ((strncmp(&tempstr[0], "ALWAYS_VALIDATE", 15) == 0)
                     && tokenlength == 15) {
                _dods_always_validate = atoi(value);
            }
            else if ((strncmp(&tempstr[0], "VALIDATE_SSL", 12) == 0)
                     && tokenlength == 12) {
                d_validate_ssl = atoi(value);
            }
            else if (strncmp(&tempstr[0], "AIS_DATABASE", 12) == 0
                     && tokenlength == 12) {
                d_ais_database = value;
	    }
            else if (strncmp(&tempstr[0], "COOKIE_JAR", 10) == 0
                     && tokenlength == 10) {
		// if the value of COOKIE_JAR starts with a slash, use it as
		// is. However, if it does not start with a slash, prefix it
		// with the directory that contains the .dodsrc file.
		if (value[0] == '/') {
		    d_cookie_jar = value;
		}
		else {
		    d_cookie_jar = d_rc_file_path.substr(0, d_rc_file_path.find(".dodsrc")) + string(value);
		}
		DBG(cerr << "set cookie jar to: " << d_cookie_jar << endl);
            }
            else if ((strncmp(&tempstr[0], "PROXY_SERVER", 12) == 0)
                     && tokenlength == 12) {
                // Setup a proxy server for all requests.
		// The original syntax was <protocol>,<machine> where the
		// machine could also contain the user/pass and port info.
		// Support that but also support machine prefixed by
		// 'http://' with and without the '<protocol>,' prefix. jhrg
		// 4/21/08 (see bug 1095).
                string proxy = value;
                string::size_type comma = proxy.find(',');

                // Since the <protocol> is now optional, the comma might be
                // here. If it is, check that the protocol given is http.
                if (comma != string::npos) {
		    d_dods_proxy_server_protocol = proxy.substr(0, comma);
		    downcase(d_dods_proxy_server_protocol);
		    if (d_dods_proxy_server_protocol != "http")
			throw Error("The only supported protocol for a proxy server is \"HTTP\". Correct your \".dodsrc\" file.");
		    proxy = proxy.substr(comma + 1);
		}
		else {
		    d_dods_proxy_server_protocol = "http";
		}

		// Look for a 'protocol://' prefix; skip if found
		string::size_type protocol = proxy.find("://");
		if (protocol != string::npos) {
		    proxy = proxy.substr(protocol + 3);
		}

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
		    d_dods_proxy_server_port = 80;
                }
            }
            else if ((strncmp(&tempstr[0], "NO_PROXY_FOR", 12) == 0)
                     && tokenlength == 12) {
                // Setup a proxy server for all requests.
                string no_proxy = value;
                string::size_type comma = no_proxy.find(',');

                // Since the protocol is required, the comma *must* be
                // present. We could throw an Error on the malformed line...
                if (comma == string::npos) {
		    d_dods_no_proxy_for_protocol = "http";
		    d_dods_no_proxy_for_host = no_proxy;
		    d_dods_no_proxy_for = true;
		}
		else {
		    d_dods_no_proxy_for_protocol = no_proxy.substr(0, comma);
		    d_dods_no_proxy_for_host = no_proxy.substr(comma + 1);
		    d_dods_no_proxy_for = true;
		}
            }
        }

        //delete [] tempstr; tempstr = 0;

        fpi.close(); // Close the .dodsrc file. 12/14/99 jhrg

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
	DBG(cerr << "Entering check_string... (" << env_var << ")" << endl);
    struct stat stat_info;

	if (stat(env_var.c_str(), &stat_info) != 0) {
		DBG(cerr << "stat returned non-zero" << endl);
        return "";  // ENV VAR not a file or dir, bail
	}

	if (S_ISREG(stat_info.st_mode)) {
		DBG(cerr << "S_ISREG: " << S_ISREG(stat_info.st_mode) << endl);
        return env_var;  // ENV VAR is a file, use it
	}

    // ENV VAR is a directory, does it contain .dodsrc? Can we create
    // .dodsrc if it's not there?
    if (S_ISDIR(stat_info.st_mode)) {
		DBG(cerr << "S_ISDIR: " << S_ISDIR(stat_info.st_mode) << endl);
        if (*env_var.rbegin() != DIR_SEP_CHAR) // Add trailing / if missing
            env_var += DIR_SEP_STRING;
        // Trick: set d_cache_root here in case we're going to create the
        // .dodsrc later on. If the .dodsrc file exists, its value will
        // overwrite this value, if not write_rc_file() will use the correct
        // value. 09/19/03 jhrg
        d_cache_root = env_var + string(".dods_cache") + DIR_SEP_STRING;
        env_var += ".dodsrc";
        if (stat(env_var.c_str(), &stat_info) == 0 &&
			S_ISREG(stat_info.st_mode)) {
			DBG(cerr << "Found .dodsrc in \"" << env_var << "\"" << endl);
            return env_var; // Found .dodsrc in ENV VAR
		}

        // Didn't find .dodsrc in ENV VAR and ENV VAR is a directory; try to
        // create it. Note write_rc_file uses d_cache_root (set above) when
        // it creates the RC file's contents.
		if (write_rc_file(env_var)) {
			DBG(cerr << "Wrote .dodsrc in \"" << env_var << "\"" << endl);
            return env_var;
		}
    }

    // If we're here, then we've neither found nor created the RC file.
	DBG(cerr << "could neither find nor create a .dodsrc file" << endl);
    return "";
}

/** Examine an environment variable. If the env variable is set, then if
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
    _dods_use_cache = false;
    _dods_cache_max = 20;
    _dods_cached_obj = 5;
    _dods_ign_expires = 0;
    _dods_default_expires = 86400;
    _dods_always_validate = 0;

    _dods_deflate = 0;
    d_validate_ssl = 1;

    //flags for PROXY_SERVER=<protocol>,<host url>
    // New syntax PROXY_SERVER=[http://][user:pw@]host[:port]
    d_dods_proxy_server_protocol = "";
    d_dods_proxy_server_host = "";
    d_dods_proxy_server_port = 0;
    d_dods_proxy_server_userpw = "";

    _dods_proxy_server_host_url = ""; // deprecated

    // PROXY_FOR is deprecated.
    // flags for PROXY_FOR=<regex>,<proxy host url>,<flags>
    _dods_proxy_for = false; // true if proxy_for is used.
    _dods_proxy_for_regexp = "";
    _dods_proxy_for_proxy_host_url = "";
    _dods_proxy_for_regexp_flags = 0;

    //flags for NO_PROXY_FOR=<protocol>,<host>,<port>
    // New syntax NO_PROXY_FOR=<host|domain>
    d_dods_no_proxy_for = false;
    d_dods_no_proxy_for_protocol = ""; // deprecated
    d_dods_no_proxy_for_host = "";
    // default to port 0 if not specified. This means all ports. Using 80
    // will fail when the URL does not contain the port number. That's
    // probably a bug in libwww. 10/23/2000 jhrg
    _dods_no_proxy_for_port = 0; // deprecated

    d_cookie_jar = "";

#ifdef WIN32
    string homedir = string("C:") + string(DIR_SEP_STRING) + string("Dods");
    d_rc_file_path = check_string(homedir);
    if (d_rc_file_path.empty()) {
	homedir = string("C:") + string(DIR_SEP_STRING) + string("opendap");
	d_rc_file_path = check_string(homedir);
    }
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
    DBG(cerr << "Looking for .dodsrc in: " << d_rc_file_path << endl);

    if (!d_rc_file_path.empty())
        read_rc_file(d_rc_file_path);
}

RCReader::~RCReader()
{}

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
RCReader::initialize_instance()
{
    DBGN(cerr << "RCReader::initialize_instance() ... ");

    RCReader::_instance = new RCReader;
    atexit(RCReader::delete_instance);

    DBG(cerr << "exiting." << endl);
}

RCReader*
RCReader::instance()
{
	DBG(cerr << "Entring RCReader::instance" << endl);
    // The instance_control variable is defined at the top of this file.
    // 08/07/02 jhrg
    pthread_once(&instance_control, initialize_instance);

    DBG(cerr << "Instance value: " << hex << _instance << dec << endl);

    return _instance;
}

} // namespace libdap
