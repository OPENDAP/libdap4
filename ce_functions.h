
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

// Declarations for CE functions.
//
// 1/15/99 jhrg

#ifndef _ce_functions_h
#define _ce_functions_h

#if 0
bool func_member(int argc, BaseType *argv[], DDS &dds);
bool func_null(int argc, BaseType *argv[], DDS &dds);
BaseType *func_nth(int argc, BaseType *argv[], DDS &dds);
#endif
BaseType *func_length(int argc, BaseType *argv[], DDS &dds);

void func_grid_select(int argc, BaseType *argv[], DDS &dds);

// $Log: ce_functions.h,v $
// Revision 1.7  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.6  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.5  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.4.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.4  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.3  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.2  1999/01/21 02:52:01  jimg
// Fixed the stupid comments...
//
// Revision 1.1  1999/01/15 22:07:08  jimg
// Moved code from util.h.

#endif // _ce_functions_h
