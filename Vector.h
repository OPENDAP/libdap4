
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// This is the interface definition file for the abstract class
// Vector. Vector is the parent class for List and Array.

/* 
 * $Log: Vector.h,v $
 * Revision 1.30  2000/09/21 16:22:09  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.29  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.28  2000/06/16 18:15:00  jimg
 * Merged with 3.1.7
 *
 * Revision 1.26.6.1  2000/06/07 23:08:31  jimg
 * Added code to explicitly delete BaseType *s in _vec.
 * Also tried recoding using DLList, but that didn't fix the problem I was
 * after---fixed in the client code but decided to leave this is with #if 0
 * just in case.
 *
 * Revision 1.27  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.26.20.1  2000/06/02 18:29:32  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.26.14.1  2000/01/28 22:14:07  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.26  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.25  1999/04/29 02:29:34  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.24  1999/01/21 20:42:01  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.23  1998/11/10 00:56:44  jimg
 * Fixed up the doc++ comments.
 *
 * Revision 1.22  1998/09/17 17:01:12  jimg
 * Fixed errant documentation.
 *
 * Revision 1.21.6.1  1999/02/02 21:57:04  jimg
 * String to string version
 *
 * Revision 1.21  1998/03/17 17:51:28  jimg
 * Added an implementation of element_count().
 *
 * Revision 1.20  1998/02/05 20:13:59  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.19  1998/02/04 14:55:33  tom
 * Another draft of documentation.
 *
 * Revision 1.18  1998/01/12 14:28:00  tom
 * Second pass at class documentation.
 *
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

#ifdef WIN32
#include <rpc.h>
#include <winsock.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include "BaseType.h"
#include "DDS.h"

  /** Holds a one-dimensional array of DODS data types.  This class
      takes two forms, depending on whether the elements of the vector
      are themselves simple or compound objects. This class contains
      common functionality for the List and Array classes, and should
      rarely be used directly.

      When each element of the class is a simple data type, the Vector
      is implemented as a simple array of C types, rather than as an
      array of BaseType data types.  A single private ``template''
      BaseType instance (#_var#) is used to hold information in common
      to all the members of the array.  The template is also used as a
      container to pass values back and forth to an application
      program, as in #var()#.

      If the elements of the vector are themselves compound data
      types, the array is stored as a vector of BaseType pointers (see
      the DODS class {\bf BaseTypePtrVec}). The template is still used to
      hold information in common to all the members of the array, but
      is not used to pass information to and from the application
      program. 

      @memo Holds a one-dimensional collection of DODS data types.  
      @see BaseType 
      @see List
      @see Array
      */

class Vector: public BaseType {
private:
    int _length;		// number of elements in the vector
    BaseType *_var;		// base type of the Vector

    void *_buf;			// array which holds cardinal data

#if 1
    vector<BaseType *> _vec;
#endif
#if 0
    DLList<BaseType *> _vec;
#endif

protected:
    // This function copies the private members of Vector.
    void _duplicate(const Vector &v);

public:
    /** The Vector constructor requires the name of the variable to be
	created, and a pointer to an object of the type the Vector is to
	hold.  The name may be omitted, which will create a nameless
	variable.  The template object may not be omitted.
      
	@param n A string containing the name of the variable to be
	created. 
	@param v A pointer to a variable of the type to be included 
	in the Vector. 
	@param t The type of the resulting Vector object, from the Type
	enum list.  There is no DODS Vector object, so all uses of this
	method will be from the List or Array classes.  This defaults to
	#dods_null_c#.

	@see Type
	@memo The List constructor.  */
    Vector(const string &n = "", BaseType *v = 0, const Type &t = dods_null_c);

    /** The Vector copy constructor. */
    Vector(const Vector &rhs);

    virtual ~Vector();

    const Vector &operator=(const Vector &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

    virtual int element_count(bool leaves);

    /** This function sets the #send_p# flag for both the Vector itself
	and its element template.  This does not matter much when the
	Vector contains simple data types, but does become significant
	when the Vector contains compound types.  

	@memo Indicates that the data is ready to send. */
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

	@memo Returns the width of the data, in bytes. */
    virtual unsigned int width();

    /** Returns the number of elements in the vector. Note that some
	child classes of Vector use the length of -1 as a flag value.

	@see Array::append_dim */
    virtual int length();

    /** Sets the length of the vector.  This function does not allocate
	any new space. */
    virtual void set_length(int l);

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset) = 0;

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
	allocated at all, this argument has no effect. */
    virtual unsigned int val2buf(void *val, bool reuse = false);

    /** Copies data from the Vector buffer.  This function assumes that
	{\it val} points to an array large enough to hold N instances of
	the `C' representation of the element type.  In the case of a
	Vector containing compound elements, this function assumes that
	{\it val} points to an array large enough to hold N instances of
	the DODS class used to represent that type.

	Use this function only with Vectors containing simple DODS
	types.  See #set_vec()# to access members of Vectors containing
	compound types.

	@return The number of bytes used to store the array.
	@param val A pointer to a pointer to the memory into which the
	class data will be copied.  If the value pointed to is NULL,
	memory will be allocated to hold the data, and the pointer value
	modified accordingly.  The calling program is responsible for
	deallocating the memory indicated by this pointer.  
	@see Vector::set_vec */
    virtual unsigned int buf2val(void **val);

    /** Sets an element of the vector to a given value.  If the type of
	the input and the type of the Vector do not match, an error
	condition is returned.

	Use this function only with Vectors containing compound DODS
	types.  See #buf2val()# to access members of Vectors containing
	simple types.

	NOTE: The memory allocated by this function should be freed using
	delete, \emph{not} delete[]!
 
	@memo Sets element #i# to value #val#.
	@return void
	was a type mismatch.
	@param i The index of the element to be changed.
	@param val A pointer to the value to be inserted into the
	array.  
	@see Vector::buf2val */
    void set_vec(unsigned int i, BaseType *val);

    /** Resizes a Vector.  If the input length is greater than the
	current length of the Vector, new memory is allocated (the
	Vector moved if necessary), and the new entries are appended to
	the end of the array and padded with Null values.  If the input
	length is shorter, the tail values are discarded. */
    void vec_resize(int l);

    /** Returns a copy of the template array element. If the Vector contains
	simple data types, the template will contain the value of the last
	vector element accessed with the {\tt Vector::var(int i)} function, if
	any. If no such access has been made, or if the Vector contains
	compound data types, the value of the template instance is undefined.

	Note that the parameter {\it exact\_match} is not used by this mfunc.

	@param name The name of the variabe to find.
	@param exact_match Unused.
	@return A pointer to the BaseType if found, otherwise null.
	@see Vector::var */
    virtual BaseType *var(const string &name = "", bool exact_match = true);

    /** This version of var(...) searches for {\it name} and returns a
	pointer to the BaseType object if found. It uses the same search
	algorithm as above when {\it exact\_match} is false. In addition to
	returning a pointer to the variable, it pushes onto {\it s} a
	BaseType pointer to each constructor type that ultimately contains
	{\it name}.

	@param name Find the variable whose name is {\it name}.
	@param s Record the path to {\it name}.
	@return A pointer to the named variable. */
    virtual BaseType *var(const string &name, btp_stack &s);

    /** Returns a pointer to the specified Vector element.  For Vectors
	containing simple data types, the element returned will be a
	copy of the indicated element.  For compound types, the return
	pointer will indicate the element itself.

	@param i The index of the desired Vector element.  Zero
	indicates the first element of the Vector.
	@return A pointer to a BaseType class instance containing
	the value of the indicated element.
	@see BaseType::var */
    virtual BaseType *var(unsigned int i);

    /** Sets the value of the template variable.  */
    virtual void add_var(BaseType *v, Part p = nil);

    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(string &msg, bool all = false);
};

#endif /* _Vector_h */
