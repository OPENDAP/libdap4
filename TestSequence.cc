
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class TestStructure. See TestByte.cc
//
// jhrg 1/12/95
//
// Note that the test code here to read values from a data file works only
// for single level sequences - that is, it does *not* work for sequences
// that contain other sequences. jhrg 2/2/98 

// $Log: TestSequence.cc,v $
// Revision 1.21  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.20.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
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

#ifdef WIN32
using namespace std;
#endif

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

// Read values from text files. Sequence instances are stored on separate
// lines. Line can be no more than 255 characters long.

bool 
TestSequence::read(const string &dataset, int &error)
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

    // If at EOF, return false indicating no more data. Leave error as it is.
    if (_input.eof())
	return false;

    // Read a line at a time and extract the values. Any line without values
    // is skipped. EOF ends the file.
    while (true) {
	_input.getline(line, 256);
	if (_input.eof())
	    return false;	// error unchanged, nominally false.
	if (!_input) {
	    error = 1;
	    DBG(cerr << "Input file error" << endl);
	    return false;
	}

	string l = line;
	if (l.find_first_not_of(" \t\n\r") != l.npos || l[0] == '#')	// Blank or comment line
	    continue;
	else
	    break;		// Assume valid line.
    }

    istrstream iss(line);

    for (Pix p = first_var(); p; next_var(p)) {
	// Don't use the read mfuncs since for now within Test* they always
	// return the same hardcoded values (not much use for testing
	// sequences).
	switch (var(p)->type()) {
	  case dods_byte_c: {
	      unsigned int ui;
	      iss >> ui;
	      char b = ui;
	      var(p)->val2buf((void*)&b);
	      var(p)->set_read_p(true);
	      break;
	  }
	  case dods_int32_c: {
	      int i;
	      iss >> i;
	      DBG(cerr << "Int32 value read :" << i << endl);
	      var(p)->val2buf((void*)&i);
	      var(p)->set_read_p(true);
	      break;
	  }
	  case dods_uint32_c: {
	      unsigned int ui;
	      iss >> ui;
	      var(p)->val2buf((void*)&ui);
	      var(p)->set_read_p(true);
	      break;
	  }
	  case dods_float64_c: {
	      double d;
	      iss >> d;
	      var(p)->val2buf((void*)&d);
	      var(p)->set_read_p(true);
	      break;
	  }
	  case dods_str_c:
	  case dods_url_c: {
	      string s;
	      iss >> s;
	      var(p)->val2buf((void*)&s);
	      var(p)->set_read_p(true);
	      break;
	  }

	  case dods_array_c:
	  case dods_list_c:
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
