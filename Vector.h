
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
 * Revision 1.17  1997/12/18 15:06:14  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.16  1997/12/18 14:57:05  tom
 * merged conflicts
 *
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

  /** Holds a one-dimensional array of DODS data types.  This class
      takes two forms, depending on whether the elements of the vector
      are themselves simple of compound objects.

      When each element of the class is a simple data type, the Vector
      is implemented as a simple array of C types, rather than as an
      array of BaseType data types.  A single private ``template''
      BaseType instance (#_var#) is used to hold information in common
      to all the members of the array.  The template is also used as a
      container to pass values back and forth to an application
      program, as in #var()#.

      If the elements of the vector are themselves compound data
      types, the array is stored as a vector of BaseType pointers (see
      the g++ class {\bf BaseTypePtrVec}). The template is still used to
      hold information in common to all the members of the array, but
      is not used to pass information to and from the application
      program. 

      @memo Holds an array of DODS data types.  
      @see BaseType */

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

  /** This function sets the #send_p# flag for both the Vector itself
      and its element template.  This does not matter much when the
      Vector contains simple data types, but does become significant
      when the Vector contains compound types.  

      @memo Indicates that the data is ready to send.
      */
    virtual void set_send_p(bool state); 

  /** This function sets the #read_p# flag for both the Vector itself
      and its element template.  This does not matter much when the
      Vector contains simple data types, but does become significant
      when the Vector contains compound types.

      @memo Indicates that the data is ready to send.  */
    virtual void set_read_p(bool state);

  /** Returns the number of bytes needed to hold the {\it entire}
      array.  This is equal to #length()# times the width of each
      element. 

      @memo Returns the width of the data, in bytes.
      */
    virtual unsigned int width();

  /** Returns the number of elements in the vector. Note that some
      child classes of Vector use the length of -1 as a flag value.

      @see Array::append_dim
      */
    virtual int length();	// a length of -1 is a flag value in Array

  /** Sets the length of the vector.  This function does not allocate
      any new space.
      */
    virtual void set_length(int l);

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

  /** Copies data into the class instance buffer.  This function
      assumes that the input {\it val} indicates memory which
      contains, in row major order, enough elements of the correct
      type to fill the array. For an array of a cardinal type the
      memory is simply copied in whole into the Vector buffer.  For
      compound types, the subsidiary #val2buf# is called #length()#
      times on each successive piece of {\it val}.

      @memo Reads data into the Vector buffer.
      @return The number of bytes used by the array.
      @param val A pointer to the input data.
      @param reuse A boolean value, indicating whether the class
      internal data storage can be reused or not.  If this argument is
      TRUE, the class buffer is assumed to be large enough to hold the
      incoming data, and it is {\it not} reallocated.  If FALSE, new
      storage is allocated.  If the internal buffer has not been
      allocated at all, this argument has no effect.
      */
    virtual unsigned int val2buf(void *val, bool reuse = false);

  /** Copies data from the Vector buffer.  This function assumes that
      {\it val} points to an array large enough to hold N instances of
      the `C' representation of the element type.  In the case of a
      Vector containing compound elements, this function assumes that
      {\it val} points to an array large enough to hold N instances of
      the DODS class used to represent that type.

      @return The number of bytes used to store the array.
      @param val A pointer to a pointer to the memory into which the
      class data will be copied.  If the value pointed to is NULL,
      memory will be allocated to hold the data, and the pointer value
      modified accordingly.  The calling program is responsible for
      deallocating the memory indicated by this pointer.  */
    virtual unsigned int buf2val(void **val);

  /** Sets an element of the vector to a given value.  If the type of
      the input and the type of the Vector do not match, an error
      condition is returned.
 
      @memo Sets element #i# to value #val#.
      @return TRUE if the values were successfully set. FALSE if there
      was a type mismatch.
      @param i The index of the element to be changed.
      @param val A pointer to the value to be inserted into the
      array.  */
    bool set_vec(int i, BaseType *val);

  /** Resizes a Vector.  If the input length is greater than the
      current length of the Vector, new memory is allocated (the
      Vector moved if necessary), and the new entries are appended to
      the end of the array and padded with Null values.  If the input
      length is shorter, the tail values are discarded.
      */
    void vec_resize(int l);

  /** Returns a copy of the template array element, #_var#.
      If the Vector contains simple data types, this will contain the
      value of the last vector element accessed with the
      #Vector::var(int i)# function, if any.  If no such access has
      been made, or if the Vector contains compound data types, the
      value of the template instance is undefined.

      @memo Returns a pointer to a copy of the array template.
      @see Vector::var
      */
    virtual BaseType *var(const String &name = (char *)0);

  /** Returns a pointer to the specified Vector element.  For Vectors
      containing simple data types, the element returned will be a
      copy of the indicated element.  For compound types, the return
      pointer will indicate the element itself.

      @param i The index of the desired Vector element.  Zero
      indicates the first element of the Vector.
      @return A pointer to a BaseType class instance containing
      the value of the indicated element.
      @see BaseType::var
      */
    virtual BaseType *var(unsigned int i);

  /** Sets the value of the template variable.
   */
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
