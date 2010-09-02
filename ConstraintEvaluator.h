// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2006 OPeNDAP, Inc.
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef constraint_evaluator_h
#define constraint_evaluator_h

#include <list>

#ifndef _clause_h
#include "Clause.h"
#endif

namespace libdap
{

/** @brief Evaluate a constraint expression */
class ConstraintEvaluator
{
private:
    // This struct is used to hold all the known `user defined' functions
    // (including those that are `built-in').
    struct function
    {
        string name;
        bool_func b_func;
        btp_func bt_func;
        proj_func p_func;

        function(const string &n, const bool_func f)
                : name(n), b_func(f), bt_func(0), p_func(0)
        {}
        function(const string &n, const btp_func f)
                : name(n), bt_func(f), p_func(0)
        {}
        function(const string &n, const proj_func f)
                : name(n), bt_func(0), p_func(f)
        {}
        function(): name(""), bt_func(0), p_func(0)
        {}
    };

    vector<Clause *> expr;      // List of CE Clauses

    vector<BaseType *> constants;// List of temporary objects

    list<function> functions; // Known external functions

    // The default versions of these methods will break this class. Because
    // Clause does not support deep copies, that class will need to be modified
    // before these can be properly implemented. jhrg 4/3/06
    ConstraintEvaluator(const ConstraintEvaluator &)
    {}
    ConstraintEvaluator &operator=(const ConstraintEvaluator &)
    {
        throw InternalErr(__FILE__, __LINE__, "Unimplemented method");
    }

    friend class func_name_is;

public:
    typedef std::vector<Clause *>::const_iterator Clause_citer ;
    typedef std::vector<Clause *>::iterator Clause_iter ;

    typedef std::vector<BaseType *>::const_iterator Constants_citer ;
    typedef std::vector<BaseType *>::iterator Constants_iter ;

    typedef std::list<function>::const_iterator Functions_citer ;
    typedef std::list<function>::iterator Functions_iter ;

    ConstraintEvaluator();
    virtual ~ConstraintEvaluator();

    void add_function(const string &name, bool_func f);
    void add_function(const string &name, btp_func f);
    void add_function(const string &name, proj_func f);

    bool find_function(const string &name, bool_func *f) const;
    bool find_function(const string &name, btp_func *f) const;
    bool find_function(const string &name, proj_func *f) const;

    void append_clause(int op, rvalue *arg1, rvalue_list *arg2);
    void append_clause(bool_func func, rvalue_list *args);
    void append_clause(btp_func func, rvalue_list *args);

    bool functional_expression();
    bool boolean_expression();
    bool eval_selection(DDS &dds, const string &dataset);
    BaseType *eval_function(DDS &dds, const string &dataset);

    // New for libdap 3.11. These methods provide a way to evaluate multiple
    // functions in one CE
    bool function_clauses();
    DDS *eval_function_clauses(DDS &dds);

    Clause_iter clause_begin();
    Clause_iter clause_end();
    bool clause_value(Clause_iter &i, DDS &dds);

    void parse_constraint(const string &constraint, DDS &dds);
    void append_constant(BaseType *btp);

};

} // namespace libdap

#endif // constraint_evaluator_h
