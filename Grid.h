
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

/* 
 * $Log: Grid.h,v $
 * Revision 1.32  1999/04/29 02:29:30  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.31  1998/09/17 17:19:42  jimg
 * Added leaf_match and exact_match.
 * Added two new versions of the var member function.
 *
 * Revision 1.30  1998/08/31 21:47:31  jimg
 * Changed check_semantics so that the array and map vectors must of composed
 * of simple-type elements.
 *
 * Revision 1.29.6.1  1999/02/02 21:56:59  jimg
 * String to string version
 *
 * Revision 1.29  1998/03/17 17:32:36  jimg
 * Added an implementation of element_count().
 *
 * Revision 1.28  1998/02/05 20:13:54  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.27  1998/01/12 14:27:58  tom
 * Second pass at class documentation.
 *
 * Revision 1.26  1997/12/18 15:06:11  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.25  1997/10/09 22:19:21  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.24  1997/08/11 18:19:16  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.23  1997/06/05 23:16:13  jimg
 * Added components() and projection_yields_grid() mfuncs.
 *
 * Revision 1.22  1997/03/08 19:02:03  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.21  1997/02/28 01:29:05  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.20  1996/06/04 21:33:33  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.19  1996/05/31 23:29:48  jimg
 * Updated copyright notice.
 *
 * Revision 1.18  1996/05/16 22:50:02  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.17  1996/03/05 18:08:59  jimg
 * Added ce_eval to serailize member function.
 *
 * Revision 1.16  1995/12/09  01:06:47  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.15  1995/12/06  21:56:26  jimg
 * Added `constrained' flag to print_decl.
 * Removed third parameter of read.
 * Modified print_decl() to print only those parts of a dataset that are
 * selected when `constrained' is true.
 *
 * Revision 1.14  1995/10/23  23:20:57  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.13  1995/08/26  00:31:34  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.12  1995/08/22  23:48:25  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.11  1995/05/10  13:45:19  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.10  1995/03/04  14:35:01  jimg
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
 * Revision 1.9  1995/02/10  02:23:06  jimg
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
 * Revision 1.8  1995/01/19  21:59:17  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.7  1995/01/18  18:40:08  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:48  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/15  21:25:45  dan
 * Added print_val() member function.
 *
 * Revision 1.4  1994/11/22  14:05:57  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:55  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:45:29  jimg
 * Added mfunc check_semantics().
 * Added sanity checking on the variable list (is it empty?).
 */

#ifndef _Grid_h
#define _Grid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include <SLList.h>
#include <Pix.h>

#include "BaseType.h"
#include "config_dap.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

/** The Grid data type is a collection of an Array and a set of ``Map''
    vectors.  The Map vectors are one-dimensional arrays corresponding
    to each dimension of the central Array.  Using this scheme, a Grid
    can represent, in a rectilinear array, data which is not in
    reality rectilinear.  An example will help make it clear.

    Assume that the following array contains measurements of some real
    quantity, conducted at nine different points in space:

    \begin{verbatim}
    A = [ 1  2  3  4 ]
        [ 2  4  6  8 ]
        [ 3  6  9  12]
    \end{verbatim}

    To locate this Array in the real world, we could note the location
    of one corner of the grid, and the grid spacing.  This would allow
    us to calculate the location of any of the other points of the
    Array. 

    This approach will not work, however, unless the grid spacing is
    precisely regular.  If the distance between Row 1 and Row 2 is not
    the same as the distance between Row 2 and Row 3, the scheme will
    break down.  The solution is to equip the Array with two Map
    vectors that define the location of each row or column of the
    array:

    \begin{verbatim}
         A = [ 1  2  3  4 ] Row = [ 0 ]
             [ 2  4  6  8 ]       [ 3 ]
             [ 3  6  9  12]       [ 8 ]

    Column = [ 0  2  8  27]
    \end{verbatim}

    The real location of the point in the first row and column of the
    array is now exactly fixed at (0,0), and the point in the last row
    and last column is at (8,27). 

    The Grid data type has two parts: an Array, and a singly-linked
    list of Map vectors to describe the Array.  The access functions
    for this class include a function to return the Array
    (#array_var()#), and a set of functions for cycling through the
    list of Map vectors.

    @memo Holds the Grid data type.
    @see Array
    */

class Grid: public BaseType {
private:
    BaseType *_array_var;
    SLList<BaseTypePtr> _map_vars;
    
    void _duplicate(const Grid &s);

public:
  /** The Grid constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The Grid constructor.
      */
    Grid(const string &n = "");
  /** The Grid copy constructor. */
    Grid(const Grid &rhs);
    virtual ~Grid();
    
    const Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate() = 0;

    virtual int element_count(bool leaves = false);

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    /** Note the paramter #exact_match# is not used by this mfunc.

	@see BaseType.h */
    virtual BaseType *var(const string &name, bool exact_match = true);

    virtual BaseType *var(const string &name, btp_stack &s);

    virtual void add_var(BaseType *bt, Part part);

  /** Returns the Grid Array. */
    BaseType *array_var();

  /** Returns the index of the first Map vector. */
    Pix first_map_var();
  /** Increments the Map vector index. */
    void next_map_var(Pix &p);
  /** Given an index, returns the corresponding Map vector. */
    BaseType *map_var(Pix p);

    virtual unsigned int width();

  /** Returns the number of components in the Grid object.  This is
      equal to one plus the number of Map vectors.  If there is a
      constraint expression in effect, the number of dimensions needed
      may be smaller than the actual number in the stored data.  (Or
      the Array might not even be requested.) In this case, a user can
      request the smaller number with the {\it constrained} flag.

      @memo Returns the number of components in the Grid object. 
      @return The number of components in the Grid object.
      @param constrained If TRUE, the function returns the number of
      components contained in the constrained Grid.  Since a
      constraint expression might well eliminate one or more of the
      Grid dimensions, this number can be lower than the actual number
      of components.  If this parameter is FALSE (the default), the
      actual number of components will be returned.  */
    virtual int components(bool constrained = false);

  /** Returns TRUE if the current projection will yield a Grid that
      will pass the #check_semantics()# function. A Grid that, when
      projected, will not pass the #check_semantics()# function must
      be sent as either a Structure of Arrays or a single Array
      depending on the projection.

      The function first checks to see whether the Array is present.
      Then, for each dimension in the Array part, the function checks
      the corresponding Map vector to make sure it is present in the
      projected Grid. If for each projected dimension in the Array
      component, there is a matching Map vector, then the Grid is
      valid.

      @return TRUE if the projected grid is still a Grid.  FALSE
      otherwise. 
      */
    virtual bool projection_yields_grid();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset, int &error) = 0;

  /** Returns the size of the Grid type.  Use the #val2buf()#
      functions of the member elements to insert values into the Grid
      buffer. */
    virtual unsigned int val2buf(void *buf, bool reuse = false);
  /** Returns the size of the Grid type.  Use the #buf2val()#
      functions of the member elements to read values from the Grid
      buffer. */
    virtual unsigned int buf2val(void **val);

  /** If the projected Grid is not a valid grid, this function will
      convert the declaration to an Array or Structure, whichever
      seems more appropriate.

      @memo Prints the Grid declaration only if a valid Grid.
      @see Array
      @see Structure
      */
    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true);

    /** Return true if this Grid is well formed. The array dimensions and
	number of map vectors must match and both the array and maps must be
	of simple-type elements. */
    virtual bool check_semantics(string &msg, bool all = false);
};

#endif

