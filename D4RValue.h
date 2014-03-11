
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
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

#ifndef _D4RValue_h
#define _D4RValue_h

#include <D4Function.h>

namespace libdap
{

class BaseType;
class D4Constant;
class D4RValue;

class D4RValueList
{
private:
	std::vector<D4RValue *> d_rvalues;

public:
	typedef std::vector<D4RValue *>::iterator D4RValueListIter;

	D4RValueList() { }
	D4RValueList(D4RValue *rv) { add_rvalue(rv); }

	~D4RValueList() { }

	std::vector<D4RValue *> &get_rvalues() {
		return d_rvalues;
	}

	void add_rvalue(D4RValue *rv) {
		d_rvalues.push_back(rv);
	}

	D4RValue *get_rvalue(D4RValueListIter &i) {
		return (*i);
	}

	D4RValueListIter rvalue_begin() { return d_rvalues.begin(); }
	D4RValueListIter rvalue_end() { return d_rvalues.end(); }

	unsigned int size() const { return d_rvalues.size(); }

};

/** Holds the RValues for the D4 function parser
 *
 */
class D4RValue
{
private:
    BaseType *d_variable;	// This is a weak pointer

    D4Function d_func;  	// (weak) Pointer to a function returning BaseType *
    D4RValueList *d_args;  	// Strong pointer to arguments to the function; delete

    unsigned long long d_u_int_val;
    long long d_int_val;
    double d_real_val;
    std::string d_string_val;

    enum value_kind {
    	unknown,
    	basetype,
    	function,
    	uinteger,
    	integer,
    	real,
    	string
    };

    value_kind d_value_kind;

public:
    D4RValue() : d_variable(0), d_func(0), d_args(0), d_value_kind(unknown) { }

    D4RValue(BaseType *btp)  : d_variable(btp), d_func(0), d_args(0), d_value_kind(basetype) { }
    D4RValue(D4Function f, D4RValueList *args)  : d_variable(0), d_func(f), d_args(args), d_value_kind(function) { }

    D4RValue(unsigned long long ui) : d_variable(0), d_func(0), d_args(0), d_u_int_val(ui), d_value_kind(uinteger) { }
    D4RValue(long long i) : d_variable(0), d_func(0), d_args(0), d_int_val(i), d_value_kind(integer) { }
    D4RValue(double r) : d_variable(0), d_func(0), d_args(0), d_real_val(r), d_value_kind(real) { }
    D4RValue(std::string s) : d_variable(0), d_func(0), d_args(0), d_string_val(s), d_value_kind(string) { }

    virtual ~D4RValue() {
    	// d_variable and d_func are weak pointers; don't delete.
    	delete d_args;
    }

    // This is the call that will be used to return the value of a function.
    // jhrg 3/10/14
    BaseType *value(DMR &dmr);
};

} // namespace libdap
#endif // _RValue_h
