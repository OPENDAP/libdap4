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

// $Log: DDS.h,v $
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

#ifndef _DDS_h
#define _DDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include "config_dap.h"

#include <iostream>
#include <string>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"
#include "Clause.h"
#include "expr.h"
#include "RValue.h"
#include "debug.h"

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
    refer to {\it The DODS User Guide}. 
    
    The DDS has an ASCII representation, which is what is transmitted
    from a DODS server to a client.  Here is the DDS representation of
    an entire dataset containing a time series of worldwide grids of
    sea surface temperatures:

    \begin{verbatim}
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
    \end{verbatim}

    If the data request to this dataset includes a constraint
    expression, the corresponding DDS might be different.  For
    example, if the request was only for northern hemisphere data
    at a specific time, the above DDS might be modified to appear like
    this:

    \begin{verbatim}
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
    \end{verbatim}

    Since the constraint has narrowed the area of interest, the range
    of latitude values has been halved, and there is only one time
    value in the returned array.  Note that the simple arrays (#lat#,
    #lon#, and #time#) described in the dataset are also part of the
    #sst# Grid object.  They can be requested by themselves or as part
    of that larger object.

    See the {\it The DODS User Guide}, or the documentation of the
    BaseType class for descriptions of the DODS data types.


    @memo Holds a DODS Data Descriptor Structure.
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

    SLList<BaseType *> vars;	// Variables at the top level 
    
    SLList<Clause *> expr;	// List of CE Clauses

    SLList<BaseType *> constants;// List of temporary objects

    SLList<function> functions; // Known external functions

    void duplicate(const DDS &dds);
    BaseType *leaf_match(const string &name);
    BaseType *exact_match(const string &name);

public:
    /** Creates a DDS with the given string for its name. */
    DDS(const string &n = "");

    /** The DDS copy constructor. */
    DDS(const DDS &dds);

    ~DDS();

    DDS & operator=(const DDS &rhs); 

    /** Get and set the dataset's name.  This is the name of the dataset
	itself, and is not to be confused with the name of the file or
	disk on which it is stored.

	@name Dataset Name Accessors */

    //@{
      
    /** Returns the dataset's name. */
    string get_dataset_name();

    /** Sets the dataset name. */
    void set_dataset_name(const string &n);
    //@}

    /** Get and set the dataset's filename. This is the physical
	location on a disk where the dataset exists.  The dataset name
	is simply a title.

	@name File Name Accessor
	@see Dataset Name Accessors */

    //@{
    /** Gets the dataset file name. */
    string filename();
    /** Set the dataset's filename. */
    void filename(const string &fn);
    //@}

    /** Methods for manipulating the DDS contents. 

	@memo Methods for manipulating the variables in a DDS.
	@name Variable Methods */

    //@{

    /** Adds a variable to the DDS. */
    void add_var(BaseType *bt);

    /** Removes a variable from the DDS. */
    void del_var(const string &n);

    /** Returns a pointer to a variable from the DDS. 

	@name var() */

    //@{
    /** Returns a pointer to the named variable. If the name contains one or
	more field separators then the function looks for a variable whose
	name matches exactly. If the name contains no field separators then
	the mfunc looks first in the top level and then in all subsequent
	levels and returns the first occurrence found. In general, this
	function searches constructor types in the order in which they appear
	in the DDS, but there is no requirement that it do so. 

	\note{If a dataset contains two constructor types which have field
	names that are the same (say point.x and pair.x) you should always
	use fully qualified names to get each of those variables.}

	@return A pointer to the variable or null if not found. */
    BaseType *var(const string &n);

    /** Returns a pointer to the named variable.
	@return A pointer to the variable or null if not found. */
    BaseType *var(const char *n);

    /** Search for for variable {\it n} as above but record all
	compound type variables which ultimately contain {\it n} on
	{\it s}. This stack can then be used to mark the contained
	compound-type variables as part of the current projection.

	@return A BaseType pointer to the variable {\it n} or 0 if {\it n}
	could not be found. */
    BaseType *var(const string &n, btp_stack &s);

    /** Returns a pointer to the indicated variable. */
    BaseType *var(Pix p);
    //@}

    /** Returns the first variable in the DDS. */
    Pix first_var();

    /** Increments the DDS variable counter to point at the next
	variable. */
    void next_var(Pix &p);

    /** Returns the number of variables in the DDS. */
    int num_var();
    //@} This is the end of the `Methods for manip...' subsection.

    /** Each DDS carries with it a list of external functions it can use to
	evaluate a constraint expression. If a constraint contains any of
	these functions, the entries in the list allow the parser to evaluate
	it. The functions are of two types: those that return boolean values,
	and those that return real (also called BaseType) values.

	These methods are used to manipulate this list of known
	external functions.

	@name External Function Accessors
	@memo Manipulate the list of external functions.  */

    //@{
    /** Add a function to the list. */
    template <class FUNC_T> void add_function(const string &name, FUNC_T f);

    /** Find a Boolean function with a given name in the function list. */
    bool find_function(const string &name, bool_func *f) const;

    /** Find a BaseType function with a given name in the function list. */
    bool find_function(const string &name, btp_func *f) const;

    /** Find a projection function with a given name in the function list. */
    bool find_function(const string &name, proj_func *f) const;

    //@}

    /** These member functions are used to access and manipulate the
	constraint expression that may be part of a DDS.  Most of them
	are only used by the constraint expression parser.

	Refer to {\it The DODS User Manual} for a complete description
	of constraint expressions.

	@name Constraint Expression
	@memo Constraint expression manipulators and accessors.
	@see Clause */

    //@{
    /** Returns a pointer to the first clause in a parsed constraint
	expression. */
    Pix first_clause();
    /** Increments a pointer to indicate the next clause in a parsed
	constraint expression. */
    void next_clause(Pix &p);
    /** Returns a clause of a parsed constraint expression. */
    Clause &clause(Pix p);

    /** Returns the value of the indicated clause of a constraint
	expression. */
    bool clause_value(Pix p, const string &dataset);

    /** Adds a clause to a constraint expression.

	@name append\_clause()
    */

    //@{
    /** This function adds an operator clause to the constraint
	expression. 

	@memo Adds an operator clause to the constraint expression.
	@param op An integer indicating the operator in use.  These
	values are generated by {\tt bison}.
	@param arg1 A pointer to the argument on the left side of the
	operator. 
	@param arg2 A pointer to a list of the arguments on the right
	side of the operator.
    */

    void append_clause(int op, rvalue *arg1, rvalue_list *arg2);
    /** This function adds a boolean function clause to the constraint
	expression. 

	@memo Adds a boolean function clause to the constraint
	expression. 
	@param func A pointer to a boolean function from the list of
	supported functions.
	@param args A list of arguments to that function.
	@see External Function Accessors */
    void append_clause(bool_func func, rvalue_list *args);

    /** This function adds a real-valued (BaseType) function clause to
	the constraint expression.

	@memo Adds a real-valued function clause to the constraint
	expression. 
	@param func A pointer to a boolean function from the list of
	supported functions.
	@param args A list of arguments to that function.
	@see External Function Accessors */
    void append_clause(btp_func func, rvalue_list *args);
    //@}

    /** Does the current constraint expression return a BaseType
	pointer? */
    bool functional_expression();

    /** Does the current constraint expression return a boolean value? */ 
    bool boolean_expression();

    /** Evaluate a function-valued constraint expression. */
    BaseType *eval_function(const string &dataset);

    /** Evaluate a boolean-valued constraint expression. */
    bool eval_selection(const string &dataset);
    
    /** Parse a constraint expression.

	@name parse\_constraint */

    //@{
    /** Parse the constraint expression given the current DDS. 

	@memo Parse a constraint expression .  
	@return Returns true if the constraint expression parses
	without error, otherwise false.
	@param constraint A string containing the constraint
	expression. 
	@param os The output stream on which to write error objects and
	messages. 
	@param server If true, send errors back to client instead of
	displaying errors on the default output stream. */ 
    bool parse_constraint(const string &constraint, ostream &os = cout,
			  bool server = true);

    /** @param out A FILE pointer to which error objects should be wrtten. */
    bool parse_constraint(const string &constraint, FILE *out,
			  bool server = true);
    //@}

    /** The DDS maintains a list of BaseType pointers for all the constants
	that the constraint expression parser generates. These objects are
	deleted by the DDS destructor. Note that there are no list accessors;
	these constants are never accessed from the list. The list is simply
	a convenient way to make sure the constants are disposed of properly.

	@memo Append a constant to the list of constants generated by
	the constraint expression parser. */
    void append_constant(BaseType *btp);

    /** Read a DDS from a file.  This method calls a generated parser,
	#ddsparse()#, to interpret an ASCII representation of a DDS, and
	regenerate that DDS in memory.

	@memo Read a DDS from an external source.
	@name Read Methods
	@see Print Methods */

    //@{

    /** Parse a DDS from a file with the given name. */
    bool parse(string fname);

    /** Parse a DDS from a file indicated by the input file descriptor. */
    bool parse(int fd);

    /** Parse a DDS from a file indicated by the input file descriptor. */
    bool parse(FILE *in=stdin);
    //@}

    /** These methods create an ASCII representation of the DDS. This is the
	form in which the DDS is transmitted to the client process. A DDS can
	be output entire, or subject to the constraint of a constraint
	expression. In most cases, a constrained DDS will be smaller than the
	original, since the purpose of the expression is to discard some of
	the data in the dataset.

	@name Print Methods
	@memo Creates an ASCII representation of the DDS.
	@see Read Methods */

    //@{
    /** Print the entire DDS on the specified output stream. */
    bool print(ostream &os = cout);
    /** Print the entire DDS to the specified file. */
    bool print(FILE *out);

    /** Print the constrained DDS to the specified file. */
    bool print_constrained(ostream &os = cout);
    /** Print a constrained DDS to the specified file. */
    bool print_constrained(FILE *out);
    //@}

    /** This function sends the variables described in the constrained DDS to
	the output described by the FILE pointer. This function calls
	#parse_constraint()#, #BaseType::read()#, and
	#BaseType::serialize()#.

	@memo Sends the data described by the DDS to a client.
	@return True if successful.
	@param dataset The name of the dataset to send.
	@param constraint A string containing the entire constraint
	expression received in the original data request.
	@param out A pointer to the output buffer for the data.
	@param compressed If true, send compressed data.
	@see parse_constraint
	@see BaseType::read
	@see BaseType::serialize */
    bool send(const string &dataset, const string &constraint, FILE *out, 
	      bool compressed = true);

    /** Mark the member variable #send_p# flags to {\it state}. */
    void mark_all(bool state);

    /** Mark the #send_p# flag of the named variable to {\it state}. */
    bool mark(const string &name, bool state);

    /** Check the semantics of each of the variables represented in the
	DDS. 

	@memo Check member variable semantics.
	@return True if all the members are correct.
	@param all If true, recursively check the individual members of
	compound variables.
	@see BaseType::check_semantics */
    bool check_semantics(bool all = false);
};

#endif
