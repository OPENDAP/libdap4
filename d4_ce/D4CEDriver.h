/*
 * D4CEDriver.h
 *
 *  Created on: Aug 8, 2013
 *      Author: jimg
 */

#ifndef D4CEDRIVER_H_
#define D4CEDRIVER_H_

#include <string>

#include "D4CEScanner.h"
#include "d4_ce_parser.tab.hh"

namespace libdap {

/*
 *
 */
class D4CEDriver {
public:
	D4CEDriver() : trace_scanning(false), trace_parsing(false), result(false), expression("") { }

	virtual ~D4CEDriver() { }

	// Parse this string; throw an error (not yet impl)
	bool parse(const std::string &expr);

	std::string expression;
	bool result;

	bool trace_scanning;
	bool trace_parsing;

	void error(const libdap::location &l, const std::string &m);
};

} /* namespace libdap */
#endif /* D4CEDRIVER_H_ */
