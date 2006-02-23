
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

// Implementation for TestInt16. See TestByte.cc
//
// 3/22/99 jhrg


#include "config.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
#endif

#include "TestInt16.h"

extern int test_variable_sleep_interval;

void
TestInt16::_duplicate(const TestInt16 &ts)
{
    d_series_values = ts.d_series_values;
}

TestInt16::TestInt16(const string &n) : Int16(n), d_series_values(false)
{
    _buf = 1;
}

TestInt16::TestInt16(const TestInt16 &rhs) : Int16(rhs), TestCommon(rhs)
{
    _duplicate(rhs);
}

TestInt16 &
TestInt16::operator=(const TestInt16 &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Int16 &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
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

    if (test_variable_sleep_interval > 0)
	sleep(test_variable_sleep_interval);

    if (get_series_values()) {
       _buf = (short)(16 * _buf);
    }
    else {
        _buf = 32000;
    }

    set_read_p(true);
    
    return true;
}

// $Log: TestInt16.cc,v $
// Revision 1.12  2005/03/30 23:12:01  jimg
// Modified to use the new factory class.
//
// Revision 1.11  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.8.2.4  2005/01/18 23:21:44  jimg
// All Test* classes now handle copy and assignment correctly.
//
// Revision 1.8.2.3  2005/01/14 19:38:37  jimg
// Added support for returning cyclic values.
//
// Revision 1.10  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.8.2.2  2004/07/02 20:41:52  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.9  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.8.2.1  2003/07/23 23:56:36  jimg
// Now supports a simple timeout system.
//
// Revision 1.8  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.7  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.6.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
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
// Included the header config.h in this file. config.h has been
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
