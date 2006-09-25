
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

static char rcsid[] not_used = {"$Id$"};

#include <iostream>
#include <vector>
#include <algorithm>

#define DODS_DEBUG

#include "BaseType.h"
#include "Array.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"
#include "RValue.h"

#include "GSEClause.h"
#include "GeoConstraint.h"

#include "ce_functions.h"
#include "gse_parser.h"
#include "gse.tab.h"
#include "debug.h"
#include "util.h"

#if defined(_MSC_VER) && (_MSC_VER == 1200)  //  VC++ 6.0 only
using std::vector<GSEClause *>;
#else
using std::vector;
#endif

int gse_parse(void *arg);
void gse_restart(FILE *in);

// Glue routines declared in gse.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void * buffer);
void *gse_string(const char *yy_str);

namespace libdap {
        
/** Given a BaseType pointer, extract the string value it contains and return
    it. 
    
    @param arg The BaseType pointer
    @return A C++ string
    @exception Error thrown if the referenced BaseType object does not contain
    a DAP String. */ 
string
extract_string_argument(BaseType *arg) throw(Error)
{
    if (arg->type() != dods_str_c)
	throw Error(malformed_expr, 
		    "The function requires a DAP string-type argument.");
    
    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
            "The CE Evaluator built an argument list where some constants held no values.");

    string *sp = 0;
    arg->buf2val((void **)&sp);
    string s = *sp;
    delete sp; sp = 0;

    DBG(cerr << "s: " << s << endl);

    return s;
}

template<class T> 
static void
set_array_using_double_helper(Array *a, double *src, int src_len) throw(Error)
{
    T *values = new T[src_len];
    for (int i = 0; i < src_len; ++i)
        values[i] = (T)src[i];
    a->val2buf(values, true);
    delete[] values;
}

/** Given an array that holds some sort of numeric data, load it with values
    using an array of doubles. This function makes several assumptions. First,
    it assumes the caller really wants to put the doubles into whatever types
    the array holds! Caveat emptor. Second, it assumes that if the size of 
    source (\e src) array is different than the destination (\e dest) the
    caller has made a mistake. In that case it will throw an Error object. 
    
    After setting that values, this method sets the \c read_p property for
    \e dest.
    
    @param dest An Array. The values are written to this array, reusing
    its storage. Existing values are lost.
    @param src The source data.
    @param src_len The number of elements in the \e src array.
    @exception Error Thrown if \e dest is not a numeric-type array (Byte, ...,
    Float64) or if the number of elements in \e src does not match the number
    is \e dest. */
void
set_array_using_double(Array *dest, double *src, int src_len) throw(Error)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ( dest->type() == dods_array_c
         && !dest->var()->is_simple_type() 
         || dest->var()->type() == dods_str_c
         || dest->var()->type() == dods_url_c )
        throw InternalErr(__FILE__, __LINE__,
            "The function requires a DAP numeric-type array argument.");
                    
    // Test sizes. Note that Array::length() takes any constraint into account
    // when it returns the length. Even if this was removed, the 'helper'
    // function this uses calls Vector::val2buf() which uses Vector::width()
    // which in turn uses length(). 
    if (dest->length() != src_len)
        throw InternalErr(__FILE__, __LINE__,
            "The source and destination array sizes don't match."); 
                
    // The types of arguments that the CE Parser will build for numeric 
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than 
    // just arguments.
    switch (dest->var()->type()) {
        case dods_byte_c:
            set_array_using_double_helper<dods_byte>(dest, src, src_len);
            break;
        case dods_uint16_c:
            set_array_using_double_helper<dods_uint16>(dest, src, src_len);
            break;
        case dods_int16_c:
            set_array_using_double_helper<dods_int16>(dest, src, src_len);
            break;
        case dods_uint32_c:
            set_array_using_double_helper<dods_uint32>(dest, src, src_len);
            break;
        case dods_int32_c:
            set_array_using_double_helper<dods_int32>(dest, src, src_len);
            break;
        case dods_float32_c:
            set_array_using_double_helper<dods_float32>(dest, src, src_len);
            break;
        case dods_float64_c:
            set_array_using_double_helper<dods_float64>(dest, src, src_len);
            break;
        default:
            throw InternalErr(__FILE__, __LINE__, 
               "The argument list built by the CE parser contained an unsupported numeric type.");
    }
    
    // Set the read_p property.
    dest->set_read_p(true);
}

template<class T> 
static double *
extract_double_array_helper(Array * a) throw(Error)
{
    int length = a->length();
    double *dest = new double[length];
    T *b = new T[length];
    a->buf2val((void **) &b);
    for (int i = 0; i < length; ++i)
        dest[i] = (double)b[i];
    delete[] b;
    return dest;
}

/** Given a pointer to an Array which holds a numeric type, extract the 
    values and return in an array of doubles. This function allocates the
    array using 'new double[n]' so delete[] can be used when you are done
    the data. */
double *
extract_double_array(Array *a) throw(Error)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ( a->type() == dods_array_c
         && !a->var()->is_simple_type() 
         || a->var()->type() == dods_str_c
         || a->var()->type() == dods_url_c )
        throw Error(malformed_expr, 
                    "The function requires a DAP numeric-type array argument.");
                    
    if (!a->read_p())
        throw InternalErr(__FILE__, __LINE__,
            string("The Array '") + a->name() + "'does not contain values.");
            
    // The types of arguments that the CE Parser will build for numeric 
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than 
    // just arguments.
    switch (a->var()->type()) {
        case dods_byte_c:
            return extract_double_array_helper<dods_byte>(a);
        case dods_uint16_c:
            return extract_double_array_helper<dods_uint16>(a);
        case dods_int16_c:
            return extract_double_array_helper<dods_int16>(a);
        case dods_uint32_c:
            return extract_double_array_helper<dods_uint32>(a);
        case dods_int32_c:
            return extract_double_array_helper<dods_int32>(a);
        case dods_float32_c:
            return extract_double_array_helper<dods_float32>(a);
        case dods_float64_c:
            return extract_double_array_helper<dods_float64>(a);
        default:
            throw InternalErr(__FILE__, __LINE__, 
               "The argument list built by the CE parser contained an unsupported numeric type.");
    }
}

/** Given a BaseType pointer, extract the numeric value it contains and return
    it in a C++ double. 
    
    @param arg The BaseType pointer
    @return A C++ double
    @exception Error thrown if the referenced BaseType object does not contain
    a DAP numeric value. */ 
double
extract_double_value(BaseType *arg) throw(Error)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ( !arg->is_simple_type() 
         || arg->type() == dods_str_c
         || arg->type() == dods_url_c )
        throw Error(malformed_expr, 
                    "The function requires a DAP numeric-type argument.");
                    
    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
            "The CE Evaluator built an argument list where some constants held no values.");
            
    // The types of arguments that the CE Parser will build for numeric 
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than 
    // just arguments.
    switch (arg->type()) {
        case dods_byte_c: {
            dods_byte i;
            dods_byte *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)(i);
        }
        case dods_uint16_c: {
            dods_uint16 i;
            dods_uint16 *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)(i);
        }
        case dods_int16_c: {
            dods_int16 i;
            dods_int16 *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)(i);
        }
        case dods_uint32_c: {
            dods_uint32 i;
            dods_uint32 *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)(i);
        }
        case dods_int32_c: {
            dods_int32 i;
            dods_int32 *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)(i);
        }
        case dods_float32_c: {
            dods_float32 i;
            dods_float32 *pi = & i;
            arg->buf2val((void **)&pi);
            return (double)i;
        }
        case dods_float64_c: {
            DBG(cerr << "arg->value(): " << dynamic_cast<Float64*>(arg)->value() << endl);
            dods_float64 i;
            dods_float64 *pi = & i;
            arg->buf2val((void **)&pi);
            DBG(cerr << "i: " << i << endl);
            return i;
        }
        default:
            throw InternalErr(__FILE__, __LINE__, 
               "The argument list built by the CE parser contained an unsupported numeric type.");
    }
}

#if 0
// In reality no server implements this; it _should_ be removed. 03/28/05 jhrg
BaseType *
func_length(int argc, BaseType *argv[], DDS &dds) throw(Error)
{
    if (argc != 1) {
	throw Error("Wrong number of arguments to length().");
    }
    
    switch (argv[0]->type()) {

      case dods_sequence_c: {
	  Sequence *var = dynamic_cast<Sequence *>(argv[0]);
	  if (!var)
	      throw Error("Expected a Sequence variable in length()");
	  dods_int32 result = var->length();
    
	  BaseType *ret = dds.get_factory()->NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);
    
	  return ret;
      }

      default:
	throw Error("Wrong type argument to length()");
    }

    return 0;
}
#endif

/** This server-side function returns 1. 
    This function is mostly for testing purposes. */
BaseType *
func_one(int argc, BaseType *argv[], DDS &dds) throw(Error)
{
    Byte *one = new Byte("one");
    (void) one->set_value(1);
    return one;
}


static void
parse_gse_expression(gse_arg *arg, BaseType *expr) throw(Error)
{
    gse_restart(0);		// Restart the scanner.
    void *cls = gse_string(extract_string_argument(expr).c_str());
    // gse_switch_to_buffer(cls); // Get set to scan the string.
    bool status = gse_parse((void *)arg) == 0;
    gse_delete_buffer(cls);
    if (!status)
	throw Error(malformed_expr, "Error parsing grid selection.");
}

/** The grid function uses a set of relational expressions to form a selection
    within a Grid variable based on the values in the Grid's map vectors.
    Thus, if a Grid has a 'temperature' map which ranges from 0.0 to 32.0
    degrees, it's possible to request the vlaues of the Grid that fall between
    10.5 and 12.5 degrees without knowing to which array indeces those values
    correspond. The function takes one or more arguments:<ul>
    <li>The name of a Grid.</li>
    <li>Zero or more strings which hold relational expressions of the form:<ul>
        <li><code>&lt;map var&gt; &lt;relop&gt; &lt;constant&gt;</code></li>
        <li><code>&lt;constant&gt; &lt;relop&gt; &lt;map var&gt; &lt;relop&gt; &lt;constant&gt;</code></li>
        </ul></li>
    </ul>
    
    Each of the relation expressions is applied to the Grid and the result is
    returned.

    @note Since this is a funcion and one of the arguments is the grid, the 
    grid is read (using the Grid::read() method) at the time the argument list
    is built.
    
    @todo In order to be used by geogrid() , this code may have to be modified 
    so that the maps and array are not re-read by the serialize() method. It 
    might also be a good idea to change from the '?grid(SST,"10<time<20")'
    syntax in a URL to '?SST&grid(SST,"10<time<20")' even though it's more
    verbose in the URL, it would make the function a true 'secection operator'
    and allow several grids to be returned with selections in one request.
    
    @param argc The number of values in argv.
    @param argv An array of BaseType pointers which hold the arguments to be
    passed to geogrid. The arguments may be Strings, Integers, or Reals, subject
    to the above constraints.
    @param dds The DDS which holds the Grid.
    @see geogrid() (func_geogrid_select) A function which has logic specific 
    to longitude/latitude selection. */
void 
projection_function_grid(int argc, BaseType *argv[], DDS &dds,
                         ConstraintEvaluator &) throw(Error)
{
    DBG(cerr << "Entering projection_function_grid..." << endl);

    if (argc < 1)
	throw Error("Wrong number of arguments to grid(), there must be at least one argument.");

    Grid *grid = dynamic_cast<Grid*>(argv[0]);
    if (!grid)
	throw Error("The first argument to grid() must be a Grid variable!");

    // Mark this grid as part of the current projection.
    if (!dds.mark(grid->name(), true))
	throw Error("Could not find the variable: " + grid->name());

    // argv[1..n] holds strings; each are little expressions to be parsed.
    // When each expression is parsed, the parser makes a new instance of
    // GSEClause. GSEClause checks to make sure the named map really exists
    // in the Grid and that the range of values given makes sense.
    vector<GSEClause *> clauses;
    gse_arg *arg = new gse_arg(grid);
    for (int i = 1; i < argc; ++i) {
	parse_gse_expression(arg, argv[i]);
	clauses.push_back(arg->get_gsec());
    }
    delete arg; arg = 0;

    // In this loop we have to iterate over the map vectors and the grid
    // dimensions at the same time and set the grid's array's constraint to
    // match that of the map vectors. Maybe we need an interface in Grid to
    // do this? 9/21/2001 jhrg
    Array *grid_array = grid->get_array();

    // Basic plan: For each map, look at each clause and set start and stop
    // to be the intersection of the ranges in those clauses.
    Grid::Map_iter piter = grid->map_begin() ;
    Array::Dim_iter grid_dim = grid_array->dim_begin() ;
    for (; piter != grid->map_end(); piter++, grid_dim++)
    {
        Array *map = dynamic_cast<Array *>((*piter)) ;
	string map_name = map->name();

	// a valid Grid Map is a vector.
	Array::Dim_iter qiter = map->dim_begin();

	int start = map->dimension_start(qiter);
	int stop = map->dimension_stop(qiter);

	vector<GSEClause*>::iterator cs_iter;
	for (cs_iter = clauses.begin(); cs_iter != clauses.end(); cs_iter++) {
	    GSEClause *gsec = *cs_iter;
	    if (gsec->get_map_name() == map_name) {
                DBG(cerr << "map_name: " << map_name << endl);
                DBG(cerr << "Map starts at: " << start << endl);
                DBG(cerr << "GSE Clause start: " << gsec->get_start() << endl);
                // Set start to the maximum of either its current value or the
                // value in current clause. At the end of this loop, start
                // should be the max value of any of the (matching) clause's
                // start values. This is necesary because one map may be named
                // in several clauses such as 'first>3' and 'first<7'. When the
                // second expression is evaluated the clause will have a start
                // value of 0 while the array will have a starting value 4 
                // (assume first = {0,1,2,...,9}). Note that the stop and start
                // values are the Map indices, not the values in the Maps, so
                // it makes sense to think of the start always being smaller 
                // than the stop and the indices monotonically increasing.
                start = max(start, gsec->get_start());

                stop = min(stop, gsec->get_stop());
                
		if (start > stop) {
                    ostringstream msg;
                    msg 
<< "The expresions passed to grid() do not result in an inclusive \n"
<< "subset of '" << gsec->get_map_name() << "'. The map's values range "
<< "from " << gsec->get_map_min_value() << " to " << gsec->get_map_max_value()
<< ".";
		    throw Error(msg.str());
		}
		// This map is constrained, set read_p so that during
		// serialization new values will be read according to the
		// constraint set here. 9/21/2001 jhrg
		map->set_read_p(false);
	    }
	}
                    		
	DBG(cerr << "Setting constraint on " << map->name() \
	    << "[" << start << ":" << stop << "]" << endl);

	// Stride is always one.
	Array::Dim_iter fd = map->dim_begin() ;
	map->add_constraint(fd, start, 1, stop);
	grid_array->add_constraint(grid_dim, start, 1, stop);
    }
    
    // Make sure we reread the grid's array, too. 9/24/2001 jhrg
    grid_array->set_read_p(false);

    DBG(cerr << "Exiting projection_function_grid." << endl);
}

BaseType *
function_grid(int argc, BaseType *argv[], DDS &dds) throw(Error)
{
}

/** The geogrid function returns the part of a Grid which includes a 
    geographically specified rectangle. The arguments to the function are
    the name of a Grid, the left-top and right-bottom points of the rectable
    and zero or more relational expressions of the sort that the grid frunction
    accepts. The constraints on the arguments are:<ul>
    <li>The Grid must have Latitude and Longitude map vectors. Those are 
    discovered by looking for map vectors which satisfy enough of any one of
    a set of conventions to make the identification of those map vectors 
    positive or by guessing which maps are which. The set of conventions
    supported is: COARDS, CF 1.0, GDT and CSC (see 
    http://www.unidata.ucar.edu/software/netcdf/conventions.html). If the 
    geogrid guesses at the maps, it adds an attribute (geogrid_warning) which
    says so. (in version 1.1)</li>
    <li>The rectangle corner points are in Longitude-Latitude. Longitude may be
    given using -180 to 180 or 0 to 360. For data sources with global coverage,
    geogrid assumes that the Longitude axis is circular. For requests made using
    0/359 notation, it assumes it is modulus 360. Requests made using -180/179
    notation cannot use values outside that range.</li>
    <li>The notation used to specify the rectangular region determines the
    notation used in the longitude/latitude map vectors of the Grid returned by
    the function.</li>
    <li>There are no restrictions on the relational expressions beyond those
    for the grid() (see func_grid_select()) function.</li>
    </ul>
    
    @note The geogrid() function is implemented as a 'BaseType function' which
    means that there can be only one function per request and no other variables
    may be named in the request.
    
    @param argc The number of values in argv.
    @param argv An array of BaseType pointers which hold the arguments to be
    passed to geogrid. The arguments may be Strings, Integers, or Reals, subject
    to the above constraints.
    @param dds The DDS which holds the Grid. This DDS \e must include
    attributes.
    @return The constrained and read Grid, ready to be sent. */
BaseType *
function_geogrid(int argc, BaseType *argv[], DDS &dds)
{
    if (argc < 5)
        throw Error("Wrong number of arguments to geogrid(), there must be at least five arguments,\n\
        A Grid followed by the left-top and right-bottom points of a longitude-latitude bounding box.");

    Grid *grid = dynamic_cast<Grid*>(argv[0]);
    if (!grid)
        throw Error("The first argument to geogrid() must be a Grid variable!");
        
    if (grid->get_array()->dimensions() > 3)
        throw Error("The geogrid() function works only with Grids of one to three dimensions.");

    // Mark this grid as part of the current projection.
    if (!dds.mark(grid->name(), true))
        throw Error("Could not find the variable: " + grid->name());
        
    // dup the grid before reading
    Grid *l_grid = dynamic_cast<Grid*>(grid->ptr_duplicate());
    if (!l_grid)
        throw InternalErr(__FILE__, __LINE__, "Expected a Grid.");
        
    // Build a GeoConstraint object. If there are no longitude/latitude maps
    // then this constructor throws an Error.
    GeoConstraint gc(l_grid, dds.get_dataset_name(), dds);
    
    // This sets the bounding box and modifies the maps to match the notation
    // of the box (0/359 or -180/179)
    double left = extract_double_value(argv[1]); 
    double top = extract_double_value(argv[2]);
    double right = extract_double_value(argv[3]);
    double bottom = extract_double_value(argv[4]);
    gc.set_bounding_box(left, top, right, bottom);
        
    gc.apply_constraint_to_data();
#if 0     
    l_grid->read(dds.get_dataset_name());
#endif
    return l_grid;        
}

void
register_functions(ConstraintEvaluator &ce)
{
    ce.add_function("grid", projection_function_grid);
    ce.add_function("geogrid", function_geogrid);
    ce.add_function("one", func_one);    
}

} // namespace libdap
