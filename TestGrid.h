
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the TestGrid ctor class. See TestByte.h
//
// jhrg 1/13/95

/* $Log: TestGrid.h,v $
/* Revision 1.8  1996/05/31 23:30:20  jimg
/* Updated copyright notice.
/*
 * Revision 1.7  1996/05/16 22:50:17  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.6  1996/04/05 00:21:52  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.5  1995/12/09  01:07:17  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.4  1995/12/06  19:55:38  jimg
 * Changes read() member function from three arguments to two.
 *
 * Revision 1.3  1995/02/10  02:33:55  jimg
 * Modified Test<class>.h and .cc so that they used to new definitions of
 * read_val().
 * Modified the classes read() so that they are more in line with the
 * class library's intended use in a real subclass set.
 *
 * Revision 1.2  1995/01/19  21:59:35  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.1  1995/01/19  20:20:46  jimg
 * Created as an example of subclassing the class hierarchy rooted at
 * BaseType.
 *
 */

#ifndef _TestGrid_h
#define _TestGrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"

class TestGrid: public Grid {
public:
    TestGrid(const String &n = (char *)0);
    virtual ~TestGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);
};

#endif

