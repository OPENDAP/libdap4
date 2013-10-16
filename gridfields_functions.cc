// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2011,2012 OPeNDAP, Inc.
// Authors: James Gallagher <jgallagher@opendap.org>
//         Scott Moe <smeest1@gmail.com>
//         Bill Howe <billhowe@cs.washington.edu>
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

// NOTE: This file is built only when the gridfields library is linked with
// the netcdf_handler (i.e., the handler's build is configured using the
// --with-gridfields=... option to the 'configure' script).
#if 0 // Disabling cruft code from earlier ugrid work - ndp 03/25/2013

#include "config.h"

#include <limits.h>

#include <cstdlib>      // used by strtod()
#include <cerrno>
#include <cmath>
#include <iostream>
#include <sstream>

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
#include "util.h"

#include <gridfields/restrict.h>
#include <gridfields/gridfield.h>
#include <gridfields/grid.h>
#include <gridfields/cell.h>
#include <gridfields/cellarray.h>
#include <gridfields/array.h>
#include <gridfields/implicit0cells.h>
#include <gridfields/gridfieldoperator.h>

//  We wrapped VC++ 6.x strtod() to account for a short coming
//  in that function in regards to "NaN".  I don't know if this
//  still applies in more recent versions of that product.
//  ROM - 12/2007
#ifdef WIN32
#include <limits>
double w32strtod(const char *, char **);
#endif

using namespace std;
using namespace libdap;

namespace libdap {

/** Given a BaseType pointer, extract the string value it contains and return
 it.

 @param arg The BaseType pointer
 @return A C++ string
 @exception Error thrown if the referenced BaseType object does not contain
 a DAP String. */
static string extract_string_argument(BaseType * arg)
{
    if (arg->type() != dods_str_c)
        throw Error(malformed_expr, "The function requires a DAP string argument.");

    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
                "The CE Evaluator built an argument list where some constants held no values.");

    Str &dapString = dynamic_cast<Str&>(*arg);
    string s = dapString.value();

    DBG(cerr << "extract_string_argument() - s: " << s << endl);

    return s;
}

template<class T>
static void set_array_using_double_helper(Array * a, double *src, int src_len)
{
    T *values = new T[src_len];
    for (int i = 0; i < src_len; ++i)
        values[i] = (T) src[i];

    a->set_value(values, src_len);

    delete[] values;
}

template<typename DODS, typename T>
static T *extract_array_helper(Array *a)
{
    int length = a->length();

    DBG(cerr << "extract_array_helper() - " << "Allocating: " << length << endl);
    DODS *b = new DODS[length];


    DBG(cerr << "extract_array_helper() - "  << "Assigning value." << endl);
    a->value(b);

    DBG(cerr << "extract_array_helper() - "  << "Array values extracted.  Casting/Copying..." << endl);
    T *dest = new T[length];

    for (int i = 0; i < length; ++i)
        dest[i] = (T) b[i];
    delete[]b;

    DBG(cerr << "extract_array_helper() - "  << "Returning extracted values." << endl);

    return dest;
}

/**
 * Extract data from a DAP array and return those values in a gridfields
 * array. This function sets the \e send_p property of the DAP Array and
 * uses its \e read() member function to get values. Thus, it should work
 * for values stored in any type of data source (e.g., file) for which the
 * Array class has been specialized.
 *
 * @param a The DAP Array. Extract values from this array
 * @return A GF::Array
 */
static GF::Array *extract_gridfield_array(Array *a) {
    if ((a->type() == dods_array_c && !a->var()->is_simple_type())
            || a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
    throw Error(malformed_expr,
            "The function requires a DAP numeric-type array argument.");

    a->set_send_p(true);
    a->read();

    // Construct a GridField array from a DODS array
    GF::Array *gfa;

    switch (a->var()->type()) {
        case dods_byte_c:
            gfa = new GF::Array(a->var()->name(), GF::INT);
            gfa->shareIntData(extract_array_helper<dods_byte, int>(a), a->length());
            break;
        case dods_uint16_c:
            gfa = new GF::Array(a->var()->name(), GF::INT);
            gfa->shareIntData(extract_array_helper<dods_uint16, int>(a), a->length());
            break;
        case dods_int16_c:
            gfa = new GF::Array(a->var()->name(), GF::INT);
            gfa->shareIntData(extract_array_helper<dods_int16, int>(a), a->length());
            break;
        case dods_uint32_c:
            gfa = new GF::Array(a->var()->name(), GF::INT);
            gfa->shareIntData(extract_array_helper<dods_uint32, int>(a), a->length());
            break;
        case dods_int32_c:
            gfa = new GF::Array(a->var()->name(), GF::INT);
            gfa->shareIntData(extract_array_helper<dods_int32, int>(a), a->length());
            break;
        case dods_float32_c:
            gfa = new GF::Array(a->var()->name(), GF::FLOAT);
            gfa->shareFloatData(extract_array_helper<dods_float32, float>(a), a->length());
            break;
        case dods_float64_c:
            gfa = new GF::Array(a->var()->name(), GF::FLOAT);
            gfa->shareFloatData(extract_array_helper<dods_float64, float>(a), a->length());
            break;
        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown DAP type encountered when converting to gridfields array");
    }
    return gfa;
};

/*
 If the array has the exact dimensions in the vector dims, in the same order,
 return true.  Otherwise return false.

 */
static bool same_dimensions(Array *arr, vector<Array::dimension> &dims) {
    vector<Array::dimension>::iterator dit;
    Array::Dim_iter ait;
    DBG(cerr << "same_dimensions test for array " << arr->name() << endl);
    DBG(cerr << "  array dims: ");
    for (ait = arr->dim_begin(); ait!=arr->dim_end(); ++ait) {
        DBG(cerr << (*ait).name << ", ");
    }
    DBG(cerr << endl);
    DBG(cerr << "  rank dims: ");
    for (dit = dims.begin(); dit!=dims.end(); ++dit) {
        DBG(cerr << (*dit).name << ", " << endl);
        for (ait = arr->dim_begin(); ait!=arr->dim_end(); ++ait) {
            Array::dimension dd = *dit;
            Array::dimension ad = *ait;
            DBG(cout<<dd.name<<" "<<ad.name<<" "<<dd.size<<" "<<ad.size<<endl);
            if (dd.name != ad.name
                    or dd.size != ad.size
                    or dd.stride != ad.stride
                    or dd.stop != ad.stop)
            return false;
        }
        DBG(cerr << endl);
    }
    return true;
}


/** Given a pointer to an Array that holds a numeric type, extract the
 values and return in an array of T. This function allocates the
 array using 'new T[n]' so delete[] can be used when you are done
 the data. */
template<typename T>
static T *extract_array(Array * a)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if ((a->type() == dods_array_c && !a->var()->is_simple_type())
            || a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
    throw Error(malformed_expr,
            "The function requires a DAP numeric-type array argument.");

    a->set_send_p(true);
    a->read();
    // This test should never pass due to the previous two lines; 
    // reading here seems to make 
    // sense rather than letting the caller forget to do so.
    // is read() idemopotent?
    if (!a->read_p())
    throw InternalErr(__FILE__, __LINE__,
            string("The Array '") + a->name() +
            "'does not contain values. send_read_p() not called?");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (a->var()->type()) {
        case dods_byte_c:
        return extract_array_helper<dods_byte, T>(a);

        case dods_uint16_c:
        DBG(cerr << "extract_array() - " << "dods_uint32_c" << endl);
        return extract_array_helper<dods_uint16, T>(a);

        case dods_int16_c:
        DBG(cerr << "extract_array() - " << "dods_int16_c" << endl);
        return extract_array_helper<dods_int16, T>(a);

        case dods_uint32_c:
        DBG(cerr << "extract_array() - " << "dods_uint32_c" << endl);
        return extract_array_helper<dods_uint32, T>(a);

        case dods_int32_c:
        DBG(cerr << "extract_array() - " << "dods_int32_c" << endl);
        return extract_array_helper<dods_int32, T>(a);

        case dods_float32_c:
        DBG(cerr << "extract_array() - " << "dods_float32_c" << endl);
        // Added the following line. jhrg 8/7/12
        return extract_array_helper<dods_float32, T>(a);

        case dods_float64_c:
        DBG(cerr << "extract_array() - " << "dods_float64_c" << endl);
        return extract_array_helper<dods_float64, T>(a);

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
static double extract_double_value(BaseType * arg)
{
    // Simple types are Byte, ..., Float64, String and Url.
    if (!arg->is_simple_type() || arg->type() == dods_str_c || arg->type() == dods_url_c)
        throw Error(malformed_expr, "The function requires a DAP numeric-type argument.");

    if (!arg->read_p())
        throw InternalErr(__FILE__, __LINE__,
                "The CE Evaluator built an argument list where some constants held no values.");

    // The types of arguments that the CE Parser will build for numeric
    // constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
    // Expanded to work for any numeric type so it can be used for more than
    // just arguments.
    switch (arg->type()) {
        case dods_byte_c:
            return (double) (dynamic_cast<Byte&>(*arg).value());
        case dods_uint16_c:
            return (double) (dynamic_cast<UInt16&>(*arg).value());
        case dods_int16_c:
            return (double) (dynamic_cast<Int16&>(*arg).value());
        case dods_uint32_c:
            return (double) (dynamic_cast<UInt32&>(*arg).value());
        case dods_int32_c:
            return (double) (dynamic_cast<Int32&>(*arg).value());
        case dods_float32_c:
            return (double) (dynamic_cast<Float32&>(*arg).value());
        case dods_float64_c:
            return dynamic_cast<Float64&>(*arg).value();
        default:
            throw InternalErr(__FILE__, __LINE__,
                    "The argument list built by the CE parser contained an unsupported numeric type.");
    }
}

#if 0
// These static functions could be moved to a class that provides a more
// general interface for COARDS/CF someday. Assume each BaseType comes bundled
// with an attribute table.

// These are included here because the ugrid code might want to use attribute
// values bound to various variables and this illustrates how that could be
// done. jhrg 8/20/12

// This was ripped from parser-util.cc
static double string_to_double(const char *val)
{
    char *ptr;
    errno = 0;
    // Clear previous value. 5/21/2001 jhrg

#ifdef WIN32
    double v = w32strtod(val, &ptr);
#else
    double v = strtod(val, &ptr);
#endif

    if ((v == 0.0 && (val == ptr || errno == HUGE_VAL || errno == ERANGE)) || *ptr != '\0') {
        throw Error(malformed_expr, string("Could not convert the string '") + val + "' to a double.");
    }

    double abs_val = fabs(v);
    if (abs_val > DODS_DBL_MAX || (abs_val != 0.0 && abs_val < DODS_DBL_MIN))
        throw Error(malformed_expr, string("Could not convert the string '") + val + "' to a double.");

    return v;
}

/** Look for any one of a series of attribute values in the attribute table
 for \e var. This function treats the list of attributes as if they are ordered
 from most to least likely/important. It stops when the first of the vector of
 values is found. If the variable (var) is a Grid, this function also looks
 at the Grid's Array for the named attributes. In all cases it returns the
 first value found.
 @param var Look for attributes in this BaseType variable.
 @param attributes A vector of attributes; the first one found will be returned.
 @return The attribute value in a double. */
static double get_attribute_double_value(BaseType *var, vector<string> &attributes)
{
    // This code also builds a list of the attribute values that have been
    // passed in but not found so that an informative message can be returned.
    AttrTable &attr = var->get_attr_table();
    string attribute_value = "";
    string values = "";
    vector<string>::iterator i = attributes.begin();
    while (attribute_value == "" && i != attributes.end()) {
        values += *i;
        if (!values.empty())
            values += ", ";
        attribute_value = attr.get_attr(*i++);
    }

    // If the value string is empty, then look at the grid's array (if it's a
    // grid) or throw an Error.
    if (attribute_value.empty()) {
        if (var->type() == dods_grid_c)
            return get_attribute_double_value(dynamic_cast<Grid&>(*var).get_array(), attributes);
        else
            throw Error(malformed_expr,
                    string("No COARDS/CF '") + values.substr(0, values.length() - 2)
                            + "' attribute was found for the variable '" + var->name() + "'.");
    }

    return string_to_double(remove_quotes(attribute_value).c_str());
}

static double get_attribute_double_value(BaseType *var, const string &attribute)
{
    AttrTable &attr = var->get_attr_table();
    string attribute_value = attr.get_attr(attribute);

    // If the value string is empty, then look at the grid's array (if it's a
    // grid or throw an Error.
    if (attribute_value.empty()) {
        if (var->type() == dods_grid_c)
            return get_attribute_double_value(dynamic_cast<Grid&>(*var).get_array(), attribute);
        else
            throw Error(malformed_expr,
                    string("No COARDS '") + attribute + "' attribute was found for the variable '" + var->name()
                            + "'.");
    }

    return string_to_double(remove_quotes(attribute_value).c_str());
}


static double get_y_intercept(BaseType *var)
{
    vector<string> attributes;
    attributes.push_back("add_offset");
    attributes.push_back("add_off");
    return get_attribute_double_value(var, attributes);
}

static double get_slope(BaseType *var)
{
    return get_attribute_double_value(var, "scale_factor");
}

static double get_missing_value(BaseType *var)
{
    return get_attribute_double_value(var, "missing_value");
}
#endif

/**
 Subset an irregular mesh (aka unstructured grid).

 @param argc Count of the function's arguments
 @param argv Array of pointers to the functions arguments
 @param dds Reference to the DDS object for the complete dataset.
 This holds pointers to all of the variables and attributes in the
 dataset.
 @param btpp Return the function result in an instance of BaseType
 referenced by this pointer to a pointer. We could have used a
 BaseType reference, instead of pointer to a pointer, but we didn't.
 This is a value-result parameter.

 @return void

 @exception Error Thrown If the Array is not a one dimensional
 array. */
void
function_ugrid_restrict(int argc, BaseType * argv[], DDS &dds, BaseType **btpp)
{
    static string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"ugrid_restrict\" version=\"0.1\">\n" +
    "Server function for Unstructured grid operations.\n" +
    "</function>";

    if (argc == 0) {
        Str *response = new Str("info");
        response->set_value(info);
        *btpp = response;
        return;
    }
#if 0
    static string info2 =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"ugrid_restrict\" version=\"0.1\">\n" +
    "code for Unstructured grid operations.\n" +
    "</function>";
    // FIXME What/Why are there 7 args? Why test this case? Below it clearly takes only two...
    if (argc == 7) {
        Str *response = new Str("info2");
        response->set_value(info);
        *btpp = response;
        return;
    }
#endif

    // Check number of arguments; DBG is a macro. Use #define
    // DODS_DEBUG to activate the debugging stuff.
    if (argc != 2)
        throw Error(malformed_expr,"Wrong number of arguments to ugrid_demo. ugrid_restrict(dim:int32, condition:string); was passed " + long_to_string(argc) + " argument(s)");

    if (argv[0]->type() != dods_int32_c)
        throw Error(malformed_expr,"Wrong type for first argument. ugrid_restrict(dim:int32, condition:string); was passed a/an " + argv[0]->type_name());

    if (argv[1]->type() != dods_str_c)
        throw Error(malformed_expr,"Wrong type for second argument. ugrid_restrict(dim:int32, condition:string); was passed a/an " + argv[1]->type_name());

    // keep track of which DDS dimensions correspond to GF dimensions

    map<GF::Dim_t, vector<Array::dimension> > rank_dimensions;

    // TODO Leaked?
    // TODO This is the 'domain' data?
    GF::Grid *G = new GF::Grid("result");

    // 1) Find the nodes
    DBG(cerr << "Reading 0-cells" << endl);
    GF::AbstractCellArray *nodes = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {

        BaseType *bt = *vi;
        // TODO allow variables that are not arrays; just ignore them
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        AttrTable &at = arr.get_attr_table();
        DBG(cerr << "Array: " << arr.name() << endl);

        int node_count = -1;    //error condition
        // FIXME
        // Look at the spec: http://bit.ly/ugrid_cf. There's no attribute
        // called 'grid_location' but there is an attribute called 'location'
        // and it can have the value 'node' (or 'face').
        AttrTable::Attr_iter loc = at.simple_find("grid_location");

        if (loc != at.attr_end()) {

            if (at.get_attr(loc, 0) == "node") {
                node_count = 1;
                Array::Dim_iter di = arr.dim_begin();
                DBG(cerr << "Interpreting 0-cells from dimensions: ");
                // FIXME This will overwrite previous Arrays that have 'grid_location'
                // attributes with a value of 'node'.
                rank_dimensions[0] = vector<Array::dimension>();
                // FIXME use the Array::size() method?
                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    // These dimensions define the nodes.
                    DBG(cerr << di->name << ", ");
                    rank_dimensions[0].push_back(*di);
                    node_count *= di->c_size;
                }
                DBG(cerr << endl);

                nodes = new GF::Implicit0Cells(node_count);

                break;
            } // Bound to nodes?
        } // Has a "grid_location" attribute?
    }

    if (!nodes)
        throw Error("Could not find a grid_location attribute and/or its node set.");

    // Attach the nodes to the grid
    // TODO Is this '0' the same as the '0' in 'rank_dimensions[0]'? See the
    // note/question below...
    G->setKCells(nodes, 0);

    // 2) For each k, find the k-cells
    // k = 2, for now
    DBG(cerr << "Reading 2-cells" << endl);
    GF::CellArray *twocells = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        // TODO Allow variables that are not Arrays; ignore as above
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        DBG(cerr << "Array: " << arr.name() << endl);

        AttrTable &at = arr.get_attr_table();

        // FIXME
        // There is no longer a 'cell_type' attribute in the spec...
        AttrTable::Attr_iter iter_cell_type = at.simple_find("cell_type");

        if (iter_cell_type != at.attr_end()) {
            string cell_type = at.get_attr(iter_cell_type, 0);
            DBG(cerr << cell_type << endl);
            if (cell_type == "tri_ccw") {
                // Ok, we expect triangles
                // which means a shape of 3xN
                // FIXME: The loop below checks that the array is 3xN and
                // stores the size of the second dimension in twocell_count
                int twocell_count = -1, i=0;
                // FIXME total_size is only used in the loop below when the attribute
                // 'index_origin' is found but that attribute is not defined in the spec.
                // Do we need this?
                int total_size = 1;
                rank_dimensions[2] = vector<Array::dimension>();
                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    total_size *= di->c_size;
                    rank_dimensions[2].push_back(*di);
                    if (i == 0) {
                        if (di->c_size != 3) {
                            DBG(cerr << "Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes." << endl);
                            throw Error(malformed_expr,"Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes.");
                        }
                    }
                    if (i == 1) {
                        twocell_count = di->c_size;
                    }
                    if (i>1) {
                        DBG(cerr << "Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN" << endl);
                        throw Error(malformed_expr,"Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN");
                    }
                    i++;
                }

                // interpret the array data as triangles
                // FIXME Can allocate cellids without copying arr's values
                GF::Node *cellids = extract_array<GF::Node>(&arr);
                GF::Node *cellids2 = extract_array<GF::Node>(&arr);
                // FIXME
                // This loop appears to reorganize cellids so that it contains
                // in in three consecutive values (0,1,2; 3,4,5; ...) the values
                // from cellids2 0,N,2N; 1,1+N,1+2N; ...
                // But cellids2 is never used anywhere else; consider rewriting
                // so it does this repacking operation.
                for (int j=0;j<twocell_count;j++) {   cellids[3*j]=cellids2[j];
                    cellids[3*j+1]=cellids2[j+twocell_count];
                    cellids[3*j+2]=cellids2[j+2*twocell_count];
                }

                // adjust for index origin
                // FIXME There's no 'index_origin' attribute in the spec.
                AttrTable::Attr_iter iter_index_origin = at.simple_find("index_origin");
                if (iter_index_origin != at.attr_end()) {
                    DBG(cerr << "Found an index origin attribute." << endl);
                    AttrTable::entry *index_origin_entry = *iter_index_origin;
                    int index_origin;
                    if (index_origin_entry->attr->size() == 1) {
                        AttrTable::entry *index_origin_entry = *iter_index_origin;
                        string val = (*index_origin_entry->attr)[0];
                        DBG(cerr << "Value: " << val << endl);
                        stringstream buffer(val);
                        // what happens if string cannot be converted to an integer?
                        buffer >> index_origin;
                        DBG(cerr << "converted: " << index_origin << endl);
                        if (index_origin != 0) {
                            for (int j=0; j<total_size; j++) {
                                cellids[j] -= index_origin;
                            }
                        }
                    }
                    else {
                        throw Error(malformed_expr,"Index origin attribute exists, but either no value supplied, or more than one value supplied.");
                    }
                }

                // Create the cell array
                // TODO Is this '3' the same as the '3' in '3xN'?
                twocells = new GF::CellArray(cellids, twocell_count, 3);

                // Attach it to the grid
                // TODO Is this '2' the same as the '2' in 'rank_dimensions[2]'?
                G->setKCells(twocells, 2);
            }
        }
    }

    if (!twocells)
        throw Error("Could not find cell array of CCW triangles");

    // 3) For each var, bind it to the appropriate dimension

    // For each variable in the data source:
    GF::GridField *input = new GF::GridField(G);

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;

        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            DBG(cerr << "Data Array: " << arr->name() << endl);
            GF::Array *gfa = extract_gridfield_array(arr);

            // Each rank is associated with a sequence of dimensions
            // Vars that have the same dimensions should be bound to the grid at that rank
            // (Note that in gridfields, Dimension and rank are synonyms.  We
            // use the latter here to avoid confusion).
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;
            for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                bool same = same_dimensions(arr, iter->second);
                if (same) {
                    // This var should be bound to rank k
                    // TODO This code sets AddAttribute(0, <grid_location --> 'node'>)
                    // (which occurs three times in test4.nc - X, Y, and nodedata) and
                    // AddAttribute(2, <the 3xN var>).
                    DBG(cerr << "Adding Attribute: " << gfa->sname() << endl);
                    input->AddAttribute(iter->first, gfa);
                }
                else {
                    // This array does not appear to be associated with any
                    // rank of the unstructured grid. Ignore for now.
                    // TODO Anything else we should do?
                    // FIXME Free the storage!!
                }
            }
        } // Ignore if not an array type. Anything else we should do?
    }

    //FIXME Tell James what dim is about...

    int dim = extract_double_value(argv[0]);
    string filter_expr = extract_string_argument(argv[1]);
    // not used jhrg 11/15/12 int nodenumber=input->Card(0);

    GF::RestrictOp op = GF::RestrictOp(filter_expr, dim, input);
    GF::GridField *R = new GF::GridField(op.getResult());

    // 4) Convert back to a DDS BaseType

    // Create variables for each cell dimension
    // Create variables for each attribute at each rank

    R->GetGrid()->normalize();

    Structure *construct = new Structure("construct");

    // FIXME This code loops through the DDS and finds the variables
    // that were the sources of information used by Gridfields (there are four)
    // and uses those names to make the names of the four result variables.
    // It also copies the attribute table from those variables.
    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;

            AttrTable &arrattr2 = arr->get_attr_table();

            if(arrattr2.simple_find("cell_type")!=arrattr2.attr_end())
            {
                GF::CellArray* Inb=(GF::CellArray*)(R->GetGrid()->getKCells(2));
                Int32 *witnessn4 = new Int32(arr->name());
                Array *Nodes = new Array(arr->name(),witnessn4);
                vector< vector<int> > nodes2 = Inb->makeArrayInts();
                vector<dods_int32> node1;
                vector<dods_int32> node2;
                vector<dods_int32> node3;
                for (unsigned int j=0; j < nodes2.size(); j++) {
                    node1.push_back(nodes2.at(j).at(0));
                    node2.push_back(nodes2.at(j).at(1));
                    node3.push_back(nodes2.at(j).at(2));
                }
                Int32 *witnessn1=new Int32("nodes1");
                Int32 *witnessn2=new Int32("nodes2");
                Int32 *witnessn3=new Int32("nodes3");
                Array *Node1=new Array("trinode1",witnessn1);
                Array *Node2=new Array("trinode2",witnessn2);
                Array *Node3=new Array("trinode3",witnessn3);
                Node1->append_dim(node1.size(),"dim-1");

                Node2->append_dim(node2.size(),"dim-1");
                Node3->append_dim(node3.size(),"dim-1");

                Node1->set_value(node1,node1.size());
                Node2->set_value(node2,node2.size());
                Node3->set_value(node3,node3.size());

                Nodes->append_dim(3,"three");
                Nodes->append_dim(node1.size(),"tris");
                Nodes->reserve_value_capacity(3*node1.size());
                Nodes->set_value_slice_from_row_major_vector(*Node1,0);
                Nodes->set_value_slice_from_row_major_vector(*Node2,Node1->length());
                Nodes->set_value_slice_from_row_major_vector(*Node3,Node1->length()+Node2->length());
                AttrTable &arrattr1 = arr->get_attr_table();
                Nodes->set_attr_table(arrattr1);

                construct->add_var_nocopy(Nodes);
            }
            else {
                for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                    bool same = same_dimensions(arr, iter->second);
                    if (same) {
                        // This var should be bound to rank k
                        Float64 *witness2 = new Float64(arr->name());

                        GF::Array* gfa = R->GetAttribute(iter->first, arr->name());

                        vector<dods_float64> GFA = gfa->makeArrayf();

                        Array *Nodes = new Array(arr->name(), witness2);
                        Nodes->append_dim(GFA.size(), "nodes");
                        Nodes->set_value(GFA,GFA.size());

                        AttrTable &arrattr1 = arr->get_attr_table();
                        Nodes->set_attr_table(arrattr1);
                        // AttrTable &arrattr = Nodes->get_attr_table();

                        construct->add_var_nocopy(Nodes);
                    }
                    else {
                        // This array does not appear to be associated with
                        // any rank of the unstructured grid. Ignore for now.
                        // Anything else we should do?
                    }
                }
            }
        }
    }

#if 0
    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;
            for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                bool same = same_dimensions(arr, iter->second);
                if (same) {
                    GF::Array* gfa = R->GetAttribute(iter->first, arr->name());
                }
                else {
                    //This array does not appear to be associated with any
                    // rank of the unstructured grid.Ignore for now.
                    // Anything else we should do?
                }

            }
        }
    }
#endif
    // TODO Needed?
    //GF::Grid *newgrid = R->GetGrid();

    *btpp = construct;

    return;
}


/**
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 * #######################################################################################################
 */



static string ugrSyntax = "ugr(dim:int32, condition:string)";

struct ugr_args {
	int dimension;
	string filterExpression;
};

static ugr_args processArgs(int argc, BaseType * argv[]){

	ugr_args args;

    // Check number of arguments; DBG is a macro. Use #define
    // DODS_DEBUG to activate the debugging stuff.
    if (argc != 2)
        throw Error(malformed_expr,"Wrong number of arguments to ugrid restrict function: "+ugrSyntax+" was passed " + long_to_string(argc) + " argument(s)");

    BaseType *bt;


    //FIXME Process the first arg, which is "dimension" or something - WE DON'T REALLY KNOW.
    bt = argv[0];
    if (bt->type() != dods_int32_c)
        throw Error(malformed_expr,"Wrong type for first argument, expected DAP Int32. "+ugrSyntax+"  was passed a/an " + bt->type_name());
    //FIXME Tell James what dim is about...
    args.dimension = extract_double_value(bt);



    // Process the second argument, the relational expression used to restrict the ugrid content.
    bt = argv[1];
    if (bt->type() != dods_str_c)
        throw Error(malformed_expr,"Wrong type for third argument, expected DAP String. "+ugrSyntax+"  was passed a/an " + bt->type_name());
    args.filterExpression = extract_string_argument(bt);

    return args;



#if 0
    // ORIGINAL IMPLEMENTATION


    // Check number of arguments; DBG is a macro. Use #define
    // DODS_DEBUG to activate the debugging stuff.
    if (argc != 3)
        throw Error(malformed_expr,"Wrong number of arguments to ugrid restrict function: "+ugrSyntax+" was passed " + long_to_string(argc) + " argument(s)");

    //FIXME Process the first arg, which is "dimension" or something - WE DON'T REALLY KNOW.
    if (argv[0]->type() != dods_int32_c)
        throw Error(malformed_expr,"Wrong type for first argument. "+ugrSyntax+"  was passed a/an " + argv[0]->type_name());
    //FIXME Tell James what dim is about...
    int dim = extract_double_value(argv[0]);


    // Process the second argument, the range Variable selected by the user.
    if (argv[1]->type() != dods_array_c)
        throw Error(malformed_expr,"Wrong type for second argument. "+ugrSyntax+"  was passed a/an " + argv[1]->type_name());
    Array &rangeVar;
    try {
    	rangeVar = dynamic_cast<Array&>(*argv[1]);
    }
    catch(std::bad_cast &e) {
        throw Error(malformed_expr,"Wrong type for second argument. "+ugrSyntax+"  was passed a/an " + argv[1]->type_name());
    }


    // Process the third argument, the relation expression used to restrict the ugrid content..
    if (argv[2]->type() != dods_str_c)
        throw Error(malformed_expr,"Wrong type for third argument. "+ugrSyntax+"  was passed a/an " + argv[2]->type_name());
    string filter_expr = extract_string_argument(argv[2]);


    // not used jhrg 11/15/12 int nodenumber=input->Card(0);

#endif

}


/**
 * This implementation iterates over the variables in the dataset. If a variable hs an attribute named "grid_location"
 * (which is now called "location" in the ugrid specification) whose value is equal to 'node' then it would build one
 * of these Implicit0Cells things using the computed capacity of the array as the parameter to the constructor. However,
 * it will build one for only the first variable in the dataset that had the matching attribute conditions, and then it
 * calls "break;" and leave the variable iteration loop. So in the end only the first variable matching the attribute
 * condition is utilized, which seems problematic if the dataset is is more complex than anticipated.
 *
 * Ultimately it would appear that the code is simply trying to determine the number of nodes in the ugrid.
 * I think this can be more effectively done either by utilizing a user supplied range variable, or following the ugrid
 * specification and locating the variable with the attribute "cf_role" whose value is "mesh_topology", and working it
 * out from there (based on the spec).
 *
 */
static GF::AbstractCellArray *getNodes(DDS &dds, map<GF::Dim_t, vector<Array::dimension> > &rank_dimensions)
{
#if 0
    // 1) Check The Range Nodes
    DBG(cerr << "Checking Range Variable..." << endl);
    GF::Implicit0Cells *nodes = NULL;

    AttrTable &at = rangeVar->get_attr_table();
    DBG(cerr << "The user submitted the range array: " << rangeNodes.name() << endl);

    AttrTable::Attr_iter loc = at.simple_find("location");
    if (loc != at.attr_end()) {
        DBG(cerr << "Array: " << rangeNodes.name() << " has a 'location' attribute."<< endl);
        string value = at.get_attr(loc, 0);
        DBG(cerr << "Attribute 'location' has value of '" << value << "'"<< endl);
        if (value == "node") {
        	int node_count = rangeVar->length();
            nodes = new GF::Implicit0Cells(node_count);
            return nodes;
        }
        else {
            throw Error("The requested range variable '"+rangeVar->name()+"' has a 'location' attribute " +
            		"that is not equal to 'node'. location='"+value+"'");
        }
    }
    else {
        throw Error("The requested range variable '"+rangeVar->name()+"' that does not have the " +
        		"required 'location' attribute.");
    }
#endif



    // 1) Check The Range Nodes
    DBG(cerr << "Checking Range Variable..." << endl);
    GF::AbstractCellArray *nodes = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {

        BaseType *bt = *vi;
        // TODO allow variables that are not arrays; just ignore them
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        AttrTable &at = arr.get_attr_table();
        DBG(cerr << "Array: " << arr.name() << endl);

        int node_count = -1;    //error condition
        // FIXME
        // Look at the spec: http://bit.ly/ugrid_cf. There's no attribute
        // called 'grid_location' but there is an attribute called 'location'
        // and it can have the value 'node' (or 'face').
        AttrTable::Attr_iter loc = at.simple_find("grid_location");

        if (loc != at.attr_end()) {

            if (at.get_attr(loc, 0) == "node") {


                node_count = 1;
                Array::Dim_iter di = arr.dim_begin();
                DBG(cerr << "Interpreting 0-cells from dimensions: ");
                // FIXME This will overwrite previous Arrays that have 'grid_location'
                // attributes with a value of 'node'.
                rank_dimensions[0] = vector<Array::dimension>();
                // FIXME use the Array::size() method?


                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    // These dimensions define the nodes.
                    DBG(cerr << di->name << ", ");
                    rank_dimensions[0].push_back(*di);
                    node_count *= di->c_size;
                }
                DBG(cerr << endl);

                nodes = new GF::Implicit0Cells(node_count);
                break;
            } // Bound to nodes?
        } // Has a "grid_location" attribute?
    }

    if (!nodes)
        throw Error("Could not find a grid_location attribute and/or its node set.");

    return nodes;





}

static GF::CellArray *getRankTwoKCells(DDS &dds, map<GF::Dim_t, vector<Array::dimension> > &rank_dimensions)
{

    // 2) For each k, find the k-cells
    // k = 2, for now
    DBG(cerr << "Reading 2-cells" << endl);
    GF::CellArray *twocells = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        // TODO Allow variables that are not Arrays; ignore as above
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        DBG(cerr << "Array: " << arr.name() << endl);

        AttrTable &at = arr.get_attr_table();

        // FIXME
        // There is no longer a 'cell_type' attribute in the spec...
        AttrTable::Attr_iter iter_cell_type = at.simple_find("cell_type");

        if (iter_cell_type != at.attr_end()) {
            string cell_type = at.get_attr(iter_cell_type, 0);
            DBG(cerr << cell_type << endl);
            if (cell_type == "tri_ccw") {
                // Ok, we expect triangles
                // which means a shape of 3xN
                // FIXME: The loop below checks that the array is 3xN and
                // stores the size of the second dimension in twocell_count
                int twocell_count = -1, i=0;
                // FIXME total_size is only used in the loop below when the attribute
                // 'index_origin' is found but that attribute is not defined in the spec.
                // Do we need this?
                int total_size = 1;
                rank_dimensions[2] = vector<Array::dimension>();
                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    total_size *= di->c_size;
                    rank_dimensions[2].push_back(*di);
                    if (i == 0) {
                        if (di->c_size != 3) {
                            DBG(cerr << "Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes." << endl);
                            throw Error(malformed_expr,"Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes.");
                        }
                    }
                    if (i == 1) {
                        twocell_count = di->c_size;
                    }
                    if (i>1) {
                        DBG(cerr << "Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN" << endl);
                        throw Error(malformed_expr,"Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN");
                    }
                    i++;
                }

                // interpret the array data as triangles
                // FIXME Can allocate cellids without copying arr's values
                GF::Node *cellids = extract_array<GF::Node>(&arr);
                GF::Node *cellids2 = extract_array<GF::Node>(&arr);
                // FIXME
                // This loop appears to reorganize cellids so that it contains
                // in in three consecutive values (0,1,2; 3,4,5; ...) the values
                // from cellids2 0,N,2N; 1,1+N,1+2N; ...
                // But cellids2 is never used anywhere else; consider rewriting
                // so it does this repacking operation.
                for (int j=0;j<twocell_count;j++) {   cellids[3*j]=cellids2[j];
                    cellids[3*j+1]=cellids2[j+twocell_count];
                    cellids[3*j+2]=cellids2[j+2*twocell_count];
                }

                // adjust for index origin
                // FIXME There's no 'index_origin' attribute in the spec.
                AttrTable::Attr_iter iter_index_origin = at.simple_find("index_origin");
                if (iter_index_origin != at.attr_end()) {
                    DBG(cerr << "Found an index origin attribute." << endl);
                    AttrTable::entry *index_origin_entry = *iter_index_origin;
                    int index_origin;
                    if (index_origin_entry->attr->size() == 1) {
                        AttrTable::entry *index_origin_entry = *iter_index_origin;
                        string val = (*index_origin_entry->attr)[0];
                        DBG(cerr << "Value: " << val << endl);
                        stringstream buffer(val);
                        // what happens if string cannot be converted to an integer?
                        buffer >> index_origin;
                        DBG(cerr << "converted: " << index_origin << endl);
                        if (index_origin != 0) {
                            for (int j=0; j<total_size; j++) {
                                cellids[j] -= index_origin;
                            }
                        }
                    }
                    else {
                        throw Error(malformed_expr,"Index origin attribute exists, but either no value supplied, or more than one value supplied.");
                    }
                }

                // Create the cell array
                // TODO Is this '3' the same as the '3' in '3xN'?
                twocells = new GF::CellArray(cellids, twocell_count, 3);

                return twocells;
            }
        }
    }

    throw Error("Could not find cell array of CCW triangles");


}

static GF::GridField *getInputCells(DDS &dds, map<GF::Dim_t, vector<Array::dimension> > &rank_dimensions, GF::Grid *G)
{
    // 3) For each var, bind it to the appropriate dimension

    // For each variable in the data source:
    GF::GridField *input = new GF::GridField(G);

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;

        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            DBG(cerr << "Data Array: " << arr->name() << endl);
            GF::Array *gfa = extract_gridfield_array(arr);

            // Each rank is associated with a sequence of dimensions
            // Vars that have the same dimensions should be bound to the grid at that rank
            // (Note that in gridfields, Dimension and rank are synonyms.  We
            // use the latter here to avoid confusion).
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;
            for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                int gridDimKey = iter->first; // either 0 or 2
                vector<Array::dimension> arrDims = iter->second;
                bool same = same_dimensions(arr, arrDims);
                if (same) {
                    // This var should be bound to rank k
                    // TODO This code sets AddAttribute(0, <grid_location --> 'node'>)
                    // (which occurs three times in test4.nc - X, Y, and nodedata) and
                    // AddAttribute(2, <the 3xN var>).
                    DBG(cerr << "Adding Attribute: " << gfa->sname() << endl);
                    input->AddAttribute(gridDimKey, gfa);
                }
                else {
                    // This array does not appear to be associated with any
                    // rank of the unstructured grid. Ignore for now.
                    // TODO Anything else we should do?
                    // FIXME Free the storage!!
                }
            }
        } // Ignore if not an array type. Anything else we should do?
    }

    return input;

}

static Structure *convertUgridToDapObject(DDS &dds, map<GF::Dim_t, vector<Array::dimension> > &rank_dimensions, GF::GridField *R)
{
    // 4) Convert back to a DDS BaseType

    // Create variables for each cell dimension
    // Create variables for each attribute at each rank

    R->GetGrid()->normalize();

    Structure *construct = new Structure("construct");

    // FIXME This code loops through the DDS and finds the variables
    // that were the sources of information used by Gridfields (there are four)
    // and uses those names to make the names of the four result variables.
    // It also copies the attribute table from those variables.
    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;

            AttrTable &arrattr2 = arr->get_attr_table();

            if(arrattr2.simple_find("cell_type")!=arrattr2.attr_end())
            {
                GF::CellArray* Inb=(GF::CellArray*)(R->GetGrid()->getKCells(2));
                Int32 *witnessn4 = new Int32(arr->name());
                Array *Nodes = new Array(arr->name(),witnessn4);
                vector< vector<int> > nodes2 = Inb->makeArrayInts();
                vector<dods_int32> node1;
                vector<dods_int32> node2;
                vector<dods_int32> node3;
                for (unsigned int j=0; j < nodes2.size(); j++) {
                    node1.push_back(nodes2.at(j).at(0));
                    node2.push_back(nodes2.at(j).at(1));
                    node3.push_back(nodes2.at(j).at(2));
                }
                Int32 *witnessn1=new Int32("nodes1");
                Int32 *witnessn2=new Int32("nodes2");
                Int32 *witnessn3=new Int32("nodes3");
                Array *Node1=new Array("trinode1",witnessn1);
                Array *Node2=new Array("trinode2",witnessn2);
                Array *Node3=new Array("trinode3",witnessn3);
                Node1->append_dim(node1.size(),"dim-1");

                Node2->append_dim(node2.size(),"dim-1");
                Node3->append_dim(node3.size(),"dim-1");

                Node1->set_value(node1,node1.size());
                Node2->set_value(node2,node2.size());
                Node3->set_value(node3,node3.size());

                Nodes->append_dim(3,"three");
                Nodes->append_dim(node1.size(),"tris");
                Nodes->reserve_value_capacity(3*node1.size());
                Nodes->set_value_slice_from_row_major_vector(*Node1,0);
                Nodes->set_value_slice_from_row_major_vector(*Node2,Node1->length());
                Nodes->set_value_slice_from_row_major_vector(*Node3,Node1->length()+Node2->length());
                AttrTable &arrattr1 = arr->get_attr_table();
                Nodes->set_attr_table(arrattr1);

                construct->add_var_nocopy(Nodes);
            }
            else {
                for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                    bool same = same_dimensions(arr, iter->second);
                    if (same) {
                        // This var should be bound to rank k
                        Float64 *witness2 = new Float64(arr->name());

                        GF::Array* gfa = R->GetAttribute(iter->first, arr->name());

                        vector<dods_float64> GFA = gfa->makeArrayf();

                        Array *Nodes = new Array(arr->name(), witness2);
                        Nodes->append_dim(GFA.size(), "nodes");
                        Nodes->set_value(GFA,GFA.size());

                        AttrTable &arrattr1 = arr->get_attr_table();
                        Nodes->set_attr_table(arrattr1);
                        // AttrTable &arrattr = Nodes->get_attr_table();

                        construct->add_var_nocopy(Nodes);
                    }
                    else {
                        // This array does not appear to be associated with
                        // any rank of the unstructured grid. Ignore for now.
                        // Anything else we should do?
                    }
                }
            }
        }
    }

    return construct;
}


/**
 Subset an irregular mesh (aka unstructured grid).

 @param argc Count of the function's arguments
 @param argv Array of pointers to the functions arguments
 @param dds Reference to the DDS object for the complete dataset.
 This holds pointers to all of the variables and attributes in the
 dataset.
 @param btpp Return the function result in an instance of BaseType
 referenced by this pointer to a pointer. We could have used a
 BaseType reference, instead of pointer to a pointer, but we didn't.
 This is a value-result parameter.

 @return void

 @exception Error Thrown If the Array is not a one dimensional
 array. */
void
function_ugrOLD(int argc, BaseType * argv[], DDS &dds, BaseType **btpp)
{
    static string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"ugr\" version=\"0.1\">\n" +
    "Server function for Unstructured grid operations.\n" +
    "</function>";

    if (argc == 0) {
        Str *response = new Str("info");
        response->set_value(info);
        *btpp = response;
        return;
    }

    ugr_args args = processArgs(argc,argv);

#if 0
    // Check number of arguments; DBG is a macro. Use #define
    // DODS_DEBUG to activate the debugging stuff.
    if (argc != 3)
        throw Error(malformed_expr,"Wrong number of arguments to ugrid restrict function: "+ugrSyntax+" was passed " + long_to_string(argc) + " argument(s)");

    //FIXME Process the first arg, which is "dimension" or something - WE DON'T REALLY KNOW.
    if (argv[0]->type() != dods_int32_c)
        throw Error(malformed_expr,"Wrong type for first argument. "+ugrSyntax+"  was passed a/an " + argv[0]->type_name());
    //FIXME Tell James what dim is about...
    int dim = extract_double_value(argv[0]);


    // Process the second argument, the range Variable selected by the user.
    if (argv[1]->type() != dods_array_c)
        throw Error(malformed_expr,"Wrong type for second argument. "+ugrSyntax+"  was passed a/an " + argv[1]->type_name());
    Array &rangeVar;
    try {
    	rangeVar = dynamic_cast<Array&>(*argv[1]);
    }
    catch(std::bad_cast &e) {
        throw Error(malformed_expr,"Wrong type for second argument. "+ugrSyntax+"  was passed a/an " + argv[1]->type_name());
    }


    // Process the third argument, the relation expression used to restrict the ugrid content..
    if (argv[2]->type() != dods_str_c)
        throw Error(malformed_expr,"Wrong type for third argument. "+ugrSyntax+"  was passed a/an " + argv[2]->type_name());
    string filter_expr = extract_string_argument(argv[2]);


    // not used jhrg 11/15/12 int nodenumber=input->Card(0);

#endif

    // keep track of which DDS dimensions correspond to GF dimensions
    map<GF::Dim_t, vector<Array::dimension> > rank_dimensions;

    // TODO Leaked?
    // TODO This is the 'domain' data?
    GF::Grid *G = new GF::Grid("result");


    // 1) Get The Range Nodes
    GF::AbstractCellArray *nodes = getNodes(dds,rank_dimensions);

    // Attach the range nodes to the grid
    // TODO Is this '0' the same as the '0' in 'rank_dimensions[0]'? See the note/question below...
    G->setKCells(nodes, 0);

#if 0
    // 1) Check The Range Nodes
    DBG(cerr << "Checking Range Variable..." << endl);
    GF::AbstractCellArray *nodes = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {

        BaseType *bt = *vi;
        // TODO allow variables that are not arrays; just ignore them
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        AttrTable &at = arr.get_attr_table();
        DBG(cerr << "Array: " << arr.name() << endl);

        int node_count = -1;    //error condition
        // FIXME
        // Look at the spec: http://bit.ly/ugrid_cf. There's no attribute
        // called 'grid_location' but there is an attribute called 'location'
        // and it can have the value 'node' (or 'face').
        AttrTable::Attr_iter loc = at.simple_find("location");

        if (loc != at.attr_end()) {

            if (at.get_attr(loc, 0) == "node") {


                node_count = 1;
                Array::Dim_iter di = arr.dim_begin();
                DBG(cerr << "Interpreting 0-cells from dimensions: ");
                // FIXME This will overwrite previous Arrays that have 'grid_location'
                // attributes with a value of 'node'.
                rank_dimensions[0] = vector<Array::dimension>();
                // FIXME use the Array::size() method?


                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    // These dimensions define the nodes.
                    DBG(cerr << di->name << ", ");
                    rank_dimensions[0].push_back(*di);
                    node_count *= di->c_size;
                }
                DBG(cerr << endl);

                nodes = new GF::Implicit0Cells(node_count);
                break;
            } // Bound to nodes?
        } // Has a "grid_location" attribute?
    }

    if (!nodes)
        throw Error("Could not find a grid_location attribute and/or its node set.");

    // Attach the nodes to the grid
    // TODO Is this '0' the same as the '0' in 'rank_dimensions[0]'? See the
    // note/question below...
    G->setKCells(nodes, 0);


#endif

    // 2) Get rank 2 k-cells.
    GF::CellArray *twocells = getRankTwoKCells(dds, rank_dimensions);

    // Attach it to the grid
    // TODO Is this '2' the same as the '2' in 'rank_dimensions[2]'?
    G->setKCells(twocells, 2);


#if 0
    // 2) For each k, find the k-cells
    // k = 2, for now
    DBG(cerr << "Reading 2-cells" << endl);
    GF::CellArray *twocells = NULL;

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        // TODO Allow variables that are not Arrays; ignore as above
        if (bt->type() != dods_array_c)
            continue;

        Array &arr = dynamic_cast<Array&>(*bt);
        DBG(cerr << "Array: " << arr.name() << endl);

        AttrTable &at = arr.get_attr_table();

        // FIXME
        // There is no longer a 'cell_type' attribute in the spec...
        AttrTable::Attr_iter iter_cell_type = at.simple_find("cell_type");

        if (iter_cell_type != at.attr_end()) {
            string cell_type = at.get_attr(iter_cell_type, 0);
            DBG(cerr << cell_type << endl);
            if (cell_type == "tri_ccw") {
                // Ok, we expect triangles
                // which means a shape of 3xN
                // FIXME: The loop below checks that the array is 3xN and
                // stores the size of the second dimension in twocell_count
                int twocell_count = -1, i=0;
                // FIXME total_size is only used in the loop below when the attribute
                // 'index_origin' is found but that attribute is not defined in the spec.
                // Do we need this?
                int total_size = 1;
                rank_dimensions[2] = vector<Array::dimension>();
                for (Array::Dim_iter di = arr.dim_begin(); di!= arr.dim_end(); di++) {
                    total_size *= di->c_size;
                    rank_dimensions[2].push_back(*di);
                    if (i == 0) {
                        if (di->c_size != 3) {
                            DBG(cerr << "Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes." << endl);
                            throw Error(malformed_expr,"Cell array of type 'tri_ccw' must have a shape of 3xN, since triangles have three nodes.");
                        }
                    }
                    if (i == 1) {
                        twocell_count = di->c_size;
                    }
                    if (i>1) {
                        DBG(cerr << "Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN" << endl);
                        throw Error(malformed_expr,"Too many dimensions for a cell array of type 'tri_ccw'.  Expected shape of 3XN");
                    }
                    i++;
                }

                // interpret the array data as triangles
                // FIXME Can allocate cellids without copying arr's values
                GF::Node *cellids = extract_array<GF::Node>(&arr);
                GF::Node *cellids2 = extract_array<GF::Node>(&arr);
                // FIXME
                // This loop appears to reorganize cellids so that it contains
                // in in three consecutive values (0,1,2; 3,4,5; ...) the values
                // from cellids2 0,N,2N; 1,1+N,1+2N; ...
                // But cellids2 is never used anywhere else; consider rewriting
                // so it does this repacking operation.
                for (int j=0;j<twocell_count;j++) {   cellids[3*j]=cellids2[j];
                    cellids[3*j+1]=cellids2[j+twocell_count];
                    cellids[3*j+2]=cellids2[j+2*twocell_count];
                }

                // adjust for index origin
                // FIXME There's no 'index_origin' attribute in the spec.
                AttrTable::Attr_iter iter_index_origin = at.simple_find("index_origin");
                if (iter_index_origin != at.attr_end()) {
                    DBG(cerr << "Found an index origin attribute." << endl);
                    AttrTable::entry *index_origin_entry = *iter_index_origin;
                    int index_origin;
                    if (index_origin_entry->attr->size() == 1) {
                        AttrTable::entry *index_origin_entry = *iter_index_origin;
                        string val = (*index_origin_entry->attr)[0];
                        DBG(cerr << "Value: " << val << endl);
                        stringstream buffer(val);
                        // what happens if string cannot be converted to an integer?
                        buffer >> index_origin;
                        DBG(cerr << "converted: " << index_origin << endl);
                        if (index_origin != 0) {
                            for (int j=0; j<total_size; j++) {
                                cellids[j] -= index_origin;
                            }
                        }
                    }
                    else {
                        throw Error(malformed_expr,"Index origin attribute exists, but either no value supplied, or more than one value supplied.");
                    }
                }

                // Create the cell array
                // TODO Is this '3' the same as the '3' in '3xN'?
                twocells = new GF::CellArray(cellids, twocell_count, 3);

                // Attach it to the grid
                // TODO Is this '2' the same as the '2' in 'rank_dimensions[2]'?
                G->setKCells(twocells, 2);
            }
        }
    }

    if (!twocells)
        throw Error("Could not find cell array of CCW triangles");

#endif

    // 3) Build the input cells for the grid

    GF::GridField *inputCells = getInputCells(dds,rank_dimensions,G);

    // Build the restriction operator;
    GF::RestrictOp op = GF::RestrictOp(args.filterExpression, args.dimension, inputCells);

    // Apply the operator and get the result;
    GF::GridField *R = new GF::GridField(op.getResult());

#if 0

    // 3) For each var, bind it to the appropriate dimension

    // For each variable in the data source:
    GF::GridField *input = new GF::GridField(G);

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;

        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            DBG(cerr << "Data Array: " << arr->name() << endl);
            GF::Array *gfa = extract_gridfield_array(arr);

            // Each rank is associated with a sequence of dimensions
            // Vars that have the same dimensions should be bound to the grid at that rank
            // (Note that in gridfields, Dimension and rank are synonyms.  We
            // use the latter here to avoid confusion).
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;
            for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                bool same = same_dimensions(arr, iter->second);
                if (same) {
                    // This var should be bound to rank k
                    // TODO This code sets AddAttribute(0, <grid_location --> 'node'>)
                    // (which occurs three times in test4.nc - X, Y, and nodedata) and
                    // AddAttribute(2, <the 3xN var>).
                    DBG(cerr << "Adding Attribute: " << gfa->sname() << endl);
                    input->AddAttribute(iter->first, gfa);
                }
                else {
                    // This array does not appear to be associated with any
                    // rank of the unstructured grid. Ignore for now.
                    // TODO Anything else we should do?
                    // FIXME Free the storage!!
                }
            }
        } // Ignore if not an array type. Anything else we should do?
    }

    int dim = extract_double_value(argv[0]);
    string filter_expr = extract_string_argument(argv[1]);
    // not used jhrg 11/15/12 int nodenumber=input->Card(0);

    GF::RestrictOp op = GF::RestrictOp(filter_expr, dim, input);
    GF::GridField *R = new GF::GridField(op.getResult());

#endif

    // 4) Get the GridField back in a DAP representation of a ugrid.
    Structure *construct = convertUgridToDapObject(dds,rank_dimensions,R);
    *btpp = construct;


#if 0
    // 4) Convert back to a DDS BaseType

    // Create variables for each cell dimension
    // Create variables for each attribute at each rank

    R->GetGrid()->normalize();

    Structure *construct = new Structure("construct");

    // FIXME This code loops through the DDS and finds the variables
    // that were the sources of information used by Gridfields (there are four)
    // and uses those names to make the names of the four result variables.
    // It also copies the attribute table from those variables.
    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        if (bt->type() == dods_array_c) {
            Array *arr = (Array *)bt;
            map<GF::Dim_t, vector<Array::dimension> >::iterator iter;

            AttrTable &arrattr2 = arr->get_attr_table();

            if(arrattr2.simple_find("cell_type")!=arrattr2.attr_end())
            {
                GF::CellArray* Inb=(GF::CellArray*)(R->GetGrid()->getKCells(2));
                Int32 *witnessn4 = new Int32(arr->name());
                Array *Nodes = new Array(arr->name(),witnessn4);
                vector< vector<int> > nodes2 = Inb->makeArrayInts();
                vector<dods_int32> node1;
                vector<dods_int32> node2;
                vector<dods_int32> node3;
                for (unsigned int j=0; j < nodes2.size(); j++) {
                    node1.push_back(nodes2.at(j).at(0));
                    node2.push_back(nodes2.at(j).at(1));
                    node3.push_back(nodes2.at(j).at(2));
                }
                Int32 *witnessn1=new Int32("nodes1");
                Int32 *witnessn2=new Int32("nodes2");
                Int32 *witnessn3=new Int32("nodes3");
                Array *Node1=new Array("trinode1",witnessn1);
                Array *Node2=new Array("trinode2",witnessn2);
                Array *Node3=new Array("trinode3",witnessn3);
                Node1->append_dim(node1.size(),"dim-1");

                Node2->append_dim(node2.size(),"dim-1");
                Node3->append_dim(node3.size(),"dim-1");

                Node1->set_value(node1,node1.size());
                Node2->set_value(node2,node2.size());
                Node3->set_value(node3,node3.size());

                Nodes->append_dim(3,"three");
                Nodes->append_dim(node1.size(),"tris");
                Nodes->reserve_value_capacity(3*node1.size());
                Nodes->set_value_slice_from_row_major_vector(*Node1,0);
                Nodes->set_value_slice_from_row_major_vector(*Node2,Node1->length());
                Nodes->set_value_slice_from_row_major_vector(*Node3,Node1->length()+Node2->length());
                AttrTable &arrattr1 = arr->get_attr_table();
                Nodes->set_attr_table(arrattr1);

                construct->add_var_nocopy(Nodes);
            }
            else {
                for( iter = rank_dimensions.begin(); iter != rank_dimensions.end(); ++iter ) {
                    bool same = same_dimensions(arr, iter->second);
                    if (same) {
                        // This var should be bound to rank k
                        Float64 *witness2 = new Float64(arr->name());

                        GF::Array* gfa = R->GetAttribute(iter->first, arr->name());

                        vector<dods_float64> GFA = gfa->makeArrayf();

                        Array *Nodes = new Array(arr->name(), witness2);
                        Nodes->append_dim(GFA.size(), "nodes");
                        Nodes->set_value(GFA,GFA.size());

                        AttrTable &arrattr1 = arr->get_attr_table();
                        Nodes->set_attr_table(arrattr1);
                        // AttrTable &arrattr = Nodes->get_attr_table();

                        construct->add_var_nocopy(Nodes);
                    }
                    else {
                        // This array does not appear to be associated with
                        // any rank of the unstructured grid. Ignore for now.
                        // Anything else we should do?
                    }
                }
            }
        }
    }
    // TODO Needed?
    //GF::Grid *newgrid = R->GetGrid();

    *btpp = construct;
#endif

    return;
}



/**
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 */

/**
 * Splits the string on the passed char. Returns vector of substrings.
 * TODO make this work on situations where multiple spaces doesn't hose the split()
 */
vector<string> &splitStr(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/**
 * Splits the string on the passed char. Returns vector of substrings.
 */
vector<string> splitStr(const string &s, char delim) {
    vector<string> elems;
    return splitStr(s, delim, elems);
}

/*
 fvcom_mesh {
    String face_coordinates "lonc latc";
    String face_node_connectivity "nv";
    String standard_name "mesh_topology";
    Int32 dimension 2;
    String node_coordinates "lon lat";
 }

struct MeshTopology {
	vector<string>  *nodeCoordinateNames;
	vector<string>  *faceCoordinateNames;
	string faceNodeConnectivityName;
	int dimension;
	vector<Array *> *nodeCoordinateArrays;
	vector<Array *> *faceCoordinateArrays;
	Array *faceNodeConnectivityArray;
};
 */

/**
 *  UGrid vocabulary
 */
const string _cfRole               = "cf_role";
const string _standardName         = "standard_name";
const string _meshTopology         = "mesh_topology";
const string _nodeCoordinates      = "node_coordinates";
const string _faceNodeConnectivity = "face_node_connectivity";
const string _dimension            = "dimension";
const string _location             = "location";
const string _gridLocation         = "grid_location";
const string _node                 = "node";
const string _start_index          = "start_index";



/**
 * Function syntax
 */
string UgridRestrictSyntax = "ugr(dim:int32, rangeVariable:string, [rangeVariable:string, ... ] condition:string)";


/**
 * Function Arguments
 */
struct UgridRestrictArgs {
	int dimension;
	vector<Array *> rangeVars;
	string filterExpression;
};


// Returns true iff the submitted BaseType variable has an attribute called aName attribute whose value is aValue.
static bool checkAttributeValue(BaseType *bt, string aName, string aValue){

    AttrTable &at = bt->get_attr_table();
    DBG(cerr << "checkAttributeValue() - " << "Checking to see if the variable " << bt->name()
    		<< "' has an attribute '"<< aName << "' with value '" << aValue << "'"<<endl);


    // Confirm that submitted variable has an attribute called aName whose value is aValue.
    AttrTable::Attr_iter loc = at.simple_find(aName);
    if (loc != at.attr_end()) {
        DBG(cerr << "checkAttributeValue() - " << "'" << bt->name() << "' has a attribute named '" << aName << "'"<< endl);
        string value = at.get_attr(loc, 0);
        DBG(cerr << "checkAttributeValue() - " << "Attribute '"<< aName <<"' has value of '" << value << "'"<< endl);
        if (value != aValue) {
            return false;
        }
        return true;
    }
    return false;

}

/**
 * Checks the passed BaseType attributes as follows: If the BaseType has a "cf_role" attribute and it's value is the same as
 * aValue return true. If it doesn't have a "cf_role" attribute, then if there is a "standard_name" attribute and it's value is
 * the same as aValue then  return true. All other outcomes return false.
 */
static bool matchesCfRoleOrStandardName(BaseType *bt, string aValue){
    // Confirm that submitted variable has a 'location' attribute whose value is "node".
    if(!checkAttributeValue(bt,_cfRole,aValue)){
    	// Missing the 'cf_role' attribute? Check for a 'standard_name' attribute whose value is "aValue".
    	if(!checkAttributeValue(bt,_standardName,aValue)){
    		return false;
    	}
    }
    return true;
}


/*
 If the two arrays have the exact dimensions in the same order, with the same name, size, start, stop, and stride values,
 return true.  Otherwise return false.
 */
static bool same_dimensions(Array *arr1, Array *arr2) {
	Array::Dim_iter ait1;
	Array::Dim_iter ait2;
	DBG(cerr<< "same_dimensions() - "  << "comparing array " << arr1->name() << " and array " << arr2->name() << endl);

	if(arr1->dimensions(true) != arr1->dimensions(true))
		return false;

	// We start walking both sets of ArrayDimensions at the beginning and increment each together.
	// We end the loop by testing for the end of one set of dimensions because we have already tested
	// that the two sets are the same size.
	for (ait1=arr1->dim_begin(), ait2=arr2->dim_begin();
			ait1!=arr1->dim_end();
			++ait1, ++ait2) {
		Array::dimension ad1 = *ait1;
		Array::dimension ad2 = *ait2;
		DBG(cerr << "same_dimensions() - " << "Comparing: "<< arr1->name() << "["<< ad1.name << "=" << ad1.size << "] AND "<< arr2->name() << "[" << ad2.name << "=" << ad2.size << "] "<< endl);
		if (ad2.name != ad1.name or ad2.size != ad1.size
				or ad2.stride != ad1.stride or ad2.stop != ad1.stop)
			return false;
	}
	if(ait2!=arr2->dim_end())
		return false;

	return true;
}


/**
 * Process the functions arguments and return the structure containing their values.
 */
static UgridRestrictArgs processUgrArgs(int argc, BaseType * argv[]){

    DBG(cerr << "processUgrArgs() - BEGIN" << endl);

	UgridRestrictArgs args;
	args.rangeVars =  vector<Array *>();

    // Check number of arguments; DBG is a macro. Use #define
    // DODS_DEBUG to activate the debugging stuff.
    if (argc < 3)
        throw Error(malformed_expr,"Wrong number of arguments to ugrid restrict function: "+UgridRestrictSyntax+" was passed " + long_to_string(argc) + " argument(s)");


    BaseType * bt;


    // ---------------------------------------------
    // Process the first arg, which is "dimension" or something - WE DON'T REALLY KNOW. (see FIXME below)
    // FIXME Ask Bill/Scott what this is about. Eliminate if not needed.
    bt = argv[0];
    if (bt->type() != dods_int32_c)
        throw Error(malformed_expr,"Wrong type for first argument, expected DAP Int32. "+UgridRestrictSyntax+"  was passed a/an " + bt->type_name());
    //FIXME Tell James what dim is about...
    args.dimension = extract_double_value(bt);

    // ---------------------------------------------
    // Process the last argument, the relational expression used to restrict the ugrid content.
    bt = argv[argc-1];
    if (bt->type() != dods_str_c)
        throw Error(malformed_expr,"Wrong type for third argument, expected DAP String. "+UgridRestrictSyntax+"  was passed a/an " + bt->type_name());
    args.filterExpression = extract_string_argument(bt);



    // --------------------------------------------------
    // Process the range variables selected by the user.
    // We know that argc>=3, because we checked so the
    // following loop will try to find at least one rangeVar,
    // and it won't try to process the first or last members
    // of argv.
    for(int i=1; i<(argc-1) ;i++){
        bt = argv[i];
        if (bt->type() != dods_array_c)
            throw Error(malformed_expr,"Wrong type for second argument, expected DAP Array. "+UgridRestrictSyntax+"  was passed a/an " + bt->type_name());

        Array *newRangeVar = dynamic_cast<Array*>(bt);
        if(newRangeVar == 0) {
            throw Error(malformed_expr,"Wrong type for second argument. "+UgridRestrictSyntax+"  was passed a/an " + bt->type_name());
        }

        DBG(cerr << "processUgrArgs() - The user submitted the range data array: " << newRangeVar->name() << endl);

        // Confirm that submitted variable has a 'location' attribute whose value is "node".
        if(!checkAttributeValue(newRangeVar,_location,_node)){
        	// Missing the 'location' attribute? Check for a 'grid_location' attribute whose value is "node".
        	if(!checkAttributeValue(newRangeVar,_gridLocation,_node)){
                throw Error("The requested range variable '"+newRangeVar->name()+"' has neither a '"+_location+"' attribute " +
                		"or a "+_gridLocation+" attribute whose value is equal to '"+ _node + "'.");
        	}
        }

        vector<Array *>::iterator it;
        for(it=args.rangeVars.begin(); it!=args.rangeVars.end(); ++it) {\
        	Array *rangeVar = *it;
    		if(!same_dimensions(newRangeVar,rangeVar))
    			throw Error("The dimensions of the requested range variable "+newRangeVar->name()+" does not match the shape "
    					+" of the node range (data) array "+rangeVar->name());
        }

        args.rangeVars.push_back(newRangeVar);
    }
    DBG(cerr << "processUgrArgs() - END" << endl);

    return args;

}


/**
 * Finds the first occurrence of a variable that has either an attribute named "cf_role" or and attribute named "standard_name"
 * whose value is set to "mesh_topology"
 */
static BaseType *getMeshTopologyVariable(DDS &dds)
{
    DBG(cerr << "getMeshTopologyVariable() - "  << "Searching dataset for Ugrid mesh_topology variable.." << endl);

    for (DDS::Vars_iter vi = dds.var_begin(); vi != dds.var_end(); vi++) {
        BaseType *bt = *vi;
        AttrTable at = bt->get_attr_table();
        if(matchesCfRoleOrStandardName(bt,_meshTopology)){
            DBG(cerr << "getMeshTopologyVariable() - "  << "DONE" << endl);
        	return bt;
        }
    }

    throw Error("Could not find the mesh topology! No variable in the dataset has an attribute named '"+_cfRole+"' or "+
    		"'"+_standardName+"' whose value is equal to '"+_meshTopology+"'.");

}


/**
 * Returns the coordinate variables identified in the meshTopology variable's node_coordinates attribute.
 * throws an error if the node_coordinates attribute is missing, if the coordinates are not arrays, and
 * if the arrays are not all the same shape.
 */
static vector<Array *> *getNodeCoordinateArrays(BaseType *meshTopology, DDS &dds)
{
    DBG(cerr << "getNodeCoordinatesArrays() - "  << "BEGIN Gathering node coordinate arrays..." << endl);

	string node_coordinates;
    AttrTable at = meshTopology->get_attr_table();

    AttrTable::Attr_iter iter_nodeCoors = at.simple_find(_nodeCoordinates);
    if (iter_nodeCoors != at.attr_end()) {
        node_coordinates = at.get_attr(iter_nodeCoors, 0);
    }
    else {
    	throw Error("Could not locate the "+_nodeCoordinates+" attribute in the "+_meshTopology+" variable! "+
    			"The mesh_topology variable is named "+meshTopology->name());
    }

    vector<Array *> *nodeCoordinateArrays  = new vector<Array *>();

    // Split the node_coordinates string up on spaces
    // TODO make this work on situations where multiple spaces in the node_coorindates string doesn't hose the split()
    vector<string> nodeCoordinateNames = splitStr(node_coordinates,' ');


    // Find each variable in the resulting list
    vector<string>::iterator coorName_it;
    for(coorName_it=nodeCoordinateNames.begin(); coorName_it!=nodeCoordinateNames.end(); ++coorName_it) {
    	string nodeCoordinateName = *coorName_it;

    	//Now that we have the name of the coordinate variable get it from the DDS!!
    	BaseType *btp = dds.var(nodeCoordinateName);
        if(btp==0)
        	throw Error("Could not locate the "+_nodeCoordinates+" variable named '"+nodeCoordinateName+"'! "+
        			"The mesh_topology variable is named "+meshTopology->name());


        Array *newNodeCoordArray = dynamic_cast<Array*>(btp);
        if(newNodeCoordArray == 0) {
            throw Error(malformed_expr,"Node coordinate variable '"+nodeCoordinateName+"' is not an Array type. It's an instance of " + btp->type_name());
        }


        // Make sure this node coordinate variable has the same shape as all the others on the list - error if not true.
        vector<Array *>::iterator cachedCoorVar_it;
        for(cachedCoorVar_it=nodeCoordinateArrays->begin(); cachedCoorVar_it!=nodeCoordinateArrays->end(); ++cachedCoorVar_it) {
        	Array *cachedNodeCoordinateArray = *cachedCoorVar_it;
        	if(!same_dimensions(newNodeCoordArray,cachedNodeCoordinateArray))
            	throw Error("The node coordinate array '"+nodeCoordinateName+"' is not the same shape as the cached node coordinate "+
            			" array '"+cachedNodeCoordinateArray->name()+"'! "+
            			"The mesh_topology variable is named "+meshTopology->name());
        }
        // Add variable to returned vector.
        nodeCoordinateArrays->push_back(newNodeCoordArray);

    }
    DBG(cerr << "getNodeCoordinatesArrays() - "  << "DONE" << endl);

    return nodeCoordinateArrays;

}


/**
 * Locates the the DAP variable identified by the face_node_connectivity attribute of the
 * meshTopology variable.
 */
static Array *getFaceNodeConnectivityArray(BaseType *meshTopology, DDS &dds)
{

    DBG(cerr << "getFaceNodeConnectivityArray() - "  << "Locating FNCA" << endl);

	string face_node_connectivity_var_name;
    AttrTable at = meshTopology->get_attr_table();

    AttrTable::Attr_iter iter_fnc = at.simple_find(_faceNodeConnectivity);
    if (iter_fnc != at.attr_end()) {
    	face_node_connectivity_var_name = at.get_attr(iter_fnc, 0);
    }
    else {
    	throw Error("Could not locate the "+_faceNodeConnectivity+" attribute in the "+_meshTopology+" variable! "+
    			"The mesh_topology variable is named "+meshTopology->name());
    }

	// Find the variable using the name

    BaseType *btp = dds.var(face_node_connectivity_var_name);

    if(btp==0)
    	throw Error("Could not locate the "+_faceNodeConnectivity+" variable named '"+face_node_connectivity_var_name+"'! "+
    			"The mesh_topology variable is named "+meshTopology->name());

    // Additional QC??

    // Is it an array?
    Array *fncArray = dynamic_cast<Array*>(btp);
    if(fncArray == 0) {
        throw Error(malformed_expr,"Face Node Connectivity variable '"+face_node_connectivity_var_name+"' is not an Array type. It's an instance of " + btp->type_name());
    }


    DBG(cerr << "getFaceNodeConnectivityArray() - "  << "Got FCNA '"+fncArray->name()+"'" << endl);

    return fncArray;


}

/**
 * Retrieves the size of the second dimension from a 3xN array. Throws an
 * Error if the array is not the correct shape.
 */
static int getNfrom3byNArray(Array *array){

	int dimCount = array->dimensions(true);

	if(dimCount != 2)
		throw Error("Expected a 2 dimensional array. The array '" + array->name() + "' has " + long_to_string(dimCount) + " dimensions.");

	// Check the first dimension to be sure it's size is 3.
	Array::Dim_iter di = array->dim_begin();
    if (di->c_size != 3) {
    	string msg = "Expected a 2 dimensional array with shape of 3xN! The array "+array->name()+" has a first "+
        		"dimension of size "+ long_to_string(di->c_size);
        DBG(cerr << msg << endl);
        throw Error(malformed_expr,msg);
    }

    // Return the size of the second dimension;
    di++;
    return di->c_size;

}


/**
 * Takes a row major 3xN Face node connectivity DAP array
 * and converts it to a collection GF::Nodes organized as
 * 0,N,2N; 1,1+N,1+2N;
 *
 * This is the inverse operation to getGridFieldCellArrayAsDapArray()
 *
 */
static GF::Node *getFncArrayAsGFNodes(Array *fncVar){

	int nodeCount = getNfrom3byNArray(fncVar);

    // interpret the array data as triangles
    GF::Node *cellids = new GF::Node[fncVar->length()];

    GF::Node *cellids2 = extract_array<GF::Node>(fncVar);

    // Reorganize the cell ids so that cellids contains
    // the cells in three consecutive values (0,1,2; 3,4,5; ...).
    // The the values from  fncVar now in cellids2 and ar organized
    // as 0,N,2N; 1,1+N,1+2N; ...
    for (int j=0;j<nodeCount;j++) {
    	cellids[3*j]=cellids2[j];
        cellids[3*j+1]=cellids2[j+nodeCount];
        cellids[3*j+2]=cellids2[j+2*nodeCount];
    }
    return cellids;
}

/**
 * Returns the value of the "start_index" attribute for the passed Array. If the start_index
 * is missing the value 0 is returned.
 */
static int getStartIndex(Array *array){
    AttrTable &at = array->get_attr_table();
    AttrTable::Attr_iter start_index_iter = at.simple_find(_start_index);
    if (start_index_iter != at.attr_end()) {
        DBG(cerr << "getStartIndex() - "<< "Found the "<< _start_index<<" attribute." << endl);
        AttrTable::entry *start_index_entry = *start_index_iter;
        if (start_index_entry->attr->size() == 1) {
            string val = (*start_index_entry->attr)[0];
            DBG(cerr << "getStartIndex() - " << "value: " << val << endl);
            stringstream buffer(val);
            // what happens if string cannot be converted to an integer?
            int start_index;;

            buffer >> start_index;
            return start_index;
        }
        else {
            throw Error(malformed_expr,"Index origin attribute exists, but either no value supplied, or more than one value supplied.");
        }
    }
    return 0;
}


/**
 * Converts a a row major 3xN Face node connectivity DAP array into a GF::CellArray
 */
static GF::CellArray *getFaceNodeConnectivityCells(Array *faceNodeConnectivityArray)
{
    DBG(cerr << "getFaceNodeConnectivityCells() - Building face node connectivity Cell " <<
    		"array from the Array "<< faceNodeConnectivityArray->name() << endl);

	int rank2CellCount = getNfrom3byNArray(faceNodeConnectivityArray);

    int total_size = 3 * rank2CellCount;

    GF::Node *cellids = getFncArrayAsGFNodes(faceNodeConnectivityArray);

    // adjust for the start_index (cardinal or ordinal array access)
    int startIndex = getStartIndex(faceNodeConnectivityArray);
    if (startIndex != 0) {
		for (int j=0; j<total_size; j++) {
			cellids[j] -= startIndex;
		}
	}
    // Create the cell array
    // TODO Is this '3' the same as the '3' in '3xN'?
    // If so, then this is where we extend the code for faces with more sides.
    GF::CellArray *rankTwoCells = new GF::CellArray(cellids, rank2CellCount, 3);

    DBG(cerr << "getFaceNodeConnectivityCells() - DONE" << endl);
    return rankTwoCells;


}


/**
 * Get a new 3xN DAP Array of Int32 with the same name, attributes, and dimension names
 * as the templateArray. Make the new array's second dimension size N.
 * Returns a DAP Array with an Int32 type template.
 */
static Array *getNewFcnDapArray(Array *templateArray, int N){
	
	// Is the template array a 2D array?
	int dimCount = templateArray->dimensions(true);
	if(dimCount != 2)
		throw Error("Expected a 2 dimensional array. The array '" + templateArray->name() + "' has " + long_to_string(dimCount) + " dimensions.");

	// Is the template array really 3xN?
	Array::Dim_iter di = templateArray->dim_begin();
    if (di->c_size != 3) {
    	string msg = "Expected a 2 dimensional array with shape of 3xN! The array "+templateArray->name()+" has a first "+
        		"dimension of size "+ long_to_string(di->c_size);
        DBG(cerr << msg << endl);
        throw Error(malformed_expr,msg);
    }

    // Get a new template variable for our new array (should be just like the template for the source array)
	//BaseType *arrayTemplate = getDapVariableInstance(templateArray->var(0)->name(),templateArray->var(0)->type());
	Array *newArray = new Array( templateArray->name(), new Int32(templateArray->name()));

    //Add the first dimension (size 3 same same as template array's first dimension)
	newArray->append_dim(3,di->name);

    // Add the second dimension to the result array, but use only the name from the template array's
	// second dimension. The size will be from the passed parameter N
	di++;
	newArray->append_dim(N,di->name);

	newArray->set_attr_table(templateArray->get_attr_table());

	// make the new array big enough to hold all the values.
	newArray->reserve_value_capacity(3*N);

#if 0
	DBG(cerr<<"getNewFcnDapArray() -"<<endl<<endl;
	    cerr << "Newly minted Array: "<< endl;
		newArray->print_val(cerr);
	    cerr<<endl<<endl;
	   )
#endif

	return newArray;
	
}

/**
 * Takes a GF::GridField, extracts it's rank2 GF::CellArray. The GF::CellArray content is
 * extracted and re-packed into a 3xN DAP Array. This is the inverse operation to
 * getFncArrayAsGFNodes()
 */
static Array *getGridFieldCellArrayAsDapArray(GF::GridField *resultGridField, Array *sourceFcnArray){

    DBG(cerr << "getGridFieldCellArrayAsDapArray() - BEGIN" << endl);

	// Get the rank 2 k-cells from the GridField object.
	GF::CellArray* Inb=(GF::CellArray*)(resultGridField->GetGrid()->getKCells(2));

	// This is a vector of size N holding vectors of size 3
	vector< vector<int> > nodes2 = Inb->makeArrayInts();

	Array *resultFcnDapArray = getNewFcnDapArray(sourceFcnArray,nodes2.size());

	// Make a vector to hold the re-packed cell nodes.
	vector<dods_int32> rowMajorNodes;

	// Re-pack the mesh nodes.
	for (unsigned int firstDim=0; firstDim<3 ; firstDim++) {
		for (unsigned int secondDim=0; secondDim < nodes2.size(); secondDim++) {
			dods_int32 val = nodes2.at(secondDim).at(firstDim);
			rowMajorNodes.push_back(val);
		}
	}

	DBG(
		cerr << "getGridFieldCellArrayAsDapArray() - rowMajorNodes: " << endl << "{";
		for (unsigned int j=0; j < rowMajorNodes.size(); j++) {
			dods_int32 val = rowMajorNodes.at(j);
			cerr << val << ", ";
		}
		cerr << "}" << endl;
	)

	// Add them to the DAP array.
	resultFcnDapArray->set_value(rowMajorNodes,rowMajorNodes.size());

	DBG(
		cerr << "getGridFieldCellArrayAsDapArray() - DAP Array: "<< endl;
	    resultFcnDapArray->print_val(cerr);
	   )

    DBG(cerr << "getGridFieldCellArrayAsDapArray() - DONE" << endl);

	return resultFcnDapArray;

}


/**
 * Retrieves a single dimensional rank 0 GF attribute array from a GF::GridField and places the data into
 * DAP array of the appropriate type.
 */
static Array *getRankZeroAttributeNodeSetAsDapArray(
		GF::GridField *resultGridField, Array *sourceArray) {

    DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - BEGIN" << endl);

	// The result variable is assumed to be bound to the GridField with rank 0
	// Try to get the Attribute from rank 0 with the same name as the source array
    DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - Retrieving GF::GridField Attribute '" <<
    		sourceArray->name() << "'" << endl);
    GF::Array* gfa = resultGridField->GetAttribute(0, sourceArray->name());

	Array *dapArray;
	BaseType *templateVar = sourceArray->var();
	string dimName;

	switch (templateVar->type()) {
		case dods_byte_c:
		case dods_uint16_c:
		case dods_int16_c:
		case dods_uint32_c:
		case dods_int32_c: {
			// Get the data
		    DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - GF::Array was made from some type of int, retrieve it as such." << endl);
			vector<dods_int32> GF_ints = gfa->makeArray();
			// Make a DAP array to put the data into.
			dapArray = new Array(sourceArray->name(),
					new Int32(sourceArray->name()));
			// Add the dimension
			dimName = sourceArray->dimension_name(sourceArray->dim_begin());
			dapArray->append_dim(GF_ints.size(), dimName);
			// Add the data
			dapArray->set_value(GF_ints, GF_ints.size());
			break;
		}
		case dods_float32_c:
		case dods_float64_c: {
			// Get the data
		    DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - GF::Array was made from some type of float, retrieve it as such." << endl);
			vector<dods_float64> GF_floats = gfa->makeArrayf();
			// Make a DAP array to put the data into.
			dapArray = new Array(sourceArray->name(),
					new Float64(sourceArray->name()));
			// Add the dimension
			dimName = sourceArray->dimension_name(sourceArray->dim_begin());
			dapArray->append_dim(GF_floats.size(), dimName);
			// Add the data
			dapArray->set_value(GF_floats, GF_floats.size());
			break;
		}
		default:
			throw InternalErr(__FILE__, __LINE__,
					"Unknown DAP type encountered when converting to gridfields array");
	}

	// Copy the source objects attributes.
	dapArray->set_attr_table(sourceArray->get_attr_table());

    DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - DONE" << endl);

	return dapArray;
}

/**
 * Builds the DAP response content from the GF::GridField result object.
 */
static Structure *convertResultGridFieldToDapObject(GF::GridField *resultGridField, BaseType *meshTopologyVar, vector<Array *> *rangeVars, vector<Array *> *coordinateVars, Array *sourceFaceNodeConnectivityArray)
{
    DBG(cerr << "convertResultGridFieldToDapObject() - BEGIN" << endl);

    DBG(cerr << "convertResultGridFieldToDapObject() - Normalizing Grid." << endl);
    resultGridField->GetGrid()->normalize();

    Structure *result = new Structure("ugr_result");
    
    // FIXME fix the names of the variables in the mesh_topology attributes
    // If the server side function can be made to return a DDS or a collection of BaseType's then the
    // names won't change and the original mesh_topology variable and it's metadata will be valid
    DBG(cerr << "convertResultGridFieldToDapObject() - Adding mesh_topology variable to the response." << endl);
    result->add_var(meshTopologyVar);

    // Add the coordinate node arrays to the response.
    DBG(cerr << "convertResultGridFieldToDapObject() - Adding the coordinate node arrays to the response." << endl);
    vector<Array *>::iterator it;
    for(it=coordinateVars->begin(); it!=coordinateVars->end(); ++it) {
    	Array *sourceCoordinateArray = *it;
    	Array *resultCoordinateArray = getRankZeroAttributeNodeSetAsDapArray(resultGridField,sourceCoordinateArray);
        result->add_var_nocopy(resultCoordinateArray);
    }
    
    // Add the range variable data arrays to the response.
    DBG(cerr << "convertResultGridFieldToDapObject() - Adding the range variable data arrays to the response." << endl);
    for(it=rangeVars->begin(); it!=rangeVars->end(); ++it) {
    	Array *rangeVar = *it;
    	Array *resultRangeVar = getRankZeroAttributeNodeSetAsDapArray(resultGridField,rangeVar);
        result->add_var_nocopy(resultRangeVar);
    }

    // Add the new face node connectivity array - make sure it has the same attributes as the original.
    DBG(cerr << "convertResultGridFieldToDapObject() - Adding the new face node connectivity array to the response." << endl);
    Array *resultFaceNodeConnectivityDapArray = getGridFieldCellArrayAsDapArray(resultGridField,sourceFaceNodeConnectivityArray);
	result->add_var_nocopy(resultFaceNodeConnectivityDapArray);

    DBG(cerr << "convertResultGridFieldToDapObject() - DONE" << endl);
    return result;
}


/**
 Subset an irregular mesh (aka unstructured grid).

 @param argc Count of the function's arguments
 @param argv Array of pointers to the functions arguments
 @param dds Reference to the DDS object for the complete dataset.
 This holds pointers to all of the variables and attributes in the
 dataset.
 @param btpp Return the function result in an instance of BaseType
 referenced by this pointer to a pointer. We could have used a
 BaseType reference, instead of pointer to a pointer, but we didn't.
 This is a value-result parameter.

 @return void

 @exception Error Thrown If the Array is not a one dimensional
 array. */
void
function_ugr(int argc, BaseType * argv[], DDS &dds, BaseType **btpp)
{
    DBG(cerr << "function_ugr() - BEGIN" << endl);

    static string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"ugrid_restrict\" version=\"0.1\">\n" +
    "Server function for Unstructured grid operations.\n" +
    "usage: " + UgridRestrictSyntax +"\n"
    "</function>";

    if (argc == 0) {
        Str *response = new Str("info");
        response->set_value(info);
        *btpp = response;
        return;
    }

    // Process and QC the arguments
    UgridRestrictArgs args = processUgrArgs(argc,argv);

    // For convenience, cache the pointer to the collection of user selected range variables
    //    Array *rangeVar =  args.rangeVar;
    vector<Array *> &rangeVars =  args.rangeVars;

    // FIXME Look at the range variables and sort them by the different mesh_topology variables that they associate themselves with.

    // FIXME Locate all the meshTopology variables.


    // ----------------------------------------------------------------------------------------------------
    // Collect the Mesh Topology components. FIXME - make this work with multiple mesh_topology variables
    // Locate the mesh_topology variable in the dataset as defined in the ugrid specification
    BaseType *meshTopologyVariable = getMeshTopologyVariable(dds);

    // Retrieve the node coordinate arrays for the mesh
    vector<Array *> *nodeCoordinates = getNodeCoordinateArrays(meshTopologyVariable,dds);

    // Make sure that the requested range variable is the same shape as the node coordinate arrays
    // We only need to test the first nodeCoordinate array against the first rangeVar array
    // because we have already made sure all of the node coordinate arrays are the same size and
    // that all the rangeVar arrays are the same size. This is just to compare the two collections.
    Array *firstCoordinate = (*nodeCoordinates)[0];
    Array *firstRangeVar = rangeVars[0];
	if(!same_dimensions(firstRangeVar,firstCoordinate))
		throw Error("The dimensions of the requested range variable "+firstRangeVar->name()+" does not match the shape "
				+" of the node coordinate array "+firstCoordinate->name());
    
    // Locate the Face Node Connectivity array.
	Array *faceNodeConnectivityArray = getFaceNodeConnectivityArray(meshTopologyVariable,dds);

    // ----------------------------------
    // OK, so up to this point we have not read any data from the data set, but we have QC'd the inputs and verified that
    // it looks like the request is consistent with the semantics of the dataset.
    // Now it's time to read some data and pack it into the GridFields library...
	
    // Start building the Grid for the GridField operation.
	// This is, I think essentially a representation of the
	// mesh_topology

    // TODO Is the Grid G Leaked?
    // TODO This is the 'domain' data?
    DBG(cerr << "function_ugr() - Constructing new GF::Grid" << endl);
    GF::Grid *G = new GF::Grid("result");

    // 1) Make the implicit nodes - same size as the range and the coordinate node arrays
    DBG(cerr << "function_ugr() - Building and adding implicit range Nodes to the GF::Grid" << endl);
    int node_count = rangeVars[0]->length();
    GF::AbstractCellArray *nodes = new GF::Implicit0Cells(node_count);
    // Attach the implicit nodes to the grid at rank 0
    G->setKCells(nodes, 0);

    // Attach the Mesh to the grid.
    // Get the face node connectivity cells (i think these correspond to the GridFields K cells of Rank 2)
    DBG(cerr << "function_ugr() - Building face node connectivity Cell array from the DAP version" << endl);
    GF::CellArray *faceNodeConnectivityCells = getFaceNodeConnectivityCells(faceNodeConnectivityArray);

    // Attach the Mesh to the grid at rank 2
    // TODO Is this 2 the same as the value of the "dimension" attribute in the "mesh_topology" variable?
    DBG(cerr << "function_ugr() - Attaching Cell array to GF::Grid" << endl);
    G->setKCells(faceNodeConnectivityCells, 2);

    // The Grid is complete. Now we make a GridField from the Grid
    DBG(cerr << "function_ugr() - Construct new GF::GridField from GF::Grid" << endl);
    GF::GridField *inputCells = new GF::GridField(G);
    GF::Array *gfa;

    // We add the coordinate data (using GridField->addAttribute() to the GridField at
    // grid dimension 0 ( key?, rank?? whatever this is)
    DBG(cerr << "function_ugr() - Adding node coordinates to GF::GridField at rank 0" << endl);
    vector<Array *>::iterator it;
    for(it=nodeCoordinates->begin(); it!=nodeCoordinates->end(); ++it) {
    	Array *nc = *it;
    	gfa = extract_gridfield_array(nc);
    	inputCells->AddAttribute(0,gfa);
    }

    // We add the requested range data arrays to the GridField at grid dimension key (rank?? whatever this is) 0.
    DBG(cerr << "function_ugr() - Adding range variables to GF::GridField at rank 0" << endl);
    for(it=rangeVars.begin(); it!=rangeVars.end(); ++it) {
    	Array *rangeVar = *it;
    	gfa = extract_gridfield_array(rangeVar);
    	inputCells->AddAttribute(0,gfa);
    }

    // We add faceNodeConnectivity data at grid dimension key (rank?? whatever this is) 2.
    DBG(cerr << "function_ugr() - Adding face node connectivity Cell array to GF::GridField at rank 2" << endl);
	gfa = extract_gridfield_array(faceNodeConnectivityArray);
	inputCells->AddAttribute(2,gfa);


    try {
        // Build the restriction operator;
        DBG(cerr << "function_ugr() - Constructing new GF::RestrictOp using user "<<
        		"supplied dimension value and filter expression combined with the GF:GridField " << endl);
    	GF::RestrictOp op = GF::RestrictOp(args.filterExpression, args.dimension, inputCells);


    	// Apply the operator and get the result;
        DBG(cerr << "function_ugr() - Applying GridField operator." << endl);
        GF::GridField *R = new GF::GridField(op.getResult());


        // Get the GridField back in a DAP representation of a ugrid.
        // TODO This returns a single structure but it would make better sense to the
        // world if it could return a vector of objects and have them appear at the
        // top level of the DDS.
        // FIXME Because the metadata attributes hold the key to understanding the response we
        // need to allow the user to request DAS and DDX for the function call.
        DBG(cerr << "function_ugr() - Converting result GF:GridField to DAP data structure.." << endl);
        Structure *construct = convertResultGridFieldToDapObject(R, meshTopologyVariable, &rangeVars, nodeCoordinates, faceNodeConnectivityArray);
        *btpp = construct;

        //delete R;


    }
    catch(std::bad_alloc &e) {
        throw Error("Unable to construct GF::RestrictOp. Bad Allocation Exception. std::bad_alloc.where(): '"+string(e.what())+"'");
    }
    //delete inputCells;


    DBG(cerr << "function_ugr() - END" << endl);


    return;
}


void function_newUGR(){
	// look at user supplied range (data) vars.
	// Make sure you apply constrains
	// QC vars (shapes, etc)
	// for each range var check for a mesh attribute
	// use the value of the mesh attribute to locate the mesh_topology var for that variable
	// make a collection pof all the requested range vars for each mesh topology
	// look at the location attriobute for each rangeVar
	// - we know what to do with node data, so do that
	// - figure out what to do with face data.
	// build one GF::GridField for each mesh_topology (and maybe one for faces and one for nodes for each mesh_topology
	// allpy GF::RestrictOp using user supplied filter expression to every GF::GridField
	// get results
	// un pack into a DAP object


}


} // namespace libdap
#endif // Disabling cruft code from earlier ugrid work - ndp 03/25/2013
