
// -*- c++ -*-

// Constants, types and function prototypes for use with the DAP parsers. 
//
// jhrg 2/3/96

// $Log: parser.h,v $
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
#define YYERROR_VERBOSE 1
#define ID_MAX 256

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "Error.h"

// Note that that the struct PARSER_ARG has a `universal' ctor but special
// accessors for each type of object that might be an argument to the parser.

struct parser_arg {
    void *_object;		// nominally a pointer to an object
    int _status;		// parser status

    parser_arg() : _object(0), _status(0) {}
    parser_arg(void *obj) : _object(obj), _status(0) {}

    Error &error() { return *(Error *)_object; }
    int &status() { return _status; }
};

void parse_error(const char *s, const int line_num);
void save_str(char *dst, const char *src, const int line_num);
int check_byte(const char *val, const int num);
int check_int(const char *val, const int num);
int check_float(const char *val, const int num);
int check_url(const char *val, const int num);

#endif // _parser_h
