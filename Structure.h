
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for the class Structure. A structure contains a single set of
// variables, all at the same lexical level. Of course, a structure may
// contain other structures... The variables contained in a structure are
// stored by instances of this class in a SLList of BaseType pointers.
//
// jhrg 9/14/94

#ifndef _structure_h
#define _structure_h 1

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

#ifndef _dds_h
#include "DDS.h"
#endif

#ifdef TRACE_NEW
#include "trace_new.h"
#endif


/** This data type is used to hold a collection of related data types,
    in a manner roughly corresponding to a C structure.  The member
    types can be simple or compound types, and can include other
    Structures. 

    The DODS structure is defined as a singly-linked list.  This means
    that Structure elements can be accessed either by name, with the
    <tt>var()</tt> function, or by their position in the list, either with
    the overloaded version of <tt>var()</tt>, or the combination of the
    <tt>first_var()</tt> and <tt>next_var()</tt> functions.

    The <tt>val2buf()</tt> and <tt>buf2val()</tt> functions only
    return the size of 
    the structure.  To read parts of a DODS Structure into an
    application program, use the <tt>buf2val()</tt> function of the element
    of the Structure in question. 

    Note that the predicate-setting functions <tt>set_send_p()</tt> and
    <tt>set_read_p()</tt> set their flags for the Structure as well as for
    each of the Structure's member elements.

    Similar to C, you can refer to members of Structure elements
    with a ``.'' notation.  For example, if the Structure has a member
    Structure called ``Tom'' and Tom has a member Float32 called
    ``shoe_size'', then you can refer to Tom's shoe size as
    ``Tom.shoe_size''. 
    
    @todo Refactor with Sequence moving methods up into Constructor.

    @brief Holds a structure (aggregate) type.
*/

class Structure: public Constructor {
private:
#if 0
    std::vector<BaseType *> _vars;
#endif
    
    void _duplicate(const Structure &s);
    BaseType *leaf_match(const string &name, btp_stack *s = 0);
    BaseType *exact_match(const string &name, btp_stack *s = 0);

public:
    Structure(const string &n = "");

#if 0
    typedef std::vector<BaseType *>::const_iterator Vars_citer ;
    typedef std::vector<BaseType *>::iterator Vars_iter ;
#endif

    Structure(const Structure &rhs);
    virtual ~Structure();

    Structure &operator=(const Structure &rhs);
    virtual BaseType *ptr_duplicate();

    virtual int element_count(bool leaves = false);
    virtual bool is_linear();

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual unsigned int width();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    // Do not store values in memory as for C; force users to work with the
    // C++ objects as defined by the DAP.

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual BaseType *var(const string &name, bool exact_match = true,
			  btp_stack *s = 0);

    virtual BaseType *var(const string &name, btp_stack &s);

    virtual void add_var(BaseType *bt, Part part = nil);

#if 0
    Pix first_var();

    void next_var(Pix p);

    BaseType *var(Pix p);
#endif

#if 0
    /** Returns an iterator referencing the first structure element. */
    Vars_iter var_begin() ;

    /** Returns an iterator referencing the end of the list of structure
	elements. Does not reference the last structure element. */
    Vars_iter var_end() ;

    Vars_iter get_vars_iter(int i);
#endif

#if 0
    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_decl(FILE *out, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);
#endif

    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true);

    virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true);

    /** Prints the Structure and all elements of any Sequences contained
	within. 
	@deprecated
	@see print_all_vals(FILE *out, XDR *src, DDS *dds, string space, bool print_decl_p);
    */
    virtual void print_all_vals(ostream& os, XDR *src, DDS *dds,
				string space = "", bool print_decl_p = true);

    /** Prints the Structure and all elements of any Sequences contained
	within. 
	@deprecated
	@see Sequence::print_all_vals
    */
    virtual void print_all_vals(FILE *out, XDR *src, DDS *dds,
				string space = "", bool print_decl_p = true);

    virtual bool check_semantics(string &msg, bool all = false);
};

/* 
 * $Log: Structure.h,v $
 * Revision 1.46  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.45  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.43.2.2  2003/04/15 01:17:12  jimg
 * Added a method to get the iterator for a variable (or map) given its
 * index. To get the iterator for the ith variable/map, call
 * get_vars_iter(i).
 *
 * Revision 1.44  2003/02/21 00:14:25  jimg
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
 * Revision 1.37.4.13  2002/12/27 19:34:42  jimg
 * Modified the var() methods so that www2id() is called before looking
 * up identifier names. See bug 563.
 *
 * Revision 1.37.4.12  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.37.4.11  2002/10/29 22:21:01  pwest
 * added operator== and operator!= operators to IteratorAdapter and
 * IteratorAdapterT classes to handle Pix == Pix use.
 *
 * Revision 1.37.4.10  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.37.4.9  2002/10/18 22:47:52  jimg
 * Resolved conflicts with Rob's changes
 *
 * Revision 1.37.4.8  2002/09/22 14:21:11  rmorris
 * Changed 'vector' to 'std::vector', as the using directive is no longer
 * cutting it for VC++ in this case.  Made on of the iterator typedef's
 * public as it is used external to this class.
 *
 * Revision 1.37.4.7  2002/09/12 22:49:58  pwest
 * Corrected signature changes made with Pix to IteratorAdapter changes. Rather
 * than taking a reference to a Pix, taking a Pix value.
 *
 * Revision 1.37.4.6  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.37.4.5  2002/08/08 06:54:57  jimg
 * Changes for thread-safety. In many cases I found ugly places at the
 * tops of files while looking for globals, et c., and I fixed them up
 * (hopefully making them easier to read, ...). Only the files RCReader.cc
 * and usage.cc actually use pthreads synchronization functions. In other
 * cases I removed static objects where they were used for supposed
 * improvements in efficiency which had never actually been verifiied (and
 * which looked dubious).
 *
 * Revision 1.41  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.40  2002/06/03 22:21:15  jimg
 * Merged with release-3-2-9
 *
 * Revision 1.37.4.4  2002/05/22 16:57:51  jimg
 * I modified the `data type classes' so that they do not need to be
 * subclassed for clients. It might be the case that, for a complex client,
 * subclassing is still the best way to go, but you're not required to do
 * it anymore.
 *
 * Revision 1.37.4.3  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.39  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.37.4.2  2001/09/25 20:27:31  jimg
 * Added is_linear().
 *
 * Revision 1.38  2001/06/15 23:49:02  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.37.4.1  2001/06/05 06:49:19  jimg
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
 * Revision 1.37  2000/09/22 02:17:21  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.36  2000/09/21 16:22:08  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.35  2000/08/02 22:46:49  jimg
 * Merged 3.1.8
 *
 * Revision 1.32.6.1  2000/08/02 21:10:07  jimg
 * Removed the header config_dap.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.34  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.33  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.32.20.1  2000/06/02 18:29:31  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.32.14.1  2000/01/28 22:14:06  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.32  1999/05/04 19:47:22  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.31  1999/04/29 02:29:31  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.30  1998/09/17 17:07:15  jimg
 * Fixes to the documentation.
 * Added leaf_match and exact_match mfuncs (support for the new var member
 * function).
 *
 * Revision 1.29.6.1  1999/02/02 21:57:02  jimg
 * String to string version
 *
 * Revision 1.29  1998/04/03 17:44:27  jimg
 * Patch from Jake Hamby. Added print_all_vals member function.
 *
 * Revision 1.28  1998/03/17 17:50:52  jimg
 * Added an implementation of element_count().
 *
 * Revision 1.27  1998/02/05 20:13:57  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.26  1998/01/12 14:28:00  tom
 * Second pass at class documentation.
 *
 * Revision 1.25  1997/12/18 15:06:13  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.24  1997/10/09 22:19:25  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.23  1997/08/11 18:19:20  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.22  1997/03/08 19:02:10  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.21  1997/02/28 01:29:10  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.20  1996/06/04 21:33:47  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.19  1996/05/31 23:30:06  jimg
 * Updated copyright notice.
 *
 * Revision 1.18  1996/05/16 22:50:08  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.17  1996/03/05 17:32:37  jimg
 * Added ce_eval to serailize member function.
 *
 * Revision 1.16  1995/12/09  01:07:01  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.15  1995/12/06  21:56:33  jimg
 * Added `constrained' flag to print_decl.
 * Removed third parameter of read.
 * Modified print_decl() to print only those parts of a dataset that are
 * selected when `constrained' is true.
 *
 * Revision 1.14  1995/10/23  23:21:05  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.13  1995/08/26  00:31:50  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.12  1995/08/22  23:48:22  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.11  1995/05/10  13:45:32  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.10  1995/03/04  14:35:07  jimg
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
 * Revision 1.9  1995/02/10  02:22:58  jimg
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
 * Revision 1.8  1995/01/19  21:59:26  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.7  1995/01/18  18:39:58  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:51  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/16  15:17:15  dan
 * Removed inheritance from class CtorType, now directly inherits
 * from class BaseType.
 *
 * Revision 1.4  1994/11/22  14:06:13  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:48  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:45:27  jimg
 * Added mfunc check_semantics().
 * Added sanity checking on the variable list (is it empty?).
 */

#endif // _structure_h
