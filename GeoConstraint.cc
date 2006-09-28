
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

static char id[] not_used =
    { "$Id$" };

#include <math.h>
#include <string.h>

#include <iostream>
#include <sstream>

//#define DODS_DEBUG

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
static void remove_quotes(string & value)
{
    if (!value.empty() && value[0] == '"'
        && value[value.length() - 1] == '"') {
        value.erase(0, 1);
        value.erase(value.length() - 1);
    }

    return;
}

/** This is used with find_if(). The GeoConstraint class holds a set of strings
    which are prefixes for variable names. Using the regular find() locates
    only the exact matches, using find_if() with this functor makes is easy
    to treat those set<string> objects as collections of prefixes. Sort of...
    this code really tests for the presence of the 'prefix' anywhere in the 
    target string. */
class is_prefix {
  private:
    string s;
  public:
    is_prefix(const string & in):s(in) {
    } bool operator() (const string & prefix) {
        return s.find(prefix) != string::npos;
    }
};

/** Look in the containers which hold the units attributes and variable name
    prefixes which are considered as identifying a vector as being a latitude
    or longitude vector. 
    
    @param units A container with a bunch of units attribute values.
    @param names A container with a bunch of variable name prefixes.
    @param var_units The value of the 'units' attribute for this variable.
    @param var_name The name of the variable.
    @return True if the units_value matches any of the accepted attributes
    exactly or if the name_value starts with any of the accepted prefixes */
static bool
unit_or_name_match(set < string > units, set < string > names,
                   const string & var_units, const string & var_name)
{
    return (units.find(var_units) != units.end()
            || find_if(names.begin(), names.end(), is_prefix(var_name)) != names.end());
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
bool GeoConstraint::find_lat_lon_maps()
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
            && unit_or_name_match(d_coards_lat_units, d_lat_names,
                                  units_value, map_name)) {

            // Set both d_latitude (a pointer to the real map vector) and
            // d_lat, a vector of the values represented as doubles. It's easier
            // to work with d_lat, but it's d_latitude that needs to be set
            // when constraining the grid. Also, record the grid variable's
            // dimension iterator so that it's easier to set the Grid's Array
            // (which also has to be constrained).
            d_latitude = dynamic_cast < Array * >(*m);
            if (!d_latitude->read_p())
                d_latitude->read(d_dataset);

            d_lat = extract_double_array(d_latitude);   // throws Error
            d_lat_length = d_latitude->length();

            d_lat_grid_dim = d;
        }

        if (!d_longitude        // && !units_value.empty() 
            && unit_or_name_match(d_coards_lon_units, d_lon_names,
                                  units_value, map_name)) {

            d_longitude = dynamic_cast < Array * >(*m);
            if (!d_longitude->read_p())
                d_longitude->read(d_dataset);

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
    constriants which use 90/-90 notation, so there's no need to figure out
    what sort of notation they use.
    
    @note This function assumes that if one of the two values is
    negative, then the notation is or the -180/179 form, otherwise not. 
    If the user asks for 30 degrees to 50 degress (or 50 to 30,
    for that matter), there's no real way to tell which notation they are 
    using. 
    
    @param left The left side of the bounding box, in degrees
    @param right The right side of the boubding box
    @return The notation (pos or neg_pos) */
GeoConstraint::Notation
    GeoConstraint::categorize_notation(double left,
                                       double right) const
{
    return (left < 0 || right < 0) ? neg_pos : pos;
}

/* A private method to translate the longitude constraint from -180/179
   notation to 0/359 notation.
   @param left Value-result parameter; the left side of the bounding box
   @parm right Value-result parameter; the right side of the bounding box */
void
GeoConstraint::transform_constraint_to_pos_notation(double &left,
                                                    double &right) const
{
    if (left < 0 || right < 0) {        // double check
        left += 180;
        right += 180;
    }
}

/** Given that the Grid has a longitude map that uses the 'neg_pos' notation,
    transform it to the 'pos' notation. This method modifies the d_longitude
    Array. */
void GeoConstraint::transform_longitude_to_pos_notation()
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
void GeoConstraint::transform_longitude_to_neg_pos_notation()
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
void GeoConstraint::find_longitude_indeces(double left, double right,
                                           int &longitude_index_left,
                                           int &longitude_index_right) const
{
    // Use all values 'modulo 360' to take into account the cases where the 
    // constraint and/or the longitude vector are given using values greater
    // than 360 (i.e., when 380 is used to mean 20).
    double t_left = fmod(left, 360.0);
    double t_right = fmod(right, 360.0);

    // Find the place where 'longitude starts.' That is, what value of the
    // index 'i' corresponds to the smallest value of d_lon. Why we do this:
    // Some data sources use offset longitude axes so that the 'seam' is
    // shifted to a place other than the date line.
    int i = 0;
    int smallest_lon_index = 0;
    double smallest_lon = fmod(d_lon[smallest_lon_index], 360.0);
    while (i < d_lon_length - 1) {
        if (smallest_lon > fmod(d_lon[i], 360.0)) {
            smallest_lon_index = i;
            smallest_lon = fmod(d_lon[smallest_lon_index], 360.0);
        }
        ++i;
    }
    DBG2(cerr << "smallest_lon_index: " << smallest_lon_index << endl);

    // Scan from the index of the smallest value looking for the place where
    // the value is greater than or equal to the left most point of the bounding
    // box.
    i = smallest_lon_index;
    bool done = false;
    DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) < t_left: " << fmod(d_lon[i], 360.0)
             << " < " << t_left << endl);
    while (!done && fmod(d_lon[i], 360.0) < t_left) {
        DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) < t_left: " << fmod(d_lon[i], 360.0)
                 << " < " << t_left << endl);
        i = ++i % d_lon_length;
        if (i == smallest_lon_index)
            done = true;
    }
    longitude_index_left = i;

    // Assume the vector is cirular --> the largest value is next to the 
    // smallest.
    int largest_lon_index = (smallest_lon_index-1 + d_lon_length) % d_lon_length;
    DBG2(cerr << "largest_lon_index: " << largest_lon_index << endl);
    i = largest_lon_index;
    done = false;
    DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) > t_right: " << fmod(d_lon[i], 360.0)
             << " > " << t_right << endl);
    while (!done && fmod(d_lon[i], 360.0) > t_right) {
        DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) > t_right: " << fmod(d_lon[i], 360.0)
                 << " > " << t_right << endl);
        // This is like modulus but for 'counting down'
        i = (i == 0) ? d_lon_length - 1 : i - 1;
        if (i == largest_lon_index)
            done = true;
    }
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
void GeoConstraint::find_latitude_indeces(double top, double bottom,
                                          LatitudeSense sense,
                                          int &latitude_index_top,
                                          int &latitude_index_bottom) const
{
    // Scan from the top down
    int i = 0;
    if (sense == normal)
        while (i < d_lat_length-1 && d_lat[i] > top)
            ++i;
    else
        while (i < d_lat_length-1 && d_lat[i] < top)
            ++i;

    latitude_index_top = i;

    // and from the bottom up
    i = d_lat_length - 1;
    if (sense == normal)
        while (i > 0 && d_lat[i] < bottom)
            --i;
    else
        while (i > 0 && d_lat[i] > bottom)
            --i;

    latitude_index_bottom = i;
}

/** Take a look at the latitude vector values and record whether the world is 
    normal or upside down.
    @return normal or inverted. */
GeoConstraint::LatitudeSense GeoConstraint::categorize_latitude()
{
    return d_lat[0] >= d_lat[d_lat_length - 1] ? normal : inverted;
}

/** Given the top and bottom sides of the bounding box, use the Grid or Array
    latitude information to constrain the data to that bounding box. 
    
    @note Make \e sure to call this before set_bounding_box_longitude().
    
    @param top The top side of the bounding box, in degress
    @param bottom The bottom side */
void GeoConstraint::set_bounding_box_latitude(double top,
                                              double bottom)
{
    // Record the 'sense' of the latitude for use here and later on.
    d_latitude_sense = categorize_latitude();

    // This is simpler than the longitude case because there's no need to test
    // for several notations, no need to accommodate them in the return, no
    // modulo arithmetic for the axis and no need to account for a constraint with
    // two disconnected parts to be joined.
    find_latitude_indeces(top, bottom, d_latitude_sense,
                          d_latitude_index_top, d_latitude_index_bottom);
}

// Use 'index' as the pivot point. Move the behind index to the front of
// the vector and those points in front of and at index to the rear.
static void
swap_vector_ends(char *dest, char *src, int len, int index, int elem_sz)
{
    memcpy(dest, src + index * elem_sz, (len - index) * elem_sz);

    memcpy(dest + (len - index) * elem_sz, src, index * elem_sz);
}

/** Reorder the elements in the longitude map so that the longitude constraint no 
    longer crosses the edge of the map's storage. The d_lon field is
    modified.
    
    @note The d_lon vector always has double values regardless of the type
    of d_longitude.
    
    @param d_longitude_index_left The left edge of the bounding box. */
void GeoConstraint::reorder_longitude_map(int longitude_index_left)
{
    double *tmp_lon = new double[d_lon_length];

    swap_vector_ends((char *) tmp_lon, (char *) d_lon, d_lon_length,
                     longitude_index_left, sizeof(double));

    memcpy(d_lon, tmp_lon, d_lon_length * sizeof(double));

    delete[]tmp_lon;
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
    
    @todo First read the entire array into temporary storage. Then use a
    temporary vector to reorder each row of that copy of the array. Finally,
    copy the data back into the array and delete the temporary array and 
    vector. Once this works, optimize. */
void GeoConstraint::reorder_data_longitude_axis()
{
    // Read the two parts of the Grid's Array using the read() method.
    // Then combine them into one lump of data stored in the GeoConstraint
    // object.

    // Make a constraint for the 'left' part, which goes from the left index
    // to the right side of the array. Use read() to get the data.
    Array & a = *d_grid->get_array();

    //a.clear_constraint();

    DBG(cerr << "Constraint for the left half: " << d_longitude_index_left
             << ", " << d_lon_length-1 << endl);
             
    a.add_constraint(d_lon_grid_dim, d_longitude_index_left, 1,
                     d_lon_length-1);
    a.set_read_p(false);
    a.read(d_dataset);
    DBG2(a.print_val(stderr));
    char *left_data = 0;
    int left_size = a.buf2val((void **) &left_data);

    // Build a constraint for the 'right' part, which 
    // goes from the left edge of the array to the right index and read those
    // data.
    //a.clear_constraint();

    DBG(cerr << "Constraint for the right half: " << 0
             << ", " << d_longitude_index_right << endl);
             
    a.add_constraint(d_lon_grid_dim, 0, 1, d_longitude_index_right);
    a.set_read_p(false);
    a.read(d_dataset);
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
    int left_elements = (d_lon_length - d_longitude_index_left) * elem_width;
    int right_elements = (d_longitude_index_right + 1) * elem_width;
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

/** Given the left and right sides of the bounding box, use the Grid or Array
    longitude information to constrain the data to that bounding box. This
    method takes into account the two different notations commonly used to 
    specify longitude (0/359 and -180/179) and that the longitude axis is
    cyclic so that 360 == 0 (== 720, ...).
    
    @note This reads data because it is easier to reorder the arry here... Is this true???
    
    @todo Make this method correctly reorder the Grid/Array when the longitude
    constraint crosses the edge of the data's storage.
    
    @param left The left side of the bounding box, in degress
    @param right The right side */
void GeoConstraint::set_bounding_box_longitude(double left,
                                               double right)
{
    // Categorize the notation used by the bounding box (0/359 or -180/179).
    d_longitude_notation = categorize_notation(left, right);

    // If the notation uses -180/179, transform the request to 0/359 notation.
    if (d_longitude_notation == neg_pos)
        transform_constraint_to_pos_notation(left, right);

    // If the grid uses -180/179, transform it to 0/359 as well. This will make
    // subsequent logic easier and adds only a few extra operations, even with
    // large maps.
    Notation longitude_notation =
        categorize_notation(d_lon[0], d_lon[d_lon_length - 1]);

    if (longitude_notation == neg_pos)
        transform_longitude_to_pos_notation();

    // Find the longitude map indeces that correspond to the bounding box.
    find_longitude_indeces(left, right, d_longitude_index_left,
                           d_longitude_index_right);
}

/** @brief Initialize GeoConstraint with a Grid.
    @param grid Set the GeoConstraint to use this Grid variable. It is the 
    caller's responsibility to ensure that the value \e grid is a valid Grid 
    variable.
    @param ds_name The name of the dataset. Passed to BaseType::read().
    @param dds Use this DDS to get global attributes.  
 */
GeoConstraint::GeoConstraint(Grid * grid, const string & ds_name,
                             const DDS & dds)
:d_grid(grid), d_dataset(ds_name), d_dds(dds), d_grid_array_data(0),
d_grid_array_data_size(0), d_latitude(0), d_longitude(0), d_lat(0),
d_lon(0), d_bounding_box_set(false), d_latitude_sense(normal),
d_longitude_notation(pos)
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

    d_lat_names.insert("COADSY");
    d_lat_names.insert("lat");

    d_lon_names.insert("COADSX");
    d_lon_names.insert("lon");

    // Is this Grid a geo-referenced grid? Throw Error if not.
    if (!find_lat_lon_maps())
        throw Error(string("The grid '") + d_grid->name()
                    +
                    "' does not have identifiable latitude/longitude map vectors.");
}

/** Set the bounding box for this constraint. After calling this method the
    Grid or Array variable passed to this object will be constrained by
    the given longitude/latitude box. This may read the grid's array data
    using Array::read() if the longitude constraint crosses the right edge
    of the Grid's array boundry (i.e. the constraint is split across the 
    edge of the array) because this will rearrange those parts to make a single 
    array and reorder the longitude map to match that change. It's easier to 
    read the data inside set_bounding_box_longitude() because that's where
    it's easiest to reorder the longitude map and Grid Array data.
    
    @param left The left side of the bounding box.
    @param right The right side
    @param top The top
    @param bottom The bottom */
void GeoConstraint::set_bounding_box(double left, double top,
                                     double right,
                                     double bottom)
{
    // Ensure this method is called only once. What about pthreads?
    // The method Array::reset_constraint() might make this so it could be
    // called more than once. jhrg 8/30/06
    if (d_bounding_box_set)
        throw
            InternalErr
            ("It is not possible to register more than one geo constraint on a grid.");

    set_bounding_box_latitude(top, bottom);
    set_bounding_box_longitude(left, right);
    
    DBG(cerr << "Bounding box (tlbr): " << d_latitude_index_top << ", " 
             << d_longitude_index_left << ", " 
             << d_latitude_index_bottom << ", "
             << d_longitude_index_right << endl);

    d_bounding_box_set = true;
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

void GeoConstraint::apply_constraint_to_data()
{
    if (!d_bounding_box_set)
        throw
            InternalErr
            ("The Latitude and Longitude constraints must be set before calling apply_constraint_to_data().");

    Array::Dim_iter fd = d_latitude->dim_begin();
    if (d_latitude_sense == inverted) {
        int tmp = d_latitude_index_top;
        d_latitude_index_top = d_latitude_index_bottom;
        d_latitude_index_bottom = tmp;
    }
    
    // It's esy to flip the Latitude values; if the bottom index value
    // is before/above the top index, return an error explaining that.
    if ( d_latitude_index_top > d_latitude_index_bottom )
        throw Error("The upper and lower latitude indices appear to be reversed. Please provide the latitude bounding box numbers giving the northern-most latitude first.");

    // Constrain the lat vector and lat dim of the array
    d_latitude->add_constraint(fd, d_latitude_index_top, 1,
                               d_latitude_index_bottom);
    d_grid->get_array()->add_constraint(d_lat_grid_dim,
                                        d_latitude_index_top, 1,
                                        d_latitude_index_bottom);

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

        // Now the data are all in local storage

        // alter the indices; the left index has now been moved to 0, and the right
        // index is now at lon_vector_length-left+right.
        d_longitude_index_right =
            d_lon_length - d_longitude_index_left + d_longitude_index_right;
        d_longitude_index_left = 0;
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
    if (d_longitude_notation == neg_pos) {
        transform_longitude_to_neg_pos_notation();
    }
    // Apply constraint; stride is always one and maps only have one dimension
    fd = d_longitude->dim_begin();
    d_longitude->add_constraint(fd, d_longitude_index_left, 1,
                                d_longitude_index_right);

    d_grid->get_array()->add_constraint(d_lon_grid_dim,
                                        d_longitude_index_left,
                                        1, d_longitude_index_right);

    // Transfer values from the local lat vector to the Grid's
    set_array_using_double(d_latitude, &d_lat[d_latitude_index_top],
                           d_latitude_index_bottom - d_latitude_index_top +
                           1);

    set_array_using_double(d_longitude, &d_lon[d_longitude_index_left],
                           d_longitude_index_right -
                           d_longitude_index_left + 1);

    // ... and then the Grid's array if it has been read.
    if (d_grid_array_data) {
        int size = d_grid->get_array()->val2buf(d_grid_array_data);
        if (size != d_grid_array_data_size)
            throw
                InternalErr
                ("Expected data size not copied to the Grid's buffer.");
        d_grid->set_read_p(true);
    } else {
        d_grid->get_array()->read(d_dataset);
    }
}
