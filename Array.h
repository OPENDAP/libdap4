
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Class for array variables. The dimensions of the array are stored in the
// list SHAPE. 
//
// jhrg 9/6/94

#ifndef _array_h
#define _array_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#include <SLList.h>

#ifndef _dods_limits_h
#include "dods-limits.h"
#endif

#ifndef _vector_h
#include "Vector.h"
#endif

const int DODS_MAX_ARRAY = DODS_INT_MAX;

/** This class is used to hold arrays of other DODS data. The elements of the
    array can be simple or compound data types. There is no limit on the
    number of dimensions an array can have, or on the size of each dimension.

    If desired, the user can give each dimension of an array a name. You can,
    for example, have a 360x180 array of temperatures, covering the whole
    globe with one-degree squares. In this case, you could name the first
    dimension ``Longitude'' and the second dimension ``Latitude''. This can
    help prevent a great deal of confusion.

    The Array is used as part of the Grid class, where the dimension names
    are crucial to its structure. The dimension names correspond to ``Map''
    vectors, holding the actual values for that column of the array.

    Each array dimension carries with it its own projection information. The
    projection inforamtion takes the form of three integers: the start, stop,
    and stride values. This is clearest with an example. Consider a
    one-dimensional array 10 elements long. If the start value of the
    dimension constraint is 3, then the constrained array appears to be seven
    elements long. If the stop value is changed to 7, then the array appears
    to be five elements long. If the stride is changed to two, the array will
    appear to be 3 elements long. Array constraints are written as:
    <b>[start:stride:stop]</b>.

    \verbatim
    A = [1 2 3 4 5 6 7 8 9 10]

    A[3::] = [4 5 6 7 8 9 10]

    A[3::7] = [4 5 6 7 8]

    A[3:2:7] = [4 6 8]

    A[0:3:9] = [1 4 7 10]
    \endverbatim

    \note{DODS uses zero-based indexing.}

    @brief A multidimensional array of identical data types.
    @see Grid
    @see List */

class Array: public Vector {
private:
    struct dimension {		// each dimension has a size and a name
	int size;
	string name;
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
    Array(const string &n = "", BaseType *v = 0);
    Array(const Array &rhs);
    virtual ~Array();

    Array &operator=(const Array &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

  /** This function should read local data and fill in the data
      buffer.  When reading the data, the read function should use the
      constraint and selection information available for each
      dimension of the array to decide how much of the array to read.
      Only the values to be transmitted with <tt>serialize()</tt> must be
      read. 

      The implementation of this function is part of creating a new
      DODS server, and is left for the user.  For other details, refer
      to the description of the <tt>read()</tt> function in the BaseType
      class. 

      @see BaseType::read
      */
    virtual bool read(const string &dataset) = 0;

    void update_length(int size);

    void append_dim(int size, string name = "");

    void add_constraint(Pix p, int start, int stride, int stop);

    void reset_constraint();

    void clear_constraint();

    Pix first_dim();

    void next_dim(Pix &p);

    int dimension_size(Pix p, bool constrained = false);

    int dimension_start(Pix p, bool constrained = false);

    int dimension_stop(Pix p, bool constrained = false);

   int dimension_stride(Pix p, bool constrained = false);

    string dimension_name(Pix p);

    unsigned int dimensions(bool constrained = false);

    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(string &msg, bool all = false);
};

/* 
 * $Log: Array.h,v $
 * Revision 1.52  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.51  2002/05/23 15:22:39  tom
 * modified for doxygen
 *
 * Revision 1.50  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.48.4.2  2001/09/25 20:37:16  jimg
 * Added to documentation strings.
 *
 * Revision 1.49  2001/06/15 23:49:01  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.48.4.1  2001/06/05 06:49:19  jimg
 * Added the Constructor class which is to Structures, Sequences and Grids
 * what Vector is to Arrays and Lists. This should be used in future
 * refactorings (I thought it was going to be used for the back pointers).
 * Introduced back pointers so children can refer to their parents in
 * hierarchies of variables.
 * Added to Sequence methods to tell if a child sequence is done
 * deserializing its data.
 * Fixed the operator=() and copy ctors; removed redundency from
 * _duplicate().
 * Changed the way serialize and deserialize work for sequences. Now SOI and
 * EOS markers are written for every `level' of a nested Sequence. This
 * should fixed nested Sequences. There is still considerable work to do
 * for these to work in all cases.
 *
 * Revision 1.48  2000/09/22 02:17:18  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.47  2000/09/21 16:22:07  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.46  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.45  2000/06/07 18:06:57  jimg
 * Merged the pc port branch
 *
 * Revision 1.44.20.1  2000/06/02 18:11:19  rmorris
 * Mod's for Port to Win32.
 *
 * Revision 1.44.14.1  2000/01/28 22:14:04  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.44  1999/05/04 19:47:20  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.43  1999/04/29 02:29:26  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.42  1999/01/21 20:42:00  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.41  1998/12/15 20:49:40  jimg
 * Added to the documentation on add_constraint().
 *
 * Revision 1.40.4.1  1999/02/02 21:56:55  jimg
 * String to string version
 *
 * Revision 1.40  1998/05/19 22:25:14  jimg
 * Fixed up some comments about reset_ and clear_constraint.
 *
 * Revision 1.39  1998/05/18 23:05:08  jimg
 * Fixed the documentation of update_length. This member function is
 * deprecated.
 *
 * Revision 1.38  1998/05/18 22:21:58  jimg
 * Removed part of the class doc comment about selection and projection since
 * they were not quite correct.
 *
 * Revision 1.37  1998/02/05 20:13:49  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.36  1998/02/04 14:55:30  tom
 * Another draft of documentation.
 *
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

#endif // _array_h
