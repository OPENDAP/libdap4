
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

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

#include "BaseType.h"
#include "DDS.h"

#if 0
#include "config_dap.h"
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
    #var()# function, or by their position in the list, either with
    the overloaded version of #var()#, or the combination of the
    #first_var()# and #next_var()# functions.

    The #val2buf()# and #buf2val()# functions only return the size of
    the structure.  To read parts of a DODS Structure into an
    application program, use the #buf2val()# function of the element
    of the Structure in question. 

    Note that the predicate-setting functions #set_send_p()# and
    #set_read_p()# set their flags for the Structure as well as for
    each of the Structure's member elements.

    Similar to C, you can refer to members of Structure elements
    with a ``.'' notation.  For example, if the Structure has a member
    Structure called ``Tom'' and Tom has a member Float32 called
    ``shoe\_size'', then you can refer to Tom's shoe size as
    ``Tom.shoe\_size''. 
    
    @memo Holds a structure (aggregate) type. */

class Structure: public BaseType {
private:
    SLList<BaseType *> _vars;
    
    void _duplicate(const Structure &s);
    BaseType *leaf_match(const string &name);
    BaseType *exact_match(const string &name);

public:
    /** The Structure constructor requires only the name of the variable
	to be created.  The name may be omitted, which will create a
	nameless variable.  This may be adequate for some applications. 
      
	@param n A string containing the name of the variable to be
	created. 

	@memo The Structure constructor. */
    Structure(const string &n = "");

    /** The Structure copy constructor. */
    Structure(const Structure &rhs);
    virtual ~Structure();

    const Structure &operator=(const Structure &rhs);
    virtual BaseType *ptr_duplicate() = 0;

    virtual int element_count(bool leaves = false);

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual unsigned int width();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset) = 0;

    // Do not store values in memory as for C; force users to work with the
    // C++ objects as defined by the DAP.

    /** Returns the size of the structure. */
    virtual unsigned int val2buf(void *val, bool reuse = false);
    /** Returns the size of the structure. */
    virtual unsigned int buf2val(void **val);

    /** Returns a pointer to the specified Structure element. */
    virtual BaseType *var(const string &name, bool exact_match = true);

    virtual BaseType *var(const string &name, btp_stack &s);

    /** Adds an element to a Structure. */
    virtual void add_var(BaseType *bt, Part p = nil);

    /** Returns the pseudo-index (Pix) of the first structure element. */
    Pix first_var();

    /** Increments the input index to point to the next element in the
	structure. */
    void next_var(Pix &p);

    /** Returns a pointer to the {\it p}th element. */
    BaseType *var(Pix p);

    virtual void print_decl(ostream &os, string space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true);

    /** Prints the Structure and all elements of any Sequences contained
	within. 
	@see Sequence::print_all_vals
    */
    virtual void print_all_vals(ostream& os, XDR *src, DDS *dds,
				string space = "", bool print_decl_p = true);

    virtual bool check_semantics(string &msg, bool all = false);
};

/* 
 * $Log: Structure.h,v $
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
