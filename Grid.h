
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

#ifndef _grid_h
#define _grid_h 1

#include <vector>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _array_h
#include "Array.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

namespace libdap
{

class D4Grup;
class XMLWriter;

/** The Grid data type is a collection of an Array and a set of ``Map''
    vectors.  The Map vectors are one-dimensional arrays corresponding
    to each dimension of the central Array.  Using this scheme, a Grid
    can represent, in a rectilinear array, data which is not in
    reality rectilinear.  An example will help make it clear.

    Assume that the following array contains measurements of some real
    quantity, conducted at nine different points in space:

    <pre>
    A = [ 1  2  3  4 ]
        [ 2  4  6  8 ]
        [ 3  6  9  12]
    </pre>

    To locate this Array in the real world, we could note the location
    of one corner of the grid, and the grid spacing.  This would allow
    us to calculate the location of any of the other points of the
    Array.

    This approach will not work, however, unless the grid spacing is
    precisely regular.  If the distance between Row 1 and Row 2 is not
    the same as the distance between Row 2 and Row 3, the scheme will
    break down.  The solution is to equip the Array with two Map
    vectors that define the location of each row or column of the
    array:

    <pre>
         A = [ 1  2  3  4 ] Row = [ 0 ]
             [ 2  4  6  8 ]       [ 3 ]
             [ 3  6  9  12]       [ 8 ]

    Column = [ 0  2  8  27]
    </pre>

    The real location of the point in the first row and column of the
    array is now exactly fixed at (0,0), and the point in the last row
    and last column is at (8,27).

    The Grid data type has two parts: an Array, and a singly-linked
    list of Map vectors to describe the Array.  The access functions
    for this class include a function to return the Array
    (<tt>get_array()</tt>), and a set of functions for cycling through the
    list of Map vectors.

    @todo Move, in some sense, the d_map_vars up to Constructor. Look at using
    Constructor's _var field for these.
    @todo Along the same lines as the previous item, consider removing the
    Part enum and adopting the convention that the first variable added is
    the array and any subsequent variables are maps.

    @brief Holds the Grid data type.
    @see Array
    */

class Grid: public Constructor
{
private:
    //BaseType *d_array_var;	// weak pointer to the
    bool d_is_array_set;

protected: // subclasses need access [mjohnson 11 nov 2009]
    void m_duplicate(const Grid &s);

public:

    Grid(const string &n);
    Grid(const string &n, const string &d);
    Grid(const Grid &rhs);
    virtual ~Grid();

    typedef std::vector<BaseType *>::const_iterator Map_citer ;
    typedef std::vector<BaseType *>::iterator Map_iter ;
    typedef std::vector<BaseType *>::reverse_iterator Map_riter ;


    Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate();

    virtual void transform_to_dap4(D4Group *root, Constructor *container);

    virtual bool is_dap2_only_type();

    virtual void add_var(BaseType *bt, Part part);
    virtual void add_var_nocopy(BaseType *bt, Part part);

    virtual void set_array(Array* p_new_arr);
    virtual Array* add_map(Array* p_new_map, bool add_copy);
    virtual Array* prepend_map(Array* p_new_map, bool add_copy);

    BaseType *array_var();
    Array *get_array();

    // virtual unsigned int width(bool constrained = false);

    virtual int components(bool constrained = false);

    virtual bool projection_yields_grid();

    virtual void clear_constraint();

    virtual void print_decl(ostream &out, string space = "    ",
                            bool print_semi = true,
                            bool constraint_info = false,
                            bool constrained = false);

    virtual void print_xml(ostream &out, string space = "    ",
                           bool constrained = false);
    virtual void print_xml_writer(XMLWriter &xml, bool constrained = false);

    virtual void print_val(ostream &out, string space = "",
                           bool print_decl_p = true);

    virtual void print_decl(FILE *out, string space = "    ",
                            bool print_semi = true,
                            bool constraint_info = false,
                            bool constrained = false);
    virtual void print_xml(FILE *out, string space = "    ",
                           bool constrained = false);
    virtual void print_val(FILE *out, string space = "",
                           bool print_decl_p = true);

    virtual void transfer_attributes(AttrTable *at_container);

    virtual bool check_semantics(string &msg, bool all = false);

    Map_iter map_begin() ;
    Map_iter map_end() ;
    Map_riter map_rbegin() ;
    Map_riter map_rend() ;
    Map_iter get_map_iter(int i);

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif // _grid_h

