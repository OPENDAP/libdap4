
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
 
// (c) COPYRIGHT URI/MIT 1998-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#ifndef _rvalue_h
#define _rvalue_h

/** Holds the rvalues for the parser, Clause objects and evaluator.

    @see Clause.h
    @see DDS.h
    @see expr.y */

class rvalue {
private:
    BaseType *value;
    btp_func func;		// pointer to a function returning BaseType *
    std::vector<rvalue *> *args;		// arguments to the function

public:
    typedef std::vector<rvalue *>::iterator Args_iter ;
    typedef std::vector<rvalue *>::const_iterator Args_citer ;

    rvalue(BaseType *bt);
    rvalue(btp_func f, vector<rvalue *> *a);
    rvalue();

    virtual ~rvalue();
    string value_name();

    BaseType *bvalue(const string &dataset, DDS &dds);
};

// This type def must come after the class definition above. It is used in
// the Clause and DDS classes.
typedef std::vector<rvalue *> rvalue_list;
typedef std::vector<rvalue *>::const_iterator rvalue_list_citer ;
typedef std::vector<rvalue *>::iterator rvalue_list_iter ;

BaseType **build_btp_args(rvalue_list *args, DDS &dds);

// $Log: RValue.h,v $
// Revision 1.14  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.12.2.1  2004/12/23 21:08:52  jimg
// Minor fixes.
//
// Revision 1.13  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.12  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.11  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.10.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.10  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.9  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.6.4.3  2002/10/29 22:21:01  pwest
// added operator== and operator!= operators to IteratorAdapter and
// IteratorAdapterT classes to handle Pix == Pix use.
//
// Revision 1.6.4.2  2002/09/22 14:18:36  rmorris
// I had to make one of the iterator typedef's public as it is used
// external to this class - If it was making it through GNU C++, I don't
// know why.  VC++ can't swallow that.  Changes all uses of vector to
// std::vector as the 'using' directive no longer cuts it in this case.
//
// Revision 1.6.4.1  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.8  2002/06/18 19:37:03  tom
// l
// Removed SLList from constructor call sequence, to match .cc file.
//
// Revision 1.7  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.6  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.5  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.4.6.1  2000/08/01 21:09:36  jimg
// Destructor is now virtual
//
// Revision 1.4  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.3  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/01/21 20:42:01  tom
// Fixed comment formatting problems for doc++
//
// Revision 1.1  1998/10/21 16:14:17  jimg
// Added. Based on code that used to be in expr.h/cc

#endif // _rvalue_h
