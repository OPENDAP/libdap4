/*
 * D4CEDriver.h
 *
 *  Created on: Aug 8, 2013
 *      Author: jimg
 */

#ifndef D4CEDRIVER_H_
#define D4CEDRIVER_H_

#include <string>
#include <vector>
#include <stack>

namespace libdap {

class location;
class DMR;
class BaseType;
class Array;
class D4Dimension;

/**
 * Driver for the DAP4 Constraint Expression parser.
 */
class D4CEDriver {
	struct index {
		// start and stride are simple numbers; stop is either the stopping index or
		// if to_end is true, is ignored and the subset runs to the end of the dimension
		unsigned long long start, stride, stop;
		// true if the slice indicates it does not contain a specific 'stop' value but
		// goes to the end, whatever that value is.
		bool rest;
		// An empty slice ([]) means either the entire dimension or apply the shared
		// dimension slice, depending on whether the corresponding shared dimension has
		// been sliced.
		bool empty;

		// Added because the parser code needs it. Our code does not use this. jhrg 11/26/13
		index(): start(0), stride(0), stop(0), rest(false), empty(false) {}
		index(unsigned long long i, unsigned long long s, unsigned long long e, bool r, bool em)
			: start(i), stride(s), stop(e), rest(r), empty(em) {}
	};

	index make_index() { return index(0, 1, 0, true /*rest*/, true /*empty*/); }

	index make_index(const std::string &is);

	index make_index(const std::string &i, const std::string &s, const std::string &e);
	index make_index(const std::string &i, unsigned long long s, const std::string &e);

	index make_index(const std::string &i, const std::string &s);
	index make_index(const std::string &i, unsigned long long s);

	bool d_trace_scanning;
	bool d_trace_parsing;
	bool d_result;
	std::string d_expr;

	DMR *d_dmr;

	std::vector<index> d_indexes;

	std::stack<BaseType*> d_basetype_stack;

	// d_expr should be set by parse! Its value is used by the parser right before
	// the actual parsing operation starts. jhrg 11/26/13
	std::string *expression() { return &d_expr; }
#if 0
	void set_array_slices(const std::string &id, Array *a);
#endif
	void search_for_and_mark_arrays(BaseType *btp);
	BaseType *mark_variable(BaseType *btp);
	BaseType *mark_array_variable(BaseType *btp);

	D4Dimension *slice_dimension(const std::string &id, const index &i);

	void push_index(const index &i) { d_indexes.push_back(i); }

	void push_basetype(BaseType *btp) { d_basetype_stack.push(btp); }
	BaseType *top_basetype() const { return d_basetype_stack.empty() ? 0 : d_basetype_stack.top(); }
	// throw on pop with an empty stack?
	void pop_basetype() { d_basetype_stack.pop(); }

	void throw_not_found(const std::string &id);

	friend class D4CEParser;

public:
	D4CEDriver() : d_trace_scanning(false), d_trace_parsing(false), d_result(false), d_expr(""), d_dmr(0) { }
	D4CEDriver(DMR *dmr) : d_trace_scanning(false), d_trace_parsing(false), d_result(false), d_expr(""), d_dmr(dmr) { }

	virtual ~D4CEDriver() { }

	bool parse(const std::string &expr);

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
