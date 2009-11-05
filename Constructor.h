
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

#ifndef _constructor_h
#define _constructor_h 1


#include <vector>

#ifndef _basetype_h
#include "BaseType.h"
#endif



namespace libdap
{

/** Common methods for all constructor types. */
class Constructor: public BaseType
{
private:
    Constructor();  // No default ctor.
    BaseType *find_hdf4_dimension_attribute_home(AttrTable::entry *source);

protected:
    std::vector<BaseType *> _vars;

    void _duplicate(const Constructor &s);
    virtual AttrTable *find_matching_container(AttrTable::entry *source,
            BaseType **dest_variable);

    Constructor(const string &n, const Type &t);
    Constructor(const string &n, const string &d, const Type &t);

    Constructor(const Constructor &copy_from);
public:
    typedef std::vector<BaseType *>::const_iterator Vars_citer ;
    typedef std::vector<BaseType *>::iterator Vars_iter ;
    typedef std::vector<BaseType *>::reverse_iterator Vars_riter ;

    virtual ~Constructor();

    Constructor &operator=(const Constructor &rhs);
    virtual void transfer_attributes(AttrTable::entry *entry);

    Vars_iter var_begin();
    Vars_iter var_end();
    Vars_riter var_rbegin();
    Vars_riter var_rend();
    Vars_iter get_vars_iter(int i);
    BaseType *get_var_index(int i);

    virtual bool is_linear();

    virtual void print_decl(ostream &out, string space = "    ",
                            bool print_semi = true,
                            bool constraint_info = false,
                            bool constrained = false);

    virtual void print_xml(ostream &out, string space = "    ",
                           bool constrained = false);

#if FILE_METHODS
    virtual void print_decl(FILE *out, string space = "    ",
                            bool print_semi = true,
                            bool constraint_info = false,
                            bool constrained = false);
    virtual void print_xml(FILE *out, string space = "    ",
                           bool constrained = false);
#endif

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _constructor_h
