// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

// $Log: DDS.h,v $
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

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"
#include "Clause.h"
#include "expr.h"
#include "debug.h"

class DDS {
private:
    // This struct is used to hold all the known `user defined' functions
    // (including those that are `built-in'). 
    struct function {
	String name;
	bool_func b_func;
	btp_func bt_func;

	function(const String &n, const bool_func f)
	    : name(n), b_func(f), bt_func(0) {}
	function(const String &n, const btp_func f)
	    : name(n), b_func(0), bt_func(f) {}
	function(): name(""), b_func(0), bt_func(0) {}
    };

    String name;		// The dataset name

    String _filename;		// File name (or other OS identifier) for
				// dataset or part of dataset.

    SLList<BaseTypePtr> vars;	// Variables at the top level 
    
    SLList<Clause> expr;	// List of CE Clauses

    SLList<BaseTypePtr> constants;// List of temporary objects

    SLList<function> functions; // Known external functions

    void duplicate(const DDS &dds);

public:
    DDS(const String &n = (char *)0);
    DDS(const DDS &dds);
    ~DDS();

    DDS & operator=(const DDS &rhs);

    String get_dataset_name();
    void set_dataset_name(const String &n);

    /// Get the dataset's filename.
    String filename();

    /// Set the dataset's filename.
    void filename(const String &fn);

    void add_var(BaseType *bt);
    void del_var(const String &n);
    BaseType *var(const String &n);
    BaseType *var(const char *n); // to avoid cast of char * to Pix.

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);
    int num_var();

    // Interface to the parsed expression
    Pix first_clause();
    void next_clause(Pix &p);
    Clause &clause(Pix p);
    bool clause_value(Pix p, const String &dataset);

    void append_clause(int op, rvalue *arg1, rvalue_list *arg2);
    void append_clause(bool_func func, rvalue_list *args);
    void append_clause(btp_func func, rvalue_list *args);

    // DDS maintains a list of BaseType *s for all the constants that the
    // expr parser generates. These objects can be deleted when we are done
    // with the DDS.
    void append_constant(BaseType *btp);

    // manipulate the FUNCTIONS member.
    void add_function(const String &name, bool_func f);
    void add_function(const String &name, btp_func f);
    bool find_function(const String &name, bool_func *f) const;
    bool find_function(const String &name, btp_func *f) const;

    /// Does the current constraint expression return a BaseType pointer?
    bool functional_expression();

    /// Evaluate a function-valued CE.
    BaseType *eval_function(const String &dataset);

    /// Does the current constraint expression return a boolean value?
    bool boolean_expression();

    /// Evaluate a boolean-valued CE.
    bool eval_selection(const String &dataset);

    // evaluate the projectons
    void mark_all(bool state);
    bool mark(const String &name, bool state);

    // Interface to the parser
    bool parse(String fname);
    bool parse(int fd);
    bool parse(FILE *in=stdin);

    // Print the entire DDS.
    bool print(ostream &os = cout);
    bool print(FILE *out);

    // Print only those parts of the DDS marked for transmission after
    // evaluating a constraint expression. I.E., print the current projection.
    bool print_constrained(ostream &os = cout);
    bool print_constrained(FILE *out);

    // Check the semantics of the variables in the DDS. if ALL is true,
    // recursively descend aggregate variables.
    bool check_semantics(bool all = false);

    /// Parse a constraint expression .
    /** Parse the constraint expression #constraint# given the current
        #dds#. If #server# is true, then don't display error objects, rather
        send then back to the client for display. Error objects/messages are
        written to #os#.

        Returns true is the constraint expression parses without error, 
	otherwise false. */
    bool parse_constraint(const String &constraint, ostream &os = cout,
			  bool server = true);

    // Send variable(s) described by the constraint expression CONSTRIANT
    // from DATASET. if FLUSH is true, flush the output buffer upon
    // completion. Use OUT as the output buffer if not null, otherwise use
    // STDOUT. This mfunc uses eval_constraint(), BaseType::read() and
    // BaseType::serailize() as well as other mfuncs.
    bool send(const String &dataset, const String &constraint, FILE *out, 
	      bool compressed = true);
};

#endif
