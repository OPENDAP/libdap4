
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
#include <vector>

#include "ce_expr.tab.hh"   // Use the same codes for D4 as we use in DAP2

namespace libdap
{

class D4Rvalue;
class D4FilterClause;

/**
 * @brief List of DAP4 Filter Clauses
 *
 */
class D4FilterClauseList
{
private:
    std::vector<D4FilterClause *> d_clauses;

    void m_duplicate(const D4FilterClauseList &src);

public:
    typedef std::vector<D4FilterClause *>::iterator iter;
    typedef std::vector<D4FilterClause *>::const_iterator citer;

    D4FilterClauseList() { }
    D4FilterClauseList(const D4FilterClauseList &src) { m_duplicate(src); }

    D4FilterClauseList(D4FilterClause *c) { add_clause(c); }

    virtual ~D4FilterClauseList();

    D4FilterClauseList &operator=(const D4FilterClauseList &rhs) {
        if (this == &rhs)
            return *this;

        m_duplicate(rhs);

        return *this;
    }

    void add_clause(D4FilterClause *c) {
        d_clauses.push_back(c);
    }

    D4FilterClause *get_clause(unsigned int i) {
        return d_clauses.at(i);
    }

    citer cbegin() const { return d_clauses.begin(); }
    citer cend() const { return d_clauses.end(); }

    unsigned int size() const { return d_clauses.size(); }

    // get the clause value; this version supports functional clauses
    bool value(DMR &dmr);

    bool value();
};

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
 * being sent to a remote client).
 *
 * For filter clauses that are to be applied to a Sequence, each D4RValue
 * will either be a constant or a BaseType* that will reference one of the
 * Sequences fields. The method 'value()' is effectively the evaluator for
 * the clause and nominally reads values from the rvalue objects.
 *
 * @note Potential optimization: Because Sequences might have an optimized
 * representation as a STL vector of some built in types, there could be a
 * value() method that takes a value and compares it to the clause's constant
 * value using the supplied op.
 *
 * @note The 'ND' and 'map' ops are 'still just an idea' parts.
 */
class D4FilterClause
{
public:
	enum ops {
		// Stock relops
		null = 0,
		less = SCAN_LESS,
		greater = SCAN_GREATER,
		less_equal = SCAN_LESS_EQL,
		greater_equal = SCAN_GREATER_EQL,
		equal = SCAN_EQUAL,
		not_equal = SCAN_NOT_EQUAL,
		// Regex match for strings
		match = SCAN_REGEXP,
		// The mapping operator; not sure if this will be implemented
		map,
		// No Data 'operator' for array filtering; may not be impl'd
		ND
	};

private:
    /** The operator */
    ops d_op;

    D4RValue *d_arg1, *d_arg2;

    D4FilterClause() : d_op(null), d_arg1(0), d_arg2(0) { }

    void m_duplicate(const D4FilterClause &rhs);

    // These methods factor out first the first argument and then the
    // second. I could write one really large cmp() for all of this...
    //template<typename T> bool cmp(ops op, BaseType *arg1, T arg2);
    bool cmp(ops op, BaseType *arg1, BaseType *arg2);

    friend class D4FilterClauseList;

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

    D4FilterClause(const D4FilterClause &src) {
        m_duplicate(src);
    }

    D4FilterClause &operator=(const D4FilterClause &rhs) {
        if (this == &rhs)
            return *this;

        m_duplicate(rhs);

        return *this;
    }

    virtual ~D4FilterClause() {
    	delete d_arg1;
    	delete d_arg2;
    }

    // get the clause value; this version supports functional clauses
    bool value(DMR &dmr);

    bool value();
};

} // namespace libdap

#endif // _clause_h
