// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Float64 type.
//
// jhrg 9/7/94

// $Log: Float32.h,v $
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

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include "BaseType.h"

/** Holds a 32-bit floating point value.

    @see BaseType
    */

class Float32: public BaseType {
protected:
    float _buf;

public:
  /** The Float32 constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A String containing the name of the variable to be
      created. 

      @memo The Float32 constructor.
      */
    Float32(const String &n = (char *)0);
    virtual ~Float32() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool ops(BaseType &b, int op, const String &dataset);
};

/** A pointer to a Float32 instance. */
typedef Float32 * Float32Ptr;

#endif

