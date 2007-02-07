
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

static char rcsid[] not_used =
    { "$Id$" };

#include <errno.h>      // used by strtod()
#include <limits.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <algorithm>

//#define DODS_DEBUG

#include "BaseType.h"
#include "Array.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"
#include "RValue.h"

#include "GSEClause.h"
#include "GridGeoConstraint.h"
#include "ArrayGeoConstraint.h"

#include "ce_functions.h"
#include "gse_parser.h"
#include "gse.tab.h"
#include "debug.h"
#include "util.h"

//  We wrapped VC++ 6.x strtod() to account for a short comming
//  in that function in regards to "NaN".  I don't know if this
//  still applies in more recent versions of that product.
//  ROM - 12/2007
#ifdef WIN32
#include <limits>
double w32strtod(const char *,char **);
#endif
using std::vector;

int gse_parse(void *arg);
void gse_restart(FILE * in);

// Glue routines declared in gse.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void *buffer);
void *gse_string(const char *yy_str);

namespace libdap
{

/** Is \e lhs equal to \e rhs? Use epsilon to determine equality. */
inline bool
double_eq(double lhs, double rhs, double epsilon = 1.0e-5)
{
    return fabs(lhs - rhs) < (fabs(lhs + rhs) / epsilon);
}
 
/** Given a BaseType pointer, extract the string value it contains and return
    it.

    @param arg The BaseType pointer
    @return A C++ string
    @exception Error thrown if the referenced BaseType object does not contain
    a DAP String. */
string extract_string_argument(BaseType * arg)
{
    if (arg->type() != dods_str_c)
        throw Error(malformed_expr,
                    "The function requires a DAP string-type argument.");

    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
                          "The CE Evaluator built an argument list where some constants held no values.");

    string *sp = 0;
    arg->buf2val((void **) &sp);
    string s = *sp;
    delete sp;
    sp = 0;

    DBG(cerr << "s: " << s << endl);

    return s;
}
template < class T >
static void
set_array_using_double_helper(Array * a, double *src,
                              int src_len)
{
    T *values = new T[src_len];
    for (int i = 0; i < src_len; ++i)
        values[i] = (T) src[i];
    a->val2buf(values, true);
    delete[]values;
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
void set_array_using_double(Array * dest, double *src, int src_len)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if (dest->type() == dods_array_c && !dest->var()->is_simple_type()
        || dest->var()->type() == dods_str_c
        || dest->var()->type() == dods_url_c)
        throw InternalErr(__FILE__, __LINE__,
                          "The function requires a DAP numeric-type array argument.");

    // Test sizes. Note that Array::length() takes any constraint into account
    // when it returns the length. Even if this was removed, the 'helper'
    // function this uses calls Vector::val2buf() which uses Vector::width()
    // which in turn uses length().
    if (dest->length() != src_len)
        throw InternalErr(__FILE__, __LINE__,
                          "The source and destination array sizes don't match ("
                          + long_to_string(src_len) + " versus "
                          + long_to_string(dest->length()) + ").");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (dest->var()->type()) {
        case dods_byte_c:
            set_array_using_double_helper < dods_byte > (dest, src,
                    src_len);
            break;
        case dods_uint16_c:
            set_array_using_double_helper < dods_uint16 > (dest, src,
                    src_len);
            break;
        case dods_int16_c:
            set_array_using_double_helper < dods_int16 > (dest, src,
                    src_len);
            break;
        case dods_uint32_c:
            set_array_using_double_helper < dods_uint32 > (dest, src,
                    src_len);
            break;
        case dods_int32_c:
            set_array_using_double_helper < dods_int32 > (dest, src,
                    src_len);
            break;
        case dods_float32_c:
            set_array_using_double_helper < dods_float32 > (dest, src,
                    src_len);
            break;
        case dods_float64_c:
            set_array_using_double_helper < dods_float64 > (dest, src,
                    src_len);
            break;
        default:
            throw InternalErr(__FILE__, __LINE__,
                              "The argument list built by the CE parser contained an unsupported numeric type.");
    }

    // Set the read_p property.
    dest->set_read_p(true);
}

template < class T >
static double *extract_double_array_helper(Array * a)
{
    int length = a->length();
    double *dest = new double[length];
    T *b = new T[length];
    a->buf2val((void **) &b);
    for (int i = 0; i < length; ++i)
        dest[i] = (double) b[i];
    delete[]b;
    return dest;
}

/** Given a pointer to an Array which holds a numeric type, extract the
    values and return in an array of doubles. This function allocates the
    array using 'new double[n]' so delete[] can be used when you are done
    the data. */
double *extract_double_array(Array * a)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if (a->type() == dods_array_c && !a->var()->is_simple_type()
        || a->var()->type() == dods_str_c
        || a->var()->type() == dods_url_c)
        throw Error(malformed_expr,
                    "The function requires a DAP numeric-type array argument.");

    if (!a->read_p())
        throw InternalErr(__FILE__, __LINE__,
                          string("The Array '") + a->name() +
                          "'does not contain values.");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (a->var()->type()) {
        case dods_byte_c:
            return extract_double_array_helper < dods_byte > (a);
        case dods_uint16_c:
            return extract_double_array_helper < dods_uint16 > (a);
        case dods_int16_c:
            return extract_double_array_helper < dods_int16 > (a);
        case dods_uint32_c:
            return extract_double_array_helper < dods_uint32 > (a);
        case dods_int32_c:
            return extract_double_array_helper < dods_int32 > (a);
        case dods_float32_c:
            return extract_double_array_helper < dods_float32 > (a);
        case dods_float64_c:
            return extract_double_array_helper < dods_float64 > (a);
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
double extract_double_value(BaseType * arg)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if (!arg->is_simple_type()
        || arg->type() == dods_str_c || arg->type() == dods_url_c)
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
            dods_byte *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) (i);
        }
        case dods_uint16_c: {
            dods_uint16 i;
            dods_uint16 *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) (i);
        }
        case dods_int16_c: {
            dods_int16 i;
            dods_int16 *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) (i);
        }
        case dods_uint32_c: {
            dods_uint32 i;
            dods_uint32 *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) (i);
        }
        case dods_int32_c: {
            dods_int32 i;
            dods_int32 *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) (i);
        }
        case dods_float32_c: {
            dods_float32 i;
            dods_float32 *pi = &i;
            arg->buf2val((void **) &pi);
            return (double) i;
        }
        case dods_float64_c: {
            DBG(cerr << "arg->value(): " << dynamic_cast <
                Float64 * >(arg)->value() << endl);
            dods_float64 i;
            dods_float64 *pi = &i;
            arg->buf2val((void **) &pi);
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
BaseType *func_length(int argc, BaseType * argv[], DDS & dds)
{
    if (argc != 1) {
        throw Error("Wrong number of arguments to length().");
    }

    switch (argv[0]->type()) {

        case dods_sequence_c: {
            Sequence *var = dynamic_cast < Sequence * >(argv[0]);
            if (!var)
                throw
                Error("Expected a Sequence variable in length()");
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

/** This server-side function returns version information for the server-side
    functions. */
BaseType *function_version(int argc, BaseType * argv[], DDS &,
                   const string &)
{
    string help = "\
Usage: version() returns plain text information about the available functions.\
       version(\"xml\") returns the informationin an XML 1.0 document.\
       version(\"help\") returns this text.";

    string value = "\
Function set: version 1.0, grid 1.0, geogrid 1.0b2, geoarray 0.9b1, linear_scale 1.0b1";

    string xml_value = "<?xml version=\"1.0\"?>\
    <functions>\
    <function name=\"version\" version=\"1.0\"/>\
    <function name=\"grid\" version=\"1.0\"/>\
    <function name=\"geogrid\" version=\"1.0\"/>\
    <function name=\"geoarray\" version=\"1.0\"/>\
    <function name=\"linear_scale\" version=\"1.0\"/>\
    </functions>";

    Str *response = new Str("version");
    
    if (argc == 1 && extract_string_argument(argv[0]) == "help")
        response->set_value(help);
    else if (argc == 1 && extract_string_argument(argv[0]) == "xml")
        response->set_value(xml_value);
    else
        response->set_value(value);
        
    return response;
}

static void parse_gse_expression(gse_arg * arg, BaseType * expr)
{
    gse_restart(0);         // Restart the scanner.
    void *cls = gse_string(extract_string_argument(expr).c_str());
    // gse_switch_to_buffer(cls); // Get set to scan the string.
    bool status = gse_parse((void *) arg) == 0;
    gse_delete_buffer(cls);
    if (!status)
        throw Error(malformed_expr, "Error parsing grid selection.");
}

static void apply_grid_selection_expr(Grid * grid, GSEClause * clause)
{
    // Basic plan: For each map, look at each clause and set start and stop
    // to be the intersection of the ranges in those clauses.
    Grid::Map_iter map_i = grid->map_begin();
    while (map_i != grid->map_end()
           && (*map_i)->name() != clause->get_map_name())
        ++map_i;

    if (map_i == grid->map_end())
        throw Error("The map vector '" + clause->get_map_name()
                    + "' is not in the grid '" + grid->name() + "'.");

    // Use pointer arith & the rule that map order must match array dim order
    Array::Dim_iter grid_dim = (grid->get_array()->dim_begin()
                                + (map_i - grid->map_begin()));

    Array *map = dynamic_cast < Array * >((*map_i));
    int start =
        max(map->dimension_start(map->dim_begin()),
            clause->get_start());
    int stop =
        min(map->dimension_stop(map->dim_begin()), clause->get_stop());

    if (start > stop) {
        ostringstream msg;
        msg <<
        "The expresions passed to grid() do not result in an inclusive \n"
        << "subset of '" << clause->get_map_name()
        << "'. The map's values range " << "from " <<
        clause->get_map_min_value() << " to "
        << clause->get_map_max_value() << ".";
        throw Error(msg.str());
    }

    DBG(cerr << "Setting constraint on " << map->name()
        << "[" << start << ":" << stop << "]" << endl);

    // Stride is always one.
    map->add_constraint(map->dim_begin(), start, 1, stop);
    grid->get_array()->add_constraint(grid_dim, start, 1, stop);
}

static void
apply_grid_selection_expressions(Grid * grid,
                                 vector < GSEClause * >clauses)
{
    vector < GSEClause * >::iterator clause_i = clauses.begin();
    while (clause_i != clauses.end())
        apply_grid_selection_expr(grid, *clause_i++);

    grid->set_read_p(false);
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
        <li><code>&lt;constant&gt; &lt;relop&gt; &lt;map var&gt; &lt;relop&gt; 
                  &lt;constant&gt;</code></li>
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
    @param dataset Name of the dataset.
    @see geogrid() (func_geogrid_select) A function which has logic specific
    to longitude/latitude selection. */
BaseType *function_grid(int argc, BaseType * argv[], DDS &,
                        const string & dataset)
{
    DBG(cerr << "Entering function_grid..." << endl);

    if (argc < 1)
        throw Error(
"Wrong number of arguments to grid(), there must be at least one argument.");

    Grid *original_grid = dynamic_cast < Grid * >(argv[0]);
    if (!original_grid)
        throw Error("The first argument to grid() must be a Grid variable!");

    // Duplicate the grid; DODSFilter::send_data() will delete the variable
    // after serializing it.
    Grid *l_grid =
        dynamic_cast < Grid * >(original_grid->ptr_duplicate());
    
    DBG(cerr << "grid: past initialization code" << endl);

    // Read the maps. Do this before calling parse_gse_expression(). Avoid
    // reading the array until the constraints have been applied because it
    // might be really large.
    
    // This version makes sure to set the send_p flags which is needed for
    // the hdf4 handler (and is what should be done in general).
    Grid::Map_iter i = l_grid->map_begin();
    while (i != l_grid->map_end())
        (*i++)->set_send_p(true);
    l_grid->read(dataset);
    
#if 0
    // Old, pre-hdf_handler compatible version.
    Grid::Map_iter i = l_grid->map_begin();
    while (i != l_grid->map_end())
        (*i++)->read(dataset);
#endif
    DBG(cerr << "grid: past map read" << endl);

    // argv[1..n] holds strings; each are little expressions to be parsed.
    // When each expression is parsed, the parser makes a new instance of
    // GSEClause. GSEClause checks to make sure the named map really exists
    // in the Grid and that the range of values given makes sense.
    vector < GSEClause * >clauses;
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

    l_grid->read(dataset);

    return l_grid;
}

/** The geogrid function returns the part of a Grid which includes a
    geographically specified rectangle. The arguments to the function are the
    name of a Grid, the left-top and right-bottom points of the rectable and
    zero or more relational expressions of the sort that the grid frunction
    accepts. The constraints on the arguments are:<ul> <li>The Grid must have
    Latitude and Longitude map vectors. Those are discovered by looking for
    map vectors which satisfy enough of any one of a set of conventions to
    make the identification of those map vectors positive or by guessing
    which maps are which. The set of conventions supported is: COARDS, CF
    1.0, GDT and CSC (see
    http://www.unidata.ucar.edu/software/netcdf/conventions.html). If the
    geogrid guesses at the maps, it adds an attribute (geogrid_warning) which
    says so. (in version 1.1)</li> <li>The rectangle corner points are in
    Longitude-Latitude. Longitude may be given using -180 to 180 or 0 to 360.
    For data sources with global coverage, geogrid assumes that the Longitude
    axis is circular. For requests made using 0/359 notation, it assumes it
    is modulus 360. Requests made using -180/179 notation cannot use values
    outside that range.</li> <li>The notation used to specify the rectangular
    region determines the notation used in the longitude/latitude map vectors
    of the Grid returned by the function.</li> <li>There are no restrictions
    on the relational expressions beyond those for the grid() (see
    func_grid_select()) function.</li> </ul>

    @note The geogrid() function is implemented as a 'BaseType function'
    which means that there can be only one function per request and no other
    variables may be named in the request.

    @param argc The number of values in argv.
    @param argv An array of BaseType pointers which hold the arguments to be
    passed to geogrid. The arguments may be Strings, Integers, or Reals,
    subject to the above constraints.
    @param dds The DDS which holds the Grid. This DDS \e must include
    attributes.
    @param dataset Name of the dataset.
    @return The constrained and read Grid, ready to be sent. */
BaseType *function_geogrid(int argc, BaseType * argv[], DDS &,
                           const string & dataset)
{
    if (argc < 5)
        throw
            Error
            ("Wrong number of arguments to geogrid(), there must be at least five\n\
arguments, A Grid followed by the left-top and right-bottom points of a\n\
longitude-latitude bounding box.");

    Grid *l_grid = dynamic_cast < Grid * >(argv[0]->ptr_duplicate());
    if (!l_grid)
        throw Error(
"The first argument to geogrid() must be a Grid variable!");

    // Read the maps. Do this before calling parse_gse_expression(). Avoid
    // reading the array until the constraints have been applied because it
    // might be really large.
    //
    // Trick: Some handlers build Grids from a combination of Array
    // variables and attributes. Those handlers (e.g., hdf4) use the send_p
    // property to determine which parts of the Grid to read *but they can
    // only read the maps from within Grid::read(), not the map's read()*.
    // Since the Grid's array does not have send_p set, it will not be read
    // by the call below to Grid::read().
    Grid::Map_iter i = l_grid->map_begin();
    while (i != l_grid->map_end())
        (*i++)->set_send_p(true);
    l_grid->read(dataset);
    // Calling read() above sets the read_p flag for the entire grid; clear it
    // for the grid's array so that later on the code will be sure to read it
    // under all circumstances.
    l_grid->get_array()->set_read_p(false);
    DBG(cerr << "geogrid: past map read" << endl);

    // Look for Grid Selection Expressions tacked onto the end of the BB
    // specification. If there are any, evaluate them before evaluating the BB.
    if (argc > 5) {
        // argv[5..n] holds strings; each are little Grid Selection Expressions
        // to be parsed and evaluated.
        vector < GSEClause * >clauses;
        gse_arg *arg = new gse_arg(l_grid);
        for (int i = 5; i < argc; ++i) {
            parse_gse_expression(arg, argv[i]);
            clauses.push_back(arg->get_gsec());
        }
        delete arg;
        arg = 0;

        apply_grid_selection_expressions(l_grid, clauses);
    }

    try {
        // Build a GeoConstraint object. If there are no longitude/latitude
        // maps then this constructor throws Error.
        GridGeoConstraint gc(l_grid, dataset);

        // This sets the bounding box and modifies the maps to match the
        // notation of the box (0/359 or -180/179)
        double top = extract_double_value(argv[1]);
        double left = extract_double_value(argv[2]);
        double bottom = extract_double_value(argv[3]);
        double right = extract_double_value(argv[4]);
        gc.set_bounding_box(left, top, right, bottom);
        DBG(cerr << "geogrid: past bounding box set" << endl);

        // This also reads all of the data into the grid variable
        gc.apply_constraint_to_data();
        DBG(cerr << "geogrid: past apply constraint" << endl);

        // In this function the l_grid pointer is the same as the pointer returned
        // by this call. The caller of the function must free the pointer.
        return gc.get_constrained_grid();
    }
    catch(Error & e) {
        throw;
    }
    catch(exception & e) {
        throw
            InternalErr(string
                        ("A C++ exception was thrown from inside geogrid(): ")
                        + e.what());

    }
}

// These static functions could be moved to a class that provides a more
// general interface for COARDS/CF someday. Assume each BaseType comes bundled
// with an attribute table.

// This was ripped from parser-util.cc
static double
string_to_double(const char *val)
{
    char *ptr;
    errno = 0;                  // Clear previous value. 5/21/2001 jhrg

#ifdef WIN32
    double v = w32strtod(val, &ptr);
#else
    double v = strtod(val, &ptr);
#endif

    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE))
        || *ptr != '\0') {
        throw Error(string("Could not convert the string '") + val + "' to a double.");
    }

    double abs_val = fabs(v);
    if (abs_val > DODS_DBL_MAX || (abs_val != 0.0 && abs_val < DODS_DBL_MIN))
        throw Error(string("Could not convert the string '") + val + "' to a double.");

    return v;
}

static double
get_attribute_double_value(BaseType *var, vector<string> &attributes)
{
    AttrTable &attr = var->get_attr_table();
    string attribute_value = "";
    string values = "";
    vector<string>::iterator i = attributes.begin();
    while (attribute_value == "" && i != attributes.end()) {
        values += *i; if (!values.empty()) values += ", ";
        attribute_value = attr.get_attr(*i++);
    }
    
    // If the value string is empty, then look at the grid's array (if it's a
    // grid or throw an Error.
    if (attribute_value.empty()) {
        if (var->type() == dods_grid_c)
            return get_attribute_double_value(dynamic_cast<Grid&>(*var).get_array(), attributes);
        else
            throw Error(string("No COARDS '") + values
                        + "' attribute was found for the variable '"
                        + var->name() + "'.");
    }
                    
    return string_to_double(attribute_value.c_str());    
}

static double
get_attribute_double_value(BaseType *var, const string &attribute)
{
    AttrTable &attr = var->get_attr_table();
    string attribute_value = attr.get_attr(attribute);
    
    // If the value string is empty, then look at the grid's array (if it's a
    // grid or throw an Error.
    if (attribute_value.empty()) {
        if (var->type() == dods_grid_c)
            return get_attribute_double_value(dynamic_cast<Grid&>(*var).get_array(), attribute);
        else
            throw Error(string("No COARDS '") + attribute 
                        + "' attribute was found for the variable '"
                        + var->name() + "'.");
    }
                    
    return string_to_double(attribute_value.c_str());    
}

static double
get_y_intercept(BaseType *var)
{
    vector<string> attributes;
    attributes.push_back("add_offset");
    attributes.push_back("add_off");
    return get_attribute_double_value(var, attributes);
}

static double
get_slope(BaseType *var)
{
    return get_attribute_double_value(var, "scale_factor");
}

static double
get_missing_value(BaseType *var)
{
    return get_attribute_double_value(var, "missing_value");
}

/** Given a BaseType, scale it using 'y = mx + b'. Either provide the 
    constants 'm' and 'b' or the function will look for the COARDS attributes
    'scale_factor' and 'add_offset'.
    
    @param argc
    @param argv
    @param dds
    @param dataset
    @return The scaled variable, represented using Float64
    @exception Error Thrown if either constants are not given and COARDS 
    attributes for them cannot be found OR if the source variable is not a
    numeric scalar, Array or Grid. */
BaseType *
function_linear_scale(int argc, BaseType * argv[], DDS &, const string & dataset)
{
    // Check for 1 or 3 arguments: 1 --> use attributes; 3 --> m & b supplied
    DBG(cerr << "argc = " << argc << endl);
    if ( !(argc == 1 || argc == 3 || argc == 4) )
        throw Error(

"Wrong number of arguments to linear_scale(). There must be either one or\n\
three arguments. If one argument is given, it must be the name of a variable\n\
to scale (COARDS attributes will be used to determine the slope and\n\
y-intercept). If three arguments are given, then the second and third are\n\
assumed to be the slope and y-intercept.");

    // Get m & b
    bool use_missing;
    double m, b, missing;
    if (argc == 4) {
        m = extract_double_value(argv[1]);
        b = extract_double_value(argv[2]);
        missing = extract_double_value(argv[3]);
        use_missing = true;
    }
    else if (argc == 3) {
        m = extract_double_value(argv[1]);
        b = extract_double_value(argv[2]);
        use_missing = false;
    }
    else {
        m = get_slope(argv[0]);
        b = get_y_intercept(argv[0]);
        // This is not the best plan; the get_missing_value() function should
        // do something other than throw, but to do that would require mayor
        // surgery on get_attribute_double_value().
        try {
            missing = get_missing_value(argv[0]);
            use_missing = true;
        }
        catch (Error &e) {
            use_missing = false;
        }
    }
    
    DBG(cerr << "m: " << m << ", b: " << b << endl);
    DBG(cerr << "use_missing: " << use_missing << ", missing: " << missing << endl);
    
    // Read the data, scale and return the result. Must replace the new data
    // in a constructor (i.e., Array part of a Grid).
    BaseType *dest = 0;
    double *data;
    if (argv[0]->type() == dods_grid_c) {
        Array &source = *dynamic_cast<Grid&>(*argv[0]).get_array();
        source.set_send_p(true);
        source.read(dataset);
        data = extract_double_array(&source);
        int length = source.length();
        int i = 0;
        while (i < length) {
            DBG2(cerr << "data[" << i << "]: " << data[i] << endl);
            if (!use_missing || !double_eq(data[i], missing))
                data[i] = data[i] * m + b;
            DBG2(cerr << " >> data[" << i << "]: " << data[i] << endl);
            ++i;
        }

        // Vector::add_var will delete the existing 'template' variable
        Float64 *temp_f = new Float64(source.name());
        source.add_var(temp_f);
        source.val2buf(static_cast<void*>(data), false);
        delete temp_f;              // add_var copies and then adds.
        dest = argv[0];
    }
    else if (argv[0]->is_vector_type()) {
        Array &source = dynamic_cast<Array&>(*argv[0]);
        source.set_send_p(true);
        // If the array is really a map, make sure to read using the Grid 
        // because of the HDF4 handler's odd behavior WRT dimensions.
        if (source.get_parent()->type() == dods_grid_c)
            source.get_parent()->read(dataset);
        else
            source.read(dataset);        
        data = extract_double_array(&source);
        int length = source.length();
        int i = 0;
        while (i < length) {
            if (!use_missing || !double_eq(data[i], missing))
                data[i] = data[i] * m + b;
            ++i;
        }
        
        Float64 *temp_f = new Float64(source.name());
        source.add_var(temp_f);
        source.val2buf(static_cast<void*>(data), false);
        delete temp_f;              // add_var copies and then adds.
        
        dest = argv[0];
    }
    else if (argv[0]->is_simple_type()
             && !(argv[0]->type() == dods_str_c 
                  || argv[0]->type() == dods_url_c)) {
        double data = extract_double_value(argv[0]);
        if (!use_missing || !double_eq(data, missing))
            data = data * m + b;
        
        dest = new Float64(argv[0]->name());
        dest->val2buf(static_cast<void*>(&data));
    }
    else {
        throw Error("The linear_scale() function works only for numeric Grids, Arrays and scalars."); 
    }
    
    return dest;
}

/** Perform a selection on the array using geographical coordinates. This 
    function takes several groups of arguments. 
    <ul>
    <li>geoarray(var, top, left, bottom, right)</li>
    <li>geoarray(var, top, left, bottom, right, var_top, v_left, v_bottom, v_right)</li>
    <li>geoarray(var, top, left, bottom, right, var_top, v_left, v_bottom, v_right, projection, datum)</li>
    </ul>
    
    @note Only the plat-carre projection and wgs84 datum are currently
    supported.
    @param argc
    @param argv
    @param dds
    @param dataset
    @return The Array, constrained by the selection
    @exception Error Thrown if thins go awry. */
BaseType *
function_geoarray(int argc, BaseType * argv[], DDS &, const string & dataset)
{
    // Check for 1 or 3 arguments: 1 --> use attributes; 3 --> m & b supplied
    DBG(cerr << "argc = " << argc << endl);
    if ( !(argc == 5 || argc == 9 || argc == 11) )
        throw Error(

"Wrong number of arguments to geoarray(). The geoarray() function supports\n\
three different sets of arguments:\n\
\n\
geoarray(var,left,top,right,bottom)\n\
geoarray(var,left,top,right,bottom,var_left,var_top,var_right,var_bottom)\n\
geoarray(var,left,top,right,bottom,var_left,var_top,var_right,var_bottom,projection,datum)\n\
\n\
In the first version 'var' is the target of the selection and 'left', 'top',\n\
'right' and 'bottom' are the corners of a longitude-latitude box that defines\n\
the selection. In the second version the 'var_left', ..., parameters give the\n\
longitude and latitude extent of the entire array. The projection and dataum are\n\
assumed to be Plat-Carre and wgs84. In the last version the the projection and\n\
datum of the image are also provided.");

    // Check the Array (and dup because the caller will free the variable).
    Array *l_array = dynamic_cast < Array * >(argv[0]->ptr_duplicate());
    if (!l_array)
        throw Error(
"The first argument to geoarray() must be an Array variable!");

    try {
        
    // Read the bounding box and variable extents from the params
    double bb_top = extract_double_value(argv[1]);
    double bb_left = extract_double_value(argv[2]);
    double bb_bottom = extract_double_value(argv[3]);
    double bb_right = extract_double_value(argv[4]);
    
    ArrayGeoConstraint *agc = 0;
    switch (argc) {
        case 5:
            agc = new ArrayGeoConstraint(l_array, dataset);
            break;
        case 9: {
            double var_top = extract_double_value(argv[5]);
            double var_left = extract_double_value(argv[6]);
            double var_bottom = extract_double_value(argv[7]);
            double var_right = extract_double_value(argv[8]);
            agc = new ArrayGeoConstraint(l_array, dataset,
                                         var_left, var_top, var_right, var_bottom);
            break;
        }
        case 11: {
            double var_top = extract_double_value(argv[5]);
            double var_left = extract_double_value(argv[6]);
            double var_bottom = extract_double_value(argv[7]);
            double var_right = extract_double_value(argv[8]);
            string projection = extract_string_argument(argv[9]);
            string datum = extract_string_argument(argv[10]);
            agc = new ArrayGeoConstraint(l_array, dataset,
                                         var_left, var_top, var_right, var_bottom,
                                         projection, datum);
            break;
        }
        default:
            throw InternalErr(__FILE__, __LINE__, "Wrong number of args to geoarray.");
    }

        agc->set_bounding_box(bb_left, bb_top, bb_right, bb_bottom);

        // This also reads all of the data into the grid variable
        agc->apply_constraint_to_data();

        // In this function the l_grid pointer is the same as the pointer returned
        // by this call. The caller of the function must free the pointer.
        return agc->get_constrained_array();
    }
    catch(Error & e) {
        throw;
    }
    catch(exception & e) {
        throw
            InternalErr(string
                        ("A C++ exception was thrown from inside geoarray(): ")
                        + e.what());

    }
}

void register_functions(ConstraintEvaluator & ce)
{
    ce.add_function("grid", function_grid);
    ce.add_function("geogrid", function_geogrid);
    ce.add_function("linear_scale", function_linear_scale);
    ce.add_function("geoarray", function_geoarray);
    ce.add_function("version", function_version);
}

}                               // namespace libdap
