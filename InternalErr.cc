
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the InternalErr class.

// $Log: InternalErr.cc,v $
// Revision 1.1  1999/05/04 19:43:47  jimg
// Added
//

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: InternalErr.cc,v 1.1 1999/05/04 19:43:47 jimg Exp $"};

#include <stdio.h>

#include <string>
#include <strstream>

#include "InternalErr.h"

InternalErr::InternalErr() : Error()
{
}

InternalErr::InternalErr(string msg)
    : Error(unknown_error, "", undefined_prog_type, 0) 
{
    ostrstream oss;
    oss << "An internal error was encounterd:" << endl
	<< msg << endl
	<< "Please report this to support@unidata.ucar.edu" << ends;
    _error_message  = oss.str();
    oss.freeze(0);
}

InternalErr::InternalErr(string msg, ProgramType pt, char *pgm)
    : Error(unknown_error, "", pt, pgm) 
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
