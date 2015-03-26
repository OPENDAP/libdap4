
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

#include <vector>

#include <dods-datatypes.h>
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
	typedef std::vector<D4RValue *>::iterator iter;

	D4RValueList() { }
	D4RValueList(D4RValue *rv) { add_rvalue(rv); }

	~D4RValueList();

	void add_rvalue(D4RValue *rv) {
		d_rvalues.push_back(rv);
	}

	D4RValue *get_rvalue(unsigned int i) {
		return d_rvalues.at(i);
	}

	iter begin() { return d_rvalues.begin(); }
	iter end() { return d_rvalues.end(); }

	unsigned int size() const { return d_rvalues.size(); }

};

/** Holds the RValues for the D4 function parser
 *
 */
class D4RValue
{
private:
    BaseType *d_variable;	// This is a weak pointer

    D4Function d_func;  	// (weak) pointer to a function returning BaseType *
    D4RValueList *d_args;  	// pointer to arguments to the function; delete

    BaseType *d_constant;	// pointer; delete

    enum value_kind {
    	unknown,
    	basetype,
    	function,
    	constant
    };

    value_kind d_value_kind;

    friend class D4RValueList;

public:
    D4RValue() : d_variable(0), d_func(0), d_args(0), d_constant(0), d_value_kind(unknown) { }

    D4RValue(BaseType *btp)  : d_variable(btp), d_func(0), d_args(0), d_constant(0), d_value_kind(basetype) { }
    D4RValue(D4Function f, D4RValueList *args)  : d_variable(0), d_func(f), d_args(args), d_constant(0), d_value_kind(function) { }

    D4RValue(unsigned long long ui);
    D4RValue(long long i);
    D4RValue(double r);
    D4RValue(std::string s);
    D4RValue(std::vector<dods_byte> &byte_args);
    D4RValue(std::vector<dods_int8> &byte_int8);
    D4RValue(std::vector<dods_uint16> &byte_uint16);
    D4RValue(std::vector<dods_int16> &byte_int16);
    D4RValue(std::vector<dods_uint32> &byte_uint32);
    D4RValue(std::vector<dods_int32> &byte_int32);
    D4RValue(std::vector<dods_uint64> &byte_uint64);
    D4RValue(std::vector<dods_int64> &byte_int64);
    D4RValue(std::vector<dods_float32> &byte_float32);
    D4RValue(std::vector<dods_float64> &byte_float64);

    virtual ~D4RValue();

    // TODO/FIXME Add methods so that it's possible to figure out if a value
    // is from a variable, function or constant. This way filters can optimize
    // access/manipulation of constant values that will be used repeatedly.

    // This is the call that will be used to return the value of a function.
    // jhrg 3/10/14
    virtual BaseType *value(DMR &dmr);
};

} // namespace libdap
#endif // _RValue_h
