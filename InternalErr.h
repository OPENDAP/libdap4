
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the InternalErr class
//
// 5/3/99 jhrg

// $Log: InternalErr.h,v $
// Revision 1.4  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.3  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.1.6.2  2000/06/02 20:23:57  jimg
// Added a constructor that takes the file name and line number. This helps in
// tracking down errors.
//
// Revision 1.2.4.2  2000/03/08 00:09:04  jgarcia
// replace ostrstream with string;added functions to convert from double and
// long to string
//
// Revision 1.2.4.1  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.2  1999/08/23 18:57:45  jimg
// Merged changes from release 3.1.0
//
// Revision 1.1.6.1  1999/08/17 16:14:29  jimg
// Removed Gui.h header.
//
// Revision 1.1  1999/05/04 19:43:47  jimg
// Added
//

#ifndef _internal_error_h
#define _internal_error_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>		// For FILE *

#include <string>

#include "Error.h"

/** The InternalErr class is used to signal that somewhere inside DODS a
    software fault was found. This class wraps the message text in some
    boiler plate that asks the error be reported to us (DODS support).

    NB: This class Adds some text to the message and might, in the future,
    hold information not also held in Error. However, all Error objects
    thrown on the server-side of DODS that cannot be resolved (and that's all
    of them for now, 5/3/99 jhrg) will be sent to the client-side \emph{using
    Error ojbects}. That is, the information recorded in an InternalErr
    object will be sent by calling the #print(...)# mfunc of Error.

    @memo A class for software fault reporting.
    @author jhrg */

class InternalErr: public Error {

public:
    /** These constructors always set the error code to #internal_error#.
	
	Note that there is no way to specify an error correction program for
	an internal error.

	@memo Constructors for the Error object
	@name Constructors */
    //@{
    /// 
    InternalErr(const string &msg);
  // I think Jose added this, but I'm not sure.
  // InternalErr(string msg, string file, int line);
    ///
    InternalErr(const string &file, const int &line, const string &msg);
    ///
    InternalErr(string msg, ProgramType pt, char *pgm);
    ///
    InternalErr();
    ///
    InternalErr(const InternalErr &copy_from);
    //@}

    virtual ~InternalErr();

    /** 
	@memo Is the InternalErr object valid?
	@return TRUE if the object is valid, FALSE otherwise. */
    bool OK();
};

#endif // _internal_error_h
