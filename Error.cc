
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the Error class.

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Error.cc,v 1.35 2004/02/19 19:42:52 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include "Error.h"
#include "parser.h"
#include "InternalErr.h"

using namespace std;

// Glue routines declared in Error.lex
extern void Error_switch_to_buffer(void *new_buffer);
extern void Error_delete_buffer(void * buffer);
extern void *Error_buffer(FILE *fp);

extern void Errorrestart(FILE *yyin);	// defined in Error.tab.c
extern int Errorparse(void *arg);	

static const char *err_messages[]={"Unknown error", "No such file", 
				   "No such variable", "Malformed expression",
				   "No authorization", "Cannot read file"};

/** Specializations of Error should use this to set the error code and
    message. */
Error::Error()
    : _error_code(undefined_error), _error_message(""), 
      _program_type(undefined_prog_type), _program(0)
{
}

/** Create an instance with a specific code and message string. This ctor
    provides a way to to use any code and string you'd like. The code can be
    one of the standard codes or it may be specific to your server. Thus a
    client which can tell it's dealing with a specific type of server can use
    the code accordingly. In general, clients simply show the error message
    to users or write it to a log file.

    @param ec The error code
    @param msg The error message string. */
Error::Error(ErrorCode ec, string msg)
    : _error_code(ec), _error_message(msg), 
      _program_type(undefined_prog_type), _program(0)
{
}

/** Create an instance with a specific message. The error code is set to \c
    unknown_error.
    
    @param msg The error message. 
    @see ErrorCode */
Error::Error(string msg)
    : _error_code(unknown_error), _error_message(msg), 
      _program_type(undefined_prog_type), _program(0)
{
}

/** @deprecated The error-correction program feature is deprecated. */
Error::Error(ErrorCode ec, string msg, ProgramType pt, char *pgm)
    : _error_code(ec), _error_message(msg), 
      _program_type(pt), _program(0)
{
    _program = new char[strlen(pgm) + 1];
    strcpy(_program, pgm);
}

Error::Error(const Error &copy_from)
    : _error_code(copy_from._error_code),
      _error_message(copy_from._error_message),
      _program_type(copy_from._program_type), _program(0)
{
    if (copy_from._program) {
	_program = new char[strlen(copy_from._program) + 1];
	strcpy(_program, copy_from._program);
    }
}    

Error::~Error()
{
    delete _program; _program = 0;
}

Error &
Error::operator=(const Error &rhs)
{
    assert(OK());

    if (&rhs == this)		// are they identical?
	return *this;
    else {
	_error_code = rhs._error_code;
	_error_message = rhs._error_message;
	_program_type = rhs._program_type;

	delete[] _program; _program = 0;
	if (rhs._program) {
	    _program = new char[strlen(rhs._program) + 1];
	    strcpy(_program, rhs._program);
	}

	assert(this->OK());

	return *this;
    }
}

/** Use this function to determine whether an Error object is
    valid.  To be a valid, an Error object must either be: 1)
    empty or contain a message and a code.

    @brief Is the Error object valid?
    @return TRUE if the object is valid, FALSE otherwise. */
bool
Error::OK() const
{
    // The object is empty - users cannot make these, but this class can!
    bool empty = ((_error_code == undefined_error) 
		  && (_error_message == "")
		  && (_program_type == undefined_prog_type) 
		  && (_program == 0));

    // Just a message - the program part is null.
    bool message = ((_error_code != undefined_error) 
		    && (_error_message != "")
		    && (_program_type == undefined_prog_type) 
		    && (_program == 0));

    // Message and program parts are in working order.
    bool program = ((_error_code != undefined_error) 
		    && (_error_message != "")
		    && (_program_type != undefined_prog_type) 
		    && (_program != 0));

    return empty || message || program;
}

/** Given an input stream (FILE *) <tt>fp</tt>, parse an Error object from
    stream. Values for fields of the Error object are parsed and \c this is
    set accordingly. This is how a client program receives an error object
    from a server.
    
    @brief Parse an Error object.
    @param fp A valid file pointer to an input stream.
    @return TRUE if no error was detected, FALSE otherwise.  */
bool
Error::parse(FILE *fp)
{
    if (!fp)
	throw InternalErr(__FILE__, __LINE__, "Null input stream"); 

    void *buffer = Error_buffer(fp);
    Error_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status;
    try {
	status = Errorparse((void *)&arg) == 0;
	Error_delete_buffer(buffer);
    }
    catch (Error &e) {
	throw InternalErr(__FILE__, __LINE__, e.error_message());
	Error_delete_buffer(buffer);
    }

    // STATUS is the result of the parser function; if a recoverable error
    // was found it will be true but arg.status() will be false.
    // I'm throwing an InternalErr here since Error objects are generated by
    // the core; they should always parse! 9/21/2000 jhrg
    if (!status || !arg.status())
	throw InternalErr(__FILE__, __LINE__, "Error parsing error object!");
    else
	return OK();		// Check object consistency
}
    
/** @deprecated Use the FILE * version instead. */
void
Error::print(ostream &os) const
{
    assert(OK());

    os << "Error {" << endl;

    os << "    " << "code = " << static_cast<int>(_error_code) << ";" << endl;
    
    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end()-1) == '"')
	os << "    " << "message = " << _error_message << ";" << endl;
    else
	os << "    " << "message = " << "\"" << _error_message << "\"" << ";" 
	   << endl;

    if (_program_type != undefined_prog_type) {
	os << "    " << "program_type = " << static_cast<int>(_program_type)
	   << ";" << endl;
	os << "    " << "program = " << _program << ";" << endl;
    }

    os << "};" << endl;
}

/** Creates a printable representation of the Error object. It is suitable
    for framing, and also for printing and sending over a network.

    The printed representation produced by this function can be parsed by the
    parse() member function. Thus parse and print form a symmetrical pair
    that can be used to send and receive an Error object over the network in
    a MIME document.
    
    @param out A pointer to the output stream on which the Error object is to
    be rendered. */
void
Error::print(FILE *out) const
{
    assert(OK());

    fprintf( out, "Error {\n" ) ;

    fprintf( out, "    code = %d;\n", static_cast<int>(_error_code) ) ;
    
    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end()-1) == '"')
	fprintf( out, "    message = %s;\n", _error_message.c_str() ) ;
    else
	fprintf( out, "    message = \"%s\";\n", _error_message.c_str() ) ;

    if (_program_type != undefined_prog_type) {
	fprintf( out, "    program_type = %d;\n",
		 static_cast<int>(_program_type) ) ;
	fprintf( out, "    program = %s;\n", _program ) ;
    }

    fprintf( out, "};\n" ) ;
}

/** @deprecated Use the set/get methods instead. */
ErrorCode
Error::error_code(ErrorCode ec)
{
    assert(OK());
    if (ec == undefined_error)
	return _error_code;
    else {
	_error_code = ec;
	// Added check to make sure that messages is not accessed beyond its
	// bounds. 02/02/04 jhrg
	if (_error_message == "" 
	    && ec > undefined_error && ec <= cannot_read_file)
	    _error_message = err_messages[ec - undefined_error - 1];

	assert(OK());
	return _error_code;
    }
}

/** Get the ErrorCode for this instance. */
ErrorCode
Error::get_error_code() const
{
    assert(OK());
    return _error_code;
}

/** Set the ErrorCode. If the current error message has not been set, use \e
    ec to set the error message. The resulting error message string is the
    same as the ErrorCode name. If \e ec is not within the range of values
    for an OPeNDAP ErrorCode, the error message is left unchanged.

    @param ec The new ErrorCode value. */
void
Error::set_error_code(ErrorCode ec)
{
    _error_code = ec;
    // Added check to make sure that messages is not accessed beyond its
    // bounds. 02/02/04 jhrg
    if (_error_message == ""
	&& ec > undefined_error && ec <= cannot_read_file)
	_error_message = err_messages[ec - undefined_error - 1];
    assert(OK());
}

/** @deprecated Use the set/get methods instead. */
string
Error::error_message(string msg)
{
    assert(OK());
    if (msg == "")
	return string(_error_message);
    else {
	_error_message = msg;
	assert(OK());
	return string (_error_message);
    }
}

/** Return the current error message. */
string
Error::get_error_message() const
{
    assert(OK());

    return string(_error_message);
}

/** Set the error message. */
void
Error::set_error_message(string msg)
{
    _error_message = msg;
    assert(OK());
}

/** @deprecated Use get_error_message() instead. */
void
Error::display_message(void *) const
{
    assert(OK());
    cerr << _error_message << endl;
}

/** @deprecated The error-correction program feature is deprecated. */
ProgramType
Error::program_type(ProgramType pt)
{
    assert(OK());
    if (pt == undefined_prog_type)
	return _program_type;
    else {
	_program_type = pt;
	return _program_type;
    }
}

/** @deprecated The error-correction program feature is deprecated. */
ProgramType
Error::get_program_type() const
{
    assert(OK());

    return _program_type;
}

/** @deprecated The error-correction program feature is deprecated. */
void
Error::set_program_type(ProgramType pt)
{
    _program_type = pt;
}

/** @deprecated The error-correction program feature is deprecated. */
char *
Error::program(char *pgm)
{
    if (pgm == 0)
	return _program;
    else {
	_program = new char[strlen(pgm) + 1];
	strcpy(_program, pgm);
	return _program;
    }
}

/** @deprecated The error-correction program feature is deprecated. */
const char *
Error::get_program() const
{
    return _program;
}

/** @deprecated The error-correction program feature is deprecated. */
void
Error::set_program(char *pgm)
{
    _program = new char[strlen(pgm) + 1];
    strcpy(_program, pgm);
}

/** @deprecated The error-correction program feature is deprecated. */
string
Error::correct_error(void *) const
{
    assert(OK());
    if (!OK())
	return string("");

    display_message(NULL);
    return string("");
}

// $Log: Error.cc,v $
// Revision 1.35  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.32.2.3  2004/02/04 00:05:11  jimg
// Memory errors: I've fixed a number of memory errors (leaks, references)
// found using valgrind. Many remain. I need to come up with a systematic
// way of running the tests under valgrind.
//
// Revision 1.32.2.2  2004/02/03 00:18:08  jimg
// Fixed bug 691. The array messages was improperly accessed. I also revamped
// the doxygen comments and re-indented the code.
//
// Revision 1.34  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.32.2.1  2003/09/06 22:37:50  jimg
// Updated the documentation.
//
// Revision 1.33  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.32  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.31  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.30.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.30  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.29  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.24.2.8  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.24.2.7  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verified (and
// which looked dubious).
//
// Revision 1.28  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.24.2.6  2002/05/26 23:36:09  jimg
// Removed code specific to the GUI-based progress indicator.
//
// Revision 1.27  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.24.2.5  2001/10/08 17:14:22  jimg
// Fixed a bug where an empty _program field was copied in operator=. This
// caused seg faults because the copy used strlen on _program; when it was null
// that meant dereferencing the null pointer.
//
// Revision 1.26  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.24.2.4  2001/08/18 00:18:07  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.24.2.3  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.25  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.24.2.2  2001/05/03 21:42:59  jimg
// Fixed a bug in parse(...) where the FILE * passed to the method was closed.
// This FILE * should be closed by the caller. When used with a Connect object,
// Connect's dtor tries to close the FILE *. If parse(...) has already closed
// it...boom.
//
// Revision 1.24.2.1  2001/04/23 22:18:15  jimg
// Added two static casts to Error::print(). This fixes a warning from g++.
//
// Revision 1.24  2000/10/30 17:21:27  jimg
// Added support for proxy servers (from cjm).
//
// Revision 1.23  2000/10/02 18:49:26  jimg
// The Error class now has const accessors
//
// Revision 1.22  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.21  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.20  2000/03/28 16:32:02  jimg
// Modified these files so that they can be built either with and without GUI
// defined. The type signatures are now the same either way. Thus we can build
// libdap++-gui and libdap++ (without GUI support). When using the later
// there's no need to link with tcl, tk or X11. This makes the executables
// smaller. It also keeps the servers from potentially needing sharable
// libraries (since X11 is often sharable) which can be hard to find unless
// they are in the standard places. I made the same changes in Connect and Gui.
//
// Revision 1.19  1999/08/23 18:57:44  jimg
// Merged changes from release 3.1.0
//
// Revision 1.18.2.1  1999/08/09 22:57:50  jimg
// Removed GUI code; reactivate by defining GUI
//
// Revision 1.18  1999/08/09 18:27:34  jimg
// Merged changes from Brent for the Gui code (progress indicator)
//
// Revision 1.17.4.1  1999/07/29 05:46:17  brent
// call Tcl / GUI directly from Gui.cc, abandon expect, and consolidate Tcl files
//
// Revision 1.17  1999/05/26 17:32:01  jimg
// Added a message for the `unknown_error' constant.
// Added a test in correct_error for a NULL Gui object. If the Gui object is
// null, display the message text on stderr and ignore the Gui object.
//
// Revision 1.16  1999/05/04 19:47:21  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.15  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.14.6.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.14.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.14  1998/03/20 00:18:55  jimg
// Fixed a bug where _program was feed into strlen even when it is NULL.
//
// Revision 1.13  1998/02/05 20:13:53  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.12  1997/08/23 00:22:23  jimg
// Changed the way that the _error_message member is processed. Now if the
// message does not have explicit double quotes, print() will add them. The
// mfunc is smart enough to add the quotes only if needed so old code (which
// provides the quotes) will still work and new code without the quotes works
// too. This makes for a more convenient use of the Error object.
//
// Revision 1.11  1997/03/05 08:15:51  jimg
// Added Cannot read file message to list of builtin messages.
//
// Revision 1.10  1997/03/05 06:53:46  jimg
// Changed display_message member function so that it uses Gui::response()
// instead of Gui::command(). The later only works for things like the progress
// popup for which expect does not need to wait. However, for things like
// dialogs, expect must wait for the user to `hit OK', hence the use of the
// response() member function.
//
// Revision 1.9  1997/02/27 01:06:47  jimg
// Fixed problem with consistency check in Error::error_code().
//
// Revision 1.8  1997/02/18 21:22:18  jimg
// Allow empty Error objects.
//
// Revision 1.7  1997/02/15 07:10:57  jimg
// Changed OK() so that empty errors return false.
// Added assert calls.
//
// Revision 1.6  1996/08/13 18:14:28  jimg
// Switched to the parser_arg object for passing parameters to/from the Error.y
// parser. NB: if an error object is bad a message is sent to stderr to avoid
// going round and round with bad error objects!
// Changed the interface to display_message; Gui is by default NULL so that
// calling it with an empty parameter list causes the message string to be sent
// to stderr.
// Changed the interface to correct_error(); it now returns a string which is
// the corrected error or "".
//
// Revision 1.5  1996/06/22 00:02:46  jimg
// Added Gui pointer to the Error oject's correct_error() and
// display_message() mfuncs. These mfuncs now used the GUI to display
// messages.
//
// Revision 1.4  1996/06/04 21:33:22  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.3  1996/06/03 06:26:51  jimg
// Added declarations for Errorparse() and Errorrestart().
//
// Revision 1.2  1996/06/01 00:03:38  jimg
// Added.
//

