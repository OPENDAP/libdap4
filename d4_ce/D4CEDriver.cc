/*
 * D4CEParserDriver.cc
 *
 *  Created on: Aug 8, 2013
 *      Author: jimg
 */

#include <string>
#include <sstream>

#include "D4CEScanner.h"
#include "D4CEDriver.h"
#include "d4_ce_parser.tab.hh"

namespace libdap {

bool D4CEDriver::parse(const std::string &expr)
{
	expression = expr;	// set for error messages.

	std::istringstream iss(expr);
	D4CEScanner *scanner = new D4CEScanner(iss);

	D4CEParser *parser = new D4CEParser(*scanner, *this /* driver */);

	if (trace_parsing()) {
		parser->set_debug_level(1);
		parser->set_debug_stream(std::cerr);
	}

	return parser->parse() == 0;
}

// This method is called from the parser (see d4_ce_parser.yy, down in the code
// section). This will be called during the call to D4CEParser::parse(), that
// is inside D4CEDriver::parse(...)
void
D4CEDriver::error(const libdap::location &l, const std::string &m)
{
	std::cerr << l << ": " << m << std::endl;
}

} /* namespace libdap */
