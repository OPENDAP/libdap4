
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the InternalErr class.

// $Log: InternalErr.cc,v $
// Revision 1.3  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.2.2.1  2000/06/02 20:23:45  jimg
// Added a constructor that takes the file name and line number. This helps in
// tracking down errors.
//
// Revision 1.2  1999/05/26 17:33:55  jimg
// Fixed a bad call to Error's ctor. For some reason calling the four arg ctor
// with the last arg == to 0 caused string to gag. At least that appears to have
// been the problem...
//
// Revision 1.1  1999/05/04 19:43:47  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: InternalErr.cc,v 1.3 2000/06/07 19:33:21 jimg Exp $"};

#include <stdio.h>

#include <string>
#include <strstream>

#include "InternalErr.h"

InternalErr::InternalErr() : Error()
{
}

InternalErr::InternalErr(string msg)
    : Error(unknown_error, msg)
{
    ostrstream oss;
    oss << "An internal error was encounterd:" << endl
	<< msg << endl
	<< "Please report this to support@unidata.ucar.edu" << ends;
    _error_message  = oss.str();
    oss.freeze(0);
}

InternalErr::InternalErr(string msg, string file, int line)
    : Error(unknown_error, msg)
{
    ostrstream oss;
    oss << "An internal error was encounterd:" << endl
	<< msg << " at " << file << ":" << line << endl
	<< "Please report this to support@unidata.ucar.edu" << ends;
    _error_message  = oss.str();
    oss.freeze(0);
}

InternalErr::InternalErr(string msg, ProgramType pt, char *pgm)
    : Error(unknown_error, msg, pt, pgm) 
{
    ostrstream oss;
    oss << "An internal error was encounterd:" << endl
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

bool
InternalErr::OK()
{
    return Error::OK();
}
