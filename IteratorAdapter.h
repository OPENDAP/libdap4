
// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//	Patrick West <pwest@ucar.edu>

#ifndef _iterator_adapter_h
#define _iterator_adapter_h

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

class IteratorAdapter
{

public:
    /* constructors */
    IteratorAdapter( ) ;
    IteratorAdapter( IteratorAdapter *iter ) ;
    IteratorAdapter( const IteratorAdapter &iter ) ;

    /* assignment operator */
    IteratorAdapter &operator=( const IteratorAdapter &iter ) ;

    /* destructor */
    virtual ~IteratorAdapter( ) ;

    /* public methods */
    virtual void first( ) ;
    virtual void next( ) ;

    virtual operator bool( ) ;
    //virtual bool operator==( void *op2 ) ;
    //virtual bool operator!=( void *op2 ) ;
    virtual bool operator==( const IteratorAdapter &i ) ;
    virtual bool operator!=( const IteratorAdapter &i ) ;

    IteratorAdapter *getIterator( ) ;

    /* reference counting */
    virtual void incref( ) ;
    virtual void decref( ) ;
    virtual void free( ) ;

protected:

private:
    /* private data members */
    IteratorAdapter *_i ;
    int _ref ;
} ;

// $Log: IteratorAdapter.h,v $
// Revision 1.3  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.2.2.5  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.2.2.4  2002/12/05 20:36:19  pwest
// Corrected problems with IteratorAdapter code, making methods non-inline,
// creating source files and template instantiation file. Cleaned up file
// descriptors and memory management problems. Corrected problem in Connect
// where the xdr source was not being cleaned up or a new one created when a
// new file was opened for reading.
//
// Revision 1.2.2.3  2002/11/05 01:11:49  jimg
// Added some boilerplate and fiddled with formatting.
//
// Revision 1.2.2.2  2002/10/29 22:21:00  pwest
// added operator== and operator!= operators to IteratorAdapter and
// IteratorAdapterT classes to handle Pix == Pix use.
//
// Revision 1.2.2.1  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.2  2002/09/13 16:27:47  jimg
// Added a CVS log.
// Added an emacs mode setting to match the rest of our code (always an ongoing
// process...). Reformatted so that it doesn't wrap in an 80 column editor.
// Added an attempt at operator==() which causes problems down in the idl
// command line client. I've wrapped it in guards; if the code proves useless,
// we should remove it.
//

#endif // _ITERATORADAPTER_H

