// -*- C++ -*-

// Interface for Byte type. 
//
// jhrg 9/7/94

/* $Log: Byte.h,v $
/* Revision 1.10  1995/08/22 23:48:13  jimg
/* Removed card() member function.
/* Removed old, deprecated member functions.
/* Changed the names of read_val and store_val to buf2val and val2buf.
/*
 * Revision 1.9  1995/03/04  14:34:57  jimg
 * Major modifications to the transmission and representation of values:
 * 	Added card() virtual function which is true for classes that
 * 	contain cardinal types (byte, int float, string).
 * 	Changed the representation of Str from the C rep to a C++
 * 	class represenation.
 * 	Chnaged read_val and store_val so that they take and return
 * 	types that are stored by the object (e.g., inthe case of Str
 * 	an URL, read_val returns a C++ String object).
 * 	Modified Array representations so that arrays of card()
 * 	objects are just that - no more storing strings, ... as
 * 	C would store them.
 * 	Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * 	an array of a structure is represented as an array of Structure
 * 	objects).
 *
 * Revision 1.8  1995/02/10  02:22:40  jimg
 * Added DBMALLOC includes and switch to code which uses malloc/free.
 * Private and protected symbols now start with `_'.
 * Added new accessors for name and type fields of BaseType; the old ones
 * will be removed in a future release.
 * Added the store_val() mfunc. It stores the given value in the object's
 * internal buffer.
 * Made both List and Str handle their values via pointers to memory.
 * Fixed read_val().
 * Made serialize/deserialize handle all malloc/free calls (even in those
 * cases where xdr initiates the allocation).
 * Fixed print_val().
 *
 * Revision 1.7  1995/01/19  21:59:12  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.6  1995/01/18  18:38:26  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.5  1995/01/11  15:54:27  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.4  1994/11/29  20:06:33  jimg
 * Added mfuncs for data transmission.
 * Made the xdr_coder function pointer xdr_bytes() while (de)serialize() uses
 * xdr_char().
 * Removed `type' from ctor parameter list.
 * Added FILE *in and *out to parameter list (they default to stdin/out).
 *
 * Revision 1.3  1994/11/22  14:05:32  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.2  1994/09/23  14:36:07  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:08:57  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Byte_h
#define _Byte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "BaseType.h"

class Byte: public BaseType {
protected:
    byte _buf;

public:
    Byte(const String &n = (char *)0);
    virtual ~Byte() {}

    virtual BaseType *ptr_duplicate() = 0;

#ifdef NEVER
    virtual bool card();
#endif
#ifdef NEVER
    virtual unsigned int size();
#endif
    virtual unsigned int width();

    virtual bool serialize(bool flush = false);
    virtual bool deserialize(bool reuse = false);

    virtual bool read(String dataset, String var_name, String constraint) = 0;
    virtual unsigned int store_val(void *val, bool reuse = false); // dep.
    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int read_val(void **val); // deprecated name
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);
};

typedef Byte * BytePtr;

#endif

