
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
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for Url.
//
// jhrg 9/7/94

#ifdef __GNUG__
#pragma implementation
#endif

#include "Url.h"
/** The URL constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 

*/

Url::Url(const string &n) : Str(n)
{
    set_type(dods_url_c);		// override the type set by Str
}

BaseType *
Url::ptr_duplicate()
{
    return new Url(*this);
}

// $Log: Url.cc,v $
// Revision 1.21  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.20  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.19  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.17.4.2  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.18  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.17.4.1  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.17  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.16  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.15.14.1  1999/02/02 21:57:04  jimg
// String to string version
//
// Revision 1.15  1996/06/04 21:33:48  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.14  1996/05/31 23:30:39  jimg
// Updated copyright notice.
//
// Revision 1.13  1996/05/14 15:38:45  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.12  1996/04/05 00:22:08  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1996/03/05 01:10:12  jimg
// Fixed comments.
//
// Revision 1.10  1995/08/23  00:25:05  jimg
// Uses the new Type enum.
//
// Revision 1.9  1995/07/09  21:29:23  jimg
// Added copyright notice.
//
// Revision 1.8  1995/03/16  17:30:45  jimg
// This class is now a subclass of Str.
//
// Revision 1.7  1995/03/04  14:34:52  jimg
// Major modifications to the transmission and representation of values:
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
//
// Revision 1.6  1995/02/10  02:22:52  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.5  1995/01/19  20:05:20  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.4  1995/01/11  15:54:37  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.3  1994/11/29  20:16:35  jimg
// Added mfunc for data transmission.
// Uses special xdr function for serialization and xdr_coder.
// Removed `type' parameter from ctor.
// Added FILE *in and *out to ctor parameter list.
//
// Revision 1.2  1994/09/23  14:36:15  jimg
// Fixed errors in comments.
//
// Revision 1.1  1994/09/15  21:08:50  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//

