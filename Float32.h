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

class Float32: public BaseType {
protected:
    float _buf;

public:
    Float32(const String &n = (char *)0);
    virtual ~Float32() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool ops(BaseType &b, int op);
};

typedef Float32 * Float32Ptr;

#endif

