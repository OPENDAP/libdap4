
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2006 OPeNDAP, Inc.
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
#include <algorithm>  //  for find_if

//#define DODS_DEBUG2

#include "debug.h"
#include "dods-datatypes.h"
#include "GeoConstraint.h"
#include "Float64.h"

#include "Error.h"
#include "InternalErr.h"
#include "ce_functions.h"
#include "util.h"

using namespace std;
using namespace libdap;

/** If an attribute value is quoted, remove the quotes.

    This function only removes a starting and ending quote if both are
    present.

    @param value A value-result parameter. */
void remove_quotes(string & value)
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
    to treat those set<string> objects as collections of prefixes. */
class is_prefix
{
private:
    string s;
public:
    is_prefix(const string & in): s(in)
    {}

    bool operator()(const string & prefix)
    {
        return s.find(prefix) == 0;
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
bool
unit_or_name_match(set < string > units, set < string > names,
                       const string & var_units, const string & var_name)
{
    return (units.find(var_units) != units.end()
            || find_if(names.begin(), names.end(),
                       is_prefix(var_name)) != names.end());
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

bool GeoConstraint::is_bounding_box_valid(double left, double top,
        double right, double bottom) const
{
    if ((left < d_lon[0] && right < d_lon[0])
        || (left > d_lon[d_lon_length-1] && right > d_lon[d_lon_length-1]))
        return false;

    if (d_latitude_sense == normal) {
        // When sense is normal, the largest values are at the origin.
        if ((top > d_lat[0] && bottom > d_lat[0])
            || (top < d_lat[d_lat_length-1] && bottom < d_lat[d_lat_length-1]))
            return false;
    }
    else {
        if ((top < d_lat[0] && bottom < d_lat[0])
            || (top > d_lat[d_lat_length-1] && bottom > d_lat[d_lat_length-1]))
            return false;
    }

    return true;
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

    DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) < t_left: "
         << fmod(d_lon[i], 360.0) << " < " << t_left << endl);

    while (!done && fmod(d_lon[i], 360.0) < t_left) {

        DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) < t_left: "
             << fmod(d_lon[i], 360.0) << " < " << t_left << endl);

        ++i;
        i = i % d_lon_length;
        if (i == smallest_lon_index)
            done = true;
    }
    if (fmod(d_lon[i], 360.0) == t_left)
        longitude_index_left = i;
    else
        longitude_index_left = (i - 1) > 0 ? i - 1 : 0;

    // Assume the vector is cirular --> the largest value is next to the
    // smallest.
    int largest_lon_index =
        (smallest_lon_index - 1 + d_lon_length) % d_lon_length;
    DBG2(cerr << "largest_lon_index: " << largest_lon_index << endl);
    i = largest_lon_index;
    done = false;

    DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) > t_right: "
         << fmod(d_lon[i], 360.0) << " > " << t_right << endl);

    while (!done && fmod(d_lon[i], 360.0) > t_right) {

        DBG2(cerr << "fmod(d_lon[" << i << "], 360.0) > t_right: "
             << fmod(d_lon[i], 360.0) << " > " << t_right << endl);

        // This is like modulus but for 'counting down'
        i = (i == 0) ? d_lon_length - 1 : i - 1;
        if (i == largest_lon_index)
            done = true;
    }
    if (fmod(d_lon[i], 360.0) == t_right)
        longitude_index_right = i;
    else
        longitude_index_right =
            (i + 1) < d_lon_length - 1 ? i + 1 : d_lon_length - 1;
}

/** Scan from the top to the bottom, and the bottom to the top, looking
    for the top and bottom bounding box edges, respectively.

    @param top The top edge of the bounding box
    @param bottom The bottom edge
    @param sense Does the array/grid store data with larger latitudes at 
    the starting indices or are the latitude 'upside down?' 
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
    if (sense == normal) {
        while (i < d_lat_length - 1 && d_lat[i] > top)
            ++i;
        if (d_lat[i] == top)
            latitude_index_top = i;
        else
            latitude_index_top = (i - 1) > 0 ? i - 1 : 0;
    }
    else {
        while (i < d_lat_length - 1 && d_lat[i] < top)
            ++i;
        latitude_index_top = i;
    }


    // and from the bottom up
    i = d_lat_length - 1;
    if (sense == normal) {
        while (i > 0 && d_lat[i] < bottom)
            --i;
        if (d_lat[i] == bottom)
            latitude_index_bottom = i;
        else
            latitude_index_bottom =
                (i + 1) < d_lat_length - 1 ? i + 1 : d_lat_length - 1;
    }
    else {
        while (i > 0 && d_lat[i] > bottom)
            --i;
        latitude_index_bottom = i;
    }
}

/** Take a look at the latitude vector values and record whether the world is
    normal or upside down.
    @return normal or inverted. */
GeoConstraint::LatitudeSense GeoConstraint::categorize_latitude() const
{
    return d_lat[0] >= d_lat[d_lat_length - 1] ? normal : inverted;
}

#if 0
/** Given the top and bottom sides of the bounding box, use the Grid or Array
    latitude information to constrain the data to that bounding box.

    @note Make \e sure to call this before set_bounding_box_longitude().

    @param top The top side of the bounding box, in degress
    @param bottom The bottom side */
void GeoConstraint::set_bounding_box_latitude(double top, double bottom)
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
#endif

// Use 'index' as the pivot point. Move the points behind index to the front of
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

    @param longitude_index_left The left edge of the bounding box. */
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

    @note This method should set the d_array_data and d_array_data_size
    fields. If those \e are set, apply_constraint_to_data() will use those
    values.

    @note First set all the other constraints, including the latitude and
    then make this call. Other constraints, besides latitude, will be simple
    range constraints. Latitude might require that values be inverted, but
    that can be done _after_ the longitude reordering takes place. The latitude
    constraint can be imposed by inverting the top and bottom indices if the
    sense of the grid is inverted, before data are read in this method.
    Then apply the longitude constraint, then invert the result of the merge, if
    needed.

    @todo Fix this code so that it works with latitude as the rightmost map */
void GeoConstraint::reorder_data_longitude_axis(Array &a)
{

    if (!get_longitude_rightmost())
        throw Error("This grid does not have Longitude as its rightmost dimension, the geogrid()\ndoes not support constraints that wrap around the edges of this type of grid.");

    DBG(cerr << "Constraint for the left half: " << get_longitude_index_left()
        << ", " << get_lon_length() - 1 << endl);

    a.add_constraint(d_lon_dim, get_longitude_index_left(), 1,
                     get_lon_length() - 1);
    a.set_read_p(false);
    a.read(get_dataset());
    DBG2(a.print_val(stderr));
#if 0
    char *left_data = 0;
    int left_size = a.buf2val((void **) & left_data);
#endif
    char *left_data = (char*)a.value();
    int left_size = a.length();
    
    // Build a constraint for the 'right' part, which
    // goes from the left edge of the array to the right index and read those
    // data.
    //a.clear_constraint();

    DBG(cerr << "Constraint for the right half: " << 0
        << ", " << get_longitude_index_right() << endl);

    a.add_constraint(d_lon_dim, 0, 1, get_longitude_index_right());
    a.set_read_p(false);
    a.read(get_dataset());
    DBG2(a.print_val(stderr));
#if 0
    char *right_data = 0;
    int right_size = a.buf2val((void **) & right_data);
#endif
    char *right_data = (char*)a.value();
    int right_size = a.length();

    // Make one big lump O'data
    d_array_data_size = left_size + right_size;
    d_array_data = new char[d_array_data_size];

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
        memcpy(d_array_data + (total_elements_per_row * i),
               left_data + (left_elements * i),
               left_elements);
        memcpy(d_array_data + left_elements + (total_elements_per_row * i),
               right_data + (right_elements * i),
               right_elements);
    }

    delete[]left_data;
    delete[]right_data;
}

#if 0
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
void GeoConstraint::set_bounding_box_longitude(double left, double right)
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
#endif

/** @brief Initialize GeoConstraint.

    @param ds_name The name of the dataset. Passed to BaseType::read().
 */
GeoConstraint::GeoConstraint(const string & ds_name)
        : d_dataset(ds_name), d_array_data(0), d_array_data_size(0),
        d_lat(0), d_lon(0),
        d_bounding_box_set(false),
        d_longitude_notation(unknown_notation),
        d_latitude_sense(unknown_sense)
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
    d_lat_names.insert("Lat");
    d_lat_names.insert("LAT");

    d_lon_names.insert("COADSX");
    d_lon_names.insert("lon");
    d_lon_names.insert("Lon");
    d_lon_names.insert("LON");
}

/** Set the bounding box for this constraint. After calling this method the
    object has values for the indexes for the latitude and longitude extent
    as well as the sense of the latitude (south pole at the top or bottom of
    the Array or Grid). These are used by the apply_constraint_to_data()
    method to actually constrain the data.

    @param left The left side of the bounding box.
    @param right The right side
    @param top The top
    @param bottom The bottom */
void GeoConstraint::set_bounding_box(double left, double top,
                                     double right, double bottom)
{
    // Ensure this method is called only once. What about pthreads?
    // The method Array::reset_constraint() might make this so it could be
    // called more than once. jhrg 8/30/06
    if (d_bounding_box_set)
        throw
        InternalErr
        ("It is not possible to register more than one geographical constraint on a variable.");

    // Record the 'sense' of the latitude for use here and later on.
    d_latitude_sense = categorize_latitude();

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

    if (!is_bounding_box_valid(left, top, right, bottom))
        throw Error("The bounding box does not intersect any data within this Grid or Array. The\ngeographical extent of these data are from latitude "
		    + double_to_string(d_lat[0]) + " to " 
		    + double_to_string(d_lat[d_lat_length-1]) 
		    + "\nand longitude " + double_to_string(d_lon[0]) 
		    + " to " + double_to_string(d_lon[d_lon_length-1])
		    + " while the bounding box provided was latitude "
		    + double_to_string(top) + " to " 
		    + double_to_string(bottom) + "\nand longitude " 
		    + double_to_string(left) + " to " 
		    + double_to_string(right));

    // This is simpler than the longitude case because there's no need to
    // test for several notations, no need to accommodate them in the return,
    // no modulo arithmetic for the axis and no need to account for a
    // constraint with two disconnected parts to be joined.
    find_latitude_indeces(top, bottom, d_latitude_sense,
                          d_latitude_index_top, d_latitude_index_bottom);


    // Find the longitude map indeces that correspond to the bounding box.
    find_longitude_indeces(left, right, d_longitude_index_left,
                           d_longitude_index_right);

    DBG(cerr << "Bounding box (tlbr): " << d_latitude_index_top << ", "
        << d_longitude_index_left << ", "
        << d_latitude_index_bottom << ", "
        << d_longitude_index_right << endl);

    d_bounding_box_set = true;
}
