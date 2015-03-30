
#ifndef D4CESCANNER_H_
#define D4CESCANNER_H_

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer d4_ceFlexLexer
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
#define YY_DECL int  libdap::D4CEScanner::yylex()

#include "d4_ce_parser.tab.hh"

namespace libdap {

class D4CEScanner : public d4_ceFlexLexer{
public:

	D4CEScanner(std::istream &in) : d4_ceFlexLexer(&in), yylval(0), loc(0) { };

	int yylex(libdap::D4CEParser::semantic_type *lval, libdap::location *l)
	{
		loc = l;
		yylval = lval;
		return( yylex() );
	}

private:
	/* hide this one from public view */
	int yylex();

	/* yyval ptr */
	libdap::D4CEParser::semantic_type *yylval;

	libdap::location *loc;
};

} /* end namespace libdap */

#endif /* D4CESCANNER_H_ */
