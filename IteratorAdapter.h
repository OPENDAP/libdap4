
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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
// Revision 1.5  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.4  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
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

