
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the class TestSequence. See TestByte.h
//
// jhrg 1/12/95

/* 
 * $Log: TestSequence.h,v $
 * Revision 1.15  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.14  1999/04/29 02:29:32  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.13.6.1  1999/02/02 21:57:03  jimg
 * String to string version
 *
 * Revision 1.13  1998/01/13 04:15:44  jimg
 * Added a copy ctor since TestSequence has its own private data members. g++
 * 2.7.2.3 (?) running on Linux complained (apparently) about it being missing.
 *
 * Revision 1.12  1997/10/09 22:19:26  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.11  1997/08/11 18:19:27  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.10  1997/07/15 21:55:30  jimg
 * Changed return type of the length member function.
 *
 * Revision 1.9  1996/05/31 23:30:30  jimg
 * Updated copyright notice.
 *
 * Revision 1.8  1996/05/29 22:08:52  jimg
 * Made changes necessary to support CEs that return the value of a function
 * instead of the value of a variable. This was done so that it would be
 * possible to translate Sequences into Arrays without first reading the
 * entire sequence over the network.
 *
 * Revision 1.7  1996/05/16 22:50:21  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.6  1996/04/05 00:22:00  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.5  1995/12/09  01:07:24  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.4  1995/12/06  19:55:43  jimg
 * Changes read() member function from three arguments to two.
 *
 * Revision 1.3  1995/02/10  02:33:59  jimg
 * Modified Test<class>.h and .cc so that they used to new definitions of
 * read_val().
 * Modified the classes read() so that they are more in line with the
 * class library's intended use in a real subclass set.
 *
 * Revision 1.2  1995/01/19  21:59:42  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.1  1995/01/19  20:20:52  jimg
 * Created as an example of subclassing the class hierarchy rooted at
 * BaseType.
 */

#ifndef _TestSequence_h
#define _TestSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"
#include <fstream>

#ifdef WIN32
using std::ifstream;
#endif

class TestSequence: public Sequence {
private:
    /// Pointer to current input file.
    ifstream _input;

    /// True if _input has been opened.
    bool _input_opened;
    
    void _duplicate(const TestSequence &ts);

public:
    TestSequence(const string &n = "");
    TestSequence(const TestSequence &rhs);

    virtual ~TestSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset, int &error);

    virtual int length();
};

#endif
