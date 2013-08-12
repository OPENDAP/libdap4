
#ifndef D4CESCANNER_H_
#define D4CESCANNER_H_

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
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

class D4CEScanner : public yyFlexLexer{
public:

	D4CEScanner(std::istream &in) : yyFlexLexer(&in), yylval( 0 ) { };

	int yylex(libdap::D4CEParser::semantic_type *lval)
	{
		yylval = lval;
		return( yylex() );
	}

private:
	/* hide this one from public view */
	int yylex();

	/* yyval ptr */
	libdap::D4CEParser::semantic_type *yylval;
};

} /* end namespace libdap */

#endif /* D4CESCANNER_H_ */
