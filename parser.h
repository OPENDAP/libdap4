
// -*- c++ -*-

// Constants used by the DAP parsers (DAS, DDS,and CE).

// $Log: parser.h,v $
// Revision 1.1  1996/02/03 07:35:44  jimg
// Added.
//

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define ID_MAX 256

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

void parse_error(const char *s, const int line_num);
void save_str(char *dst, const char *src, const int line_num);
int check_byte(const char *val, const int num);
int check_int(const char *val, const int num);
int check_float(const char *val, const int num);
int check_url(const char *val, const int num);

