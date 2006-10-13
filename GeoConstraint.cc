
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
    to treat those set<string> objects as collections of prefixes. Sort of...
    this code really tests for the presence of the 'prefix' anywhere in the
    target string. */
class is_prefix
{
private:
    string s;
public:
    is_prefix(const string & in):s(in)
    {}
    bool operator() (const string & prefix)
    {
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
bool
unit_or_name_match(set < string > units, set < string > names,
                       const string & var_units, const string & var_name)
{
    return (units.find(var_units) != units.end()
            || find_if(names.begin(), names.end(), is_prefix(var_name)) != names.end());
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
GeoConstraint::LatitudeSense 
GeoConstraint::categorize_latitude() const
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
GeoConstraint::GeoConstraint(const string &ds_name, const DDS & dds)
        :d_dataset(ds_name), d_dds(dds), d_lat(0), d_lon(0),
        d_bounding_box_set(false), d_latitude_sense(normal),
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
