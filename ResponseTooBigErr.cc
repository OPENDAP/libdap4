
// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the InternalErr class.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id"};

#include <string>

#include "ResponseTooBigErr.h"

ResponseTooBigErr::ResponseTooBigErr() : Error()
{
    _error_code=unknown_error;
}

ResponseTooBigErr::ResponseTooBigErr(const string &msg) : Error()
{
    _error_code=unknown_error;
    _error_message="";
    _error_message+="A caching error was encounterd:\n";
    _error_message+=msg+"\n";
}


// $Log: ResponseTooBigErr.cc,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.1  2002/10/08 05:30:28  jimg
// Added.
//
