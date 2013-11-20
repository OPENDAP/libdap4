/*
 * D4CEDriver.h
 *
 *  Created on: Aug 8, 2013
 *      Author: jimg
 */

#ifndef D4CEDRIVER_H_
#define D4CEDRIVER_H_

#include <string>

//#include "D4CEScanner.h"
//#include "d4_ce_parser.tab.hh"

namespace libdap {

class location;
class DMR;

/**
 * Driver for the DAP4 Constraint Expression parser.
 */
class D4CEDriver {
	bool d_trace_scanning;
	bool d_trace_parsing;
	bool d_result;

	DMR *d_dmr;

public:
	D4CEDriver() : d_trace_scanning(false), d_trace_parsing(false), d_result(false), d_dmr(0), expression("") { }
	D4CEDriver(DMR *dmr) : d_trace_scanning(false), d_trace_parsing(false), d_result(false), d_dmr(dmr), expression("") { }
	D4CEDriver(DMR *dmr, const string &expr) : d_trace_scanning(false), d_trace_parsing(false), d_result(false),
			d_dmr(dmr), expression(expr) { }

	virtual ~D4CEDriver() { }

	bool parse(const std::string &expr);

	std::string expression;	// This may have to be a public field. jhrg 11/19/13

	bool trace_scanning() const { return d_trace_scanning; }
	void set_trace_scanning(bool ts) { d_trace_scanning = ts; }

	bool trace_parsing() const { return d_trace_parsing; }
	void set_trace_parsing(bool tp) { d_trace_parsing = tp; }

	bool result() const { return d_result; }
	void set_result(bool r) { d_result = r; }

	DMR *dmr() const { return d_dmr; }
	void set_dmr(DMR *dmr) { d_dmr = dmr; }

	void error(const libdap::location &l, const std::string &m);
};

} /* namespace libdap */
#endif /* D4CEDRIVER_H_ */
