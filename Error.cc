
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the Error class.

// $Log: Error.cc,v $
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

static char rcsid[] __unused__ = {"$Id: Error.cc,v 1.6 1996/08/13 18:14:28 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include "Error.h"
#include "parser.h"

void Errorrestart(FILE *yyin);	// defined in Error.tab.c
int Errorparse(void *arg);	

Error::Error()
    : _error_code(undefined_error), _error_message(""), 
      _program_type(undefined_prog_type), _program(0)
{
}

Error::Error(ErrorCode ec, String msg)
    : _error_code(ec), _error_message(msg), 
      _program_type(undefined_prog_type), _program(0)
{
}

Error::Error(ErrorCode ec, String msg, ProgramType pt, char *pgm)
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
    _program = new char[strlen(copy_from._program) + 1];
    strcpy(_program, copy_from._program);
}    

Error::~Error()
{
    delete _program;
}

Error &
Error::operator=(const Error &rhs)
{
    if (&rhs == this)		// are they identical?
	return *this;
    else {
	_error_code = rhs._error_code;
	_error_message = rhs._error_message;
	_program_type = rhs._program_type;

	_program = new char[strlen(rhs._program) + 1];
	strcpy(_program, rhs._program);

	return *this;
    }
}

// To be a valid, an Error object must either be: 1) empty, 2) contain a
// message and a program or 3) contain only a message. Since the program is
// optional, there is no need to test for it here. 
//
// NB: This mfunc does not test for malformed messages or programs - ones
// where the code is defined but not the `data'.

bool
Error::OK()
{
    bool empty = ((_error_code == undefined_error) 
		  && (_error_message == "")
		  && (_program_type == undefined_prog_type) 
		  && (_program == 0));

    bool message = ((_error_code != undefined_error) 
		    && (_error_message != ""));

    // bool program = ((_program_type != undefined_prog_type) 
    //                 && (_program != 0))

    return empty || message;
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
Error::print(ostream &os = cout)
{
    if (!OK()) {
	cerr << "Bad Error object" << endl;
	return;
    }

    os << "Error {" << endl;

    if (_error_code != undefined_error) {
	os << "    " << "code = " << _error_code << ";" << endl;
	os << "    " << "message = " << _error_message << ";" << endl;

	if (_program_type != undefined_prog_type) {
	    os << "    " << "program_type = " << _program_type << ";" << endl;
	    os << "    " << "program = " << _program << ";" << endl;
	}
    }    

    os << "};" << endl;
}

ErrorCode
Error::error_code(ErrorCode ec = undefined_error)
{
    if (ec == undefined_error)
	return _error_code;
    else {
	_error_code = ec;
	return _error_code;
    }
}

String
Error::error_message(String msg = "")
{
    if (msg == "")
	return String(_error_message);
    else {
	_error_message = msg;
	return String (_error_message);
    }
}

void
Error::display_message(Gui *gui = 0)
{
    if (gui && gui->show_gui()) {
	String cmd = (String)"dialog " + _error_message + "\r";
	gui->command(cmd);
    }
    else
	cerr << _error_message << endl;
}

ProgramType
Error::program_type(ProgramType pt = undefined_prog_type)
{
    if (pt == undefined_prog_type)
	return _program_type;
    else {
	_program_type = pt;
	return _program_type;
    }
}

char *
Error::program(char *pgm = 0)
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

String
Error::correct_error(Gui *gui)
{
    if (!OK())
	return String("");

    if (program()) {
	String result;

	if (gui->response(program(), result))
	    return result;
	else
	    return String("");
    }
    else {
	display_message(gui);
	return String("");
    }
}
