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

namespace libdap {

bool D4CEDriver::parse(const std::string &expr)
{
	std::istringstream iss(expr);
	D4CEScanner *scanner = new D4CEScanner(iss);

	D4CEParser *parser = new D4CEParser(*scanner, *this /* driver */);

	return parser->parse() == 0;
}

#if 0
void
D4CEDriver::error(const libdap::location &l, const std::string &m)
{
	std::cerr << l << ": " << m << std::endl;
}
#endif

void
D4CEDriver::error(const std::string &m)
{
	std::cerr << m << std::endl;
}

} /* namespace libdap */
