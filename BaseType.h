
// -*- C++ -*-

// Abstract base class for the variables in a dataset. This is used to store
// the type-invariant information that describes a variable as given in the
// DODS API.
//
// jhrg 9/6/94

/* $Log: BaseType.h,v $
/* Revision 1.13  1995/02/16 22:46:02  jimg
/* Added _in private member. It is used to keep a copy of the input FILE *
/* so that when the next chunk of data is read in the previous one can be
/* closed. Since the netio library unlinks the tmp file before returning
/* the FILE *, closing it effectively deletes the tmp file.
/*
 * Revision 1.12  1995/02/10  02:41:58  jimg
 * Added new mfuncs to access _name and _type.
 * Made private and protected filed's names start with `_'.
 * Added store_val() as a abstract virtual mfunc.
 *
 * Revision 1.11  1995/01/19  21:59:10  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.10  1995/01/18  18:35:28  dan
 * Defined abstract virtual function 'readVal' which provides access
 * to the object's buf for retrieving data subsequent to deserializing.
 *
 * Revision 1.9  1995/01/11  16:06:48  jimg
 * Added static XDR pointers to BaseType class and removed the XDR pointers
 * that were class members - now there is only one xdrin and one xdrout
 * for all children of BaseType.
 * Added friend functions to help in setting the FILE * associated with
 * the XDR *s.
 * Removed FILE *in member (but FILE *out was kept as FILE * _out, mfunc
 * expunge()).
 * Changed ctor so that it no longer takes FILE * params.
 *
 * Revision 1.8  1994/12/16  22:04:21  jimg
 * Added the mfuncs var() and add_var(). These are used by ctor types. They
 * need to be defined here so that access to them via BaseType * will work
 * (actually, so the code will compile). These versions just print error
 * messages. See Array.h, ... for examples of the real mfuncs.
 *
 * Revision 1.7  1994/12/12  20:33:03  jimg
 * Added enum Part - used to be part of CtorType.
 *
 * Revision 1.6  1994/11/29  19:14:15  jimg
 * Added mroe support for data transmission; BaseType now contains enough
 * functionality to support transmission of all the simple datatypes.
 * Added in and out FILE *.
 * Added boolean flag in serialize which will cause the output buffer to
 * be flushed when data is serialized.
 * Added xdr_coder for serialization of arrays and lists.
 *
 * Revision 1.5  1994/11/22  14:05:29  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.4  1994/10/17  23:30:47  jimg
 * Added ptr_duplicate virtual mfunc. Child classes can also define this
 * to copy parts that BaseType does not have (and allocate correctly sized
 * pointers.
 * Removed protected mfunc error() -- use errmsg library instead.
 * Added formatted printing of types (works with DDS::print()).
 *
 * Revision 1.3  1994/09/23  14:34:44  jimg
 * Added mfunc check_semantics().
 * Moved definition of dtor to BaseType.cc.
 *
 * Revision 1.2  1994/09/15  21:08:56  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/09  15:28:42  jimg
 * Class for base type variables. Int32, ... inherit from this class.
 */

#ifndef _Base_Type_h
#define _Base_Type_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>
#include <rpc/xdr.h>

#include <iostream.h>
#include <String.h>

#include "config.h"

// PART names the parts of multi-section ctor types; e.g., Function has two
// sets of variables, the INDEPENDENT variables and the DEPENDENT variables.

enum Part {
    nil,			// nil is for types that don't have parts...
    independent,
    dependent,
    array,
    maps
};

class BaseType {
private:
    String _name;		// name of the instance
    String _type;		// name of the instance's type

    // _out is used to retain access to the FILE * used by _xdrout. It is
    // used by the mfunc expunge to flush the buffer.
    static FILE *_out;		// output stream for data from server
    static FILE *_in;		// like _out but for input

    void _duplicate(const BaseType &bt);

protected:
    // xdr_coder is used as an argument to xdr procedures that encode groups
    // of things (e.g., xdr_array()). Each leaf class's ctor must set this.
    xdrproc_t _xdr_coder;

    // These static pointers are (by definition) common to all members of
    // BaseType. The streams associated with them may be changed using
    // functions that are friends of this class.
    //
    // NB: It is normal for each of these two static class members to use a
    // small amount of dynamically allocated memory (allocated within the xdr
    // library using malloc). The first call to serialize() will malloc a
    // 4104 and a 136 byte block. The first call to deserialize() will malloc
    // a 4104 byte block. These will not be deallocated even when all the
    // objects are destroyed. However, once created they won't be re
    // allocated again.
    static XDR *_xdrin;		// xdr pointer for input (default: from stdin)
    static XDR *_xdrout;	// xdr pointer for output (default: to stdout)

public:
    BaseType(const String &n = (char *)0, const String &t = (char *)0,
	     xdrproc_t xdr = NULL);
    BaseType(const BaseType &copy_from);
    virtual ~BaseType();

    BaseType &operator=(const BaseType &rhs);
    virtual BaseType *ptr_duplicate() = 0; // alloc new instance and dup THIS.

    String get_var_name() const; // deprecated
    void set_var_name(const String &n); // deprecated

    String get_var_type() const; // deprecated
    void set_var_type(const String &t); // deprecated

    String name() const;
    void set_name(const String &n);

    String type() const;
    void set_type(const String &t);

    xdrproc_t xdr_coder();

    // Access to the XDR * for input and output is limited to serialize and
    // deserialize. These friend functions are used to set the FILE * used by
    // those XDR pointers. They are defined in BaseType.cc
    friend void set_xdrin(FILE *in);
    friend void set_xdrout(FILE *out);

    // The var() and add_var() mfuncs are only used by ctor classes (Array,
    // Structure, ...). Their BaseType implementations print an error
    // message.
    virtual BaseType *var(const String &name = (char *)0);
    virtual void add_var(BaseType *v, Part p = nil);

    // Return the number of bytes that are required to hold the instance's
    // value. In the case of scalar types such as Int32, this is the size of
    // one Int32 (four bytes). For a Str or Url, size() returns the number of
    // bytes needed for a char * variable, not the bytes needed for the
    // characters since that value can not be determined from type
    // information alone. For Structure, ... types size() returns the number
    // of bytes needed to store each of the fields as C would store them in a
    // struct. 
    virtual unsigned int size() = 0; // local representation size in bytes

    // Put the data into a local buffer so that it may be serialized. This
    // mfunc must be specialized for each API/format (it is not defined by
    // the classes Int32, ..., Grid - instead look in TestInt32, ... for
    // samples). 
    virtual bool read(String dataset, String var_name, String constraint) = 0;
    
    // read_val() reads the value of the variable from an internal buffer and
    // stores it in the memory referenced by *VAL. Either the caller must
    // allocate enough storage to *VAL or set it to null. In the later case,
    // new will be used to allocate storage. The caller is then responsible
    // for deallocating storage. Array and List values are stored as C would
    // store an array (N values stored sequentially). Structure, ..., Grid
    // values are stored as C would store a struct.
    virtual unsigned int read_val(void **val) = 0;

    // Store the value pointed to by VAL in the object's internal buffer. This
    // mfunc does not perform any checks, so callers must be sure that the
    // thing pointed to can actually be stored in the object's buffer.
    // Return the size (in bytes) of the information copied from VAL.
    virtual unsigned int store_val(void *val, bool reuse = false) = 0;

    // Move data to and from the net.
    virtual bool serialize(bool flush = false) = 0; 
    virtual unsigned int deserialize(bool reuse = false) = 0;
    
    // Write the buffers maintained by XDR to the associated FILE *s.
    bool expunge();

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true);
    virtual void print_val(ostream &os, String space = "",
			   bool print_decl_p = true) = 0;

    virtual bool check_semantics(bool all = false);
};

typedef BaseType * BaseTypePtr;

#endif 
