// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Float32 type.
//
// 3/22/9 jhrg9

// $Log: Float32.h,v $
// Revision 1.9  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.8.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.8  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.7  1999/03/24 23:40:05  jimg
// Added
//
// Revision 1.5.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.5  1998/01/12 14:27:57  tom
// Second pass at class documentation.
//
// Revision 1.4  1997/12/18 15:06:10  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.3  1997/09/22 23:03:28  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.2  1996/12/02 23:10:13  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.1  1996/08/26 20:17:54  jimg
// Added.
//

#ifndef _Float32_h
#define _Float32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#ifdef WIN32
#include <rpc.h>
#include <winsock.h>
#include <xdr.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include "BaseType.h"

#ifdef WIN32
using namespace std;
#endif

/** Holds a 32-bit floating point value.

    @see BaseType
    */

class Float32: public BaseType {
    /** This class allows Byte, ..., Float64 acesss to _buf to simplify and
	speed up the relational operators.

	NB: According to Stroustrup it does not matter where (public, private
	or protected) friend classes are declared. */
    friend class Byte;
    friend class Int16;
    friend class UInt16;
    friend class Int32;
    friend class UInt32;
    friend class Float64;

protected:
    dods_float32 _buf;

public:
  /** The Float32 constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The Float32 constructor. */
    Float32(const string &n = "");

    virtual ~Float32() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

#ifdef WIN32
    virtual void print_val(std::ostream &os, string space = "", 
			   bool print_decl_p = true);
#else
    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);
#endif

    virtual bool ops(BaseType *b, int op, const string &dataset);
};

#endif

