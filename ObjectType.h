
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
 
#ifndef _object_type_h
#define _object_type_h

/** When a version 2.x or greater DODS data server sends an object, it uses
    the Content-Description header of the response to indicate the type of
    object contained in the response. During the parse of the header a member
    of Connect is set to one of these values so that other mfuncs can tell
    the type of object without parsing the stream themselves.

    <pre>
     enum ObjectType {
       unknown_type,
       dods_das,
       dods_dds,
       dods_data,
       dods_error,
       web_error
     };
     </pre>

    @memo The type of object in the stream coming from the data
    server.  */

enum ObjectType {
    unknown_type,
    dods_das,
    dods_dds,
    dods_data,
    dods_error,
    web_error
};

// $Log: ObjectType.h,v $
// Revision 1.6  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.5  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.3.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.4  2003/02/20 23:10:52  jimg
// Fixed doxygen comment.
//
// Revision 1.3  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.2  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.1.2.1  2002/06/18 22:56:58  jimg
// Added
//

#endif
