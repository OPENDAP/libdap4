
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the Error class.

// $Log: Error.cc,v $
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: Error.cc,v 1.17 1999/05/26 17:32:01 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include "Error.h"
#include "parser.h"

void Errorrestart(FILE *yyin);	// defined in Error.tab.c
int Errorparse(void *arg);	

static char *messages[]={"Unknown error", "No such file", 
			 "No such variable", "Malformed expression",
			 "No authorization", "Cannot read file"};

Error::Error()
    : _error_code(undefined_error), _error_message(""), 
      _program_type(undefined_prog_type), _program(0)
{
}

Error::Error(ErrorCode ec, string msg)
    : _error_code(ec), _error_message(msg), 
      _program_type(undefined_prog_type), _program(0)
{
}

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
    delete _program;
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

	_program = new char[strlen(rhs._program) + 1];
	strcpy(_program, rhs._program);

	assert(this->OK());

	return *this;
    }
}

// To be a valid, an Error object must either be: 1) empty, 2) contain a
// message and a program or 3) contain only a message. 
//
// NB: This mfunc does not test for malformed messages or programs - ones
// where the code is defined but not the `data'.

bool
Error::OK()
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

bool
Error::parse(FILE *fp)
{
    if (!fp) {
	cerr << "Error::parse: Null input stream" << endl;
	return false;
    }

    Errorrestart(fp);

    parser_arg arg(this);

    bool status = Errorparse((void *)&arg) == 0;

    fclose(fp);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	cerr << "Error parsing error object!" << endl;
	return false;
    }
    else
	return OK();		// Check object consistancy
}
    
void
Error::print(ostream &os)
{
    assert(OK());

    os << "Error {" << endl;

    os << "    " << "code = " << _error_code << ";" << endl;
    
    // If the error message is wrapped in double quotes, print it, else, add
    // wrapping double quotes.
    if (*_error_message.begin() == '"' && *(_error_message.end()-1) == '"')
	os << "    " << "message = " << _error_message << ";" << endl;
    else
	os << "    " << "message = " << "\"" << _error_message << "\"" << ";" 
	   << endl;

    if (_program_type != undefined_prog_type) {
	os << "    " << "program_type = " << _program_type << ";" << endl;
	os << "    " << "program = " << _program << ";" << endl;
    }

    os << "};" << endl;
}

ErrorCode
Error::error_code(ErrorCode ec)
{
    assert(OK());
    if (ec == undefined_error)
	return _error_code;
    else {
	_error_code = ec;
	if (_error_message == "")
	    _error_message = messages[ec];
	assert(OK());
	return _error_code;
    }
}

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

void
Error::display_message(Gui *gui)
{
    assert(OK());
    if (gui && gui->show_gui()) {
	string cmd = (string)"dialog " + _error_message + "\r";
	string response;	// not used
	// You must use response() with dialog so that expect will wait for
	// the user to hit OK.
	gui->response(cmd, response);
    }
    else
	cerr << _error_message << endl;
}

// There ought to be check of object state after _program_type is set. jhrg
// 2/26/97

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

// Assuming the object is OK, if the Error object has only a meesage, dispay
// it in a dialog box. Once the user dismisses the dialog, return the null
// string. However, if program() is true, then source the code it returns in
// the Gui object and return the value of Gui::command(). Note that this
// means that the code in the program must run itself (i.e., in addition to
// any procedure definitions, etc. it must also contain the necessary
// instructions to popup an initial window).
//
// I added a check for non-null Gui pointer. If this mfunc is called with a
// null Gui pointer then the message text is displayed on stderr.

string
Error::correct_error(Gui *gui)
{
    assert(OK());
    if (!OK())
	return string("");

    if (gui && program()) {
	string result;

	if (gui && gui->response(program(), result))
	    return result;
	else
	    return string("");
    }
    else {
	display_message(gui);
	return string("");
    }
}
