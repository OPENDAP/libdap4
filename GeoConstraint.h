
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

/** 
    @author James Gallagher */

class GeoConstraint {
private:
    /** The longitude extents of the constraint bounding box can be expressed
        two ways: using a 0/359 notation and using a -180/179 notation. I call
        the 0/359 notation 'pos' and the -180/179 noation 'neg_pos'. */
    enum Notation {
        pos,
        neg_pos
    };
    
    Grid *d_grid;
    const DDS &d_dds;
    
    Array *d_latitude;
    Array *d_longitude;
    
    double *d_lat;
    double *d_lon;
    int d_lat_length;
    int d_lon_length;
    Array::Dim_iter d_lat_grid_dim;
    Array::Dim_iter d_lon_grid_dim;
    
    bool d_bounding_box_set;
    
    set<string> d_coards_lat_units;
    set<string> d_coards_lon_units;

    GeoConstraint();                // Hidden default constructor.

    GeoConstraint(const GeoConstraint &param); // Hide
    GeoConstraint &operator=(GeoConstraint &rhs); // Hide
    
    bool find_lat_lon_maps();
    Notation categorize_notation(double left, double right) const;
    void transform_constraint_to_pos_notation(double &left, double &right) const;
    void transform_longitude_to_pos_notation();
    void transform_longitude_to_neg_pos_notation();
    void find_longitude_indeces(double left, double right, 
                                int &longitude_index_left, 
                                int &longitude_index_right) const;
    void find_latitude_indeces(double top, double bottom, 
                                int &latitude_index_top, 
                                int &latitude_index_bottom) const;                                
    void set_bounding_box_longitude(double left, double right) throw(Error);
    void set_bounding_box_latitude(double top, double bottom) throw(Error);
    void reorder_longitude_map(int longitude_index_left);
    void reorder_data_longitude_axis(int longitude_index_left,
                                     int longitude_index_right);
                                                                                             
    friend class CEFunctionsTest; // Unit tests
    
public:
    /** @name Constructors */
    //@{
    GeoConstraint(Grid *grid, const DDS &dds) throw (Error);
    //@}
    
    void set_bounding_box(double left, double top, double right, double bottom)
        throw (Error);
};

#endif // _geo_constraint_h

