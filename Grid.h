
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
 
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

#ifndef _grid_h
#define _grid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <vector>

#include "Pix.h"

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

#if defined(TRACE_NEW) && !defined(_trace_new_h)
#include "trace_new.h"
#endif

/** The Grid data type is a collection of an Array and a set of ``Map''
    vectors.  The Map vectors are one-dimensional arrays corresponding
    to each dimension of the central Array.  Using this scheme, a Grid
    can represent, in a rectilinear array, data which is not in
    reality rectilinear.  An example will help make it clear.

    Assume that the following array contains measurements of some real
    quantity, conducted at nine different points in space:

    <pre>
    A = [ 1  2  3  4 ]
        [ 2  4  6  8 ]
        [ 3  6  9  12]
    </pre>

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

    <pre>
         A = [ 1  2  3  4 ] Row = [ 0 ]
             [ 2  4  6  8 ]       [ 3 ]
             [ 3  6  9  12]       [ 8 ]

    Column = [ 0  2  8  27]
    </pre>

    The real location of the point in the first row and column of the
    array is now exactly fixed at (0,0), and the point in the last row
    and last column is at (8,27). 

    The Grid data type has two parts: an Array, and a singly-linked
    list of Map vectors to describe the Array.  The access functions
    for this class include a function to return the Array
    (<tt>array_var()</tt>), and a set of functions for cycling through the
    list of Map vectors.

    @brief Holds the Grid data type.
    @see Array
    */

class Grid: public Constructor {
private:
    BaseType *_array_var;
    std::vector<BaseType *> _map_vars;
    
    void _duplicate(const Grid &s);

public:

    Grid(const string &n = "");
    Grid(const Grid &rhs);
    virtual ~Grid();

    typedef std::vector<BaseType *>::const_iterator Map_citer ;
    typedef std::vector<BaseType *>::iterator Map_iter ;

    
    Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate();

    virtual int element_count(bool leaves = false);

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual BaseType *var(const string &name, bool exact_match = true,
			  btp_stack *s = 0);

    virtual BaseType *var(const string &name, btp_stack &s);

    virtual void add_var(BaseType *bt, Part part);

    BaseType *array_var();

    Pix first_map_var();
    void next_map_var(Pix p);
    BaseType *map_var(Pix p);

    virtual unsigned int width();

    virtual int components(bool constrained = false);

    virtual bool projection_yields_grid();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

#if 0
    virtual bool read(const string &dataset);
#endif

    virtual unsigned int val2buf(void *buf, bool reuse = false);

    virtual unsigned int buf2val(void **val);

    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_decl(FILE *out, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true);

     virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true);

   virtual bool check_semantics(string &msg, bool all = false);
    /** Returns an iterator referencing the first Map vector. */
    Map_iter map_begin() ;

    /** Returns an iterator referencing the end of the list of Map vectors.
        It does not reference the last Map vector */
    Map_iter map_end() ;

    Map_iter get_map_iter(int i);
};

/* 
 * $Log: Grid.h,v $
 * Revision 1.45  2003/04/22 19:40:27  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.43.2.2  2003/04/15 01:17:12  jimg
 * Added a method to get the iterator for a variable (or map) given its
 * index. To get the iterator for the ith variable/map, call
 * get_vars_iter(i).
 *
 * Revision 1.44  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.43.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.43  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.42  2003/01/10 19:46:40  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.38.4.10  2002/12/27 19:34:42  jimg
 * Modified the var() methods so that www2id() is called before looking
 * up identifier names. See bug 563.
 *
 * Revision 1.38.4.9  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.38.4.8  2002/11/18 18:51:59  jimg
 * Changed the include of Pix.h from #include <Pix.h> to "Pix.h" to fix
 * a problem with the dependencies (see today's check in of Makefile.in).
 *
 * Revision 1.38.4.7  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.38.4.6  2002/09/22 14:15:43  rmorris
 * Changed the use of vector to std::vector.  The 'using' directive for VC++
 * no longer cut it in this case.
 *
 * Revision 1.38.4.5  2002/09/12 22:49:57  pwest
 * Corrected signature changes made with Pix to IteratorAdapter changes. Rather
 * than taking a reference to a Pix, taking a Pix value.
 *
 * Revision 1.38.4.4  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.41  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.40  2002/06/03 22:21:15  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.38.4.3  2002/05/22 16:57:51  jimg
 * I modified the `data type classes' so that they do not need to be
 * subclassed for clients. It might be the case that, for a complex client,
 * subclassing is still the best way to go, but you're not required to do
 * it anymore.
 *
 * Revision 1.38.4.2  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.39  2001/06/15 23:49:02  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.38.4.1  2001/06/05 06:49:19  jimg
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
 * Revision 1.38  2000/09/22 02:17:20  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.37  2000/09/21 16:22:08  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.36  2000/08/02 22:46:49  jimg
 * Merged 3.1.8
 *
 * Revision 1.33.6.1  2000/08/02 21:10:07  jimg
 * Removed the header config_dap.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.35  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.34  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.33.20.1  2000/06/02 18:21:27  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.33.14.1  2000/01/28 22:14:05  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.33  1999/05/04 19:47:21  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
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

#endif // _grid_h

