// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

#ifndef _dds_h
#define _dds_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <iostream>
#include <string>
#include <Pix.h>
#include <SLList.h>
#include <DLList.h>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _clause_h
#include "Clause.h"
#endif

#ifndef _expr_h
#include "expr.h"
#endif

#ifndef _rvalue_h
#include "RValue.h"
#endif

using std::cout;

/** The DODS Data Descriptor Object (DDS) is a data structure used by
    the DODS software to describe datasets and subsets of those
    datasets.  The DDS may be thought of as the declarations for the
    data structures that will hold data requested by some DODS client.
    Part of the job of a DODS server is to build a suitable DDS for a
    specific dataset and to send it to the client.  Depending on the
    data access API in use, this may involve reading part of the
    dataset and inferring the DDS.  Other APIs may require the server
    simply to read some ancillary data file with the DDS in it.
    \label{api:dds}

    On the server side, in addition to the data declarations, the DDS
    holds the clauses of any constraint expression that may have
    accompanied the data request from the DODS client.  The DDS object
    includes methods for modifying the DDS according to the given
    constraint expression.  It also has methods for directly modifying
    a DDS, and for transmitting it from a server to a client.

    For the client, the DDS object includes methods for reading the
    persistent form of the object sent from a server. This includes parsing
    the ASCII representation of the object and, possibly, reading data
    received from a server into a data object.

    Note that the class DDS is used to instantiate both DDS and DataDDS
    objects. A DDS that is empty (contains no actual data) is used by servers
    to send structural information to the client. The same DDS can becomes a
    DataDDS when data values are bound to the variables it defines.

    For a complete description of the DDS layout and protocol, please
    refer to <i>The DODS User Guide</i>. 
    
    The DDS has an ASCII representation, which is what is transmitted
    from a DODS server to a client.  Here is the DDS representation of
    an entire dataset containing a time series of worldwide grids of
    sea surface temperatures:

    <pre>
    Dataset {
        Float64 lat[lat = 180];
        Float64 lon[lon = 360];
        Float64 time[time = 404];
        Grid {
         ARRAY:
            Int32 sst[time = 404][lat = 180][lon = 360];
         MAPS:
            Float64 time[time = 404];
            Float64 lat[lat = 180];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    If the data request to this dataset includes a constraint
    expression, the corresponding DDS might be different.  For
    example, if the request was only for northern hemisphere data
    at a specific time, the above DDS might be modified to appear like
    this:

    <pre>
    Dataset {
        Grid {
         ARRAY:
            Int32 sst[time = 1][lat = 90][lon = 360];
         MAPS:
            Float64 time[time = 1];
            Float64 lat[lat = 90];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    Since the constraint has narrowed the area of interest, the range
    of latitude values has been halved, and there is only one time
    value in the returned array.  Note that the simple arrays (<tt>lat</tt>,
    <tt>lon</tt>, and <tt>time</tt>) described in the dataset are also
    part of the <tt>sst</tt> Grid object.  They can be requested by
    themselves or as part of that larger object.

    See the <i>The DODS User Guide</i>, or the documentation of the
    BaseType class for descriptions of the DODS data types.


    @see BaseType
    @see DAS */

class DDS {
private:
    // This struct is used to hold all the known `user defined' functions
    // (including those that are `built-in'). 
    struct function {
	string name;
	bool_func b_func;
	btp_func bt_func;
	proj_func p_func;

	function(const string &n, const bool_func f)
	    : name(n), b_func(f), bt_func(0), p_func(0) {}
	function(const string &n, const btp_func f)
	    : name(n), b_func(0), bt_func(f), p_func(0) {}
	function(const string &n, const proj_func f)
	    : name(n), b_func(0), bt_func(0), p_func(f) {}
	function(): name(""), b_func(0), bt_func(0), p_func(0) {}
    };

    string name;		// The dataset name

    string _filename;		// File name (or other OS identifier) for
				// dataset or part of dataset.

    DLList<BaseType *> vars;	// Variables at the top level 
    
    SLList<Clause *> expr;	// List of CE Clauses

    SLList<BaseType *> constants;// List of temporary objects

    SLList<function> functions; // Known external functions

protected:
    void duplicate(const DDS &dds);
    BaseType *leaf_match(const string &name, btp_stack *s = 0);
    BaseType *exact_match(const string &name, btp_stack *s = 0);

public:
  DDS(const string &n = "");

  DDS(const DDS &dds);

  virtual ~DDS();

  DDS & operator=(const DDS &rhs); 

  string get_dataset_name();

  void set_dataset_name(const string &n);

  string filename();
  void filename(const string &fn);

  void add_var(BaseType *bt);

  void del_var(const string &n);

  BaseType *var(const string &n);

  BaseType *var(const char *n);
  BaseType *var(const string &n, btp_stack &s);
  BaseType *var(const string &n, btp_stack *s = 0);
  BaseType *var(const char *n, btp_stack *s = 0);
  BaseType *var(Pix p);
 
  Pix first_var();

  void next_var(Pix &p);

  int num_var();

#if 0
    template <class FUNC_T> void add_function(const string &name, FUNC_T f);
#endif
#if 1
  void add_function(const string &name, bool_func f);
  void add_function(const string &name, btp_func f);
  void add_function(const string &name, proj_func f);
#endif

    bool find_function(const string &name, bool_func *f) const;
    bool find_function(const string &name, btp_func *f) const;
    bool find_function(const string &name, proj_func *f) const;


    /** These member functions are used to access and manipulate the
	constraint expression that may be part of a DDS.  Most of them
	are only used by the constraint expression parser.

	Refer to <i>The DODS User Manual</i> for a complete description
	of constraint expressions.

	@name Constraint Expression
	@see Clause */

    //@{
  Pix first_clause();
  void next_clause(Pix &p);
  Clause &clause(Pix p);
  bool clause_value(Pix p, const string &dataset);

  void append_clause(int op, rvalue *arg1, rvalue_list *arg2);
  void append_clause(bool_func func, rvalue_list *args);
  void append_clause(btp_func func, rvalue_list *args);

  bool functional_expression();
  bool boolean_expression();
  BaseType *eval_function(const string &dataset);

  bool eval_selection(const string &dataset);
    //@}   
    
    /** Parse a constraint expression.

	@name parse_constraint */

    //@{
    void parse_constraint(const string &constraint, ostream &os = cout,
			  bool server = true);

    void parse_constraint(const string &constraint, FILE *out,
			  bool server = true);
    //@}

  void append_constant(BaseType *btp);

    /** Read a DDS from a file.  This method calls a generated parser,
	<tt>ddsparse()</tt>, to interpret an ASCII representation of a DDS, and
	regenerate that DDS in memory.

	@name Read Methods
	@see Print Methods */

    //@{

  void parse(string fname);
  void parse(int fd);
  void parse(FILE *in=stdin);
    //@}

    /** These methods create an ASCII representation of the DDS. This is the
	form in which the DDS is transmitted to the client process. A DDS can
	be output entire, or subject to the constraint of a constraint
	expression. In most cases, a constrained DDS will be smaller than the
	original, since the purpose of the expression is to discard some of
	the data in the dataset.

	@name Print Methods
	@see Read Methods */

    //@{
  void print(ostream &os = cout);
  void print(FILE *out);
  void print_constrained(ostream &os = cout);
  void print_constrained(FILE *out);
    //@}

  bool send(const string &dataset, const string &constraint, FILE *out, 
	    bool compressed = true, const string &cgi_ver = "",
	    time_t lmt = 0);

  void mark_all(bool state);

  bool mark(const string &name, bool state);

  bool check_semantics(bool all = false);
};

// $Log: DDS.h,v $
// Revision 1.46  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.45  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.41.4.4  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.44  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.41.4.3  2001/09/25 20:33:26  jimg
// Removed debug.h
//
// Revision 1.43  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.41.4.2  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.42  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.41.4.1  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.41  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.40  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.39  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.34.2.4  2000/08/02 20:56:36  jimg
// Removed the symbol DVR from the send method's declaration. That removes the
// only dependence in this file on config_dap.h. Clients of this class can now
// included this header without also including config_dap.h.
//
// Revision 1.34.2.3  2000/07/19 19:01:56  jimg
// I made the dotr virtual! This was causing problems when request_data
// returned a DataDDS * but the object was assigned to a DDS * and later
// deleted. Since DDS:~DDS() was not virtual, the DataDDS dtor was not being
// run. 
//
// Revision 1.38  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.37  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.34.2.2  2000/06/14 17:00:53  jimg
// Changed vars from a SLList of BaseType *s to a DLList.
//
// Revision 1.36  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.35  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.34.14.1  2000/06/02 18:16:48  rmorris
// Mod's for port to Win32.
//
// Revision 1.34.2.1  2000/05/12 18:44:38  jimg
// Made the duplicate and *_match functions protected to simplfy future
// subclassing.
//
// Revision 1.34.8.1  2000/02/07 21:11:35  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.34  1999/07/22 18:47:44  tom
// fixed doc typo
//
// Revision 1.33  1999/07/22 17:11:50  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.32.8.1  1999/06/08 17:36:15  dan
// Replaced template declaration of add_function with 3 separate definitions
// for the possible instances of this method, bool*, BaseType*, void*.
//
// Revision 1.32  1999/05/05 00:51:02  jimg
// Added cgi_ver parameter to send() member function.
//
// Revision 1.31  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//
// Revision 1.30  1999/01/21 20:42:01  tom
// Fixed comment formatting problems for doc++
//
// Revision 1.29  1998/12/22 04:42:19  jimg
// Corrected some of the doc comments; fixed spelling and added error return
// values.
//
// Revision 1.28  1998/11/10 01:05:38  jimg
// This class now holds a list of Clause pointers, not objects.
//
// Revision 1.27  1998/10/21 16:40:13  jimg
// Added a proj_func member (for support of Projection Functions).
// Fixed up the doc++ comments.
//
// Revision 1.26  1998/09/17 17:20:44  jimg
// Added leaf_match and exact_match.
// Added two new versions of the var member function.
// Fixed documentation.
//
// Revision 1.25.4.1  1999/02/02 21:56:57  jimg
// String to string version
//
// Revision 1.25  1998/07/13 20:20:43  jimg
// Fixes from the final test of the new build process
//
// Revision 1.24  1998/03/20 00:12:41  jimg
// Added documentation for the new parse_constraint() member function.
//
// Revision 1.23  1998/02/04 14:55:31  tom
// Another draft of documentation.
//
// Revision 1.22  1998/01/12 14:27:56  tom
// Second pass at class documentation.
//
// Revision 1.21  1997/08/11 18:19:14  jimg
// Fixed comment leaders for new CVS version
//
// Revision 1.20  1997/03/05 08:35:03  jimg
// Added bool parameter `compressed' (defaults to true) to the send member
// function. See DDS.cc.
//
// Revision 1.19  1997/03/03 08:17:17  reza
// Changed default error object's output stream to cout. This will send it
// to the client side (versus the local server's log file, cerr).
//
// Revision 1.18  1996/12/03 00:14:58  jimg
// Added ostream and bool params to parse_constraint(). The bool parameter is
// used to tell the member function that it is running in the server of the
// client. The ostream is the sink for error objects (server side) or messages
// (client side).
//
// Revision 1.17  1996/12/02 23:14:54  jimg
// Added `filename' field and access functions. This field is for recording
// the filename associated with the dataset from which the DDS is generated.
// It does not actually have to be a filename; rather it is intended to be
// used by BaseType's read() member function when that code must access some
// OS controlled resource to get data for a particular variable. For most
// systems it will be a file, while for some systems it may be a RDB or
// blank.
//
// Revision 1.16  1996/06/04 21:33:20  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.15  1996/05/31 23:29:38  jimg
// Updated copyright notice.
//
// Revision 1.14  1996/05/29 22:08:37  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.13  1996/05/22 18:05:09  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.12  1996/04/05 00:21:28  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1996/04/04 18:41:07  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.10  1996/03/05 18:32:26  jimg
// Added the clause and function subclasses. Clause is used to hold a single
// clause of the current CE. Clause has ctors, a dtor (which is currently
// broken) and member function used to get the boolean value of the clause.
// Function is used to hold a single pointer to either a function returning a
// boolean or a BaseType *. The DDS class contains a list of clauses and a list
// of functions.
//
// Revision 1.9  1996/02/01 17:43:10  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.8  1995/12/09  01:06:39  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.7  1995/12/06  21:05:08  jimg
// Added print_constrained(): prints only those parts of the DDS that satisfy
// the constraint expression (projection + array selection).
// Added eval_constraint(): given the text of a constraint expression, evaluate
// it in the environment of the current DDS.
// Added mark*(): add the named variables to the current projection.
// Added send(): combines many functions like reading and serializing variables
// with constraint evaluation.
//
// Revision 1.6.2.2  1996/03/01 00:06:11  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.6.2.1  1996/02/23 21:37:26  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.6  1995/02/10  02:30:49  jimg
// Misc comment edits.
//
// Revision 1.5  1994/11/03  04:58:03  reza
// Added two overloading for function parse to make it consistent with DAS
// class. 
//
// Revision 1.4  1994/10/18  00:20:47  jimg
// Added copy ctor, dtor, duplicate, operator=.
// Added var() for const char * (to avoid confusion between char * and
// Pix (which is void *)).
// Switched to errmsg library.
// Added formatting to print().
//
// Revision 1.3  1994/09/23  14:42:23  jimg
// Added mfunc check_semantics().
// Replaced print mfunc stub with real code.
// Fixed some errors in comments.
//
// Revision 1.2  1994/09/15  21:09:00  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/08  21:09:42  jimg
// First version of the Dataset descriptor class. 

#endif // _dds_h
