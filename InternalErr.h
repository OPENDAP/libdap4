
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
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

// Interface for the InternalErr class
//
// 5/3/99 jhrg

#ifndef _internalerr_h
#define _internalerr_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>		// For FILE *

#include <string>

#ifndef _error_h
#include "Error.h"
#endif


/** The InternalErr class is used to signal that somewhere inside DODS a
    software fault was found. This class wraps the message text in some
    boiler plate that asks the error be reported to us (DODS support).

    NB: This class Adds some text to the message and might, in the future,
    hold information not also held in Error. However, all Error objects
    thrown on the server-side of DODS that cannot be resolved (and that's all
    of them for now, 5/3/99 jhrg) will be sent to the client-side <i>using
    Error objects</i>. That is, the information recorded in an InternalErr
    object will be sent by calling the <tt>print(...)</tt> mfunc of Error.

    @brief A class for software fault reporting.
    @author jhrg */

class InternalErr: public Error {

public:
    /** These constructors always set the error code to <tt>internal_error</tt>.
	
	Note that there is no way to specify an error correction program for
	an internal error.

	@brief Constructors for the Error object
	@name Constructors */
    //@{
    /// 
    InternalErr(const string &msg);
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

  bool OK();
};

// $Log: InternalErr.h,v $
// Revision 1.10  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.9  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.8.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.8  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.7  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.6  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.5  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
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

#endif // _internalerr_h
