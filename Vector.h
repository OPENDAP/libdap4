
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This is the interface definition file for the abstract class
// Vector. Vector is the parent class for List and Array.

/* 
 * $Log: Vector.h,v $
 * Revision 1.15  1997/12/16 00:42:53  jimg
 * The return type of set_vec() is now bool (was void).
 *
 * Revision 1.14  1997/10/09 22:19:26  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.13  1997/08/11 18:19:31  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.12  1997/03/08 19:02:13  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.11  1997/02/28 01:29:13  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.10  1996/08/13 18:40:46  jimg
 * Changes return type of length() member function from unsigned to int. A
 * return value of -1 indicates that the vector has no length.
 *
 * Revision 1.9  1996/06/04 21:33:51  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.8  1996/05/31 23:30:44  jimg
 * Updated copyright notice.
 *
 * Revision 1.7  1996/05/16 22:50:27  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.6  1996/05/14 15:38:48  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.5  1996/04/05 00:22:10  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.4  1996/03/05 01:03:04  jimg
 * Added ce_eval parameter to serialize() member function.
 * Added vec_resize() member function to class.
 *
 * Revision 1.3  1995/12/09  01:07:34  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.2  1995/12/06  19:49:35  jimg
 * Changed the var() and print_decl() mfuncs. var() now takes an index and
 * returns a pointer to the BaseType object with the correct
 * value. print_decl() takes a new flag - constrained - which causes only
 * those dimensions selected by the current constraint expression to be printed
 *
 * Revision 1.1  1995/11/22  22:30:20  jimg
 * Created.
 */

#ifndef _Vector_h
#define _Vector_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include "BaseType.h"
#include "BaseTypeVec.h"
#include "DDS.h"

class Vector: public BaseType {
private:
    int _length;		// number of elements in the vector
    BaseType *_var;		// base type of the Vector

    void *_buf;			// array which holds cardinal data

    BaseTypePtrVec _vec;	// used for vectors of all other types

protected:
    // This function copies the private members of Vector.
    void _duplicate(const Vector &v);

public:
    Vector(const String &n = (char *)0, BaseType *v = 0, 
	   const Type &t = dods_null_c);
    Vector(const Vector &rhs);

    virtual ~Vector();

    const Vector &operator=(const Vector &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual unsigned int width();
    virtual int length();	// a length of -1 is a flag value in Array
    virtual void set_length(int l);

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    bool set_vec(int i, BaseType *val);
    void vec_resize(int l);

    virtual BaseType *var(const String &name = (char *)0);

    // Return the BaseType pointer for the given Vector element. The value of
    // the element is copied into the BaseType *. See BaseType::var() for a
    // version of this mfunc that does *not* copy the value.
    virtual BaseType *var(unsigned int i);

    virtual void add_var(BaseType *v, Part p = nil);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(String &msg = String(), bool all = false);
};

#endif /* _Vector_h */
