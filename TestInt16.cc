
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implmentation of the OPeNDAP Data
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

// Implementation for TestInt16. See TestByte.cc
//
// 3/22/99 jhrg

#ifdef __GNUG__
#pragma implementation
#endif

#include "config_dap.h"
#include "TestInt16.h"

Int16 *
NewInt16(const string &n)
{
    return new TestInt16(n);
}

TestInt16::TestInt16(const string &n) : Int16(n)
{
}

BaseType *
TestInt16::ptr_duplicate()
{
    return new TestInt16(*this);
}

bool
TestInt16::read(const string &)
{
    if (read_p())
	return true;

    _buf = 32000;

    set_read_p(true);
    
    return true;
}

// $Log: TestInt16.cc,v $
// Revision 1.6  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.5  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.4  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.3.6.1  2000/08/02 20:58:26  jimg
// Included the header config_dap.h in this file. config_dap.h has been
// removed from all of the DODS header files.
//
// Revision 1.3.14.1  2000/02/17 05:03:14  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.3  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.2  1999/04/29 02:29:32  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/03/24 23:40:06  jimg
// Added
