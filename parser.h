
// -*- c++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Constants, types and function prototypes for use with the DAP parsers. 
//
// jhrg 2/3/96

// $Log: parser.h,v $
// Revision 1.4  1996/08/13 20:45:44  jimg
// Extended definition of parser_arg so that it can serve as the interface for
// all the parsers.
// Added prototypes for all the new check_* functions.
//
// Revision 1.3  1996/05/31 23:31:07  jimg
// Updated copyright notice.
//
// Revision 1.2  1996/05/14 15:39:08  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.1  1996/02/03 07:35:44  jimg
// Added.

#ifndef _parser_h
#define _parser_h

#define YYDEBUG 1
#undef YYERROR_VERBOSE
#define YY_NO_UNPUT 1

#define ID_MAX 256

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "Error.h"

/// Pass parameters by reference to a parser.
/** #parser_arg# is used to pass parameters to the bison parsers and get
  error codes and objects in return. If #status()# is true, then the
  #object()# returns a pointer to the object built during the parse process.
  If #status()# is false, then the #error()# returns a pointer to an Error
  object.

  Note that the #object()# mfunc returns a void pointer. */

struct parser_arg {
    void *_object;		// nominally a pointer to an object
    Error *_error;		// a pointer to an Error object
    int _status;		// parser status

    parser_arg() : _object(0), _error(0), _status(1) {}
    parser_arg(void *obj) : _object(obj), _error(0), _status(1) {}

    void *object() { return _object; }
    void set_object(void *obj) { _object = obj; }
    Error *error() { return _error; }
    void set_error(Error *obj) { _error = obj; }
    int status() { return _status; }
    void set_status(int val = 0) { _status = val; }
};

/// Generate error messages for the various parsers.
/** #parser_error()# generates error messages for the various parsers used
  by the DODS core. There are two versions of the function, one which takes a
  #const char *message# and a #const int line_num# and writes the message and
  line number too stderr and a second which takes an additional #parser_arg
  *arg# parameter and writes the error message into an Error object which is
  returned to the caller via the #arg# parameter. 

  NB: The second version of this function also accepts a third parameter
  (#const char *context#) which can be used to provide an additional line of
  information  beyond what is in the string #message#.

  Returns: void */

void parse_error(const char *s, const int line_num);
void parse_error(parser_arg *arg, const char *s, const int line_num,
		 const char *context = 0);

/// Save a string to a temporary variable during the parse.
/** Given a string (#const char *src#), save it to the temporary variable
  pointed to by #dst#. If the string is longer than #ID_MAX#, generate and
  error indicating that #src# was truncated to #ID_MAX# characters during the
  copy operation. There are two versions of this function; one calls the
  version of #parser_error()# which writes to stderr. The version which
  accepts the #parser_arg *arg# argument calls the version of
  #parser_error()# which generates and Error object.

  Returns: void */

void save_str(char *dst, const char *src, const int line_num);
void save_str(char *dst, const char *src, parser_arg *arg, 
	      const int line_num, const char *context);

/// Is the value a valid byte?
/** Check to see if #val# is a valid byte value. If not, generate an error
  message using #parser_error()#. There are two versions of #check_byte()#,
  one which calls #parser_error()# and prints an error message to stderr an
  one which calls #parser_error()# and generates and Error object.

  Returns: True if #val# is a byte value, False otherwise. */

int check_byte(const char *val, const int line);
int check_byte(parser_arg *arg, const char *val, const int line, 
	       const char *context);

/// Is the value a valid integer?
//* Like #check_byte()# but for 32-bit integers.

int check_int(const char *val, const int num);
int check_int(parser_arg *arg, const char *val, const int num,
	      const char *context);

/// Is the value a valid float?
//* Like #check_byte()# but for 64-bit float values.

int check_float(const char *val, const int num);
int check_float(parser_arg *arg, const char *val, const int num,
		const char *context);

/// Is tha value a valid URL?
//* Currently this function always returns true.

int check_url(const char *val, const int num);
int check_url(parser_arg *arg, const char *val, const int num,
	      const char *context);

#endif // _parser_h
 
