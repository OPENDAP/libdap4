
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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
 
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the InternalErr class.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: InternalErr.cc,v 1.10 2003/01/23 00:22:24 jimg Exp $"};

#include <stdio.h>

#include <string>
#include <strstream>

#include "InternalErr.h"
#include "util.h"

using std::endl;
using std::ends;
using std::ostrstream;

InternalErr::InternalErr() : Error()
{
    _error_code=internal_error;
}

InternalErr::InternalErr(const string &msg) : Error()
{
    _error_code=internal_error;
    _error_message="";
    _error_message+="An internal error was encountered:\n";
    _error_message+=msg+"\n";
    _error_message+="Please report this to support@unidata.ucar.edu\n";
}


//InternalErr::InternalErr(string msg, string file, int line)
//    : Error(unknown_error, msg)
InternalErr::InternalErr(const string &file, const int &line, const string &msg) : Error()
{
    _error_code=internal_error;
    _error_message="";
    _error_message+="An internal error was encountered in "+file+" at line ";
    // Jose Garcia. Next we append line to the string _error_code.
    // This function is defined in util.h
    append_long_to_string(line,10,_error_message);
    _error_message+=":\n";
    _error_message+=msg+"\n";
    _error_message+="Please report this to support@unidata.ucar.edu\n";
}

InternalErr::InternalErr(string msg, ProgramType pt, char *pgm)
    : Error(unknown_error, msg, pt, pgm) 
{
    ostrstream oss;
    oss << "An internal error was encountered:" << endl
	<< msg << endl
	<< "Please report this to support@unidata.ucar.edu" << ends;
    _error_message  = oss.str();
    oss.freeze(0);
}

InternalErr::InternalErr(const InternalErr &copy_from)
    : Error(copy_from)
{
}    

InternalErr::~InternalErr()
{
}

/** 
    @brief Is the InternalErr object valid?
    @return TRUE if the object is valid, FALSE otherwise. */
bool
InternalErr::OK()
{
    return Error::OK();
}

// $Log: InternalErr.cc,v $
// Revision 1.10  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.9  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.6.4.2  2002/12/13 04:53:23  jimg
// Fixed spelling of `encounterd.'
//
// Revision 1.8  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.7  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.6.4.1  2001/08/18 00:16:36  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.6  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.5  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exceptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to report all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.4  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.3  2000/06/07 19:33:21  jimg
// Merged with version 3.1.6
//
// Revision 1.2.2.1  2000/06/02 20:23:45  jimg
// Added a constructor that takes the file name and line number. This helps in
// tracking down errors.
//
// Revision 1.2.10.2  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and
// long to string
//
// Revision 1.2.10.1  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.2  1999/05/26 17:33:55  jimg
// Fixed a bad call to Error's ctor. For some reason calling the four arg ctor
// with the last arg == to 0 caused string to gag. At least that appears to have
// been the problem...
//
// Revision 1.1  1999/05/04 19:43:47  jimg
// Added
