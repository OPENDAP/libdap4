
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
 
// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for the Error class
//
// jhrg 4/23/96

#ifndef _error_h
#define _error_h

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include <stdio.h>		// For FILE *

#include <iostream>
#include <string>

using std::cout;
using std::string;
using std::ostream;

/** The most common errors within DODS have special codes so that they
    can be spotted easily by the DODS client software. Any error
    without a matching code gets the <tt>unknown_error</tt> code.

    <pre>
    enum ErrorCode {
       undefined_error = -1,
       unknown_error,
       internal_error,
       no_such_file,
       no_such_variable,
       malformed_expr,
       no_authorization, 
       can_not_read_file
    };
    </pre>

    @brief An enumerated type for common errors.  */
/*
enum ErrorCode {
    undefined_error = -1,
    unknown_error,		// any error not one of the followinng
    internal_error,		// something inside is broken
    no_such_file,
    no_such_variable,
    malformed_expr,
    no_authorization,
    can_not_read_file,
    cannot_read_file
    }; 
*/ 

typedef int ErrorCode; //using standard errno+netCDF error codes from server
// Internal DAP errors
#define    undefined_error   1000
#define    unknown_error     1001	
#define    internal_error    1002	      
#define    no_such_file      1003
#define    no_such_variable  1004
#define    malformed_expr    1005
#define    no_authorization  1006
#define    can_not_read_file 1007
#define    cannot_read_file  1008

/** Some Error objects may contain programs which can be used to
    correct the reported error. These programs are run using a public
    member function of the Error class. If an Error object does not
    have an associated correction program, the program type is NULL.

    <pre>
    enum ProgramType {
       undefined_prog_type = -1,
       no_program, 
       java,
       tcl
    };
    </pre>

    Note that as of DODS Core version 2.15, only the tcl
    implementation is functional.  You can include a Java program, but
    it will not work properly. 

    @brief An enumerated type for `correction programs'.*/

enum ProgramType {
    undefined_prog_type = -1,
    no_program,			// if there is no program
    java,
    tcl
};

/** The Error class is used to transport error information from the
    server to the client within DODS. It can also be used on the
    client side only.  Errors consist of an error code, string and
    optionally a function/program. The code can be used to quickly
    distinguish between certain common errors while the string is used
    to convey information about the error to the user. The error code
    should never be displayed to the user. The program or function can
    be used for error correction controlled by the user.

    This class is used on both clients and servers.  The <tt>print()</tt> and
    <tt>parse()</tt> methods are used to send the object back and forth.

    @brief A class for error processing.
    @author jhrg */

class Error {
protected:
    ErrorCode _error_code;
    string _error_message;
    ProgramType _program_type;
    char *_program;

public:
    /** It is not possible to create an Error object with only an error code;
	you must supply at a minimum a code and a message. In this case the
	correction program and program type will be null. In addition, if a
	program type is given a program <i>must</i> also be given. 

	Other class constructors should be the only callers of this object's
	default constructor.
 
	@brief Constructors for the Error object
	@name Constructors */
    //@{
    Error(ErrorCode ec, string msg);
    Error(string msg);
    Error(ErrorCode ec, string msg, ProgramType pt, char *pgm);
    Error();
    //@}

    Error(const Error &copy_from);
    virtual ~Error();

  Error &operator=(const Error &rhs);
  bool OK() const;

  bool parse(FILE *fp);

  void print(ostream &os = cout) const;

    void print(FILE *out) const;

  ErrorCode get_error_code() const;
  
  string get_error_message() const;
    
  ProgramType get_program_type() const;

  const char *get_program() const;

  void set_error_code(ErrorCode ec = undefined_error);
    
  void set_error_message(string msg = "");
    
  void set_program_type(ProgramType pt = undefined_prog_type);

  void set_program(char *program);

  void display_message(void *gui = 0) const;

  string correct_error(void *gui) const;

  /** @name Deprecated methods
      These methods should not be used because combining the accessors and
      mutators makes using const objects almost impossible for clients of
      this class. */
  //@{
  string error_message(string msg = "");
    
  ProgramType program_type(ProgramType pt = undefined_prog_type);

  char *program(char *program = 0);

  ErrorCode error_code(ErrorCode ec = undefined_error);
  //@}
};

// $Log: Error.h,v $
// Revision 1.28  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.27  2003/09/30 22:06:54  jimg
// Changes from the release-3-4 branch were small (and there was no change
// here); I copied the file since we're not yet ready to merge the 3.4
// branch but need the new file to work on translation (over in nc3-dods).
//
// Revision 1.26.2.2  2003/06/23 11:49:18  rmorris
// The #pragma interface directive to GCC makes the dynamic typing functionality
// go completely haywire under OS X on the PowerPC.  We can't use that directive
// on that platform and it was ifdef'd out for that case.
//
// Revision 1.26.2.1  2003/06/06 08:28:28  reza
// Error code changes in the error object.
//
// Revision 1.26  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.25  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.24.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.24  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.23  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.20.2.3  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.22  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.20.2.2  2002/05/26 23:36:09  jimg
// Removed code specific to the GUI-based progress indicator.
//
// Revision 1.21  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.20.2.1  2001/08/18 00:17:52  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.20  2000/10/30 17:21:27  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.19  2000/10/03 21:04:08  jimg
// Updated copyright
//
// Revision 1.18  2000/10/02 18:49:26  jimg
// The Error class now has const accessors
//
// Revision 1.17  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.16  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.15  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.14  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.13.4.1  2000/06/02 18:21:26  rmorris
// Mod's for port to Win32.
//
// Revision 1.13  2000/03/28 16:32:02  jimg
// Modified these files so that they can be built either with and without GUI
// defined. The type signatures are now the same either way. Thus we can
// build libdap++-gui and libdap++ (without GUI support). When using the
// later there's no need to link with tcl, tk or X11. This makes the
// executables smaller. It also keeps the servers from potentially needing
// sharable libraries (since X11 is often sharable) which can be hard to find
// unless they are in the standard places. I made the same changes in Connect
// and Gui.
//
// Revision 1.12.2.1  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.12  1999/12/31 00:55:10  jimg
// Fixed up the progress indicator
//
// Revision 1.11  1999/08/23 18:57:44  jimg
// Merged changes from release 3.1.0
//
// Revision 1.10.6.1  1999/08/09 22:57:50  jimg
// Removed GUI code; reactivate by defining GUI
//
// Revision 1.10  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.9  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.8  1999/01/21 02:55:54  jimg
// Added include of stdio.h for FILE *.
//
// Revision 1.7.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.7  1998/01/14 22:55:03  tom
// First draft of doc++ class documentation.
//
// Revision 1.6  1997/03/05 08:16:25  jimg
// Added Cannot read file message to list of builtin messages.
//
// Revision 1.5  1997/02/18 21:26:52  jimg
// Moved the default ctor back into the public section...
//
// Revision 1.4  1997/02/15 07:11:47  jimg
// Changed comments for doc++.
// Moved default ctor into the private part of the object.
//
// Revision 1.3  1996/08/13 18:17:02  jimg
// Removed system includes.
// Added documentation on new interfaces for display_message() and
// correct_error().
//
// Revision 1.2  1996/06/22 00:02:50  jimg
// Added Gui pointer to the Error oject's correct_error() and
// display_message() mfuncs. These mfuncs now used the GUI to display
// messages.
//
// Revision 1.1  1996/05/31 22:39:56  jimg
// Added.
//

#endif // _error_h
