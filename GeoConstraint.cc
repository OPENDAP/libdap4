
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

#include <iostream>
#include <sstream>

#include "debug.h"
#include "dods-datatypes.h"
#include "GeoConstraint.h"
#include "Float64.h"

#include "ce_functions.h"
#include "Error.h"
#include "InternalErr.h"

using namespace std;

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

/** A private method call by the constructor that searches for latitude
    and longitude map vectors. This method returns false if either map
    cannot be found. It assume that the d_grid and d_dds fields are set.
    
    @note Rules used to find Maps:<ul>
    <li>Latitude: If the Map has a units attribute of "degrees_north", 
    "degree_north", "degree_N", or "degrees_N"</li>
    <li>Longitude: If the map has a units attribute of "degrees_east" 
    (eastward positive), "degree_east", "degree_E", or "degrees_E"</li>
    </ul>
    
    @return True if the maps are found, otherwise False */
bool
GeoConstraint::find_lat_lon_maps()
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

            d_lat = extract_double_array(dynamic_cast<Array*>(*m));
            d_lat_length = dynamic_cast<Array*>(*m)->length();
            
            d_lat_grid_dim = d;
        }
            
        if (!d_longitude && !units_value.empty() 
            && d_coards_lon_units.find(units_value) != d_coards_lon_units.end()) {

            d_longitude = dynamic_cast<Array*>(*m);

            d_lon = extract_double_array(dynamic_cast<Array*>(*m));
            d_lon_length = dynamic_cast<Array*>(*m)->length();
            
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
    @param longitude_index_left Value-result parameter that holds the index
    in the grid's longitude map of the left bounding box edge. Uses a closed
    interval for the test.
    @param  longitude_index_right Value-result parameter for the right edge
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
    int longitude_index_left, longitude_index_right;
    find_longitude_indeces(left, right, longitude_index_left, longitude_index_right);

    // Does the longitude constraint cross the edge of the longitude vector?
    // If so, ...
    if (longitude_index_left > longitude_index_right) {
        throw Error("GeoConstraint::set_bounding_box() need work.");
    }
        
    // If the constraint used the -180/179 (neg_pos) notation, transform
    // the longitude map s it uses the -180/179 notation. Note that at this
    // point, d_longitude always uses the pos notation because of the earlier
    // conditional transforamtion. 
    
    // Do this _before_ applying the constraint since set_array_using_double()
    // tests the array length using Vector::length() and that method returns 
    // the length _as constrained_. We want to move all of the longitude 
    // values from d_lon back into the map, not just the number that will be
    // sent (although an optimization misght do this, it's hard to imagine 
    // that would gain much).
    if (constraint_notation == neg_pos) {
        transform_longitude_to_neg_pos_notation();
        set_array_using_double(d_longitude, d_lon, d_lon_length);
    }

    // Apply constraint, stride is always one and maps only have one dimension
    Array::Dim_iter fd = d_longitude->dim_begin() ;
    d_longitude->add_constraint(fd, longitude_index_left, 1, longitude_index_right);
    dynamic_cast<Array&>(*d_grid->array_var()).add_constraint(d_lon_grid_dim, 
                                                              longitude_index_left, 
                                                              1, 
                                                              longitude_index_right);
}

void
GeoConstraint::set_bounding_box_latitude(double top, double bottom) throw(Error)
{
    // This is simpler than the longitude case because there's no need to test
    // for several notations, no need to accommodate them in the return, no
    // modulo arithmetic in the axis and no need to account for a constraint with
    // two disconnected parts to be joined.
    int latitude_index_top, latitude_index_bottom;
    find_latitude_indeces(top, bottom, latitude_index_top, latitude_index_bottom);
    
    // Apply constraint, stride is always one and maps only have one dimension
    Array::Dim_iter fd = d_latitude->dim_begin() ;
    d_latitude->add_constraint(fd, latitude_index_top, 1, latitude_index_bottom);
    dynamic_cast<Array&>(*d_grid->array_var()).add_constraint(d_lat_grid_dim, 
                                                              latitude_index_top, 
                                                              1, 
                                                              latitude_index_bottom);
}
                                 
/** @brief Initialize GeoConstraint with a Grid.
    @param grid Set the GeoConstraint to use this Grid variable. It is the 
    caller's responsibility to ensure that the value \e grid is a valid Grid 
    variable.
    @param dds Use this DDS to get global attributes.  
 */
GeoConstraint::GeoConstraint(Grid *grid, const DDS &dds) throw (Error)
   : d_grid(grid), d_dds(dds), d_latitude(0), d_longitude(0), d_lat(0),
     d_lon(0), d_bounding_box_set(false)
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
}

void
GeoConstraint::set_bounding_box(double left, double top, 
                                double right, double bottom) throw (Error)
{
#if 0
    // Clear an existing constraint. This means that set_bounding_box() can
    // be called more than once with different values. 

    // This sets the array lengths to -1 which breaks code later.
    d_grid->clear_constraint();
#endif

    // Ensure this method is called only once. What about pthreads?
    if (d_bounding_box_set)
        throw Error("It is not possible to register more than one geo constraint on a grid.");
    d_bounding_box_set = true;
    
    set_bounding_box_longitude(left, right);
    
    set_bounding_box_latitude(top, bottom);
}


