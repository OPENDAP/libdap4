
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

#ifdef _GNUG_
#pragma interface
#endif

#include <vector>
#include "Pix.h"

#ifndef _basetype_h
#include "BaseType.h"
#endif

<<<<<<< Constructor.h

=======
/** Common methods for all constructor types. */
>>>>>>> 1.7.2.1
class Constructor: public BaseType {
private:
    Constructor();		// No default ctor.

protected:
    std::vector<BaseType *> _vars;

    void _duplicate(const Constructor &s);

public:
    typedef std::vector<BaseType *>::const_iterator Vars_citer ;
    typedef std::vector<BaseType *>::iterator Vars_iter ;

    Constructor(const string &n, const Type &t);

    Constructor(const Constructor &copy_from);
    virtual ~Constructor();

    Constructor &operator=(const Constructor &rhs);

<<<<<<< Constructor.h
    // Deprecated Pix interface
    virtual Pix first_var();
    virtual void next_var(Pix p);
    virtual BaseType *var(Pix p);

    Vars_iter var_begin();
    Vars_iter var_end();
    Vars_iter get_vars_iter(int i);

    virtual bool is_linear();

    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_decl(FILE *out, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_xml(FILE *out, string space = "    ", 
			   bool constrained =false);
=======
    virtual bool is_linear();
>>>>>>> 1.7.2.1
};

#endif // _constructor_h 
