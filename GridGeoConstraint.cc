
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

// The Grid Selection Expression Clause class.


#include "config.h"

static char id[] not_used =
    { "$Id$"
    };

#include <math.h>
#include <string.h>

#include <iostream>
#include <sstream>

//#define DODS_DEBUG

#include "debug.h"
#include "dods-datatypes.h"
#include "GridGeoConstraint.h"
#include "Float64.h"

#include "Error.h"
#include "InternalErr.h"
#include "ce_functions.h"

using namespace std;
using namespace libdap;


/** @brief Initialize GeoConstraint with a Grid.
    @param grid Set the GeoConstraint to use this Grid variable. It is the
    caller's responsibility to ensure that the value \e grid is a valid Grid
    variable.
    @param ds_name The name of the dataset. Passed to BaseType::read().
    @param dds Use this DDS to get global attributes.
 */
GridGeoConstraint::GridGeoConstraint(Grid *grid, const string &ds_name/*, const DDS &dds*/) 
        : GeoConstraint(ds_name/*, dds*/), d_grid(grid), d_grid_array_data(0),
        d_grid_array_data_size(0), d_latitude(0), d_longitude(0)
{

    // Is this Grid a geo-referenced grid? Throw Error if not.
    if (!find_lat_lon_maps())
        throw Error(string("The grid '") + d_grid->name()
                    +
                    "' does not have identifiable latitude/longitude map vectors.");
                    
    if (!lat_lon_dimensions_ok())
        throw Error("The geogrid() function will only work when the Grid's Longitude and Latitude maps are the rightmost dimensions.");
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
bool GridGeoConstraint::find_lat_lon_maps()
{
    Grid::Map_iter m = d_grid->map_begin();
    // Assume that a Grid is correct and thus has exactly as many maps at its
    // array part has dimensions. Thus don't bother to test the Grid's array
    // dimension iterator for '!= dim_end()'.
    Array::Dim_iter d = d_grid->get_array()->dim_begin();
    // The fields d_latitude and d_longitude are initialized to null
    while (m != d_grid->map_end() && (!d_latitude || !d_longitude)) {
        string units_value = (*m)->get_attr_table().get_attr("units");
        remove_quotes(units_value);
        string map_name = (*m)->name();

        // The 'units' attribute must match exactly; the name only needs to
        // match a prefix.
        if (!d_latitude
            && unit_or_name_match(get_coards_lat_units(), get_lat_names(),
                                  units_value, map_name)) {

            // Set both d_latitude (a pointer to the real map vector) and
            // d_lat, a vector of the values represented as doubles. It's easier
            // to work with d_lat, but it's d_latitude that needs to be set
            // when constraining the grid. Also, record the grid variable's
            // dimension iterator so that it's easier to set the Grid's Array
            // (which also has to be constrained).
            d_latitude = dynamic_cast < Array * >(*m);
            if (!d_latitude->read_p())
                d_latitude->read(get_dataset());

            set_lat(extract_double_array(d_latitude));   // throws Error
            set_lat_length(d_latitude->length());

            d_lat_grid_dim = d;
        }

        if (!d_longitude        // && !units_value.empty()
            && unit_or_name_match(get_coards_lon_units(), get_lon_names(),
                                  units_value, map_name)) {

            d_longitude = dynamic_cast < Array * >(*m);
            if (!d_longitude->read_p())
                d_longitude->read(get_dataset());

            set_lon(extract_double_array(d_longitude));
            set_lon_length(d_longitude->length());

            d_lon_grid_dim = d;
        }

        ++m;
        ++d;
    }

    return get_lat() && get_lon();
}

/** Are the latitude and longitude dimentions ordered so that this class can
    properly constrain the data? This method throws Error if lat and lon are
    not to two 'fastest-varying' (or 'rightmost) dimensions. It sets the 
    internal property \e longitude_rightmost if that's true. 
    
    @note Called by the constructor once find_lat_lon_maps() has returned. 
    
    @return True if the lat/lon maps are the two rightmost maps,
    false otherwise*/
bool
GridGeoConstraint::lat_lon_dimensions_ok()
{
    // get the last two map iterators
    Grid::Map_riter rightmost = d_grid->map_rbegin();
    Grid::Map_riter next_rightmost = rightmost + 1;
    
    if (*rightmost == d_longitude && *next_rightmost == d_latitude)
        set_longitude_rightmost(true);
    else if (*rightmost == d_latitude && *next_rightmost == d_longitude)
        set_longitude_rightmost(false);
    else
        return false;
        
    return true;
}

static int
count_dimensions_except_longitude(Array &a)
{
    int size = 1;
    for (Array::Dim_iter i = a.dim_begin(); (i + 1) != a.dim_end(); ++i)
        size *= a.dimension_size(i, true);

    return size;
}

/** Reorder the data values relative to the longitude axis so that the
    reordered longitude map (see GeoConstraint::reorder_longitude_map())
    and the data values match.

    @note First set all the other constraints, including the latitude and
    then make this call. Other constraints, besides latitude, will be simple
    range constraints. Latitude might require that values be inverted, but
    that can be done _after_ the longitude reordering takes place. The latitude
    constraint can be imposed by inverting the top and bottom indices if the
    sense of the grid is inverted, before data are read in this method.
    Then apply the longitude constraint, then invert the result of the merge, if
    needed.

    @todo Fix this code so that it works with latitude as the rightmost map */
void GridGeoConstraint::reorder_data_longitude_axis()
{
        
    if (!get_longitude_rightmost())
        throw Error(
"This grid does not have Longitude as its rightmost dimension, the geogrid()\n\
does not support constraints that wrap around the edges of this type of grid.");

    // Read the two parts of the Grid's Array using the read() method.
    // Then combine them into one lump of data stored in the GeoConstraint
    // object.

    // Make a constraint for the 'left' part, which goes from the left index
    // to the right side of the array. Use read() to get the data.
    Array & a = *d_grid->get_array();

    //a.clear_constraint();

    DBG(cerr << "Constraint for the left half: " << get_longitude_index_left()
        << ", " << get_lon_length()-1 << endl);

    a.add_constraint(d_lon_grid_dim, get_longitude_index_left(), 1,
                     get_lon_length()-1);
    a.set_read_p(false);
    a.read(get_dataset());
    DBG2(a.print_val(stderr));
    char *left_data = 0;
    int left_size = a.buf2val((void **) &left_data);

    // Build a constraint for the 'right' part, which
    // goes from the left edge of the array to the right index and read those
    // data.
    //a.clear_constraint();

    DBG(cerr << "Constraint for the right half: " << 0
        << ", " << get_longitude_index_right() << endl);

    a.add_constraint(d_lon_grid_dim, 0, 1, get_longitude_index_right());
    a.set_read_p(false);
    a.read(get_dataset());
    DBG2(a.print_val(stderr));
    char *right_data = 0;
    int right_size = a.buf2val((void **) &right_data);

    // Make one big lump O'data
    d_grid_array_data_size = left_size + right_size;
    d_grid_array_data = new char[d_grid_array_data_size];

    // Assume COARDS convensions are being followed: lon varies fastest.
    // These *_elements variables are actually elements * bytes/element since
    // memcpy() uses bytes.
    int elem_width = a.var()->width();
    int left_elements = (get_lon_length() - get_longitude_index_left()) * elem_width;
    int right_elements = (get_longitude_index_right() + 1) * elem_width;
    int total_elements_per_row = left_elements + right_elements;

    // Interleve the left and right_data vectors. jhrg 8/31/06
    int rows_to_copy = count_dimensions_except_longitude(a);
    for (int i = 0; i < rows_to_copy; ++i) {
        memcpy(d_grid_array_data + (total_elements_per_row * i),
               left_data + (left_elements * i),
               left_elements);
        memcpy(d_grid_array_data + left_elements + (total_elements_per_row * i),
               right_data + (right_elements * i),
               right_elements);
    }

    delete[]left_data;
    delete[]right_data;
}


/** Once the bounding box is set use this method to apply the constraint. This
    modifies the data values in the Grid so that the software in
    Vector::serialize() will work correctly. Vector::serialize() assumes that
    the BaseType::read() method is called \e after the projection is applied to
    the data. That is, the projection is applied, then data are read. but
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
    was run by ConstraintExpression::eval() then we might be able to better
    optimize how data are read, but in this case we have read all the data
    and may have alredy reorganized it. Set up the internal buffers so they
    hold the correct values and mark the Grid's array and lat/lon maps as
    read. */
void GridGeoConstraint::apply_constraint_to_data()
{
    if (!get_bounding_box_set())
        throw
        InternalErr
        ("The Latitude and Longitude constraints must be set before calling apply_constraint_to_data().");

    Array::Dim_iter fd = d_latitude->dim_begin();
    if (get_latitude_sense() == inverted) {
        int tmp = get_latitude_index_top();
        set_latitude_index_top(get_latitude_index_bottom());
        set_latitude_index_bottom(tmp);
    }

    // It's esy to flip the Latitude values; if the bottom index value
    // is before/above the top index, return an error explaining that.
    if ( get_latitude_index_top() > get_latitude_index_bottom() )
        throw Error("The upper and lower latitude indices appear to be reversed. Please provide the latitude bounding box numbers giving the northern-most latitude first.");

    // Constrain the lat vector and lat dim of the array
    d_latitude->add_constraint(fd, get_latitude_index_top(), 1,
                               get_latitude_index_bottom());
    d_grid->get_array()->add_constraint(d_lat_grid_dim,
                                        get_latitude_index_top(), 1,
                                        get_latitude_index_bottom());

    // Does the longitude constraint cross the edge of the longitude vector?
    // If so, reorder the grid's data (array), longitude map vector and the
    // local vector of longitude data used for computation.
    if (get_longitude_index_left() > get_longitude_index_right()) {
        reorder_longitude_map(get_longitude_index_left());

        // If the longitude constraint is 'split', join the two parts, reload
        // the data into the Grid's Array and make sure the Array is marked as
        // already read. This should be true for the whole Grid, but if some
        // future modification changes that, the array will be covered here.
        // Note that the following method only reads the data out and stores
        // it in this object after joining the two parts. The method
        // apply_constraint_to_data() transfers the data back from the this
        // objet to the DAP Grid variable.
        reorder_data_longitude_axis();

        // Now the data are all in local storage

        // alter the indices; the left index has now been moved to 0, and the right
        // index is now at lon_vector_length-left+right.
        set_longitude_index_right(get_lon_length() - get_longitude_index_left()
                                  + get_longitude_index_right());
        set_longitude_index_left(0);
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
    if (get_longitude_notation() == neg_pos) {
        transform_longitude_to_neg_pos_notation();
    }
    // Apply constraint; stride is always one and maps only have one dimension
    fd = d_longitude->dim_begin();
    d_longitude->add_constraint(fd, get_longitude_index_left(), 1,
                                get_longitude_index_right());

    d_grid->get_array()->add_constraint(d_lon_grid_dim,
                                        get_longitude_index_left(),
                                        1, get_longitude_index_right());

    // Transfer values from the local lat vector to the Grid's
    set_array_using_double(d_latitude, get_lat() + get_latitude_index_top(),
                           get_latitude_index_bottom() - get_latitude_index_top() + 1);

    set_array_using_double(d_longitude, get_lon() + get_longitude_index_left(),
                           get_longitude_index_right() - get_longitude_index_left() + 1);

    // ... and then the Grid's array if it has been read.
    if (d_grid_array_data) {
        int size = d_grid->get_array()->val2buf(d_grid_array_data);
        if (size != d_grid_array_data_size)
            throw
            InternalErr
            ("Expected data size not copied to the Grid's buffer.");
        d_grid->set_read_p(true);
    }
    else {
        d_grid->get_array()->read(get_dataset());
    }
}
