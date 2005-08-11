
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

// Interface for TestFloat32 type. See TestByte.h
//
// 3/22/99 jhrg

#ifndef _testfloat32_h
#define _testfloat32_h 1


#include "Float32.h"
#include "TestCommon.h"

class TestFloat32: public Float32, public TestCommon {
    bool d_series_values;
    
    void _duplicate(const TestFloat32 &ts);

public:
    TestFloat32(const string &n = (char *)0);
    TestFloat32(const TestFloat32 &rhs);

    virtual ~TestFloat32() {}

    TestFloat32 &operator=(const TestFloat32 &rhs);

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);
    void set_series_values(bool sv) { d_series_values = sv; }
    bool get_series_values() { return d_series_values; }
};

/* 
 * $Log: TestFloat32.h,v $
 * Revision 1.11  2005/01/28 17:25:12  jimg
 * Resolved conflicts from merge with release-3-4-9
 *
 * Revision 1.8.2.4  2005/01/18 23:08:47  jimg
 * All Test* classes now handle copy and assignment correctly.
 *
 * Revision 1.8.2.3  2005/01/14 19:37:38  jimg
 * Added support for returning cyclic values.
 *
 * Revision 1.10  2004/07/07 21:08:48  jimg
 * Merged with release-3-4-8FCS
 *
 * Revision 1.8.2.2  2004/07/02 20:41:52  jimg
 * Removed (commented) the pragma interface/implementation lines. See
 * the ChangeLog for more details. This fixes a build problem on HP/UX.
 *
 * Revision 1.9  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.8.2.1  2003/06/23 11:49:18  rmorris
 * The // #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.8  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.7  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.6.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.6  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.5  2000/09/22 02:17:21  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.4  2000/09/21 16:22:08  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.3.14.1  2000/02/17 05:03:14  jimg
 * Added file and line number information to calls to InternalErr.
 * Resolved compile-time problems with read due to a change in its
 * parameter list given that errors are now reported using exceptions.
 *
 * Revision 1.3  1999/05/04 19:47:22  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.2  1999/04/29 02:29:32  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.1  1999/03/24 23:40:05  jimg
 * Added
 *
 */

#endif //_testfloat32_h

