// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for the UInt32 (unsigned int 32) type. 
//
// jhrg 9/7/94

// $Log: UInt32.h,v $
// Revision 1.8  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.7.20.1  2000/06/02 18:29:32  rmorris
// Mod's for port to Win32.
//
// Revision 1.7  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.6  1999/03/24 23:35:33  jimg
// Added support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.5.6.1  1999/02/02 21:57:04  jimg
// String to string version
//
// Revision 1.5  1997/12/18 15:06:14  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.4  1997/09/22 22:39:54  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.3  1996/12/02 23:10:32  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.2  1996/10/18 16:53:36  jimg
// Fixed definition of dtor (was Int32 instead of UInt32).
//
// Revision 1.1  1996/08/26 19:40:40  jimg
// Added.
//

#ifndef _UInt32_h
#define _UInt32_h 1

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

/** Holds a 32-bit unsigned integer. 

    @see BaseType */

class UInt32: public BaseType {
    /** This class allows Byte, ..., Float64 acesss to _buf to simplify and
	speed up the relational operators.

	NB: According to Stroustrup it does not matter where (public, private
	or protected) friend classes are declared. */
    friend class Byte;
    friend class Int16;
    friend class UInt16;
    friend class Int32;
    friend class Float32;
    friend class Float64;

protected:
    dods_uint32 _buf;

public:
    UInt32(const string &n = "");
    virtual ~UInt32() {}

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

