
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Class for array variables. The dimensions of the array are stored in the
// list SHAPE. 
//
// jhrg 9/6/94

/* 
 * $Log: Array.h,v $
 * Revision 1.35  1998/01/12 14:27:54  tom
 * Second pass at class documentation.
 *
 * Revision 1.34  1997/12/18 15:06:09  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.33  1997/08/11 18:19:10  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.32  1997/03/08 19:01:54  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.31  1997/02/28 01:29:11  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.30  1996/08/13 16:44:32  jimg
 * Changed return type of add_constraint from void to bool. Now the member
 * function returns false when a bogus constraint (e.g.,bad array bounds)
 * are used.
 *
 * Revision 1.29  1996/06/22 00:13:34  jimg
 * Fixed a comment.
 *
 * Revision 1.28  1996/05/31 23:29:19  jimg
 * Updated copyright notice.
 *
 * Revision 1.27  1996/05/16 22:49:55  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.26  1996/05/06 21:14:06  jimg
 * Added dimension_start, _stop and _stride member functions to this class.
 * Changed the first argument of add_constraint from Pix &p to Pix p (the member
 * function does not change the Pix).
 *
 * Revision 1.25  1996/03/05 18:46:39  jimg
 * Replaced <limits.h> with "dods-limits.h".
 *
 * Revision 1.24  1996/02/02 00:30:58  jimg
 * Merge changes for DODS-1.1.0 into DODS-2.x
 *
 * Revision 1.23  1995/12/09  01:06:31  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.22  1995/12/06  21:37:55  jimg
 * Added members to record information about array index constraints.
 * Added mfuns to access/set those members.
 * Changed read from three to two arguments.
 *
 * Revision 1.21  1995/11/22  22:31:04  jimg
 * Modified so that the Vector class is now the parent class.
 *
 * Revision 1.20  1995/10/23  23:20:46  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.19  1995/08/26  00:31:23  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.18  1995/08/22  23:48:12  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.17.2.1  1995/09/14 20:59:52  jimg
 * Fixed declaration of, and calls to, _duplicate() by changing the formal
 * param from a pointer to a reference.
 *
 * Revision 1.17  1995/05/10  13:45:03  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.16  1995/04/28  19:53:45  reza
 * First try at adding constraints capability.
 * Enforce a new size calculated from constraint expression.
 *
 * Revision 1.15  1995/03/16  17:23:00  jimg
 * Added include of config_dap.h before all other includes.
 * Fixed deletes of buffers in read_val().
 * Added initialization of _buf in ctor.
 *
 * Revision 1.14  1995/03/04  14:34:53  jimg
 * Major modifications to the transmission and representation of values:
 * Added card() virtual function which is true for classes that
 * contain cardinal types (byte, int float, string).
 * Changed the representation of Str from the C rep to a C++
 * class represenation.
 * Chnaged read_val and store_val so that they take and return
 * types that are stored by the object (e.g., inthe case of Str
 * an URL, read_val returns a C++ String object).
 * Modified Array representations so that arrays of card()
 * objects are just that - no more storing strings, ... as
 * C would store them.
 * Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * an array of a structure is represented as an array of Structure
 * objects).
 *
 * Revision 1.13  1995/02/10  02:22:53  jimg
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
 * Revision 1.12  1995/01/19  21:59:08  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.11  1995/01/18  18:37:54  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.10  1995/01/11  15:54:40  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.9  1994/12/14  20:36:58  dan
 * Added dimensions() member function definition.
 * Removed alloc_buf() and free_buf() definitions.
 *
 * Revision 1.8  1994/12/14  17:55:23  dan
 * Removed dim() member function.
 *
 * Revision 1.7  1994/12/12  20:33:34  jimg
 * Fixed struct dimensions.
 *
 * Revision 1.6  1994/12/12  19:40:30  dan
 * Modified Array class definition removing inheritance from class CtorType
 * and to inherit directly from class BaseType.  Removed member function
 * dimensions().
 *
 * Revision 1.5  1994/12/09  21:36:34  jimg
 * Added support for named array dimensions.
 *
 * Revision 1.4  1994/11/22  14:05:22  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:43  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:31:37  jimg
 * Added check_semantics mfunc.
 * Added sanity checking for access to shape list (is it empty?).
 * Added cvs log listing to Array.cc.
 */

#ifndef _Array_h
#define _Array_h 1

#ifdef __GNUG__
#pragma interface
#endif

#ifdef NEVER
#include <limits.h>
#endif
#include <SLList.h>

#include "dods-limits.h"
#include "Vector.h"

const int DODS_MAX_ARRAY = DODS_UINT_MAX;

/** This class is used to hold arrays of other DODS data.  The
    elements of the array can be simple or compound data types.  There
    is no limit on the number of dimensions an array can have, or on
    the size of each dimension.

    If desired, the user can give each dimension of an array a name.
    You can, for example, have a 360x180 array of temperatures,
    covering the whole globe with one-degree squares.  In this case,
    you could name the first dimension ``Longitude'' and the second
    dimension ``Latitude''.  This can help prevent a great deal of
    confusion.  

    The Array is used as part of the Grid class, where the dimension
    names are crucial to its structure.  The dimension names
    correspond to ``Map'' vectors, holding the actual values for that
    column of the array.

    Each array dimension carries with it its own ``constraint''.  The
    constraint takes the form of three integers: the start, stop, and
    stride values.  This is clearest with an example.  Consider a
    one-dimensional array 10 elements long.  If the start value of the
    dimension constraint is 3, then the constrained array appears to
    be seven elements long.  If the stop value is changed to 7, then
    the array appears to be five elements long.  If the stride is
    changed to two, the array will appear to be 3 elements long.
    Array constraints are written as: #[start:stride:stop]#.

    \begin{verbatim}
    A = [1 2 3 4 5 6 7 8 9 10]

    A[3::] = [4 5 6 7 8 9 10]

    A[3::7] = [4 5 6 7 8]

    A[3:2:7] = [4 6 8]

    A[0:3:9] = [1 4 7 10]
    \end{verbatim}

    In addition to the constraint, array dimensions can be
    ``selected''.  Only selected array dimensions will be sent to the
    DODS client, and only those parts of the dimension referred to by
    the constraint will be sent.  The #add_constraint()# function is
    used to constrain and select a dimension. 

    @memo Holds multi-dimensional arrays.
    @see Grid
    @see List 
    */

class Array: public Vector {
private:
    struct dimension {		// each dimension has a size and a name
	int size;
	String name;
	int start, stop, stride;// a constraint determines these values
	int c_size;		// size of dimension once constrained
	bool selected;		// true if this dimension is selected
    };

    SLList<dimension> _shape;	// list of dimensions (i.e., the shape)

    unsigned int print_array(ostream &os, unsigned int index, 
			     unsigned int dims, unsigned int shape[]);
			     
protected:
    void _duplicate(const Array &a);
    
public:
  /** The Array constructor requires the name of the variable to be
      created, and the type of data the Array is to hold.  The name
      may be omitted, which will create a nameless variable.  The
      template pointer may also be omitted.  Note that if the template
      pointer is omitted when the Array is created, it {\it must} be
      added (with #add_var()#) before #read()# or #deserialize()# is
      called. 
      
      @param n A String containing the name of the variable to be
      created. 
      @param v A pointer to a variable of the type to be included 
      in the Array. 

      @memo The Array constructor.  */
    Array(const String &n = (char *)0, BaseType *v = 0);
  /** The Array copy constructor. */
    Array(const Array &rhs);
    virtual ~Array();

    const Array &operator=(const Array &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

  /** This function should read local data and fill in the data
      buffer.  When reading the data, the read function should use the
      constraint and selection information available for each
      dimension of the array to decide how much of the array to read.
      Only the values to be transmitted with #serialize()# must be
      read. 

      The implementation of this function is part of creating a new
      DODS server, and is left for the user.  For other details, refer
      to the description of the #read()# function in the BaseType
      class. 

      @memo Reads an array into the buffer.
      @see BaseType::read
      */
    virtual bool read(const String &dataset, int &error) = 0;


  // Changes the size of the array.  If the array exists, it is
  //    augmented by a factor of #size#.
    void update_length(int size);


  /** Given a size and a name, this function adds a dimension to the
      array.  For example, if the Array is already 10 elements long,
      calling #append_dim# with a size of 5 will transform the array
      into a 10x5 matrix.  Calling it again with a size of 2 will
      create a 10x5x2 array, and so on.

      @memo Adds a dimension to an array. 
      @param size The size of the desired new row.
      @param name The name of the new dimension.  This defaults to
      an empty string. 
      */
    void append_dim(int size, String name = "");

  /** Once a dimension has been created (see #append_dim()#), it can
      be ``constrained''.  This will make the array appear to the rest
      of the world to be smaller than it is.  This functions sets the
      constraint for a dimension, and marks that dimension
      ``selected''.

      @memo Adds a constraint to an Array dimension.  

      @param p An index (of type Pix) pointing to the dimension in the
      list of dimensions.
      @param start The start index of the constraint.
      @param stride The stride value of the constraint.
      @param stop The stop index of the constraint.
      @return TRUE on success, FALSE otherwise.  */
    bool add_constraint(Pix p, int start, int stride, int stop);

  /** Resets the constraint to select the entire dimension. */
    void reset_constraint();

  /** Clears the constraint and marks the dimension not selected. */
    void clear_constraint();

  /** Returns a pointer to the first dimension of the array. */    
    Pix first_dim();

  /** Given a dimension index, returns the index of the next
      dimension. */
    void next_dim(Pix &p);

  /** Returns the size of the dimension.  

      @param p The Pix index of the dimension.
      @param constrained If this parameter is TRUE, the function
      returns the constrained size of the array.  If the dimension is
      not selected, the function returns zero.  If it is FALSE, the
      function returns the dimension size whether or not the dimension
      is constrained.
      */
    int dimension_size(Pix p, bool constrained = false);

  /** Returns the start index of the constraint.

      @param p The Pix index of the dimension.
      @param constrained If this parameter is TRUE, the function
      returns the start index only if the dimension is selected.  If
      the dimension is not selected, the function returns zero.  If it
      is FALSE, the function returns the start index whether or not
      the dimension is constrained.
      */
    int dimension_start(Pix p, bool constrained = false);

  /** Returns the stop index of the constraint.

      @param p The Pix index of the dimension.
      @param constrained If this parameter is TRUE, the function
      returns the stop index only if the dimension is selected.  If
      the dimension is not selected, the function returns zero.  If it
      is FALSE, the function returns the stop index whether or not
      the dimension is constrained.
      */
    int dimension_stop(Pix p, bool constrained = false);

   /** Returns the stride value of the constraint.

      @param p The Pix index of the dimension.
      @param constrained If this parameter is TRUE, the function
      returns the stride value only if the dimension is selected.  If
      the dimension is not selected, the function returns zero.  If it
      is FALSE, the function returns the stride value whether or not
      the dimension is constrained.
      */
   int dimension_stride(Pix p, bool constrained = false);

  /** Returns the name of the specified dimension. */
    String dimension_name(Pix p);

  /** Returns the total number of dimensions in the array. */
    unsigned int dimensions(bool constrained = false);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(String &msg = String(), bool all = false);
};

#endif


