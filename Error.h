
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the Error class
//
// jhrg 4/23/96

// $Log: Error.h,v $
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

#ifndef _error_h
#define _error_h

#ifdef __GUNG__
#pragma interface
#endif

#include "Gui.h"


/** The most common errors within DODS have special codes so that they
    can be spotted easily by the DODS client software. Any error
    without a matching code gets the #unknown_error# code.

    \begin{verbatim}
    enum ErrorCode {
       undefined_error = -1,
       unknown_error,
       no_such_file,
       no_such_variable,
       malformed_expr,
       no_authorization,
       can_not_read_file
    };
    \end{verbatim}

    @memo An enumerated type for common errors.  */
enum ErrorCode {
    undefined_error = -1,
    unknown_error,		// any error not one of the followinng
    no_such_file,
    no_such_variable,
    malformed_expr,
    no_authorization,
    can_not_read_file
};

/** Some Error objects may contain programs which can be used to
    correct the reported error. These programs are run using a public
    member function of the Error class. If an Error object does not
    have an associated correction program, the program type is NULL.

    \begin{verbatim}
    enum ProgramType {
       undefined_prog_type = -1,
       no_program, 
       java,
       tcl
    };
    \end{verbatim}

    Note that as of DODS Core version 2.15, only the tcl
    implementation is functional.  You can include a Java program, but
    it will not work properly. 

    @memo An enumerated type for `correction programs'.*/

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

    This class is used on both clients and servers.  The #print()# and
    #parse()# methods are used to send the object back and forth.

    @memo A class for error processing.
    @author jhrg */

class Error {
private:
    ErrorCode _error_code;
    String _error_message;
    ProgramType _program_type;
    char *_program;

public:
  /** It is not possible to create an Error object with only an error code;
      you must supply at a minimum a code and a message. In this case the
      correction program and program type will be null. In addition, if a
      program type is given a program {\it must} also be given. 

      Other class constructors should be the only callers of this object's
      default constructor.
 
      @memo Constructors for the Error object
      @name Constructors
      */
  //@{
  ///
    Error(ErrorCode ec, String msg);
  ///
    Error(ErrorCode ec, String msg, ProgramType pt, char *pgm);
  ///
    Error();
  //@}

  /** Copy constructor for Error class. */
    Error(const Error &copy_from);
    
    ~Error();

  /** The assignment operator copies the error correction. */
    Error &operator=(const Error &rhs);

  /** Use this function to determine whether an Error object is
      valid. An Error object is valid if it contains either an error
      code and message and an optional program type and program.
	
      @memo Is the Error object valid?
      @return TRUE if the object is valid, FALSE otherwise.
      */
    bool OK();

  /** Given an input stream (FILE *) #fp#, parse an Error object from
      stream. Values for fields of the Error object are parsed and
      THIS is set accordingly.  This is how a DODS client might
      receive an error object from a server.
    
      @memo Parse an Error object.
      @param fp A valid file pointer to an input stream.
      @return TRUE if no error was detected, FALSE otherwise.  */
    bool parse(FILE *fp);

  /** Creates a printable representation of the Error object.  It is
      suitable for framing, and also for printing and sending over a
      network. 

      The printed representation produced by this function can be
      parsed by the parse() memeber function. Thus parse and print
      form a symetrical pair that can be used to send and receive an
      Error object over the network in a MIME document.
    
      @memo Print the Error object on the given output stream.
      @param os A pointer to the output stream on which the Error
      object is to be rendered.  
      */
    void print(ostream &os = cout);

  /** With no argument, returns the Error object's error code. With an
      argument, sets the error code to that value.
	
      @memo Get or set the error code.
      @return The Error object's error code. 
      @param ec The error code.  If this is not included, the
      undefined error code will be stored. */
    ErrorCode error_code(ErrorCode ec = undefined_error);
    
  /** With no argument, return a copy of the objet's error message string.
      With an argument, set the object's error message to that string.
    
      @memo Get or set the error code.
      @param msg The error message string.  If this is omitted, the
      function simply returns a copy of the current message string.
      @return A copy of the Error object's message string. 
      */
    String error_message(String msg = "");
    
    
  /** Either display the error message in a dialog box and offer the
      user a single `OK' button or print the message to standard
      error. If #gui# is not given, then use stderr. In addition, the
      class Gui provides other means for the user to control how
      messages are displayed and those may be used to select either
      graphical or text devices.

      @memo Display the error message in a dialog box or on stderr.
      @param gui A pointer to a valid Gui class instance.  This would
      be attached to a GUI process running on a client machine, and
      that process will display the message.  If the pointer is not
      provided, the message will be displayed on the client's stderr.

      @see Gui */
    void display_message(Gui *gui = 0);

  /** With no argument, return the program type of the error object. With
      an argument, set the error object's program type field.
      
      @memo Get or set the program type.
      @return The program type of the object. 
      @see ProgramType
      */
    ProgramType program_type(ProgramType pt = undefined_prog_type);

    
  /** With no argument, return the error correction program. With an
      argument, set the error correction program to a copy of that value.
    
      Note that this is not a pointer to a function, but a character
      string containing the entire tcl, Java, or other program.

      @memo  Get or set the error correction program.
      @return the error correction program. 
      */
    char *program(char *program = 0);

    
  /** This function runs the error correction program, if possible,
      and returns a string that can be used as the `corrected'
      value. If there is no error correction program or it is not
      possible to run the program, the function simply displays the
      error message. If the error correction program cannot be run,
      the function returns the null string.
    
      @memo Run the error correction program or print the error message.
      @return A corrected string or "".  
      @param gui A pointer to a Gui class object handling a GUI
      process on the client.
      @see Gui
      */
    String correct_error(Gui *gui);
};

#endif // _error_h
