
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

//
// jhrg 9/7/94

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: DDS.cc,v 1.57 2002/06/18 15:36:24 tom Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <assert.h>

#include <iostream>
#ifdef WIN32
#include <strstream>
#else
#include <fstream>
#endif

#include "expr.h"
#include "Clause.h"
#include "Connect.h"
#include "DDS.h"
#include "Error.h"
#include "parser.h"
#include "debug.h"
#include "util.h"
#include "escaping.h"
#include "ce_functions.h"
#include "cgi_util.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::cerr;
using std::endl;
#ifdef WIN32
using std::strstream;
#endif

void ddsrestart(FILE *yyin);	// Defined in dds.tab.c
int ddsparse(void *arg);

struct yy_buffer_state;
yy_buffer_state *expr_scan_string(const char *str);
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

    DDS &dds_tmp = const_cast<DDS &>(dds);

    // copy the things pointed to by the list, not just the pointers
    for (Pix src = dds_tmp.first_var(); src; dds_tmp.next_var(src)) {
	add_var(dds_tmp.var(src)); // add_var() dups the BaseType.
    }
}
/** Creates a DDS with the given string for its name. */
DDS::DDS(const string &n) : name(n)
{
    add_function("member", func_member);
    add_function("null", func_null);
    add_function("nth", func_nth);
    add_function("length", func_length);
    add_function("grid", func_grid_select);
}

/** The DDS copy constructor. */
DDS::DDS(const DDS &rhs)
{
    duplicate(rhs);
}

DDS::~DDS()
{
    Pix p;

    // delete all the variables in this DDS
    for (p = first_var(); p; next_var(p)) {
	delete var(p);
    }
    
    // delete all the constants created by the parser for CE evaluation
    for (p = constants.first(); p; constants.next(p)) {
	delete constants(p); constants(p) = 0; 
    }

    if (!expr.empty()) {
	for (p = first_clause(); p; next_clause(p)) {
	    delete expr(p); expr(p) = 0;
	}
    }
}

DDS &
DDS::operator=(const DDS &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

/** Get and set the dataset's name.  This is the name of the dataset
    itself, and is not to be confused with the name of the file or
    disk on which it is stored.

    @name Dataset Name Accessors */

    //@{
      
/** Returns the dataset's name. */
string 
DDS::get_dataset_name()
{ 
    return name; 
}

/** Sets the dataset name. */
void
DDS::set_dataset_name(const string &n) 
{ 
    name = n; 
}

//@}

/** Get and set the dataset's filename. This is the physical
    location on a disk where the dataset exists.  The dataset name
    is simply a title.

    @name File Name Accessor
    @see Dataset Name Accessors */

    //@{
/** Gets the dataset file name. */
string
DDS::filename()
{
    return _filename;
}

/** Set the dataset's filename. */
void
DDS::filename(const string &fn)
{
    _filename = fn;
}
//@}

/** @brief Adds a variable to the DDS. */
void
DDS::add_var(BaseType *bt)
{ 
    if(!bt)
	throw InternalErr(__FILE__, __LINE__, 
		  "Trying to add a BaseType object with a NULL pointer.");

    DBG(cerr << "In DDS::add_var(), bt's addres is: " << bt << endl);

    BaseType *btp = bt->ptr_duplicate();
    vars.append(btp);
}

/** @brief Removes a variable from the DDS. */
void 
DDS::del_var(const string &n)
{ 
    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->name() == n) {
	    delete vars(p); vars(p) = 0;
	    vars.del(p, -1);
	    return;
	}
}

/** Search for for variable <i>n</i> as above but record all
    compound type variables which ultimately contain <i>n</i> on
    <i>s</i>. This stack can then be used to mark the contained
    compound-type variables as part of the current projection.

    @return A BaseType pointer to the variable <i>n</i> or 0 if <i>n</i>
    could not be found. */
BaseType *
DDS::var(const string &n, btp_stack &s)
{
    return var(n, &s);
}    

/** @brief Find the variable with the given name.

    Returns a pointer to the named variable. If the name contains one or
    more field separators then the function looks for a variable whose
    name matches exactly. If the name contains no field separators then
    the funciton looks first in the top level and then in all subsequent
    levels and returns the first occurrence found. In general, this
    function searches constructor types in the order in which they appear
    in the DDS, but there is no requirement that it do so. 

    \note{If a dataset contains two constructor types which have field
    names that are the same (say point.x and pair.x) you should always
    use fully qualified names to get each of those variables.}

    @return A BaseType pointer to the variable or null if not found. */
BaseType *
DDS::var(const string &n, btp_stack *s)
{
    BaseType *v = exact_match(n, s);
    if (v)
	return v;

    return leaf_match(n, s);
}

BaseType *
DDS::leaf_match(const string &n, btp_stack *s) 
{
    for (Pix p = vars.first(); p; vars.next(p)) {
	BaseType *btp = vars(p);
	DBG(cerr << "Looking at " << n << " in: " << btp << endl);
	// Look for the name in the dataset's top-level
	if (btp->name() == n) {
	    DBG(cerr << "Found " << n << " in: " << btp << endl);
	    return btp;
	}
	if (btp->is_constructor_type() && (btp = btp->var(n, false, s))) {
	    return btp;
	}
    }

    return 0;			// It is not here.
}

BaseType *
DDS::exact_match(const string &name, btp_stack *s)
{
    for (Pix p = vars.first(); p; vars.next(p)) {
	BaseType *btp = vars(p);
	DBG(cerr << "Looking for " << name << " in: " << btp << endl);
	// Look for the name in the current ctor type or the top level
	if (btp->name() == name) {
	    DBG(cerr << "Found " << name << " in: " << btp << endl);
	    return btp;
	}
    }

    string::size_type dot_pos = name.find(".");
    if (dot_pos != string::npos) {
	string aggregate = name.substr(0, dot_pos);
	string field = name.substr(dot_pos + 1);

	BaseType *agg_ptr = var(aggregate, s);
	if (agg_ptr) {
	    DBG(cerr << "Descending into " << agg_ptr->name() << endl);
	    return agg_ptr->var(field, true, s);
	}
	else
	    return 0;		// qualified names must be *fully* qualified
    }

    return 0;			// It is not here.
}


/** @brief Returns a pointer to the named variable.

    This is necessary because (char *) can be cast to Pix (because
    Pix is really (void *)). This must take precedence over the
    creation of a temporary object (the string). 

    @return A pointer to the variable or null if not found. */
BaseType *
DDS::var(const char *n, btp_stack *s)
{
    return var((string)n, s);
}

/** @brief Returns the first variable in the DDS. */
Pix 
DDS::first_var()
{ 
    return vars.first(); 
}

/** @brief Increments the DDS variable counter to point at the next
    variable. */
void 
DDS::next_var(Pix &p)
{ 
    if (!vars.empty() && p)
	vars.next(p); 
}

/** Returns a pointer to the indicated variable. */
BaseType *
DDS::var(Pix p)
{ 
    if (!vars.empty() && p)
	return vars(p); 
    else
	return 0;
}

/** @brief Returns the number of variables in the DDS. */
int
DDS::num_var()
{
    return vars.length();
}

/** Returns a pointer to the first clause in a parsed constraint
    expression. */
Pix
DDS::first_clause()
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");

    return expr.first();
}

/** Increments a pointer to indicate the next clause in a parsed
    constraint expression. */
void
DDS::next_clause(Pix &p)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");
    if(!p)
	throw InternalErr(__FILE__, __LINE__, 
		    "Passing NULL pix as an index for the CE clauses list.");

    expr.next(p);
}

/** Returns a clause of a parsed constraint expression. */
Clause &
DDS::clause(Pix p)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");
    if(!p)
      throw InternalErr(__FILE__, __LINE__, 
		  "Passing NULL pix as an index for the CE clauses list.");
    
    return *expr(p);
}

/** Returns the value of the indicated clause of a constraint
    expression. */
bool
DDS::clause_value(Pix p, const string &dataset)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");

    return expr(p)->value(dataset, *this);
}


/** @brief Add a clause to a constraint expression.

    This function adds an operator clause to the constraint
    expression. 

    @param op An integer indicating the operator in use.  These
    values are generated by {\tt bison}.
    @param arg1 A pointer to the argument on the left side of the
    operator. 
    @param arg2 A pointer to a list of the arguments on the right
    side of the operator.
*/
void
DDS::append_clause(int op, rvalue *arg1, rvalue_list *arg2)
{
    Clause *clause = new Clause(op, arg1, arg2);

    expr.append(clause);
}

/** @brief Add a clause to a constraint expression.

    This function adds a boolean function clause to the constraint
    expression. 

    @param func A pointer to a boolean function from the list of
    supported functions.
    @param args A list of arguments to that function.
*/
void
DDS::append_clause(bool_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.append(clause);
}

/** @brief Add a clause to a constraint expression.

    This function adds a real-valued (BaseType) function clause to
    the constraint expression.

    @param func A pointer to a BaseType function from the list of
    supported functions.
    @param args A list of arguments to that function.
*/
void
DDS::append_clause(btp_func func, rvalue_list *args)
{
    Clause *clause = new Clause(func, args);

    expr.append(clause);
}

/** The DDS maintains a list of BaseType pointers for all the constants
    that the constraint expression parser generates. These objects are
    deleted by the DDS destructor. Note that there are no list accessors;
    these constants are never accessed from the list. The list is simply
    a convenient way to make sure the constants are disposed of properly.

    the constraint expression parser. */
void
DDS::append_constant(BaseType *btp)
{
    constants.append(btp);
}

/** Each DDS carries with it a list of external functions it can use to
    evaluate a constraint expression. If a constraint contains any of
    these functions, the entries in the list allow the parser to evaluate
    it. The functions are of two types: those that return boolean values,
    and those that return real (also called BaseType) values.

    These methods are used to manipulate this list of known
    external functions.

    @name External Function Accessors
*/
    //@{
#if 0
/** Add a function to the list. */
template<class FUNC_T>
void 
DDS::add_function(const string &name, FUNC_T f)
{
    function func(name, f);
    functions.append(func);
}
#endif

#if 1
/** @brief Add a boolean function to the list. */
void
DDS::add_function(const string &name, bool_func f)
{
    function func(name, f);
    functions.append(func);
}
/** @brief Add a BaseType function to the list. */
void
DDS::add_function(const string &name, btp_func f)
{
    function func(name, f);
    functions.append(func);
}

/** @brief Add a projection function to the list. */
void
DDS::add_function(const string &name, proj_func f)
{
    function func(name, f);
    functions.append(func);
}
#endif

/** @brief Find a Boolean function with a given name in the function list. */
bool
DDS::find_function(const string &name, bool_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).b_func)) {
	    return true;
	}

    return false;
}

/** @brief Find a BaseType function with a given name in the function list. */
bool
DDS::find_function(const string &name, btp_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).bt_func)) {
	    return true;
	}

    return false;
}

/** @brief Find a projection function with a given name in the function list. */
bool
DDS::find_function(const string &name, proj_func *f) const
{
    if (functions.empty())
	return false;

    for (Pix p = functions.first(); p; functions.next(p))
	if (name == functions(p).name && (*f = functions(p).p_func)) {
	    return true;
	}

    return false;
}
    //@}


/** @brief Does the current constraint expression return a BaseType
    pointer? */
bool
DDS::functional_expression()
{
    if (expr.empty())
	return false;

    Pix p = first_clause();
    return clause(p).value_clause();
}

/** @brief Evaluate a function-valued constraint expression. */
BaseType *
DDS::eval_function(const string &dataset)
{
    if (expr.length() != 1)
	throw InternalErr(__FILE__, __LINE__, 
			  "The length of the list of CE clauses is not 1.");

    Pix p = first_clause();
    BaseType *result;
    if (clause(p).value(dataset, *this, &result))
	return result;
    else
	return NULL;
}

/** @brief Does the current constraint expression return a boolean value? */ 
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

/** @brief Evaluate a boolean-valued constraint expression. */
bool
DDS::eval_selection(const string &dataset)
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
	if(!(clause(p).boolean_clause()))
	    throw InternalErr(__FILE__, __LINE__, 
                "A selection expression must contain only boolean clauses.");
	result = result && clause(p).value(dataset, *this);
    }

    return result;
}

/** @brief Parse a DDS from a file with the given name. */
void
DDS::parse(string fname)
{
    FILE *in = fopen(fname.c_str(), "r");

    if (!in) {
	throw Error(can_not_read_file, "Could not open: " + fname);
    }

    parse(in);

    fclose(in);
}


/** @brief Parse a DDS from a file indicated by the input file descriptor. */
void
DDS::parse(int fd)
{
    FILE *in = fdopen(dup(fd), "r");

    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    parse(in);

    fclose(in);
}

/** @brief Parse a DDS from a file indicated by the input file descriptor. 

    Read structure from IN (which defaults to stdin). If
    ddsrestart() fails, return false, otherwise return the status
    of ddsparse(). 
*/
void
DDS::parse(FILE *in)
{
    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Null input stream.");
    }

    ddsrestart(in);

    parser_arg arg(this);

    bool status = ddsparse((void *)&arg) == 0;

    DBG(cout << "Status from parser: " << status << endl);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	  throw *arg.error();
    }
}

/** @brief Print the entire DDS on the specified output stream. 

    Write structure from tables to OUT (which defaults to stdout). 

    @return TRUE
*/
void
DDS::print(ostream &os)
{
    os << "Dataset {" << endl;

    for (Pix p = vars.first(); p; vars.next(p))
	vars(p)->print_decl(os);

    os << "} " << id2www(name) << ";" << endl;
}

/** @brief Print the entire DDS to the specified file. */
void
DDS::print(FILE *out)
{
#ifdef WIN32
    strstream os;
    print(os);
    flush_stream(os, out);
#else
    ofstream os(fileno(out));
    print(os);
#endif
	return;
}

/** @brief Print the constrained DDS to the specified output
    stream.
    
    Print those parts (variables) of the DDS structure to OS that
    are marked to be sent after evaluating the constraint
    expression. 

    \note This function only works for scalars at the top level.

    @returns true.
*/
void
DDS::print_constrained(ostream &os)
{
    os << "Dataset {" << endl;

    for (Pix p = vars.first(); p; vars.next(p))
	// for each variable, indent with four spaces, print a trailing
	// semi-colon, do not print debugging information, print only
	// variables in the current projection.
	vars(p)->print_decl(os, "    ", true, false, true);

    os << "} " << id2www(name) << ";" << endl;
}
/** @brief Print a constrained DDS to the specified file. 

    Print those parts (variables) of the DDS structure to OS that
    are marked to be sent after evaluating the constraint
    expression. 

    \note This function only works for scalars at the top level.

    @returns true.
*/
void
DDS::print_constrained(FILE *out)
{
#ifdef WIN32
    strstream os;
    print_constrained(os);
    flush_stream(os, out);
#else
    ofstream os(fileno(out));
    print_constrained(os);
#endif
	return;
}

static void
print_variable(ostream &os, BaseType *var, bool constrained = false)
{
    if(!os)
	throw InternalErr(__FILE__, __LINE__, 
			  "The stream is not ready to accept data.");
    if(!var)
	throw InternalErr(__FILE__, __LINE__, 
     "Passing NULL variable to method DDS::print_variable in *this* object.");

    os << "Dataset {" << endl;

    var->print_decl(os, "    ", true, false, constrained);

    os << "} function_value;" << endl;
}

static void
print_variable(FILE *out, BaseType *var, bool constrained = false)
{
    if(!out)
	throw InternalErr(__FILE__, __LINE__, 
			  "Invalid file descriptor, NULL pointer!");
    if(!var)
	throw InternalErr(__FILE__, __LINE__, 
     "Passing NULL variable to method DDS::print_variable in *this* object.");

#ifdef WIN32
    strstream os;
    print_variable(os, var, constrained);
    flush_stream(os, out);
#else
    ofstream os(fileno(out));
    print_variable(os, var, constrained);
#endif
    return;
}

/** @brief Check the semantics of each of the variables represented in the
    DDS. 

    Check the semantics of the DDS describing a complete dataset. If ALL is
    true, check not only the semantics of THIS->TABLE, but also recurrsively
    all ctor types in the THIS->TABLE. By default, ALL is false since parsing
    a DDS input file runns semantic checks on all variables (but not the
    dataset itself.

    @return TRUE if the conventions for the DDS are not violated, FALSE
    otherwise. 
    @param all If true, recursively check the individual members of
    compound variables.
    @see BaseType::check_semantics */
bool
DDS::check_semantics(bool all)
{
    // The dataset must have a name
    if (name == "") {
	cerr << "A dataset must have a name" << endl;
	return false;
    }

    string msg;
    if (!unique_names(vars, name, "Dataset", msg))
	return false;

    if (all) 
	for (Pix p = vars.first(); p; vars.next(p))
	    if (!vars(p)->check_semantics(msg, true))
		return false;

    return true;
}

/** @brief Parse the constraint expression given the current DDS. 

    Evaluate the constraint expression; return the value of the
    expression. As a side effect, mark the DDS so that BaseType's
    mfuncs can be used to correctly read the variable's value and
    send it to the client. 

    @return void
    @param constraint A string containing the constraint
    expression. 
    @param os The output stream on which to write error objects and
    messages. 
    @param server If true, send errors back to client instead of
    displaying errors on the default output stream. */ 
void
DDS::parse_constraint(const string &constraint, ostream &os, bool server)
{
    void *buffer = expr_string(constraint.c_str());
    expr_switch_to_buffer(buffer);

    parser_arg arg(this);

    // For all errors, exprparse will throw Error. 
    exprparse((void *)&arg);

    expr_delete_buffer(buffer);
}

/** @brief Parse the constraint expression given the current DDS. 

    Evaluate the constraint expression; return the value of the
    expression. As a side effect, mark the DDS so that BaseType's
    mfuncs can be used to correctly read the variable's value and
    send it to the client. 

    @return void
    @param constraint A string containing the constraint
    expression. 
    @param out A FILE pointer to which error objects should be wrtten. 
    @param server If true, send errors back to client instead of
    displaying errors on the default output stream. 
*/
void
DDS::parse_constraint(const string &constraint, FILE *out, bool server)
{
#ifdef WIN32
    strstream os;
    parse_constraint(constraint, os, server);	
    flush_stream(os, out);
#else
    ofstream os(fileno(out));
    parse_constraint(constraint, os, server);
#endif
	return;
}

// We start two sinks, one for regular data and one for XDR encoded data.
static int
get_sinks(FILE *out, bool compressed, FILE **comp_sink, XDR **xdr_sink)
{
    // If compressing, start up the sub process.
    int childpid;	// Used to wait for compressor sub proc
    if (compressed) {
	*comp_sink = compressor(out, childpid);
	*xdr_sink = new_xdrstdio(*comp_sink, XDR_ENCODE);
    }
    else {
	*xdr_sink = new_xdrstdio(out, XDR_ENCODE);
    }
    
    return childpid;
}

// Clean up after sinks; might have to wait for the compressor process to
// stop. 
static void
clean_sinks(int childpid, bool compressed, XDR *xdr_sink, FILE *comp_sink)
{
    delete_xdrstdio(xdr_sink);
    
    if (compressed) {
	fclose(comp_sink);
	int pid;
#ifdef WIN32
	while ((pid = _cwait(NULL, childpid, NULL)) > 0) {
#else
	while ((pid = waitpid(childpid, 0, 0)) > 0) {
#endif
	    DBG(cerr << "pid: " << pid << endl);
	}
    }
}

/** This function sends the variables described in the constrained DDS to
    the output described by the FILE pointer. This function calls
    <tt>parse_constraint()</tt>, <tt>BaseType::read()</tt>, and
    <tt>BaseType::serialize()</tt>.

    @return True if successful, false otherwise.
    @param dataset The name of the dataset to send.
    @param constraint A string containing the entire constraint
    expression received in the original data request.
    @param out A pointer to the output buffer for the data.
    @param compressed If true, send compressed data.
    @param cgi_ver version information from the caller that identifies
    the server sending data.
    @param lmt A <tt>time_t</tt> value to use in making a "Last
    Modifed" header for the  sent data.
    @see parse_constraint
    @see BaseType::read
    @see BaseType::serialize */
bool 
DDS::send(const string &dataset, const string &constraint, FILE *out, 
	  bool compressed, const string &cgi_ver, time_t lmt)
{
    // Jose Garcia
    // If there is a parse error the method parse_constraint will throw 
    // an exception that will terminate the method send.
    // If parse_constraint executes with no exception
    // we will proceed with the algorithm to send the data.

    parse_constraint(constraint, out, true);
  
    bool status = true;
  
    // Handle *functional* constraint expressions specially 
    if (functional_expression()) {
	BaseType *var = eval_function(dataset);
	if (!var)
	    throw Error(unknown_error, "Error calling the CE function.");

	set_mime_binary(out, dods_data, cgi_ver,
			(compressed) ? deflate : x_plain, lmt);
      
	FILE *comp_sink;
	XDR *xdr_sink;
	int childpid = get_sinks(out, compressed, &comp_sink, &xdr_sink);
      
	print_variable((compressed) ? comp_sink : out, var, true);
	fprintf((compressed) ? comp_sink : out, "Data:\n");
      
	    // In the following call to serialize, suppress CE evaluation.
	status = var->serialize(dataset, *this, xdr_sink, false);
      
	clean_sinks(childpid, compressed, xdr_sink, comp_sink);
    }
    else {
	set_mime_binary(out, dods_data, cgi_ver,
			(compressed) ? deflate : x_plain, lmt);
    
	FILE *comp_sink;
	XDR *xdr_sink;
	int childpid = get_sinks(out, compressed, &comp_sink, &xdr_sink);

	// send constrained DDS	    
	print_constrained((compressed) ? comp_sink : out); 
	fprintf((compressed) ? comp_sink : out, "Data:\n");

	for (Pix q = first_var(); q; next_var(q)) 
	    if (var(q)->send_p()) // only process projected variables
		status = status && var(q)->serialize(dataset, *this,
						     xdr_sink, true);
    
	clean_sinks(childpid, compressed, xdr_sink, comp_sink);
    }

    return status;
}

/** @brief Mark the <tt>send_p</tt> flag of the named variable to
    <i>state</i>.  

    Mark the named variable by setting its SEND_P flag to STATE (true
    indicates that it is to be sent). Names must be fully qualified.

    @note For aggregate types this sets each part to STATE when STATE is
    True. Thus, if State is True and N is `exp1.test', then both `exp1' and
    `test' have their SEND_P flag set to True. If STATE is False, then the
    SEND_P flag of the `test' is set to False, but `exp1' is left
    unchanged. This means that a single variable can be removed from the
    current projection without removing all the other children of its
    parent. See the mfunc set_send_p().

    @return True if the named variable was found, false otherwise.
*/
bool
DDS::mark(const string &n, bool state)
{
    btp_stack *s = new btp_stack;

    DBG(cerr << "Looking for " << n << endl);

    BaseType *variable = var(n, s);
    if (!variable) {
	DBG(cerr << "Could not find variable " << n << endl);
	return false;
    }
    variable->set_send_p(state);
    DBG(cerr << "Set variable " << variable->name() << endl);

    // Now check the btp_stack and run BaseType::set_send_p for every
    // BaseType pointer on the stack.
    while (!s->empty()) {
	s->top()->BaseType::set_send_p(state);
	DBG(cerr << "Set variable " << s->top()->name() << endl);
	s->pop();
    }

    return true;
}

#if 0
    string::size_type dotpos = n.find('.');
    if (dotpos != n.npos) {
	string aggregate = n.substr(0, dotpos);
	string field = n.substr(dotpos+1);
	BaseType *variable = var(aggregate); // get first variable from DDS
	if (!variable) {
	    DBG(cerr << "Could not find variable " << n << endl);
	    return false;	// no such variable
	}
	else if (state)
	    variable->BaseType::set_send_p(state); // set iff state == true

	while ((dotpos = field.find('.')) != field.npos) {
	    aggregate = field.substr(0, dotpos);
	    variable = variable->var(aggregate); // get child var using parent
	    if (!variable) {
		DBG(cerr << "Could not find variable " << n << endl);
		return false;	// no such variable
	    }
	    else if (state)
		variable->BaseType::set_send_p(state); // set iff state == true
	    
	    field = field.substr(dotpos+1);
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

/** Mark the member variable <tt>send_p</tt> flags to
    <i>state</i>. 

    @return Void
*/
void
DDS::mark_all(bool state)
{
    for (Pix p = first_var(); p; next_var(p))
	var(p)->set_send_p(state);
}
    
// $Log: DDS.cc,v $
// Revision 1.57  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.56  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.53.4.8  2002/04/02 19:11:47  jimg
// Wrapped using std::strstream in #ifdef WIN32 since that's the only time this
// file includes the strstream header.
//
// Revision 1.53.4.7  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.53.4.6  2002/01/30 18:53:09  jimg
// Fixes to the comments.
//
// Revision 1.53.4.5  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.55  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.53.4.4  2001/07/28 01:10:42  jimg
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
// Revision 1.54  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.53.4.3  2001/05/12 00:03:13  jimg
// Changed add_var() so that it adds copies of the BaseType*s to mimic the
// behavior of Structure, ..., Grid.
// Factored two (static) functions out of send().
//
// Revision 1.53.4.2  2001/05/03 18:53:07  jimg
// Changed a comment; it was about five years out of date.
//
// Revision 1.53.4.1  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.53  2000/10/03 22:16:22  jimg
// Put debgging output in parse() method inside DBG().
//
// Revision 1.52  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.51  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.50  2000/07/09 22:05:35  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.48  2000/06/16 18:50:18  jimg
// Fixes leftover from the last merge plus needed for the merge with version
// 3.1.7.
//
// Revision 1.47  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.44.2.2  2000/06/14 17:01:40  jimg
// Fixed a bug in del_var; the BaseType pointer vars(p) must be deleted
// before calling DLList.del(p).
//
// Revision 1.46  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.45.6.1  2000/06/02 18:16:48  rmorris
// Mod's for port to Win32.
//
// Revision 1.44.8.2  2000/02/17 05:03:12  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.44.8.1  2000/02/07 21:11:35  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.45  2000/01/27 06:29:56  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.44.2.1  2000/01/26 23:56:52  jimg
// Fixed the return type of string::find.
//
// Revision 1.44  1999/07/22 17:11:50  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.43.4.1  1999/06/08 17:37:24  dan
// Replace template definition of add_function with 3 explicit
// instances of this method.  Required due to inability of gcc on certain
// architectures to link properly using template definitions.
//
// Revision 1.43  1999/05/26 17:27:48  jimg
// Replaced a serialization of an Error object with a throw to the outer layer.
// This should help smooth getting errors to the outer layer of the servers so
// they can be sent back to the clients reliably.
//
// Revision 1.42  1999/05/05 01:29:42  jimg
// The member function parse_constraint() now throws an Error object so that
// enclosing code will handle serializing the Error object.
// The member function send() takes the CGI version as an extra argument. All
// calls to the set_mime_*() functions include this version number.
//
// Revision 1.41  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//
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
// Revision 1.36 1998/10/21 16:38:12 jimg 
// The find_function() member function now checks for the name AND the
// function type before returning a value. This means that a bool and
// BaseType * function may have the same name but, because of their different
// types, still work properly in context.
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
// Revision 1.34.6.2  1999/02/05 09:32:34  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.34.6.1  1999/02/02 21:56:57  jimg
// String to string version
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
// be printed using the current constraint. Changed the call to
// print_variable() in DDS::send() so that the constrained variable is printed.
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

