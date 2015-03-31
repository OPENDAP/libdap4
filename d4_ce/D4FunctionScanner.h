
#ifndef D4_FUNCTION_SCANNER_H_
#define D4_FUNCTION_SCANNER_H_

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer d4_functionFlexLexer
#include "FlexLexer.h"
#endif

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#undef  YY_DECL
#define YY_DECL int  libdap::D4FunctionScanner::yylex()

#include "d4_function_parser.tab.hh"

namespace libdap {

class D4FunctionScanner : public d4_functionFlexLexer{
public:

	D4FunctionScanner(std::istream &in) : d4_functionFlexLexer(&in), yylval(0), loc(0) { };

	int yylex(libdap::D4FunctionParser::semantic_type *lval, libdap::location *l)
	{
		loc = l;
		yylval = lval;
		return( yylex() );
	}

private:
	/* hide this one from public view */
	int yylex();

	/* yyval ptr */
	libdap::D4FunctionParser::semantic_type *yylval;

	libdap::location *loc;
};

} /* end namespace libdap */

#endif /* D4_FUNCTION_SCANNER_H_ */
