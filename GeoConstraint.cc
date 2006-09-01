
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

// The Grid Selection Expression Clause class.


#include "config.h"

static char id[] not_used = {"$Id$"};

#include <math.h>
#include <string.h>

#include <iostream>
#include <sstream>

#define DODS_DEBUG

#include "debug.h"
#include "dods-datatypes.h"
#include "GeoConstraint.h"
#include "Float64.h"

#include "Error.h"
#include "InternalErr.h"
#include "ce_functions.h"

using namespace std;
using namespace libdap;

/** If an attribute value is quoted, remove the quotes.
    
    This function only removes a starting and ending quote if both are 
    present. 
    
    @param value A value-result parameter. */
static void
remove_quotes(string &value)
{
    if (!value.empty() && value[0] == '"' && value[value.length()-1] == '"') {
        value.erase(0,1);
        value.erase(value.length()-1);
    }
    
    return;
}

/** A private method called by the constructor that searches for latitude
    and longitude map vectors. This method returns false if either map
    cannot be found. It assumes that the d_grid and d_dds fields are set.
    
    The d_longitude, d_lon, d_lon_length and d_lon_grid_dim (and matching
    lat) fields are modified. 
    
    @note Rules used to find Maps:<ul>
    <li>Latitude: If the Map has a units attribute of "degrees_north", 
    "degree_north", "degree_N", or "degrees_N"</li>
    <li>Longitude: If the map has a units attribute of "degrees_east" 
    (eastward positive), "degree_east", "degree_E", or "degrees_E"</li>
    </ul>
    
    @return True if the maps are found, otherwise False */
bool
GeoConstraint::find_lat_lon_maps() throw(Error)
{
    Grid::Map_iter m = d_grid->map_begin();
    // Assume that a Grid is correct and thus has exactly as many maps at its
    // array part has dimensions. Thus don't bother to test the Grid's array
    // dimension iterator for '!= dim_end()'.
    Array::Dim_iter d = dynamic_cast<Array&>(*d_grid->array_var()).dim_begin();
    // The fields d_latitude and d_longitude are initialized to null
    while ( m != d_grid->map_end() && (!d_latitude || !d_longitude) ) {
        string units_value = (*m)->get_attr_table().get_attr("units");
        remove_quotes(units_value);
        if (!d_latitude && !units_value.empty() 
            && d_coards_lat_units.find(units_value) != d_coards_lat_units.end()) {

            // Set both d_latitude (a pointer to the real map vector) and
            // d_lat, a vector of the values represented as doubles. It's easier
            // to work with d_lat, but it's d_latitude that needs to be set
            // when constraing the grid. Also, record the grid variable's
            // dimension iterator so that it's easier to set the Grid's Array
            // (which also has to be constrained).
            d_latitude = dynamic_cast<Array*>(*m);

            if (!d_latitude->read_p())
                throw InternalErr(__FILE__, __LINE__, "Grid latitude map not read.");

            d_lat = extract_double_array(d_latitude); // throws Error
            d_lat_length = d_latitude->length();
            
            d_lat_grid_dim = d;
        }
            
        if (!d_longitude && !units_value.empty() 
            && d_coards_lon_units.find(units_value) != d_coards_lon_units.end()) {

            d_longitude = dynamic_cast<Array*>(*m);

            if (!d_longitude->read_p())
                throw InternalErr(__FILE__, __LINE__, "Grid longitude map not read.");

            d_lon = extract_double_array(d_longitude);
            d_lon_length = d_longitude->length();
            
            d_lon_grid_dim = d;
        }
        
        ++m;
        ++d;
    }
    
    return d_lat && d_lon;
}

/** A private method that determines if the longitude part of the bounding
    box uses 0/359 or -180/179 notation. This class only supports latitude
    constriants which use 90/-90 notation.
    
    @note There's no real way to tell if the user is thinking in term of the 
    -180/179 notation and the ask for 30 degrees to 50 degress (or 50 to 30,
    for that matter). This function assumes that if one of the two values is
    negative, then the notation is or the -180/179 form, otherwise not.
    
    @param left The left side of the bounding box, in degrees
    @param right The right side of the boubding box
    @return The notation (pos or neg_pos) */
GeoConstraint::Notation
GeoConstraint::categorize_notation(double left, double right) const
{
    return (left < 0 || right < 0) ? neg_pos: pos;
}    

/* A private method to translate the longitude constraint from -180/179
   notation to 0/359 notation.
   @param left Value-result parameter; the left side of the bounding box
   @parm right Value-result parameter; the right side of the bounding box */
void
GeoConstraint::transform_constraint_to_pos_notation(double &left, double &right) const
{
    if (left < 0 || right < 0) {        // double check
        left += 180;
        right += 180;
    }
}


/** Given that the Grid has a longitude map that uses the 'neg_pos' notation,
    transform it to the 'pos' notation. This method modifies the d_longitude
    Array. */
void
GeoConstraint::transform_longitude_to_pos_notation()
{
    // Assume earlier logic is correct (since the test is expensive)
    // for each value, add 180
    // Longitude could be represented using any of the numeric types
    for (int i = 0; i < d_lon_length; ++i)
        d_lon[i] += 180;
}

/** Given that the Grid has a longitude map that uses the 'pos' notation,
    transform it to the 'neg_pos' notation. This method modifies the 
    d_longitude Array. */
void
GeoConstraint::transform_longitude_to_neg_pos_notation()
{
    for (int i = 0; i < d_lon_length; ++i)
        d_lon[i] -= 180;
}

/** Scan from the left to the right, and the right to the left, looking
    for the left and right bounding box edges, respectively. 
    
    @param left The left edge of the bounding box
    @param right The right edge
    @param d_longitude_index_left Value-result parameter that holds the index
    in the grid's longitude map of the left bounding box edge. Uses a closed
    interval for the test.
    @param  d_longitude_index_right Value-result parameter for the right edge
    index. */
void
GeoConstraint::find_longitude_indeces(double left, double right, 
                                      int &longitude_index_left, 
                                      int &longitude_index_right) const
{
    // Use all values 'modulo 360' to take into account the cases where the 
    // constraint and/or the longitude vector are given using values greater
    // than 360 (i.e., when 380 is used to mean 20).
    double t_left = fmod(left, 360.0);
    double t_right = fmod(right, 360.0);
    
    // Scan from the left 
    int i = 0;
    while (i < d_lon_length && fmod(d_lon[i], 360.0) < t_left)
        ++i;
    longitude_index_left = i;
    
    // and from the right
    i = d_lon_length -1;
    while (i > -1 && fmod(d_lon[i], 360) > t_right)
        --i;
    longitude_index_right = i;
}

/** Scan from the top to the bottom, and the bottom to the top, looking
    for the top and bottom bounding box edges, respectively. 
    
    @param top The top edge of the bounding box
    @param bottom The bottom edge
    @param latitude_index_top Value-result parameter that holds the index
    in the grid's latitude map of the top bounding box edge. Uses a closed
    interval for the test.
    @param  latitude_index_bottom Value-result parameter for the bottom edge
    index. */
void
GeoConstraint::find_latitude_indeces(double top, double bottom, 
                                     int &latitude_index_top, 
                                     int &latitude_index_bottom) const
{
    // Scan from the top down
    int i = 0;
    while (i < d_lat_length && d_lat[i] < top)
        ++i;
    latitude_index_top = i;
    
    // and from the bottom up
    i = d_lat_length -1;
    while (i > -1 && d_lat[i] > bottom)
        --i;
    latitude_index_bottom = i;
}

/** Given the top and bottom sides of the bounding box, use the Grid or Array
    latitude information to constrain the data to that bounding box. 
    
    @note Make \e sure to call this before set_bounding_box_longitude().
    
    @param top The top side of the bounding box, in degress
    @param bottom The bottom side */
void
GeoConstraint::set_bounding_box_latitude(double top, double bottom) throw(Error)
{
    // This is simpler than the longitude case because there's no need to test
    // for several notations, no need to accommodate them in the return, no
    // modulo arithmetic in the axis and no need to account for a constraint with
    // two disconnected parts to be joined.
    find_latitude_indeces(top, bottom, d_latitude_index_top, d_latitude_index_bottom);
    
    // Apply constraint, stride is always one and maps only have one dimension
    Array::Dim_iter fd = d_latitude->dim_begin() ;
    d_latitude->add_constraint(fd, d_latitude_index_top, 1, d_latitude_index_bottom);
    dynamic_cast<Array&>(*d_grid->array_var()).add_constraint(d_lat_grid_dim, 
                                                              d_latitude_index_top, 
                                                              1, 
                                                              d_latitude_index_bottom);

    d_latitude_constraint_set = true;
}
                                 
static void
swap_vector_ends(char *dest, char *src, int len, int index, int elem_sz)
{
    memcpy(dest, src + index * elem_sz, (len - index) * elem_sz );
           
    memcpy(dest + (len - index) * elem_sz, src, index * elem_sz );
}
        
/** Reorder the elements in the longitude map so that the longitude constraint no 
    longer crosses the edge of the map's storage. The d_lon field is
    modified.
    
    @note The d_lon vector always has double values regardless of the type
    of d_longitude.
    
    @param d_longitude_index_left The left edge of the bounding box. */
void
GeoConstraint::reorder_longitude_map(int longitude_index_left)
{
    double *tmp_lon = new double[d_lon_length];
    
    swap_vector_ends((char*)tmp_lon, (char*)d_lon, d_lon_length, 
                     longitude_index_left, sizeof(double));

    memcpy(d_lon, tmp_lon, d_lon_length * sizeof(double));
    
    delete[] tmp_lon;
}
 
/** Reorder the data values relative to the longitude axis so that the
    reordered longitude map (see GeoConstraint::reorder_longitude_map()) 
    and the data values match.
    
    @todo First read the entire array into temporary storage. Then use a
    temporary vector to reorder each row of that copy of the array. Finally,
    copy the data back into the array and delete the temporary array and 
    vector. Once this works, optimize.
    
    @param d_longitude_index_left The left edge of the bounding box. */ 
void
GeoConstraint::reorder_data_longitude_axis() throw(Error)
{
    // Read the two parts of the Grid's Array using the read() method.
    // Then combine them into one lump of data stored in the GeoConstraint
    // object.
    
    // Make a constraint for the 'left' part, which goes from the left index
    // to the right side of the array. Use read() to get the data.
    Array &a = dynamic_cast<Array&>(*d_grid->array_var()); 

    a.add_constraint(d_lon_grid_dim, d_longitude_index_left, 1, d_lon_length);
    a.set_read_p(false);
    a.read(d_dataset);
    char *left_data = 0;
    int left_size = a.buf2val((void**)&left_data);

    // Build a constraint for the 'right' part, which 
    // goes from the left edge of the array to the right index and read those
    // data.
    a.add_constraint(d_lon_grid_dim, 0, 1, d_longitude_index_right);
    a.set_read_p(false);
    a.read(d_dataset);
    char *right_data = 0;
    int right_size = a.buf2val((void**)&right_data);
    
    // Make one big lump O'data, combine the left_ and right_data blobs and
    // record the result in this object
    d_grid_array_data_size = left_size + right_size;
    d_grid_array_data = new char[d_grid_array_data_size];
    int left = 0;
    int right = 0;
    int i = 0;
    // Interleve the left and right_data vectors. This works for 2 and 3
    // dimensions. Not sure about 4, 5, ... jhrg 8/31/06
    while(i < d_grid_array_data_size) {
        d_grid_array_data[i++] = left_data[left++];
        d_grid_array_data[i++] = right_data[right++];
    }

    delete [] left_data;
    delete [] right_data;
    
#if 0
    Array &a = dynamic_cast<Array&>( *(d_grid->array_var()) );

    // make sure the data are present before doing this!
    if (!a.read_p())
        throw InternalErr(__FILE__, __LINE__, "Grid  Array not read.");

    // Use d_lon_length and d_lat_length to find the size of the array to
    // save some steps.
    int element_size = a.var()->width();
    
    char *tmp_data_row = new char[d_lon_length * element_size];
#if 0
    a.buf2val((void**)&d_grid_array_data); // Allocates storage using new []
#endif
    
    int row = 0;
    while (row < d_lat_length) {
        // row_data points to the start of 'row' within d_grid_array_data
        char *row_data = d_grid_array_data + (row  * element_size * d_lon_length); 
        // Treat each row like a vector and swap left and right bounding box
        // edges; put the result in a temporary storage
        swap_vector_ends(tmp_data_row, row_data, d_lon_length, 
                         longitude_index_left, element_size);
        // Copy the result back to d_grid_array_data using 'row_data'
        memcpy(row_data, tmp_data_row, d_lon_length * element_size);

        ++row;
    }
    
    a.val2buf(d_grid_array_data, true);    // true == reuse the array's mem
    
#if 0
    delete [] tmp_data_array;
#endif
    delete tmp_data_row;
#endif
}

/** Given the left and right sides of the bounding box, use the Grid or Array
    longitude information to constrain the data to that bounding box. This
    method takes into account the two different notations commonly used to 
    specify longitude (0/359 and -180/179) and that the longitude axis is
    cyclic so that 360 == 0 (== 720, ...).
    
    @todo Make this method correctly reorder the Grid/Array when the longitude
    constraint crosses the edge of the data's storage.
    
    @param left The left side of the bounding box, in degress
    @param right The right side */
void
GeoConstraint::set_bounding_box_longitude(double left, double right) throw(Error)
{
    // Categorize the notation used by the bounding box (0/359 or -180/179).
    Notation constraint_notation = categorize_notation(left, right);
    
    // If the notation uses -180/179, transform the request to 0/359 notation.
    if (constraint_notation == neg_pos)
        transform_constraint_to_pos_notation(left, right);
    
    // If the grid uses -180/179, transform it to 0/359 as well. This will
    // subsequent logic easier and adds only a few extra operations, even with
    // large maps.
    Notation longitude_notation = categorize_notation(d_lon[0], d_lon[d_lon_length-1]);

    if (longitude_notation == neg_pos)
        transform_longitude_to_pos_notation();
        
    // Find the longitude map indeces that correspond to the bounding box.
    find_longitude_indeces(left, right, d_longitude_index_left, d_longitude_index_right);

    // Does the longitude constraint cross the edge of the longitude vector?
    // If so, reorder the grid's data (array), longitude map vector and the 
    // local vector of longitude data used for computation.
    if (d_longitude_index_left > d_longitude_index_right) {
        reorder_longitude_map(d_longitude_index_left);
        
        // If the longitude constraint is 'split', join the two parts, reload
        // the data into the Grid's Array and make sure the Array is marked as
        // already read. This should be true for the whole Grid, but if some
        // future modification changes that, the array will be covered here.
        // Note that the following method only reads the data out and stores
        // it in this object after joining the two parts. The method
        // apply_constraint_to_data() transfers the data back from the this
        // objet to the DAP Grid variable.
        reorder_data_longitude_axis();
        d_grid->array_var()->set_read_p(true);
        
        // alter the indices; the left index has now been moved to 0, and the right
        // index is now at lon_vector_length-left+right.
        d_longitude_index_right = d_lon_length - 1 - d_longitude_index_left + d_longitude_index_right;
        d_longitude_index_left = 0;
    }
    else {
        // If the longitude constraint is not split, let serialize() and read()
        // handle getting the data.
        d_grid->array_var()->set_read_p(false);
    }
    // If the constraint used the -180/179 (neg_pos) notation, transform
    // the longitude map s it uses the -180/179 notation. Note that at this
    // point, d_longitude always uses the pos notation because of the earlier
    // conditional transforamtion. 
    
    // Do this _before_ applying the constraint since set_array_using_double()
    // tests the array length using Vector::length() and that method returns 
    // the length _as constrained_. We want to move all of the longitude 
    // values from d_lon back into the map, not just the number that will be
    // sent (although an optimization might do this, it's hard to imagine 
    // it would gain much).
    if (constraint_notation == neg_pos) {
        transform_longitude_to_neg_pos_notation();
    }
        
    // Apply constraint, stride is always one and maps only have one dimension
    Array::Dim_iter fd = d_longitude->dim_begin();
    d_longitude->add_constraint(fd, d_longitude_index_left, 1, d_longitude_index_right);

    dynamic_cast<Array&>(*d_grid->array_var()).add_constraint(d_lon_grid_dim, 
                                                              d_longitude_index_left, 
                                                              1, 
                                                              d_longitude_index_right);
    d_longitude_constraint_set = true;
}

/** @brief Initialize GeoConstraint with a Grid.
    @param grid Set the GeoConstraint to use this Grid variable. It is the 
    caller's responsibility to ensure that the value \e grid is a valid Grid 
    variable.
    @param ds_name The name of the dataset. Passed to BaseType::read().
    @param dds Use this DDS to get global attributes.  
 */
GeoConstraint::GeoConstraint(Grid *grid, const string &ds_name, const DDS &dds) 
    throw (Error)
   : d_grid(grid), d_dataset(ds_name), d_dds(dds), 
     d_grid_array_data(0), d_grid_array_data_size(0),
     d_latitude(0), d_longitude(0), d_lat(0), d_lon(0), 
     d_bounding_box_set(false), d_latitude_constraint_set(false),
     d_longitude_constraint_set(false)
{
    // Build sets of attribute values for easy searching. Maybe overkill???
    d_coards_lat_units.insert("degrees_north");
    d_coards_lat_units.insert("degree_north");
    d_coards_lat_units.insert("degree_N");
    d_coards_lat_units.insert("degrees_N");

    d_coards_lon_units.insert("degrees_east");
    d_coards_lon_units.insert("degree_east");
    d_coards_lon_units.insert("degrees_E");
    d_coards_lon_units.insert("degree_E");

    // Is this Grid a geo-referenced grid? Throw Error if not.
    if (!find_lat_lon_maps())
        throw Error(string("The grid '") + d_grid->name() 
            + "' does not have identifiable latitude/longitude map vectors.");
#if 0
    // If we have a valid Grid, then slurp the data into this object's 
    // d_grid_array_data field. This is done here because the data are already
    // in the Grid (since the CE parser reads the Grid when it builds the 
    // argument list passed to the function) so there's no real loss of
    // efficiency. Maybe someday there will be a smarter data reader that 
    // reads just the constrained part of the Grid's array... jhrg 8/30/06
    d_grid->array_var()->buf2val((void**)&d_grid_array_data);
#endif
}

/** Set the bounding box for this constraint. After calling this method the
    Grid or Array variable passed to this object will be constrained by
    the given longitude/latitude box. This will read the grid's array data
    using Array::read() if the longitude constraint crosses the right edge
    of the Grid's array boundry (i.e. the constraint is split across the 
    edge of the array.
    
    @param left The left side of the bounding box.
    @param right The right side
    @param top The top
    @param bottom The bottom */
void
GeoConstraint::set_bounding_box(double left, double top, 
                                double right, double bottom) throw (Error)
{
    // Ensure this method is called only once. What about pthreads?
    // The method Array::reset_constraint() might make this so it could be
    // called more than once. jhrg 8/30/06
    if (d_bounding_box_set)
        throw InternalErr("It is not possible to register more than one geo constraint on a grid.");
    d_bounding_box_set = true;
    
    set_bounding_box_latitude(top, bottom);
    set_bounding_box_longitude(left, right);
}

/** Once the bounding box is set use this method to apply the constraint. This
    modifies the data values in the Grid so that the software in 
    Vector::serialize() will work correctly. Vector::serialize() assumes that
    the BaseType::read() method is called \e after the projection is applied to
    the data. That is, the projectionis applied, then data are read. but
    geogrid() first reads all the data values and then computes the projection.
    To make Vector::serialize() work, this method uses the projection
    information recorded in the Grid by set_bounding_box() to arrange data so
    that the information to be sent is all that is held by the Grid. Call this
    after applying any 'Grid selection expressions' of the sort that can be 
    passed to the grid() function. 
    
    @note Why do this here? The grid() function uses the standard logic in
    Vector and elsewhere to read data that's to be sent. The problem is that
    the data values need to be reordered using information only this object
    has. If this were implemented as a 'selection function' (i.e., if the code
    was run by ConstraintExpression::eval() then we might e able to better
    optimize how data are read, but in this case we have read all the data
    and may have alredy reorganized it. Set up the internal buffers so they
    hold the correct values and mark the Grid's array and lat/lon maps as
    read. */
     
void
GeoConstraint::apply_constraint_to_data() throw(Error)
{
    if (!d_latitude_constraint_set || !d_longitude_constraint_set)
        throw InternalErr("The Latitude and Longitude constraints must be set before calling apply_constraint_to_data().");

    // transfer data from this object back into the Grid; first for the 
    // latitude and longitude maps...
    set_array_using_double(d_latitude, &d_lat[d_latitude_index_top], 
                           d_latitude_index_bottom - d_latitude_index_top + 1);
                           
    set_array_using_double(d_longitude, &d_lon[d_longitude_index_left], 
                           d_longitude_index_right - d_longitude_index_left + 1);
                           
    // ... and then the Grid's array if it has been read. If the data do not 
    // need to be manipulated here, postpone the read until serialize() is
    // called.
    if (d_grid_array_data) {
        int size = d_grid->array_var()->val2buf(&d_grid_array_data);
        if (size == d_grid_array_data_size)
            throw InternalErr("Expected data size not copied to the Grid's buffer.");
    }
    
#if 0
    // Use d_lon_length and d_lat_length to find the size of the array to
    // save some steps. This looks like a bug ...
    int element_size = d_grid->array_var()->var()->width();
    
    int lon_length = d_longitude_index_right - d_longitude_index_left + 1;
    int lat_length = d_latitude_index_bottom - d_latitude_index_top + 1;
    
    char *constrained_data = new char[lat_length * lon_length * element_size];
    
    int source_row = d_latitude_index_top;
    int dest_row = 0;
    while (dest_row < lat_length) {
        // row_data points to the start of 'row' within d_grid_array_data
        char *row_data = d_grid_array_data 
                                + (source_row  * element_size * d_lon_length)
                                + d_longitude_index_left; 
        char *constrained_row_data = constrained_data
                                + (dest_row * lon_length * element_size);
                                
        memcpy(constrained_row_data, row_data, lon_length * element_size);

        ++source_row;
        ++dest_row;
    }
    
    d_grid->array_var()->val2buf((void*)constrained_data);
    
    delete [] constrained_data;
#endif
}

////////////////////////////////////////////////////////////////////////
//
// left over. might use??

#if 0
template<class T>
static void
add_to_vector(void *data, int length, int value)
{
    for (int i = 0; i < length; ++i) {
        static_cast<T*>(data)[i] += value;
    }
}

/** Add a constant value to an Array of numeric types.
    @param array Array to modify
    @param value Add this to each element of \e array. */
static void
add_to_array(Array *array, int value)
{
    char *data = 0;
    array->buf2val((void**)&data);
    int length = array->length();
    
    switch (array->var()->type()) {
        case dods_byte_c:
            add_to_vector<dods_byte>((void*)data, length, value);
            break;
        case dods_uint16_c:
            add_to_vector<dods_uint16>((void*)data, length, value);
            break;
        case dods_int16_c:
            add_to_vector<dods_int16>((void*)data, length, value);
            break;
        case dods_uint32_c:
            add_to_vector<dods_uint32>((void*)data, length, value);
            break;
        case dods_int32_c:
            add_to_vector<dods_int32>((void*)data, length, value);
            break;
        case dods_float32_c:
            add_to_vector<dods_float32>((void*)data, length, value);
            break;
        case dods_float64_c:
            add_to_vector<dods_float64>((void*)data, length, value);
            break;
        default:
            throw Error("Not a recognized numeric type");
    }
    
    array->val2buf(data, true);     // reuse true
    
    delete[] data;
}
#endif

