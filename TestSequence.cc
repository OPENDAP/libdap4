
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95
//
// Note that the test code here to read values from a data file works only
// for single level sequences - that is, it does *not* work for sequences
// that contain other sequences. jhrg 2/2/98 

#ifdef _GNUG_
#pragma implementation
#endif

#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include "TestSequence.h"

#include "debug.h"

using std::cerr;
using std::endl;
using std::istrstream;

void
TestSequence::_duplicate(const TestSequence &ts)
{
    _input_opened = ts._input_opened;
}

Sequence *
NewSequence(const string &n)
{
    return new TestSequence(n);
}

BaseType *
TestSequence::ptr_duplicate()
{
    return new TestSequence(*this);
}

TestSequence::TestSequence(const string &n) : Sequence(n)
{
    _input_opened = false;
}

TestSequence::TestSequence(const TestSequence &rhs) : Sequence(rhs)
{
    _duplicate(rhs);
}

TestSequence::~TestSequence()
{
}

TestSequence &
TestSequence::operator=(const TestSequence &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Sequence &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

// Read values from text files. Sequence instances are stored on separate
// lines. Line can be no more than 255 characters long.

bool 
TestSequence::read(const string &dataset)
{
    char line[256];

    if (read_p())
	return true;

    if (!_input_opened) {
	_input.open(dataset.c_str());
	_input_opened = true;
	// For now, use the DDS to get the variable names/types so read the 
	// first line and ignore it.
	_input.getline(line, 255);
    }

    // If at EOF, return false indicating no more data.
    if (_input.eof())
	return false;

    // Read a line at a time and extract the values. Any line without values
    // is skipped. EOF ends the file.
    while (true) {
	_input.getline(line, 256);
	if (_input.eof())
	    return false;
	if (!_input)
	    throw InternalErr(__FILE__, __LINE__, "TestSequence read error.");

	string l = line;
	if (l.find_first_not_of(" \t\n\r") != l.npos || l[0] == '#')
	    continue;
	else
	    break;		// Assume valid line.
    }

    istrstream iss(line);

    for (Vars_iter iter = var_begin(); iter != var_end(); iter++)
    {
	// Don't use the read mfuncs since for now within Test* they always
	// return the same hardcoded values (not much use for testing
	// sequences).
	switch ((*iter)->type()) {
	  case dods_byte_c: {
	      unsigned int ui;
	      iss >> ui;
	      char b = ui;
	      (*iter)->val2buf((void*)&b);
	      (*iter)->set_read_p(true);
	      break;
	  }
	  case dods_int32_c: {
	      int i;
	      iss >> i;
	      DBG(cerr << "Int32 value read :" << i << endl);
	      (*iter)->val2buf((void*)&i);
	      (*iter)->set_read_p(true);
	      break;
	  }
	  case dods_uint32_c: {
	      unsigned int ui;
	      iss >> ui;
	      (*iter)->val2buf((void*)&ui);
	      (*iter)->set_read_p(true);
	      break;
	  }
	  case dods_float64_c: {
	      double d;
	      iss >> d;
	      (*iter)->val2buf((void*)&d);
	      (*iter)->set_read_p(true);
	      break;
	  }
	  case dods_str_c:
	  case dods_url_c: {
	      string s;
	      iss >> s;
	      (*iter)->val2buf((void*)&s);
	      (*iter)->set_read_p(true);
	      break;
	  }

	  case dods_array_c:
	  case dods_structure_c:
	  case dods_sequence_c:
	  case dods_grid_c:
	  default:
	    cerr << "Broken Sequence::read() mfunc! This type not implemented"
		 << endl;
	    break;
	}
    }

    set_read_p(true);
    
    return true;
}

int
TestSequence::length()
{
    return 0;
}

// $Log: TestSequence.cc,v $
// Revision 1.31  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.30  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.29  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.28.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.28  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.27  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.23.4.5  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.23.4.4  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.26  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.23.4.3  2002/03/29 18:40:20  jimg
// Updated comments and/or removed dead code.
//
// Revision 1.25  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.23.4.2  2001/08/18 00:15:01  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.24  2001/06/15 23:49:03  jimg
// Merged with release-3-2-4.
//
// Revision 1.23.4.1  2001/06/05 06:49:19  jimg
// Added the Constructor class which is to Structures, Sequences and Grids
// what Vector is to Arrays and Lists. This should be used in future
// refactorings (I thought it was going to be used for the back pointers).
// Introduced back pointers so children can refer to their parents in
// hierarchies of variables.
// Added to Sequence methods to tell if a child sequence is done
// deserializing its data.
// Fixed the operator=() and copy ctors; removed redundency from
// _duplicate().
// Changed the way serialize and deserialize work for sequences. Now SOI and
// EOS markers are written for every `level' of a nested Sequence. This
// should fixed nested Sequences. There is still considerable work to do
// for these to work in all cases.
//
// Revision 1.23  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.22  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.21  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.20.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.20.14.1  2000/02/17 05:03:15  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.20  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.19  1999/04/29 02:29:32  jimg
// Merge of no-gnu branch
//
// Revision 1.18.6.1  1999/02/02 21:57:03  jimg
// String to string version
//
// Revision 1.18  1998/02/05 20:13:57  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.17  1998/01/13 04:16:11  jimg
// Added a copy ctor since TestSequence has its own private data members. g++
// 2.7.2.3 (?) running on Linux complained (apparently) about it being missing.
// Also added _duplicate private member function.
//
// Revision 1.16  1997/09/22 22:42:16  jimg
// Added massive amounts of code to read test data from a file.
//
// Revision 1.15  1997/07/15 21:54:57  jimg
// Changed return type of length member function.
//
// Revision 1.14  1996/08/13 20:50:47  jimg
// Changed definition of the read member function.
//
// Revision 1.13  1996/05/31 23:30:28  jimg
// Updated copyright notice.
//
// Revision 1.12  1996/05/29 22:08:50  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.11  1996/05/22 18:05:27  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.10  1996/04/05 00:21:58  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.9  1995/12/09  01:07:23  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.8  1995/12/06  19:55:26  jimg
// Changes read() member function from three arguments to two.
//
// Revision 1.7  1995/08/26  00:31:58  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.6  1995/07/09  21:29:18  jimg
// Added copyright notice.
//
// Revision 1.5  1995/05/10  17:35:31  jimg
// Removed the header file `Test.h' from the Test*.cc implementation files.
//
// Revision 1.4  1995/03/04  14:38:08  jimg
// Modified these so that they fit with the changes in the DAP classes.
//
// Revision 1.3  1995/02/10  02:33:46  jimg
// Modified Test<class>.h and .cc so that they used to new definitions of
// read_val().
// Modified the classes read() so that they are more in line with the
// class library's intended use in a real subclass set.
//
// Revision 1.2  1995/01/19  21:59:00  jimg
// Added read_val from dummy_read.cc to the sample set of sub-class
// implementations.
// Changed the declaration of readVal in BaseType so that it names the
// mfunc read_val (to be consistant with the other mfunc names).
// Removed the unnecessary duplicate declaration of the abstract virtual
// mfuncs read and (now) read_val from the classes Byte, ... Grid. The
// declaration in BaseType is sufficient along with the decl and definition
// in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
//
// Revision 1.1  1995/01/19  20:20:51  jimg
// Created as an example of subclassing the class hierarchy rooted at
// BaseType.
//
