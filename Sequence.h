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

// Interface for the class Sequence. A sequence contains a single set
// of variables, all at the same lexical level just like a strucuture
// (and like a structure, it may contain other ctor types...). Unlike
// a structure, a sequence defines a pattern that is repeated N times
// for a sequence of N elements. Thus, Sequence { String name; Int32
// age; } person; means a sequence of N persons where each contain a
// name and age. The sequence can be arbitraily long (i.e., you don't
// know N by looking at the sequence declaration.
//
// jhrg 9/14/94

#ifndef _sequence_h
#define _sequence_h 1


#include <vector>
//#include "Pix.h"

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

/** The type BaseTypeRow is used to store single rows of values in an
    instance of Sequence. Values are stored in instances of BaseType. */
typedef vector<BaseType *> BaseTypeRow;

/** This is the interface for the class Sequence. A sequence contains
    a single set of variables, all at the same lexical level just like
    a Structure.  Like a Structure, a Sequence may contain other
    compound types, including other Sequences.  Unlike a Structure, a
    Sequence defines a pattern that is repeated N times for a sequence
    of N elements. It is useful to think of a Sequence as representing
    a table of values (like a relational database), with each row of
    the table corresponding to a Sequence ``instance.''  (This usage
    can be confusing, since ``instance'' also refers to a particular
    item of class Sequence.)  For example:

    <pre>
    Sequence {
      String name; 
      Int32 age; 
    } person; 
    </pre>

    This represents a Sequence of ``person'' records, each instance of
    which contains a name and an age:

    <pre>
    Fred       34
    Ralph      23
    Andrea     29
    ...      
    </pre>

    A Sequence can be arbitrarily long, which is to say that its
    length is not part of its declaration.  A Sequence can contain
    other Sequences:

    <pre>
    Sequence {
      String name;       
      Int32 age; 
      Sequence {
        String friend;
      } friend_list;
    } person; 
    </pre>

    This is still represented as a single table, but each row contains
    the elements of both the main Sequence and the nested one:

    <pre>
    Fred       34     Norman
    Fred       34     Andrea
    Fred       34     Ralph
    Fred       34     Lisa
    Ralph      23     Norman
    Ralph      23     Andrea
    Ralph      23     Lisa
    Ralph      23     Marth
    Ralph      23     Throckmorton
    Ralph      23     Helga
    Ralph      23     Millicent
    Andrea     29     Ralph
    Andrea     29     Natasha
    Andrea     29     Norman
    ...        ..     ...
    </pre>

    Internally, the Sequence is represented by a vector of vectors. The
    members of the outer vector are the members of the Sequence. This
    includes the nested Sequences, as in the above example.

    NB: Note that in the past this class had a different behavior. It held
    only one row at a time and the deserialize(...) method had to be called
    from within a loop. This is <i>no longer true</i>. Now the
    deserailize(...) method should be called once and will read the entire
    sequence's values from the server. All the values are now stored in an
    instance of Sequence, not just a single row's.

    Because the length of a Sequence is indeterminate, there are
    changes to the behavior of the functions to read this class of
    data.  The <tt>read()</tt> function for Sequence must be written so that
    successive calls return values for successive rows of the Sequence.

    Similar to a C structure, you refer to members of Sequence
    elements with a ``.'' notation.  For example, if the Sequence has
    a member Sequence called ``Tom'' and Tom has a member Float32
    called ``shoe_size'', you can refer to Tom's shoe size as
    ``Tom.shoe_size''.
    
    @todo Refactor along with Structure moving methods up into Constructor.
    
    @todo Add an isEmpty() method which returns true if the Sequence is
    empty. This should work before and after calling deserialize().

    @brief Holds a sequence. */

class Sequence: public Constructor {
private:
    // This holds the values read off the wire. Values are stored in
    // instances of BaseType objects.
    std::vector<BaseTypeRow *> d_values;

    // The number of the row that has just been deserialized. Before
    // deserialized has been called, this member is -1. 
    int d_row_number;

    // If a client asks for certain rows of a sequence using the bracket
    // notation (<tt>[<start>:<stride>:<stop>]</tt>) primarily intended for 
    // arrays
    // and grids, record that information in the next three members. This
    // information can be used by the translation software. s.a. the accessor
    // and mutator methods for these members. Values of -1 indicate that
    // these have not yet been set.
    int d_starting_row_number;
    int d_row_stride;
    int d_ending_row_number;

    // Used to track if data has not already been sent.
    bool d_unsent_data;
    
    // Track if the Start Of Instance marker has been written. Needed to 
    // properly send EOS for only the outer Sequence when a selection 
    // returns an empty Sequence.
    bool d_wrote_soi;
    
    // This signals whether the sequence is a leaf or parent.
    bool d_leaf_sequence;

    // In a hierarchy of sequences, is the the top most?
    bool d_top_most;
    
    void _duplicate(const Sequence &s);
    BaseType *m_leaf_match(const string &name, btp_stack *s = 0);
    BaseType *m_exact_match(const string &name, btp_stack *s = 0);

    bool is_end_of_rows(int i);

    bool serialize_parent_part_one(const string &dataset, DDS &dds, XDR *sink);
    void serialize_parent_part_two(const string &dataset, DDS &dds, XDR *sink);
    bool serialize_leaf(const string &dataset, DDS &dds, XDR *sink, 
			bool ce_eval);

public:

    Sequence(const string &n = "");

    Sequence(const Sequence &rhs);

    virtual ~Sequence();

    Sequence &operator=(const Sequence &rhs);

    virtual BaseType *ptr_duplicate();

    virtual string toString();

    virtual int element_count(bool leaves = false);

    virtual bool is_linear();
public:
    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);
    virtual void set_in_selection(bool state);

    virtual unsigned int width();

    virtual int length();
    
    virtual int number_of_rows();

    virtual bool read_row(int row, const string &dataset, DDS &dds, 
			  bool ce_eval = true);

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);

    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    /// Rest the row number counter
    void reset_row_number();
    
    int get_starting_row_number();

    virtual int get_row_stride();

    virtual int get_ending_row_number();

    virtual void set_row_number_constraint(int start, int stop, int stride = 1);

    /// Get the unsent data property
    bool get_unsent_data() { return d_unsent_data; }
    
    /// Set the unsent data property
    void set_unsent_data(bool usd) { d_unsent_data = usd; }
    
    // Move me!
    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual BaseType *var(const string &name, bool exact_match = true,
			  btp_stack *s = 0);
    virtual BaseType *var(const string &n, btp_stack &s);

    virtual BaseType *var_value(size_t row, const string &name);

    virtual BaseType *var_value(size_t row, size_t i);

    virtual BaseTypeRow *row_value(size_t row);

    virtual void add_var(BaseType *, Part part = nil);
    virtual void print_one_row(FILE *out, int row, string space,
			       bool print_row_num = false);
    virtual void print_val_by_rows(FILE *out, string space = "",
				   bool print_decl_p = true,
				   bool print_row_numbers = true);
    virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true);
    virtual void print_all_vals(FILE *out, XDR *src, DDS *dds, 
				string space = "", bool print_decl_p = true);

    virtual bool check_semantics(string &msg, bool all = false);

    virtual void set_leaf_p(bool state);

    virtual bool is_leaf_sequence();
    
    virtual void set_leaf_sequence(int lvl=1);
};

/* 
 * $Log: Sequence.h,v $
 * Revision 1.60  2005/04/07 22:32:47  jimg
 * Updated doxygen comments: fixed errors; updated comments about set_read_p.
 * Removed the VirtualCtor classes. Added a README about the factory
 * classes.
 *
 * Revision 1.59  2005/01/28 17:25:12  jimg
 * Resolved conflicts from merge with release-3-4-9
 *
 * Revision 1.53.2.8  2005/01/18 23:07:07  jimg
 * Added the d_wrote_soi and d_top_most fields.
 *
 * Revision 1.53.2.7  2005/01/14 21:08:22  jimg
 * Added unsent_data property and reset_row method.
 *
 * Revision 1.53.2.6  2005/01/10 23:22:31  jimg
 * Fixed a comment from the previous commit. Made the lvl param ofset_leaf_sequence() default to one.
 *
 * Revision 1.53.2.5  2005/01/10 02:39:34  jimg
 * I turned several protected methods into static functions since it looks * like they are never subclassed (and they certainly don't need access * to Sequence's state.
 *
 * Revision 1.53.2.4  2004/12/24 00:23:56  jimg
 * Rewrote serailze(). Untested code for nested sequence transmission.
 *
 * Revision 1.53.2.3  2004/12/23 20:57:31  dan
 * Added methods set_leaf_sequence(), is_leaf_sequence() and set_leaf_p()
 * to provide mechanism to identify the innermost nested Sequence as a leaf
 * node.
 *
 * Revision 1.58  2004/11/17 23:31:59  jimg
 * I made the var_value() methods (among others) virtual. This will allow
 * me to subclass them in the netCDF CL.
 *
 * Revision 1.57  2004/07/07 21:08:48  jimg
 * Merged with release-3-4-8FCS
 *
 * Revision 1.53.2.2  2004/07/02 20:41:52  jimg
 * Removed (commented) the pragma interface/implementation lines. See
 * the ChangeLog for more details. This fixes a build problem on HP/UX.
 *
 * Revision 1.56  2003/12/10 21:11:58  jimg
 * Merge with 3.4. Some of the files contains erros (some tests fail). See
 * the ChangeLog for information about fixes.
 *
 * Revision 1.55  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.53.2.1  2003/09/06 22:55:18  jimg
 * Added set_in_selection() method. Updated the documentation.
 *
 * Revision 1.54  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.53  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.51.2.2  2003/04/15 01:17:12  jimg
 * Added a method to get the iterator for a variable (or map) given its
 * index. To get the iterator for the ith variable/map, call
 * get_vars_iter(i).
 *
 * Revision 1.52  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.51.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.51  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.50  2003/01/10 19:46:40  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.45.4.16  2002/12/27 19:34:42  jimg
 * Modified the var() methods so that www2id() is called before looking
 * up identifier names. See bug 563.
 *
 * Revision 1.45.4.15  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.45.4.14  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.45.4.13  2002/09/22 14:20:01  rmorris
 * Changed all vector to std::vector as the using directive is no longer
 * cutting it in this case.
 *
 * Revision 1.45.4.12  2002/09/13 16:29:45  jimg
 * Repaired a bad doc comment.
 *
 * Revision 1.45.4.11  2002/09/12 22:49:58  pwest
 * Corrected signature changes made with Pix to IteratorAdapter changes. Rather
 * than taking a reference to a Pix, taking a Pix value.
 *
 * Revision 1.45.4.10  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.49  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.48  2002/06/03 21:53:38  jimg
 * Removed level stuff. The level() and set_level() methods were not being used
 * anymore, so I removed them.
 *
 * Revision 1.45.4.9  2002/05/22 16:57:51  jimg
 * I modified the `data type classes' so that they do not need to be
 * subclassed for clients. It might be the case that, for a complex client,
 * subclassing is still the best way to go, but you're not required to do
 * it anymore.
 *
 * Revision 1.45.4.8  2002/03/29 18:40:20  jimg
 * Updated comments and/or removed dead code.
 *
 * Revision 1.45.4.7  2002/03/01 21:03:08  jimg
 * Significant changes to the var(...) methods. These now take a btp_stack
 * pointer and are used by DDS::mark(...). The exact_match methods have also
 * been updated so that leaf variables which contain dots in their names
 * will be found. Note that constructor variables with dots in their names
 * will break the lookup routines unless the ctor is the last field in the
 * constraint expression. These changes were made to fix bug 330.
 *
 * Revision 1.45.4.6  2002/01/17 00:42:38  jimg
 * Fixed spelling errors in the doc++ comments.
 *
 * Revision 1.47  2001/09/28 17:50:07  jimg
 * Merged with 3.2.7.
 *
 * Revision 1.45.4.5  2001/09/26 23:25:42  jimg
 * Returned the set_level() and level() methods to the class.
 *
 * Revision 1.45.4.4  2001/09/25 20:29:44  jimg
 * Added is_linear().
 *
 * Revision 1.45.4.3  2001/09/07 00:38:35  jimg
 * Sequence::deserialize(...) now reads all the sequence values at once.
 * Its call semantics are the same as the other classes' versions. Values
 * are stored in the Sequence object using a vector<BaseType *> for each
 * row (those are themselves held in a vector). Three new accessor methods
 * have been added to Sequence (row_value() and two versions of var_value()).
 * BaseType::deserialize(...) now always returns true. This matches with the
 * expectations of most client code (the seqeunce version returned false
 * when it was done reading, but all the calls for sequences must be changed
 * anyway). If an XDR error is found, deserialize throws InternalErr.
 *
 * Revision 1.46  2001/06/15 23:49:02  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.45.4.2  2001/06/05 06:49:19  jimg
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
 * Revision 1.45.4.1  2001/05/12 00:05:26  jimg
 * Added toString().
 *
 * Revision 1.45  2000/09/22 02:17:21  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.44  2000/09/21 16:22:08  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.43  2000/09/11 16:31:33  jimg
 * Added methods to make it simpler to access Sequences by row number. The new
 * methods are: get_row(), get_row_number(), get_starting_row_number(),
 * get_ending_row_number(), get_row_stride(), set_row_number_constraint(). The
 * starting and ending row numbers refer to constraints placed on the sequence,
 * to get the number of rows in the current sequence, use the length() method.
 *
 * Revision 1.42  2000/08/16 00:37:54  jimg
 * Added d_row_number field and getRowNumber method.
 *
 * Revision 1.41  2000/08/02 22:46:49  jimg
 * Merged 3.1.8
 *
 * Revision 1.38.6.1  2000/08/02 21:10:07  jimg
 * Removed the header config.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.40  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.39  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.38.20.1  2000/06/02 18:29:31  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.38.14.1  2000/01/28 22:14:06  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.38  1999/05/04 19:47:22  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.37  1999/04/29 02:29:31  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.36  1998/09/17 17:17:48  jimg
 * Added leaf_match and exact_match.
 * Added two new versions of the var member function.
 *
 * Revision 1.35.6.1  1999/02/02 21:57:01  jimg
 * String to string version
 *
 * Revision 1.35  1998/03/17 17:40:08  jimg
 * Added an implementation of element_count().
 *
 * Revision 1.34  1998/02/19 19:41:49  jimg
 * Changed name of ...end_of_sequence to ...start_of_sequence since that is
 * now how it is used. I hope this will reduce confusion.
 * Changed the name of read_end_marker to read_marker (since they are not
 * always end markers anymore).
 *
 * Revision 1.33  1998/02/05 20:13:56  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.32  1998/02/04 14:55:32  tom
 * Another draft of documentation.
 *
 * Revision 1.31  1998/01/12 14:27:59  tom
 * Second pass at class documentation.
 *
 * Revision 1.30  1997/12/18 15:06:13  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.29  1997/10/09 22:19:23  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.28  1997/08/11 18:19:18  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.27  1997/07/15 21:54:12  jimg
 * See Sequence.cc for info on changes to the length member function.
 *
 * Revision 1.26  1997/03/08 19:02:07  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.25  1997/02/28 01:29:08  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.24  1996/09/24 19:13:23  jimg
 * Fixed conflict between two version of print_all_vals prototypes.
 *
 * Revision 1.23  1996/09/23 20:16:13  jimg
 * Fixed lame declaration of print_all_vals().
 *
 * Revision 1.22  1996/08/26 21:13:01  jimg
 * Changes for version 2.07
 *
 * Revision 1.21  1996/06/04 21:33:39  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.20  1996/05/31 23:29:59  jimg
 * Updated copyright notice.
 *
 * Revision 1.19  1996/05/29 22:08:47  jimg
 * Made changes necessary to support CEs that return the value of a function
 * instead of the value of a variable. This was done so that it would be
 * possible to translate Sequences into Arrays without first reading the
 * entire sequence over the network.
 *
 * Revision 1.18  1996/05/16 22:44:53  jimg
 * Dan's changes for 2.0.
 *
 * Revision 1.17  1996/03/05 17:43:49  jimg
 * Added ce_eval to serailize member function.
 *
 * Revision 1.16  1995/12/09  01:06:55  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.15  1995/12/06  21:56:30  jimg
 * Added `constrained' flag to print_decl.
 * Removed third parameter of read.
 * Modified print_decl() to print only those parts of a dataset that are
 * selected when `constrained' is true.
 *
 * Revision 1.14  1995/10/23  23:21:03  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.13  1995/08/26  00:31:44  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.12  1995/08/22  23:48:23  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.11  1995/05/10  13:45:28  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.10  1995/03/04  14:35:04  jimg
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
 * Revision 1.9  1995/02/10  02:23:01  jimg
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
 * Revision 1.8  1995/01/19  21:59:22  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.7  1995/01/18  18:40:25  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:54  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/15  21:21:54  dan
 * Modified class Sequence inheritance hierarchy, now directly inherits
 * from class BaseType.
 *
 * Revision 1.4  1994/11/22  14:06:04  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:50  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:48:32  jimg
 * Fixed some errors in comments.
 */

#endif //_sequence_h
