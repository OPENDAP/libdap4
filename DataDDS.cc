
// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 9/19/97

// $Log: DataDDS.cc,v $
// Revision 1.7  1999/05/05 00:40:11  jimg
// Modified the DataDDS class so that a version string may begin with any
// character sequence, not just `dods'. This means that each server can identify
// itself in the version string.
//
// Revision 1.6  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.5  1998/11/10 01:01:11  jimg
// version_regex now fast compiled.
//
// Revision 1.4.4.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.4.4.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.4  1998/06/05 21:29:54  jimg
// Changed the version regex used to check for the version string pattern so
// that it includes a `wildcard' at the end. Thus versions like 2.15.1 and
// 2.15a can be read (both as major version == 2 and minor version == 15).
//
// Revision 1.3  1998/02/05 20:13:52  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.2  1997/12/16 00:37:14  jimg
// Changed _version_string_to_numbers() so that it does something sensible
// when the version string is hosed.
//
// Revision 1.1  1997/09/22 22:19:27  jimg
// Created this subclass of DDS to hold version information in the data DDS
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: DataDDS.cc,v 1.7 1999/05/05 00:40:11 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#ifdef __GNUG__
#include <strstream>
#else
#include <sstream>
#endif

#include <string>
#include <Regex.h>

#include "DataDDS.h"
#include "debug.h"

// private

// The version string looks like `DODS/2.14'

void
DataDDS::_version_string_to_numbers()
{
    static Regex version_regex("[a-z]+/[0-9]\\.[0-9]+[.0-9a-zA-Z]*", 1);

    DBG(cerr << "in version string to numbers" << endl);

    if (version_regex.match(_server_version.c_str(), _server_version.length()) != (int)_server_version.length()) {
	_server_version_major = 0;
	_server_version_minor = 0;
    }
    else {
	string num = _server_version.substr(_server_version.find('/')+1);

	istrstream iss(num.c_str());

	iss >> _server_version_major;
	char c;
	iss >> c;		// This reads the `.' in the version string
	iss >> _server_version_minor;

	DBG(cerr << "Server version: " << _server_version_major << "." \
	    << _server_version_minor << endl);
    }
}

// public

DataDDS::DataDDS(const string &n, const string &v)
    :DDS(n), _server_version(v), _sequence_level(0)
{
    _version_string_to_numbers();
}

DataDDS::~DataDDS()
{
}

void
DataDDS::set_version(const string &v)
{
    _server_version = v;
    _version_string_to_numbers();
}

int
DataDDS::get_version_major()
{
    return _server_version_major;
}

int
DataDDS::get_version_minor()
{
    return _server_version_minor;
}

int
DataDDS::sequence_level()
{
    return _sequence_level;
}

void
DataDDS::set_sequence_level(int level)
{
    _sequence_level = level;
}
