
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the Error class
//
// jhrg 4/23/96

// $Log: Error.h,v $
// Revision 1.2  1996/06/22 00:02:50  jimg
// Added Gui pointer to the Error oject's correct_error() and
// display_message() mfuncs. These mfuncs now used the GUI to display
// messages.
//
// Revision 1.1  1996/05/31 22:39:56  jimg
// Added.
//

#ifndef _error_h
#define _error_h

#ifdef __GUNG__
#pragma interface
#endif

#include <stdio.h>
#include <String.h>

#include "Gui.h"

/// Error code: an enumerated type for common errors.
//* The most common errors within DODS have special codes so that they can be
//* spotted easily by the DODS client software. Any error without a matching
//* code gets the `unknown_error' code.

enum ErrorCode {
    undefined_error = -1,
    unknown_error,		// any error not one of the followinng
    no_such_file,
    no_such_variable,
    malformed_expr,
    no_authorization
};

/// ProgramType: an enumerated type for `correction programs'.
//* Some Error objects may contain programs which can be used to correct the
//* reported error. These programs are run using a public member function of
//* the Error class. If an Error object does not have an associated 
//* correction program, the program type is NULL.

enum ProgramType {
    undefined_prog_type = -1,
    no_program,			// if there is no program
    java,
    tcl
};

/// A class for error processing.
//* The Error class is used to transport error information from the server to
//* the client within DODS. It can also be used on the client side only.
//* Errors consist of an error code, string and function/program. The code can
//* be used to quickly distinguish between certain common errors while the
//* string is used to convey information about the error to the user. The
//* error code should never be displayed to the user. The program or function
//* can be used for error correction controlled by the user.

class Error {
private:
    ErrorCode _error_code;
    String _error_message;
    ProgramType _program_type;
    char *_program;

public:
    /// Constructors for the Error object
    //* It is not possible to create an Error object with only an error code;
    //* you must supply at a minimum a code and a message. In this case the
    //* correction program and program type will be null. In addition, if a
    //* program type is given a program *must* also be given.
    Error();
    Error(ErrorCode ec, String msg);
    Error(ErrorCode ec, String msg, ProgramType pt, char *pgm);
    Error(const Error &copy_from);
    
    ~Error();

    /// Assigment copys the char * program.
    Error &operator=(const Error &rhs);

    /// Is the Error object valid?
    //* Return true if the Error object is valid, false otherwise. An Error
    //* object is valid if it is either empty or contains either an error
    //* code and message and an optional program type and program.
    //* Returns: boolean indicating that the object is valid (true) or not
    //* (false).
    bool OK();

    /// Parse an Error object.
    //* Given an input stream (FILE *) FP, parse an Error object from that
    //* stream. Values for fields of the Error object are parsed and THIS is
    //* set accordingly.
    //* Returns: true if no error was detected, false otherwise.
    bool parse(FILE *fp);

    /// Print the Error object on the given output stream.
    //* The print representation can be  parsed by the parse() mfunc. Thus
    //* parse and print form a symetrical pair that can be used to send and
    //* receive an Error object over th network in a MIME document.
    //* Returns: void
    void print(ostream &os = cout);

    /// Get or set the error code.
    //* With no arguement, returns the Error object's error code. With an
    //* argument, sets the error code to that value.
    //* Returns: the Error object's error code.
    ErrorCode error_code(ErrorCode ec = undefined_error);
    
    /// Get or set the error message string.
    //* With no argument, return a copy of the objet's error message string.
    //* With an argument, set the object's error message to that string.
    //* Returns: a copy of the Error object's message string.
    String error_message(String msg = "");
    
    /// Display the error message in a dialog box.
    //* Either display the error message in a dialog box and offer the user
    //* a single `OK' button or print the message to standard error. If the
    //* compile-time switch `TCLTK' is defined to be nonzero  and, at
    //* run-time, the environment variable DISPLAY is set use the dialog box,
    //* otherwise use standard error.
    //* Returns: void.
    void display_message(Gui *gui);

    /// Get or set the program type.
    //* With no argument, return the program type of the error object. With
    //* an argument, set the error object's program type field.
    //* Returns: the program type of the object.
    ProgramType program_type(ProgramType pt = undefined_prog_type);

    /// Get or set the error correction program.
    //* With no argument, return the error correction program. With an
    //* argument, set the error correction program to a copy of that value.
    //* Returns: the error correction program.
    char *program(char *program = 0);

    /// Run the error correction program or print the error message.
    //* Runs the error correction program, if possible, and returns a string
    //* that can be used as the `corrected' value. If there is no error
    //* correction program or it is not possible to run the program, display
    //* the error message. If the error correction program cannot be run,
    //* return the null string.
    //* Returns: A corrected string or "".
    String correct_error(Gui *gui);
};

#endif // _error_h
