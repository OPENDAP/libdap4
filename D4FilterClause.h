
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2015 OPeNDAP, Inc.
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

#ifndef _d4_filter_clause_h
#define _d4_filter_clause_h

#include <cassert>

namespace libdap
{

/**
 * @brief DAP4 filter clauses
 *
 * The DAP4 constraint expression provides a way to filter the values of
 * Sequences (and possibly arrays and coverages, although those are still
 * more ideas than anything at this point). This class holds the operator
 * and operands of one DAP4 constraint's filter clause. The object is built
 * during the parse phase of the constraint evaluation but is not evaluated
 * until the data are sent or interned (read into the DAP4 variable object
 * so that they can be used as input to some process other than directly
 * being sent to a remote client.
 *
 * For filter clauses that are to be applied to a Sequence, each D4RValue
 * will either be a constant or a BaseType* that will reference one of the
 * Sequences fields. The method 'value()' is effectively the evaluator for
 * the clause and nominally reads values from the rvalue objects.
 *
 * @note Potentail optimization: Because Sequences might have an optimized
 * representation as a STL vector of some built in types, there could be a
 * value() method that take a value and compares it to the clause's constant
 * value using the supplied op.
 *
 * @note The 'ND' and 'map' ops are the 'still just an idea' parts.
 */
class D4FilterClause
{
public:
	enum ops {
		// Stock relops
		null,
		less,
		greater,
		less_equal,
		greater_equal,
		equal,
		not_equal,
		// Regex match for strings
		match,
		// The mapping operator; not sure if this will be implemented
		map,
		// No Data 'operator' for array filtering
		ND
	};

private:
    /** The operator */
    ops d_op;

    D4RValue *d_arg1, *d_arg2;

    D4FilterClause();
    D4FilterClause(const D4FilterClause &);
    D4FilterClause &operator=(const D4FilterClause &);

    template<typename T1, typename T2> bool D4FilterClause::cmp(ops op, T1 arg1, T2 Arg2);
    bool D4FilterClause::cmp(ops op, const string &arg1, const string &arg2);

    template<typename T> bool D4FilterClause::cmp(ops op, BaseType *arg1, T arg2);
    bool D4FilterClause::cmp(ops op, BaseType *arg1, BaseType *arg2);

public:
    /**
     * Build a D4FilterClause. The clause will take ownership of
     * the two pointer arguments and delete them.
     *
     * @note When comparing an unsigned variable (UInt16) with a constant,
     * at parse time (i.e., when the D4FilterClause is made) check that the
     * constant is >= 0 and store it in an unsigned value at that time. This
     * will avoid having to make the test repeatedly during filter evaluation.
     *
     * @note When parsing a constant, extract the value from the BaseType and
     * store it in a local field, to avoid the overhead of extracting the
     * value and looking up its type over an over.
     *
     * @param op The operator
     * @param arg1 The left-hand operand
     * @param arg2 The right-hand operand
     */
    D4FilterClause(const ops op, D4RValue *arg1, D4RValue *arg2) :
    	d_op(op), d_arg1(arg1), d_arg2(arg2) {
    	assert(op != null && "null operator");
    	assert(arg1 && "null arg1");
    	assert(arg2 && "null arg2");
    }

#if 0
    /**
     * Build an empty clause.
     */
    D4FilterClause() : d_op(null), d_arg1(0), d_arg2(0) { }
#endif

    virtual ~D4FilterClause() {
    	delete d_arg1;
    	delete d_arg2;
    }

    bool value(DMR &dmr);
};

} // namespace libdap

#endif // _clause_h
