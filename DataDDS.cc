
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 9/19/97

// $Log: DataDDS.cc,v $
// Revision 1.1  1997/09/22 22:19:27  jimg
// Created this subclass of DDS to hold version information in the data DDS
//

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: DataDDS.cc,v 1.1 1997/09/22 22:19:27 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include <strstream.h>

#include <String.h>

#include "DataDDS.h"

// private

// The Zversion string looks like `DODS/2.14'

void
DataDDS::_version_string_to_numbers()
{
    String num = _server_version.after("/");

    istrstream iss((const char *)num);

    iss >> _server_version_major;
    char c;
    iss >> c;			// This reads the `.' in the version string
    iss >> _server_version_minor;
}

// public

DataDDS::DataDDS(const String &n = (char *)0, const String &v = (char *)0)
    :DDS(n), _server_version(v)
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
