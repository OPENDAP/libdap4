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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>


// These functions are used by the CE evaluator
//
// 1/15/99 jhrg

#include "config.h"

static char rcsid[]not_used = { "$Id: ce_functions.cc 26039 2012-11-15 23:29:56Z jimg $" };

#include <limits.h>

#include <cstdlib>      // used by strtod()
#include <cerrno>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

//#define DODS_DEBUG

#include "BaseType.h"
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "Error.h"
#include "debug.h"

#include "reproj_functions.h"

//  We wrapped VC++ 6.x strtod() to account for a short coming
//  in that function in regards to "NaN".  I don't know if this
//  still applies in more recent versions of that product.
//  ROM - 12/2007
#ifdef WIN32
#include <limits>
double w32strtod(const char *, char **);
#endif

using namespace std;


namespace libdap {

void function_swath2grid(int argc, BaseType * argv[], DDS &, BaseType **btpp)
{

}

#if 0
/** The grid function uses a set of relational expressions to form a selection
 within a Grid variable based on the values in the Grid's map vectors.
 Thus, if a Grid has a 'temperature' map which ranges from 0.0 to 32.0
 degrees, it's possible to request the values of the Grid that fall between
 10.5 and 12.5 degrees without knowing to which array indexes those values
 correspond. The function takes one or more arguments:<ul>
 <li>The name of a Grid.</li>
 <li>Zero or more strings which hold relational expressions of the form:<ul>
 <li><code>&lt;map var&gt; &lt;relop&gt; &lt;constant&gt;</code></li>
 <li><code>&lt;constant&gt; &lt;relop&gt; &lt;map var&gt; &lt;relop&gt;
 &lt;constant&gt;</code></li>
 </ul></li>
 </ul>

 Each of the relation expressions is applied to the Grid and the result is
 returned.

 @note Since this is a function and one of the arguments is the grid, the
 grid is read (using the Grid::read() method) at the time the argument list
 is built.

 @todo In order to be used by geogrid() , this code may have to be modified
 so that the maps and array are not re-read by the serialize() method. It
 might also be a good idea to change from the '?grid(SST,"10<time<20")'
 syntax in a URL to '?SST&grid(SST,"10<time<20")' even though it's more
 verbose in the URL, it would make the function a true 'selection operator'
 and allow several grids to be returned with selections in one request.

 @param argc The number of values in argv.
 @param argv An array of BaseType pointers which hold the arguments to be
 passed to geogrid. The arguments may be Strings, Integers, or Reals, subject
 to the above constraints.
 @param btpp A pointer to the return value; caller must delete.

 @see geogrid() (func_geogrid_select) A function which has logic specific
 to longitude/latitude selection. */
void function_grid(int argc, BaseType * argv[], DDS &, BaseType **btpp)
{
    DBG(cerr << "Entering function_grid..." << endl);

    string
            info =
                    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
                            + "<function name=\"grid\" version=\"1.0\" href=\"http://docs.opendap.org/index.php/Server_Side_Processing_Functions#grid\">\n"
                            + "</function>\n";

    if (argc == 0) {
        Str *response = new Str("info");
        response->set_value(info);
        *btpp = response;
        return;
    }

    Grid *original_grid = dynamic_cast<Grid *> (argv[0]);
    if (!original_grid)
        throw Error(malformed_expr, "The first argument to grid() must be a Grid variable!");

    // Duplicate the grid; ResponseBuilder::send_data() will delete the variable
    // after serializing it.
    BaseType *btp = original_grid->ptr_duplicate();
    Grid *l_grid = dynamic_cast<Grid *> (btp);
    if (!l_grid) {
        delete btp;
        throw InternalErr(__FILE__, __LINE__, "Expected a Grid.");
    }

    DBG(cerr << "grid: past initialization code" << endl);

    // Read the maps. Do this before calling parse_gse_expression(). Avoid
    // reading the array until the constraints have been applied because it
    // might be really large.

    // This version makes sure to set the send_p flags which is needed for
    // the hdf4 handler (and is what should be done in general).
    Grid::Map_iter i = l_grid->map_begin();
    while (i != l_grid->map_end())
        (*i++)->set_send_p(true);

    l_grid->read();

    DBG(cerr << "grid: past map read" << endl);

    // argv[1..n] holds strings; each are little expressions to be parsed.
    // When each expression is parsed, the parser makes a new instance of
    // GSEClause. GSEClause checks to make sure the named map really exists
    // in the Grid and that the range of values given makes sense.
    vector<GSEClause *> clauses;
    gse_arg *arg = new gse_arg(l_grid);
    for (int i = 1; i < argc; ++i) {
        parse_gse_expression(arg, argv[i]);
        clauses.push_back(arg->get_gsec());
    }
    delete arg;
    arg = 0;

    apply_grid_selection_expressions(l_grid, clauses);

    DBG(cerr << "grid: past gse application" << endl);

    l_grid->get_array()->set_send_p(true);

    l_grid->read();

    // Make a new grid here and copy just the parts of the Grid
    // that are in the current projection - this means reading
    // the array slicing information, extracting the correct
    // values and building destination arrays with just those
    // values.

    *btpp = l_grid;
    return;
}
#endif

void register_functions(ConstraintEvaluator & ce)
{
    ce.add_function("swath2grid", function_swath2grid);
}

} // namespace libdap
