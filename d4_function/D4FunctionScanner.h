
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.


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
