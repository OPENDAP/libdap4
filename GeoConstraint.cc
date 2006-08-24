
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

#include <iostream>
#include <sstream>

#include "debug.h"
#include "dods-datatypes.h"
#include "GeoConstraint.h"

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
    // The fields d_latitude and d_longitude are initialized to null
    while ( m != d_grid->map_end() && (!d_latitude || !d_longitude) ) {
        string units_value = (*m)->get_attr_table().get_attr("units");
        remove_quotes(units_value);
        if (!d_latitude && !units_value.empty() 
            && d_coards_lat_units.find(units_value) != d_coards_lat_units.end())
            d_latitude = dynamic_cast<Array*>(*m);
        if (!d_longitude && !units_value.empty() 
            && d_coards_lon_units.find(units_value) != d_coards_lon_units.end())
            d_longitude = dynamic_cast<Array*>(*m);
        ++m;
    }
    
    return d_latitude && d_longitude;
}

/** A private method that determines if the longitude part of the bounding
    box uses 0/360 or -180/180 notation. This class only supports latitude
    constriants which use 90/-90 notation.
    
    @note There's no real way to tell if the user is thinking in term of the 
    -180/180 notation and the ask for 30 degrees to 50 degress (or 50 to 30,
    for that matter). This function assumes that if one of the two values is
    negative, then the notation is or the -180/180 form, otherwise not.
    
    @param left The left side of the bounding box, in degrees
    @param right The right side of the boubding box
    @return The notation (pos or neg_pos) */
GeoConstraint::Notation
GeoConstraint::categorize_notation(double left, double right) const
{
    return (left < 0 || right < 0) ? neg_pos: pos;
}    

/* A private method to translate the longitude constraint from -180/180
   notation to 0/360 notation.
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

/** Given that the Grid has alongitude map that uses the 'neg_pos' notation,
    transform it to the 'pos' notation. This method modifies the d_longitude
    Array. */
void
GeoConstraint::transform_map_to_pos_notation()
{
    // Assume earlier logic is correct (since the test is expensive)
    // for each value, add 180
}

/** @brief Initialize GeoConstraint with a Grid.
    @param grid Set the GeoConstraint to use this Grid variable. It is the 
    caller's responsibility to ensure that the value \e grid is a valid Grid 
    variable.
    @param dds Use this DDS to get global attributes.  
 */
GeoConstraint::GeoConstraint(Grid *grid, const DDS &dds) throw (Error)
   : d_grid(grid), d_dds(dds), d_latitude(0), d_longitude(0)
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
    // Categorize the notation used by the bounding box (0/360 or -180/180)
    Notation constraint_notation = categorize_notation(left, right);
    
    // If the notation uses -180/180, transform the request to 0/360 notation
    if (constraint_notation == neg_pos)
        transform_constraint_to_pos_notation(left, right);
    
    // If the grid uses -180/180, transform it to 0/360 as well
    BaseType *left_lon = d_longitude->var(0);
    BaseType *right_lon = d_longitude->var(d_longitude->length());
    
    Notation map_notation = categorize_notation(extract_double_value(left_lon),
                                                extract_double_value(right_lon));
    if (map_notation == neg_pos)
        transform_map_to_pos_notation();
        
    // Analyse the constraint: Does it cross the longitude boundry of the 
    // Grid's Array? If so, translate the 
    //
    //and 'slide' so that it starts on the zero-th
    // index of the longitude map. Then perform the same translation on the 
    // grid's array data. This addresses the case where a costraint falls 
    // across the longitide boundry.
    
    // Apply constraint
    
    // If the constraint used the -180/180 notation, transform the maps to
    // use that notation
    
    // Mark the array and lat/lon maps as having been read to prevent
    // re-reading them (because that would invalidate the various translations
    // performed here.
}


