
// (c) COPYRIGHT URI/MIT 1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 9/19/97

// $Log: DataDDS.cc,v $
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

static char rcsid[] __unused__ = {"$Id: DataDDS.cc,v 1.4 1998/06/05 21:29:54 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include <strstream.h>

#include <String.h>

#include "DataDDS.h"
#include "debug.h"

// private

// The version string looks like `DODS/2.14'

void
DataDDS::_version_string_to_numbers()
{
    static Regex version_regex("dods/[0-9]\\.[0-9]+[.0-9a-zA-Z]*");

    DBG(cerr << "in version string to numbers" << endl);

    if (!_server_version.matches(version_regex)) {
	_server_version_major = 0;
	_server_version_minor = 0;
    }
    else {
	String num = _server_version.after("/");

	istrstream iss((const char *)num);

	iss >> _server_version_major;
	char c;
	iss >> c;		// This reads the `.' in the version string
	iss >> _server_version_minor;

	DBG(cerr << "Server version: " << _server_version_major << "." \
	    << _server_version_minor << endl);
    }
}

// public

DataDDS::DataDDS(const String &n = (char *)0, const String &v = (char *)0)
    :DDS(n), _server_version(v), _sequence_level(0)
{
    _version_string_to_numbers();
}

DataDDS::~DataDDS()
{
}

void
DataDDS::set_version(const String &v)
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
