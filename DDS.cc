
// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 9/7/94

// $Log: DDS.cc,v $
// Revision 1.40  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.39  1999/01/21 02:57:02  jimg
// Added ce_function.h and call to add the projection function
// `grid_selection_func' to the set of CE functions all servers know about.
//
// Revision 1.38  1999/01/13 16:59:05  jimg
// Removed call to text_to_temp() (which copied a string to a temp file so that
// the file could be parsed) with code that feeds the string directly into the
// parser/scanner.
//
// Revision 1.37  1998/11/10 01:08:06  jimg
// Changed code; now uses a list of Clause pointers instead of using a list of
// Clause objects. This makes it simpler for the projection functions to add
// `invisible' selection clauses.
//
// Revision 1.36  1998/10/21 16:38:12  jimg
// The find_function() member function now checks for the name AND the function
// type before returning a value. This means that a bool and BaseType * function
// may have the same name but, because of their different types, still work
// properly in context.
//
// Revision 1.35  1998/09/17 17:21:27  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
// Added exact_match and leaf_match.
//
// Revision 1.34  1998/03/19 23:36:58  jimg
// Fixed calls to set_mime_*().
// Removed old code (that was surrounded by #if 0 ... #endif).
// Added a version of parse_constraint(...) that works with FILE *
// Completely hacked send(...). It now takes care of setting up the compression
// sub process.
// Removed `compressed' flag from parse_constraint(...).
//
// Revision 1.33  1998/02/11 21:57:12  jimg
// Changed x_gzip to deflate. See Connect.cc/.h
//
// Revision 1.32  1997/04/15 18:02:45  jimg
// Added optional argument to print_variable functions so that the variable can
// be printed using the current constraint. Changed the call to print_variable()
// in DDS::send() so that the constrained variable is printed.
//
// Revision 1.31  1997/03/08 19:03:38  jimg
// Changed call to `unique()' to `unique_names()' (see util.cc).
//
// Revision 1.30  1997/03/05 08:12:18  jimg
// Added calls to set_mime_binary() in DDS::send().
//
// Revision 1.29  1997/02/28 01:30:17  jimg
// Corrected call to unique() in check_semantics() (added new String &msg
// parameter).
//
// Revision 1.28  1996/12/03 00:20:18  jimg
// Added ostream and bool parameters to parse_constraint(). If the bool param
// is true the the code assumes it is being run in the server. In that case
// error objects are not evaluated but instead are serialized and set to the
// client via the ostream.
//
// Revision 1.27  1996/12/02 23:15:43  jimg
// Added `filename' field and access functions.
//
// Revision 1.26  1996/11/27 22:40:19  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.25  1996/11/13 19:23:07  jimg
// Fixed debugging.
//
// Revision 1.24  1996/08/13 18:07:48  jimg
// The parser (dds.y) is now called using the parser_arg object.
// the member function eval_function() now returns a NULL BaseType * when the
// function in the CE does not exist.
//
// Revision 1.23  1996/06/04 21:33:19  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.22  1996/05/31 23:29:37  jimg
// Updated copyright notice.
//
// Revision 1.21  1996/05/29 22:08:35  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.20  1996/05/22 18:05:08  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.19  1996/05/14 15:38:20  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.18  1996/04/04 19:15:14  jimg
// Merged changes from version 1.1.1.
// Fixed bug in send() - wrong number of arguments to serialize.
//
// Revision 1.17  1996/03/05 18:38:45  jimg
// Moved many of the DDS member functions into the subclasses clause and
// function. Also, because the rvalue and func_rvalue classes (defined in
// expr.h ane expr.cc) were expanded, most of the evaluation software has been
// removed.
// Unnecessary accessor member functions have been removed since clause and
// function now have their own ctors.
//
// Revision 1.16  1996/02/01 17:43:08  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.15  1995/12/09  01:06:38  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.14  1995/12/06  21:11:24  jimg
// Added print_constrained(): Prints a constrained DDS.
// Added eval_constraint(): Evaluates a constraint expression in the
// environment of the current DDS.
// Added send(): combines reading, serailizing and constraint evaluation.
// Added mark(): used to mark variables as part of the current projection.
// Fixed some of the parse() and print() mfuncs to take uniform parameter types
// (ostream and FILE *).
// Fixed the constructors to work with const objects.
//
// Revision 1.13  1995/10/23  23:20:50  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.12  1995/08/23  00:06:30  jimg
// Changed from old mfuncs to new(er) ones.
//
// Revision 1.11.2.2  1996/03/01 00:06:09  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.11.2.1  1996/02/23 21:37:24  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.11  1995/07/09  21:28:55  jimg
// Added copyright notice.
//
// Revision 1.10  1995/05/10  13:45:13  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.9  1994/12/09  21:37:24  jimg
// Added <unistd.h> to the include files.
//
// Revision 1.8  1994/12/07  21:23:16  jimg
// Removed config
//
// Revision 1.7  1994/11/22  14:05:40  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.6  1994/11/03  04:58:02  reza
// Added two overloading for function parse to make it consistent with DAS
// class. 
//
// Revision 1.5  1994/10/18  00:20:46  jimg
// Added copy ctor, dtor, duplicate, operator=.
// Added var() for const char * (to avoid confusion between char * and
// Pix (which is void *)).
// Switched to errmsg library.
// Added formatting to print().
//
// Revision 1.4  1994/10/05  16:34:14  jimg
// Fixed bug in the parse function(s): the bison generated parser returns
// 1 on error, 0 on success, but parse() was not checking for this.
// Instead it returned the value of bison's parser function.
// Changed types of `status' in print and parser functions from int to bool.
//
// Revision 1.3  1994/09/23  14:42:22  jimg
// Added mfunc check_semantics().
// Replaced print mfunc stub with real code.
// Fixed some errors in comments.
//
// Revision 1.2  1994/09/15  21:08:39  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/08  21:09:40  jimg
// First version of the Dataset descriptor class.
// 

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: DDS.cc,v 1.40 1999/03/24 23:37:14 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include <iostream.h>
#include <stdiostream.h>

#include "expr.h"
#include "Clause.h"
#include "Connect.h"
#include "DDS.h"
#include "Error.h"
#include "parser.h"
#include "debug.h"
#include "util.h"
#include "ce_functions.h"
#include "cgi_util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void ddsrestart(FILE *yyin);	// Defined in dds.tab.c
int ddsparse(void *arg);

void exprrestart(FILE *yyin);	// Defined in expr.tab.c
int exprparse(void *arg);

// Glue routines declared in expr.lex
void expr_switch_to_buffer(void *new_buffer);
void expr_delete_buffer(void * buffer);
void *expr_string(const char *yy_str);

// Copy the stuff in DDS to THIS. The mfunc returns void because THIS gets
// the `result' of the mfunc.
//
// NB: This can't define the formal param to be const since SLList<>first()
// (which is what DDS::first_var() calls) does not define THIS to be const.

void
DDS::duplicate(const DDS &dds)
{
    name = dds.name;

    DDS &dds_tmp = (DDS &)dds;	// cast away const

    // copy the things pointed to by the list, not just the pointers
    for (Pix src = dds_tmp.first_var(); src; dds_tmp.next_var(src)) {
	BaseType *btp = dds_tmp.var(src)->ptr_duplicate();
	add_var(btp);
    }
}

DDS::DDS(const String &n) : name(n)
{
    add_function("member", func_member); // See util.cc for func_* definitions
    add_function("null", func_null);
    add_function("nth", func_nth);
    add_function("length", func_length);
    add_function("grid", func_grid_select);
}

DDS::DDS(const DDS &rhs)
{
    duplicate(rhs);
}

DDS::~DDS()
{
    Pix p;

    // delete all the variables in this DDS
    for (p = first_var(); p; next_var(p))
	delete var(p);

    // delete all the constants created by the parser for CE evaluation
    for (p = constants.first(); p; constants.next(p))
	delete constants(p);

    if (!expr.empty())
	for (p = first_clause(); p; next_clause(p))
	    delete expr(p);
}

DDS &
DDS::operator=(const DDS &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

String 
DDS::get_dataset_name()
{ 
    return name; 
}

void
DDS::set_dataset_name(const String &n) 
{ 
    name = n; 
}

String
DDS::filename()
{
    return _filename;
}

void
DDS::filename(const String &fn)
{
    _filename = fn;
}

void
DDS::add_var(BaseType *bt)
{ 
    assert(bt);

    vars.append(bt); 
}

void 
DDS::del_var(const String &n)
{ 
    Pix pp = 0;			// previous Pix

    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->name() == n) {
	    vars.del_after(pp);	// pp points to the pos before p
	    return;
	}
	else
	    pp = p;
}

BaseType *
DDS::var(const String &n, btp_stack &s)
{
    for (Pix p = vars.first(); p; vars.next(p)) {
	BaseType *btp = vars(p);
	DBG(cerr << "Looking at " << n << " in: " << btp << endl);
	// Look for the name in the dataset's top-level
	if (btp->name() == n) {
	    DBG(cerr << "Found " << n << " in: " << btp << endl);
	    return btp;
	}
	if (btp->is_constructor_type()) {
	    BaseType *btp2 = btp->var(n, s);
	    if (btp2) {
		s.push(btp);
		return btp2;
	    }
	}
    }


    return 0;			// It is not here.
}    

// Find the variable named N. Accepts both simple names and *fully* qualified
// aggregate names.
//
// Returns: A BaseType * to a variable whose name in N. If no such variable
// can be found, returns NULL.

BaseType *
DDS::var(const String &n)
{
    BaseType *v = exact_match(n);
    if (v)
	return v;

    v = leaf_match(n);
    if (v)
	return v;

    return 0;
}

BaseType *
DDS::leaf_match(const String &n) 
{
    for (Pix p = vars.first(); p; vars.next(p)) {
	BaseType *btp = vars(p);
	DBG(cerr << "Looking at " << n << " in: " << btp << endl);
	// Look for the name in the dataset's top-level
	if (btp->name() == n) {
	    DBG(cerr << "Found " << n << " in: " << btp << endl);
	    return btp;
	}
	if (btp->is_constructor_type() && (btp = btp->var(n, false))) {
	    return btp;
	}
    }


    return 0;			// It is not here.
}

BaseType *
DDS::exact_match(const String &n)
{
    if (n.contains(".")) {
	String name = (String)n; // cast away const
	String aggregate = name.before(".");
	String field = name.from(".");
	field = field.after(".");

	BaseType *agg_ptr = var(aggregate);
	if (agg_ptr)
	    return agg_ptr->var(field);	// recurse
	else
	    return 0;		// qualified names must be *fully* qualified
    }
    else {
	for (Pix p = vars.first(); p; vars.next(p)) {
	    BaseType *btp = vars(p);
	    DBG(cerr << "Looking at " << n << " in: " << btp << endl);
	    // Look for the name in the current ctor type or the top level
	    if (btp->name() == n) {
		DBG(cerr << "Found " << n << " in: " << btp << endl);
		return btp;
	    }
	}
    }

    return 0;			// It is not here.
}

// This is necessary because (char *) can be cast to Pix (because PIX is
// really (void *)). This must take precedence over the creation of a
// temporary object (the String).

BaseType *
DDS::var(const char *n)
{
    return var((String)n);
}

Pix 
DDS::first_var()
{ 
    return vars.first(); 
}

void 
DDS::next_var(Pix &p)
{ 
    if (!vars.empty() && p)
	vars.next(p); 
}

BaseType *
DDS::var(Pix p)
{ 
    if (!vars.empty() && p)
	return vars(p); 
    else
	return 0;
}

int
DDS::num_var()
{
    return vars.length();
}

Pix
DDS::first_clause()
{
    assert(!expr.empty());

    return expr.first();
}

void
DDS::next_clause(Pix &p)
{
    assert(!expr.empty() && p);

    expr.next(p);
}

Clause &
DDS::clause(Pix p)
{
    assert(!expr.empty() && p);

    return *expr(p);
}

bool
DDS::clause_value(Pix p, const String &dataset)
{
    assert(!expr.empty());

    return expr(p)->value(dataset, *this);
}


void
DDS::append_clause(int op, rvalue *arg1, rvalue_list *arg2)
{
    Clause *clause = new Clause(op, arg1, arg2);

    expr.append(clause);
}

void
DDS::append_clause(bool_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.append(clause);
}

void
DDS::append_clause(btp_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.append(clause);
}

void
DDS::append_constant(BaseType *btp)
{
    constants.append(btp);
}

#if 0
template<class FUNC_T>
void 
DDS::add_function(const String &name, FUNC_T f)
{
    function func(name, f);
    functions.append(func);
}
#endif

#if 1
void
DDS::add_function(const String &name, bool_func f)
{
    function func(name, f);
    functions.append(func);
}

void
DDS::add_function(const String &name, btp_func f)
{
    function func(name, f);
    functions.append(func);
}

void
DDS::add_function(const String &name, proj_func f)
{
    function func(name, f);
    functions.append(func);
}
#endif

bool
DDS::find_function(const String &name, bool_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).b_func)) {
	    return true;
	}

    return false;
}

bool
DDS::find_function(const String &name, btp_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).bt_func)) {
	    return true;
	}

    return false;
}

bool
DDS::find_function(const String &name, proj_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).p_func)) {
	    return true;
	}

    return false;
}

bool
DDS::functional_expression()
{
    if (expr.empty())
	return false;

    Pix p = first_clause();
    return clause(p).value_clause();
}

BaseType *
DDS::eval_function(const String &dataset)
{
    assert(expr.length() == 1);

    Pix p = first_clause();
    BaseType *result;
    if (clause(p).value(dataset, *this, &result))
	return result;
    else
	return NULL;
}

bool
DDS::boolean_expression()
{
    if (expr.empty())
	return false;

    bool boolean = true;
    for (Pix p = first_clause(); p; next_clause(p))
	boolean = boolean && clause(p).boolean_clause();
    
    return boolean;
}

bool
DDS::eval_selection(const String &dataset)
{
    if (expr.empty()) {
	DBG(cerr << "No selection recorded" << endl);
	return true;
    }

    DBG(cerr << "Eval selection" << endl);

    // A CE is made up of zero or more clauses, each of which has a boolean
    // value. The value of the CE is the logical AND of the clause
    // values. See DDS::clause::value(...) for inforamtion on logical ORs in
    // CEs. 
    bool result = true;
    for (Pix p = first_clause(); p && result; next_clause(p)) {
	// A selection expression *must* contain only boolean clauses!
	assert(clause(p).boolean_clause());
	result = result && clause(p).value(dataset, *this);
    }

    return result;
}

bool
DDS::parse(String fname)
{
    FILE *in = fopen(fname, "r");

    if (!in) {
        cerr << "Could not open: " << fname << endl;
        return false;
    }

    bool status = parse(in);

    fclose(in);

    return status;
}


bool
DDS::parse(int fd)
{
    FILE *in = fdopen(dup(fd), "r");

    if (!in) {
        cerr << "Could not access file" << endl;
        return false;
    }

    bool status = parse(in);

    fclose(in);

    return status;
}

// Read structure from IN (which defaults to stdin). If ddsrestart() fails,
// return false, otherwise return the status of ddsparse().

bool
DDS::parse(FILE *in)
{
    if (!in) {
	cerr << "DDS::parse: Null input stream" << endl;
	return false;
    }

    ddsrestart(in);

    parser_arg arg(this);

    bool status = ddsparse((void *)&arg) == 0;

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	    arg.error()->display_message();
	return false;
    }
    else
	return true;
}

// Write strucutre from tables to OUT (which defaults to stdout). 
//
// Returns true. 

bool
DDS::print(ostream &os)
{
    os << "Dataset {" << endl;

    for (Pix p = vars.first(); p; vars.next(p))
	vars(p)->print_decl(os);

    os << "} " << name << ";" << endl;
					   
    return true;
}

bool 
DDS::print(FILE *out)
{
    ostdiostream os(out);
    return print(os);
}

// Print those parts (variables) of the DDS structure to OS that are marked
// to be sent after evaluating the constraint expression.
//
// NB: this function only works for scalars at the top level.
//
// Returns true.

bool
DDS::print_constrained(ostream &os)
{
    os << "Dataset {" << endl;

    for (Pix p = vars.first(); p; vars.next(p))
	// for each variable, indent with four spaces, print a trailing
	// semi-colon, do not print debugging information, print only
	// variables in the current projection.
	vars(p)->print_decl(os, "    ", true, false, true);

    os << "} " << name << ";" << endl;
					   
    return true;
}

bool
DDS::print_constrained(FILE *out)
{
    ostdiostream os(out);
    return print_constrained(os);
}

static void
print_variable(ostream &os, BaseType *var, bool constrained = false)
{
    assert(os);
    assert(var);

    os << "Dataset {" << endl;

    var->print_decl(os, "    ", true, false, constrained);

    os << "} function_value;" << endl;
}

static void
print_variable(FILE *out, BaseType *var, bool constrained = false)
{
    assert(out);
    assert(var);

    ostdiostream os(out);
    print_variable(os, var, constrained);
}

// Check the semantics of the DDS describing a complete dataset. If ALL is
// true, check not only the semantics of THIS->TABLE, but also recurrsively
// all ctor types in the THIS->TABLE. By default, ALL is false since parsing
// a DDS input file runns semantic checks on all variables (but not the
// dataset itself.
//
// Returns: true if the conventions for the DDS are not violated, false
// otherwise. 

bool
DDS::check_semantics(bool all)
{
    // The dataset must have a name
    if (name == (char *)0) {
	cerr << "A dataset must have a name" << endl;
	return false;
    }

    String msg;
    if (!unique_names(vars, (const char *)name, (const char *)"Dataset", msg))
	return false;

    if (all) 
	for (Pix p = vars.first(); p; vars.next(p))
	    if (!vars(p)->check_semantics(msg, true))
		return false;

    return true;
}

// Evaluate the constraint expression; return the value of the expression. As
// a side effect, mark the DDS so that BaseType's mfuncs can be used to
// correctly read the variable's value and send it to the client.
//
// Returns: true if the constraint expression is valid (parses correctly) for
// the current DDS, false otherwise.

bool
DDS::parse_constraint(const String &constraint, ostream &os, bool server = true)
{
#if 0
    FILE *in = text_to_temp(constraint);

    if (!in) {
	cerr << "DDS::parse_constraint: Null input stream" << endl;
	return false;
    }

    exprrestart(in);
#endif

    exprrestart(0);
    void *buffer = expr_string((const char *)constraint);
    expr_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status = exprparse((void *)&arg) == 0;

    expr_delete_buffer(buffer);
    
    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error()) {
	    if (server) {
		set_mime_text(os, dods_error);
		arg.error()->print(os);
	    }
	    else
		arg.error()->display_message();
	}
	return false;
    }
    else
	return true;
}

bool
DDS::parse_constraint(const String &constraint, FILE *out, bool server = true)
{
    ostdiostream os(out);
    return parse_constraint(constraint, os, server);
}

// Send the named variable. This mfunc combines BaseTypes read() and
// serialize() mfuncs. It also ensures that the data (binary) is prefixed
// with a DDS which describes the binary data.
//
// NB: FLUSH defaults to false.
//
// Returns: true if successful, false otherwise.

bool 
DDS::send(const String &dataset, const String &constraint, FILE *out, 
	  bool compressed = true)
{
    bool status = true;

    if ((status = parse_constraint(constraint, out, true))) {
	// Handle *functional* constraint expressions specially 
	if (functional_expression()) {
	    BaseType *var = eval_function(dataset);
	    if (var) {
		set_mime_binary(out, dods_data, 
				(compressed) ? deflate : x_plain);

		// If compressing, start up the sub process.
		int childpid;	// Used to wait for compressor sub proc
		FILE *comp_sink = 0;
		XDR *xdr_sink;
		if (compressed) {
		    comp_sink = compressor(out, childpid);
		    xdr_sink = new_xdrstdio(comp_sink, XDR_ENCODE);
		}
		else {
		    xdr_sink = new_xdrstdio(out, XDR_ENCODE);
		}

		print_variable((compressed) ? comp_sink : out, var, true);
		fprintf((compressed) ? comp_sink : out, "Data:\n");

		// In the following call to serialize, suppress CE evaluation.
		status = var->serialize(dataset, *this, xdr_sink, false);

		delete_xdrstdio(xdr_sink);

		// Wait for the compressor sub process to stop
		if (compressed) {
		    fclose(comp_sink);
		    int pid;
		    while ((pid = waitpid(childpid, 0, 0)) > 0) {
			DBG(cerr << "pid: " << pid << endl);
		    }
		}
	    }
	    else {
		Error e(unknown_error, "Error calling the function.");
		set_mime_text(out, dods_error);
		e.print();
	    }
	}
	else {
	    set_mime_binary(out, dods_data, (compressed) ? deflate : x_plain);

	    int childpid;	// Used to wait for compressor sub proc
	    FILE *comp_sink = 0;
	    XDR *xdr_sink;
	    if (compressed) {
		comp_sink = compressor(out, childpid);
		xdr_sink = new_xdrstdio(comp_sink, XDR_ENCODE);
	    }
	    else {
		xdr_sink = new_xdrstdio(out, XDR_ENCODE);
	    }

	    // send constrained DDS	    
	    print_constrained((compressed) ? comp_sink : out); 
	    fprintf((compressed) ? comp_sink : out, "Data:\n");

	    for (Pix q = first_var(); q; next_var(q)) 
		if (var(q)->send_p()) // only process projected variables
		    status = status && var(q)->serialize(dataset, *this,
							 xdr_sink, true);

	    delete_xdrstdio(xdr_sink);
	    
	    if (compressed) {
		fclose(comp_sink);
		int pid;
		while ((pid = waitpid(childpid, 0, 0)) > 0) {
		    DBG(cerr << "pid: " << pid << endl);
		}
	    }
	}
    }

    return status;
}

// Mark the named variable by setting its SEND_P flag to STATE (true
// indicates that it is to be sent). Names must be fully qualified.
//
// NB: For aggregate types this sets each part to STATE when STATE is
// True. Thus, if State is True and N is `exp1.test', then both `exp1' and
// `test' have their SEND_P flag set to True. If STATE is Flase, then the
// SEND_P flag of the `test' is set to False, but `exp1' is left
// unchanged. This means that a single variable can be removed from the
// current projection without removing all the other children of its
// parent. See the mfunc set_send_p().
//
// Returns: True if the named variable was found, false otherwise.

bool
DDS::mark(const String &n, bool state)
{
    if (n.contains(".")) {
	String field = (String &)n; // cast away const

	String aggregate = field.before(".");
	BaseType *variable = var(aggregate); // get first variable from DDS
	if (!variable) {
	    DBG(cerr << "Could not find variable " << n << endl);
	    return false;	// no such variable
	}
	else if (state)
	    variable->BaseType::set_send_p(state); // set iff state == true

	field = field.after(".");

	while (field.contains(".")) {
	    aggregate = field.before(".");
	    variable = variable->var(aggregate); // get child var using parent
	    if (!variable) {
		DBG(cerr << "Could not find variable " << n << endl);
		return false;	// no such variable
	    }
	    else if (state)
		variable->BaseType::set_send_p(state); // set iff state == true

	    field = field.after(".");
	}

	variable->var(field)->set_send_p(state); // set last child

	return true;		// marked field and its parents
    }
    else {
	btp_stack s;
	BaseType *variable = var(n, s);
	if (!variable) {
	    DBG(cerr << "Could not find variable " << n << endl);
	    return false;
	}
	variable->set_send_p(state);
	
	// Now check the btp_stack and run BaseType::set_send_p for every
	// BaseType pointer on the stack.
	while (!s.empty()) {
	    s.top()->BaseType::set_send_p(state);
	    s.pop();
	}

	return true;
    }

    return false;		// not found
}

// Set the SEND_P member of every variable in the DDS to STATE.
//
// Returns: void

void
DDS::mark_all(bool state)
{
    for (Pix p = first_var(); p; next_var(p))
	var(p)->set_send_p(state);
}
    
