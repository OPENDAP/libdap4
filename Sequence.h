// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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

#ifdef _GNUG_
#pragma interface
#endif

#if 0

#ifdef WIN32
#include <rpc.h>
#include <winsock.h>
#include <xdr.h>
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include <SLList.h>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#if defined(TRACE_NEW) && !defined(_trace_new_h)
#include "trace_new.h"
#endif


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

    \begin{verbatim}
    Sequence {
      String name; 
      Int32 age; 
    } person; 
    \end{verbatim}

    This represents a Sequence of ``person'' records, each instance of
    which contains a name and an age:

    \begin{verbatim}
    Fred       34
    Ralph      23
    Andrea     29
    ...      
    \end{verbatim}

    A Sequence can be arbitrarily long, which is to say that its
    length is not part of its declaration.  A Sequence can contain
    other Sequences:

    \begin{verbatim}
    Sequence {
      String name;       
      Int32 age; 
      Sequence {
        String friend;
      } friend_list;
    } person; 
    \end{verbatim}

    This is still represented as a single table, but each row contains
    the elements of both the main Sequence and the nested one:

    \begin{verbatim}
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
    \end{verbatim}

    Internally, the Sequence is represented by a singly-linked list.
    The members of this list are the members of a single Sequence
    instance: one element in the series.  This includes the nested
    Sequences, as in the above example.  The list does {\it not}
    contain all the elements of the Sequence.  You can think of it as
    containing a single row.

    Each Sequence also carries a ``level'' number.  This can be used
    to good effect with some data access APIs (such as JGOFS) to
    record the level of nested Sequences.  The top-level Sequence is
    Level 0, and any Sequences it contains would be Level 1.  Any
    Sequences contained by those Level 1 Sequences would be at Level
    2, and so on.  The class contains read and set functions for this
    datum. 

    Because the length of a Sequence is indeterminate, there are
    changes to the behavior of the functions to read this class of
    data.  The #read()# function for a Sequence is designed to be
    called repeatedly, each time returning the next item in the
    Sequence. Similarly, the #deserialize()# function is here meant to
    be called repeatedly until the end of the Sequence is reached.

    Similar to a C structure, you refer to members of Sequence
    elements with a ``.'' notation.  For example, if the Sequence has
    a member Sequence called ``Tom'' and Tom has a member Float32
    called ``shoe\_size'', you can refer to Tom's shoe size as
    ``Tom.shoe\_size''.
    
    @memo Holds a sequence. */

class Sequence: public BaseType {
private:
  // Linked list of variables in this sequence.
  SLList<BaseType *> _vars;

  // Level number in a multilevel sequence.
  int _level;			

  // Was there an error reading the sequence?
  bool _seq_read_error;

  // Was there an error writing the sequence?
  bool _seq_write_error;

  // The number of the row that has just been deserialized. Before
  // deserialized has been called, this member is -1. 
  int d_row_number;

  // If a client asks for certain rows of a sequence using the bracket
  // notation (#[<start>:<stride>:<stop>]#) primarily intended for arrays and
  // grids, record that information in the next three members. This
  // information can be used by the translation software. s.a. the accessor
  // and mutator methods for these members.
  int d_starting_row_number;
  int d_row_stride;
  int d_ending_row_number;

  // Make sure the old deserialize is still around.
  bool old_deserialize(XDR *source, DDS *dds, bool reuse = false);

  void _duplicate(const Sequence &s);
  BaseType *leaf_match(const string &name);
  BaseType *exact_match(const string &name);

  bool is_end_of_rows(int i);

protected:
  void write_end_of_sequence(XDR *sink);
  void write_start_of_instance(XDR *sink);
  unsigned char read_marker(XDR *source);
  bool is_start_of_instance(unsigned char marker);
  bool is_end_of_sequence(unsigned char marker);
	
public:
  /** The Sequence constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The Sequence constructor.
  */
  Sequence(const string &n = "");
  /** The Sequence copy constructor. */
  Sequence(const Sequence &rhs);
  virtual ~Sequence();

  const Sequence &operator=(const Sequence &rhs);
  virtual BaseType *ptr_duplicate() = 0;

  virtual int element_count(bool leaves = false);

  virtual void set_send_p(bool state);
  virtual void set_read_p(bool state);

  virtual unsigned int width();

  /** Returns the number of elements in a Sequence object. Note that
      this is {\it not} the number of items in a row, but the number
      of rows in the complete sequence object. To be meaningful, this
      must be computed after constraint expresseion (CE) evaluation.
      The purpose of this function is to facilitate translations
      between Sequence objects and Array objects, particularly when
      the Sequence is too large to be transferred from the server to
      the client in its entirety.

      This function, to be useful, must be specialized for the API and
      data format in use.  

      @return The base implentation returns -1, indicating that the
      length is not known.  Sub-classes specific to a particular API
      will have a more complete implementation.
  */
  virtual int length();
    
  /** Sets the level number. */
  virtual void set_level(int lvl);
  /** Returns the level number. */
  virtual int level();

  /** Read row number #row# into the Sequence. The values of the row are
      obtained using the members of the sequence. This method calls the
      overloaded Sequence::read() method to read each row. The rows are
      counted using by the object (see get_row_number()). If a selection
      expression has been supplied, rows are counted only if they satisfy
      that expression.

      NB: The first row is row number zero. A Sequence with 100 rows will
      have row numbers 0 to 99.

      @param row The row number to read.
      @param dataset A string, often a file name, used to refer to t he
      dataset. 
      @param dds A reference to the DDS for this dataset.
      @param ce_eval If True, evaluate any CE, otherwise do not.
      @return True if there are more rows to read, False if the EOF was
      found. */
  virtual bool get_row(int row, const string &dataset, DDS &dds, 
		       bool ce_eval = true);

  virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			 bool ce_eval = true);

  /** Deserialize (read from the network) one instance of the current
      sequence. The information read is sorted into the sequence instance
      variables. 

      @memo Read one Sequence instance from the network. 

      @return TRUE if more instances remain to be read, FALSE if this
      is the last instance {\it or} if there was an error reading the
      instance. In the latter case, check the value of
      #seq_read_error()#. */
  virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

  /** Rows in a Sequence are numbered 0 to N for a Sequence with N+1 rows.
      This method returns the number of the current row (the row that has
      just been deserialized). If deserialize() has not yet been called, this
      method returns -1.

      @memo Get the current row number of the Sequence.
      @return The current row number f the Sequence. */
  int get_row_number();

  /** Return the starting row number if the sequence was constrained using
      row numbers (instead of, or in addition to, a relational constraint).
      If a relational constraint was also given, the row number corresponds
      to the row number of the sequence \emph{after} applying the relational
      constraint.

      If the bracket notation was not used to constrain this sequence, this
      method returns -1.

      @memo Get the starting row number.
      @return The starting row number. */
  int get_starting_row_number();

  /** Return the row stride number if the sequence was constrained using
      row numbers (instead of, or in addition to, a relational constraint).
      If a relational constraint was also given, the row stride is applied to
      the sequence \emph{after} applying the relational constraint.

      If the bracket notation was not used to constrain this sequence, this
      method returns -1.

      @memo Get the row stride.
      @return The row stride. */
  int get_row_stride();

  /** Return the ending row number if the sequence was constrained using row
      numbers (instead of, or in addition to, a relational constraint). If a
      relational constraint was also given, the row number corresponds to the
      row number of the sequence \emph{after} applying the relational
      constraint.

      If the bracket notation was not used to constrain this sequence, this
      method returns -1.

      @memo Get the ending row number.
      @return The ending row number. */
  int get_ending_row_number();

  /** Set the start, stop and stride for a row-number type constraint.
      This should be used only when the sequence is constrained using the
      bracket notation (which supplies start, stride and stop information).

      @param start The starting row number. The first row is row zero.
      @param stop The eding row number. The 20th row is row 19.
      @param stride The stride. A stride of two skips every other row. */
  void set_row_number_constraint(int start, int stop, int stride = 1);

  /** Was there an error reading the sequence?  This function is
      called after a call to #deserialize()# returns FALSE, implying
      {\it either} an error condition or the end of the Sequence.  The
      return from this function clears up the difference. 

      @memo Check for error or end of Sequence.
      @return This function will return TRUE if either the xdr
      function returned an error or the end of instance/sequence
      marker could not be read. If FALSE, the read error indicates the
      end of the sequence. */
  bool seq_read_error();

  /** Reads a single instance of a Sequence.
      @return TRUE on success, FALSE on failure, {\it or} the end of
      the Sequence. */
  virtual bool read(const string &dataset) = 0;

  virtual unsigned int val2buf(void *buf, bool reuse = false);
  virtual unsigned int buf2val(void **val);

  virtual BaseType *var(const string &name, bool exact_match = true);
  virtual BaseType *var(const string &name, btp_stack &s);

  /** Adds a variable to the Sequence.  Remember that if you wish to
      add a member to a nested Sequence, you must use the #add_var()#
      of that Sequence.  This means that variable names need not be
      unique among a set of nested Sequences. */ 
  virtual void add_var(BaseType *, Part p = nil);

  /** Returns an index to the first variable in a Sequence instance.
      This corresponds to the item in the first column of the table
      the Sequence represents.  It is not the first row of the table. 
  */
  Pix first_var();
  /** Increments the Sequence instance.  This returns a pointer to the
      next ``column'' in the Sequence, not the next row. */
  void next_var(Pix &p);
  /** Returns a pointer to a Sequence member.  This may be another
      Sequence. */
  BaseType *var(Pix p);

  virtual void print_decl(ostream &os, string space = "    ",
			  bool print_semi = true,
			  bool constraint_info = false,
			  bool constrained = false);

  /** Prints the first instance of the Sequence. 

      @see Sequence::print_all_vals
  */
  virtual void print_val(ostream &os, string space = "",
			 bool print_decl_p = true);

  /** Prints a formatted version of an entire Sequence (all rows, all
      columns), including nested Sequences.  This is meant to be used
      on the client side of a DODS connection, and the source of the
      Sequence data to be printed is specified with an XDR pointer. 

      @memo Print the entire sequence.
      @param os The output stream on which to print the Sequence.
      @param src The external source from which the data is to come.
      This is passed to #deserialize()#.
      @param dds The Data Descriptor Structure object corresponding to
      this dataset.  See {\it The DODS User Manual} for information
      about this structure.  This would have been received from the
      server in an earlier transmission.
      @param space The leading spaces of the output.
      @param print_del_p If TRUE, prints the declaration of the
      Sequence as well as its data.
  */
  virtual void print_all_vals(ostream& os, XDR *src, DDS *dds, 
			      string space = "", bool print_decl_p = true);

  virtual bool check_semantics(string &msg, bool all = false);
};

/* 
 * $Log: Sequence.h,v $
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
 * Removed the header config_dap.h. If this file uses the dods typedefs for
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
 * `config_dap.h' so that other libraries could have header files which were
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
