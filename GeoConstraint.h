
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

#ifndef _geo_constraint_h
#define _geo_constraint_h 1

#include <string>
#include <sstream>
#include <set>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _array_h
#include "Array.h"
#endif

#ifndef _grid_h
#include "Grid.h"
#endif

/** Encapsilate the logic needed to handle geographical constraints when they are
    applied to DAP Grid (and some Array) variables. 
    
    This class will apply a longitude/latitude bounding box to a Grid that is
    a 'georeferenced' Grid. That is, it follows the COARDS/CF conventions. This 
    may be relaxed...
    
    If the longitude range of the constraint corsses the boundry of the data arry
    so that the constraint creates two separate rectangles, this class will arrange 
    to return the result as a single Grid. It will do this by rearranging the data
    before control is passed onto the constraint evaluator and serialization
    logic. Here's a diagram of how it works:
    
    Suppose a constraint for the longitude BB starts at the left edge of L and goes
    to the right edge of R:
    <pre>
       0.0       180.0       360.0 (longitude, in degrees)
        +----------------------+
        |xxxxxyyyyyyyyyyyyzzzzz|
        -----+            +-----
        |    |            |    |
        | R  |            | L  |
        |    |            |    |
        -----+            +-----
        |                      |
        +----------------------+
    </pre>
    For example, suppose the client provides a bounding box that starts
    at 200 degrees and ends at 80. This class will first copy the Left part
    to new storage and then copy the right part, thus 'stitching together' the 
    two halves of the constraint. The result looks like:
    <pre>
     80.0  360.0/0.0  180.0  ~200.0 (longitude, in degrees)
        +----------------------+
        |zzzzzxxxxxxyyyyyyyyyyy|
        -----++-----           |
        |    ||    |           |
        | L  || R  |           |
        |    ||    |           |
        -----++-----           |
        |                      |
        +----------------------+
    </pre>
    The changes are made in the Grid variable itself, so once this is done the 
    Grid should not be re-read by the CE or serialization code.
    @author James Gallagher */

class GeoConstraint {
public:
    /** The longitude extents of the constraint bounding box can be expressed
        two ways: using a 0/359 notation and using a -180/179 notation. I call
        the 0/359 notation 'pos' and the -180/179 noation 'neg_pos'. */
    enum Notation {
        pos,
        neg_pos
    };
    
    /** Most of the time, latitude starts at the top of an arry with positive values
        and ends up at the bottom with negative ones. But sometimes... the world
        is upside down. */
    enum LatitudeSense {
        normal,
        inverted
    };
    
private:
#if 0
    const DDS &d_dds;
#endif
    const string &d_dataset;

    double *d_lat;              //< Holds the latitude values
    double *d_lon;              //< Holds the longitude values
    int d_lat_length;           //< How long is the latitude vector
    int d_lon_length;           //< ... longitude vector

    // These four are indeces of the constraint
    int d_latitude_index_top;
    int d_latitude_index_bottom;
    int d_longitude_index_left;
    int d_longitude_index_right;
    
    bool d_bounding_box_set;    //< Has the bounding box been set?
    bool d_longitude_rightmost; //< Is longitude the rightmost dimension? 
    
    Notation d_longitude_notation;
    
    LatitudeSense d_latitude_sense;
   
    // Sets of string values used to find stuff in attributes 
    set<string> d_coards_lat_units;
    set<string> d_coards_lon_units;

    set<string> d_lat_names;
    set<string> d_lon_names;

    // Hide these three automatically provided methods
    GeoConstraint();
    GeoConstraint(const GeoConstraint &param);
    GeoConstraint &operator=(GeoConstraint &rhs);

protected:
/** A protected method that searches for latitude
    and longitude map vectors. This method returns false if either map
    cannot be found.

    The d_lon, d_lon_length (and matching lat) fields are modified.

    @note Rules used to find Maps:<ul>
    <li>Latitude: If the Map has a units attribute of "degrees_north",
    "degree_north", "degree_N", or "degrees_N"</li>
    <li>Longitude: If the map has a units attribute of "degrees_east"
    (eastward positive), "degree_east", "degree_E", or "degrees_E"</li>
    </ul>

    @return True if the maps are found, otherwise False */
    virtual bool find_lat_lon_maps() = 0;
    
    /** Are the latitude and longitude dimentions ordered so that this class can
    properly constrain the data? This method throws Error if lat and lon are
    not to two 'fastest-varying' (or 'rightmost) dimensions. It sets the 
    internal property \e longitude_rightmost if that's true. 
    
    @note Called by the constructor once find_lat_lon_maps() has returned. 
    
    @return True if the lat/lon maps are the two rightmost maps,
    false otherwise*/
    virtual bool lat_lon_dimensions_ok() = 0;
    
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
    virtual void reorder_data_longitude_axis() = 0;
    
    Notation categorize_notation(double left, double right) const;
    void transform_constraint_to_pos_notation(double &left, double &right) const;
    void transform_longitude_to_pos_notation();
    void transform_longitude_to_neg_pos_notation();
    void find_longitude_indeces(double left, double right, 
                                int &longitude_index_left, 
                                int &longitude_index_right) const;
    void set_bounding_box_longitude(double left, double right) ;
    void reorder_longitude_map(int longitude_index_left);

    LatitudeSense categorize_latitude() const;
    void find_latitude_indeces(double top, double bottom, LatitudeSense sense, 
                                int &latitude_index_top, 
                                int &latitude_index_bottom) const;                                
    void set_bounding_box_latitude(double top, double bottom) ;
                                                                                             
    friend class GridGeoConstraintTest; // Unit tests
    
public:
    /** @name Constructors */
    //@{
    GeoConstraint(const string &ds_name/*, const DDS &dds*/) ;
#if 0
    GeoConstraint(Array *array, const string &ds_name, const DDS &dds) ;
    GeoConstraint(Array *array, const GeoExtent extent, 
                  const string &ds_name, const DDS &dds) ;
    GeoConstraint(Array *array, const GeoExtent extent, 
                  const Projection projection, 
                  const string &ds_name, const DDS &dds) ;
#endif
    //@}
    
    virtual ~GeoConstraint() {
        delete [] d_lat;
        delete [] d_lon;
    }

    /** @name Accessors/Mutators */
    //@{
#if 0
    const DDS &get_dds() const { return d_dds; }
#endif
    string get_dataset() const { return d_dataset; }
    
    double *get_lat() const {return d_lat;}
    double *get_lon() const {return d_lon;}
    void set_lat(double *lat) {d_lat = lat;}
    void set_lon(double *lon) {d_lon = lon;}
    
    int get_lat_length() const {return d_lat_length;}
    int get_lon_length() const {return d_lon_length;}
    void set_lat_length(int len) {d_lat_length = len; }
    void set_lon_length(int len) {d_lon_length = len; }

    // These four are indeces of the constraint
    int get_latitude_index_top() const {return d_latitude_index_top;}
    int get_latitude_index_bottom() const {return d_latitude_index_bottom;}
    void set_latitude_index_top(int top) { d_latitude_index_top = top;}
    void set_latitude_index_bottom(int bottom) { d_latitude_index_bottom = bottom;}
    
    int get_longitude_index_left() const {return d_longitude_index_left;}
    int get_longitude_index_right() const {return d_longitude_index_right;}
    void set_longitude_index_left(int left) { d_longitude_index_left = left;}
    void set_longitude_index_right(int right) { d_longitude_index_right = right;}
    
    bool get_bounding_box_set() const {return d_bounding_box_set;}
    bool get_longitude_rightmost() const {return d_longitude_rightmost;}
    void set_longitude_rightmost(bool state) {d_longitude_rightmost = state;}
     
    Notation get_longitude_notation() const {return d_longitude_notation;}
    
    LatitudeSense get_latitude_sense() const {return d_latitude_sense;}

    set<string> get_coards_lat_units() const {return d_coards_lat_units;}
    set<string> get_coards_lon_units() const {return d_coards_lon_units;}

    set<string> get_lat_names() const {return d_lat_names;}
    set<string> get_lon_names() const {return d_lon_names;}
    //@}
    
    void set_bounding_box(double left, double top, double right, double bottom);
    
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
    virtual void apply_constraint_to_data() = 0;
};

#endif // _geo_constraint_h

