
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef _object_type_h
#define _object_type_h

namespace libdap
{

/** When a version 2.x or greater DAP data server sends an object, it may use
    the Content-Description header of the response to indicate the type of
    object contained in the response.

    <pre>
     enum ObjectType {
       unknown_type,
       dods_das,
       dods_dds,
       dods_data,
       dods_ddx,
       dods_data_ddx,
       dods_error,
       web_error,

       dap4_dmr,
       dap4_data,
       dap4_error,
     };
     </pre>

    @note If this is cjanged, update the char* array 'descrip' in mime_util.h.

    @brief The type of object in the stream coming from the data
    server.  */

enum ObjectType {
    unknown_type,
    dods_das,
    dods_dds,
    dods_data,
    dods_ddx,	   	// This is the old XML DDS/DAS used prior to dap4
    dods_data_ddx,	// This is used for caching data responses
    dods_error,
    web_error,

    dap4_dmr,		// DAP4 metadata
    dap4_data,		// The DMR with a data blob
    dap4_error		// The error response for DAP4
};

} // namespace libdap

#endif
