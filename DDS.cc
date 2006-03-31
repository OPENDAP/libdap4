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

//
// jhrg 9/7/94

#include "config.h"

static char rcsid[] not_used = {"$Id$"};


#include <stdio.h>
#include <sys/types.h>

#ifdef WIN32
#include <io.h>
#include <process.h>
#include <fstream>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

#include <iostream>
#include <algorithm>
#include <functional>

#include "GNURegex.h"

#include "DAS.h"
//#include "AttrTable.h"
#include "DDS.h"
#include "expr.h"
#include "Clause.h"
#include "Error.h"
#include "InternalErr.h"
//#include "BTIterAdapter.h"
//#include "ClauseIterAdapter.h"

#include "parser.h"
#include "debug.h"
#include "util.h"
#include "escaping.h"
// #include "ce_functions.h"
// #include "cgi_util.h"


const string default_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
const string dods_namespace = "http://xml.opendap.org/ns/DAP2";

using namespace std;

void ddsrestart(FILE *yyin);	// Defined in dds.tab.c
int ddsparse(void *arg);

struct yy_buffer_state;
yy_buffer_state *expr_scan_string(const char *str);
int exprparse(void *arg);

// Glue routines declared in expr.lex
void expr_switch_to_buffer(void *new_buffer);
void expr_delete_buffer(void * buffer);
void *expr_string(const char *yy_str);

// Glue for the DDS parser defined in dds.lex
void dds_switch_to_buffer(void *new_buffer);
void dds_delete_buffer(void * buffer);
void *dds_buffer(FILE *fp);

void
DDS::duplicate(const DDS &dds)
{
    name = dds.name;
    d_factory = dds.d_factory;
    
    DDS &dds_tmp = const_cast<DDS &>(dds);

    // copy the things pointed to by the list, not just the pointers
    for (Vars_iter i = dds_tmp.var_begin(); i != dds_tmp.var_end(); i++)
    {
	add_var(*i); // add_var() dups the BaseType.
    }
}

/** Make a DDS which uses the given BaseTypeFactory to create variables. 
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method. 
    @param factory BaseTypeFactory which instantiates Byte, ..., Grid. The
    caller is responsible for freeing the object \e after deleting this DDS.
    Can also be set using set_factory().
    @param n The name of the data set. Can also be set using
    set_dataset_name(). */
DDS::DDS(BaseTypeFactory *factory, const string &n)
    : d_factory(factory), //d_local_basetype_factory(false), 
      name(n), d_timeout(0)
{
#if OLD_DDS_TRANS_CODE
    add_function("length", func_length);
    add_function("grid", func_grid_select);
#endif
}

/** The DDS copy constructor. */
DDS::DDS(const DDS &rhs) : DODSResponseObject(rhs)
{
    duplicate(rhs);
}

DDS::~DDS()
{
    // delete all the variables in this DDS
    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
    {
	BaseType *btp = *i ;
	delete btp ; btp = 0;
    }
    
    // delete all the constants created by the parser for CE evaluation
    for (Constants_iter j = constants.begin(); j != constants.end(); j++)
    {
	BaseType *btp = *j ;
	delete btp ; btp = 0;
    }

    if (!expr.empty()) {
	for (Clause_iter k = expr.begin(); k != expr.end(); k++)
	{
	    Clause *cp = *k ;
	    delete cp ; cp = 0;
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
/** Transfer a single attribute to the table held by a BaseType. If the
    attribute turns out to be a container, call transfer_attr_table. If not
    load the discrete attributes into the BaseType. Both this function and
    transfer_attr_table \e assume that you know that the attributes are
    destined for the particular BaseType.

    @param das Pointer to the DAS instance which holds the attribute \e ep.
    @param ep The attribute
    @param btp The destination 
    @param suffix When adding attributes, append \e suffix to their names. 
    Defaults to the empty string. */
void
DDS::transfer_attr(DAS *das, const AttrTable::entry *ep, BaseType *btp,
                   const string &suffix)
{
    if (ep->is_alias) {
	AttrTable *source_table = das->get_attr_table(ep->aliased_to);
	AttrTable &dest = btp->get_attr_table();
	if (source_table)
	    dest.add_container_alias(ep->name + suffix, source_table);
	else
	    dest.add_value_alias(das, ep->name + suffix, ep->aliased_to);
    }
    else if (ep->type == Attr_container) {
        ep->attributes->set_name(ep->name);
	Constructor *c = dynamic_cast<Constructor*>(btp);
	if (c)
	    transfer_attr_table(das, ep->attributes, c, suffix);
	else
	    transfer_attr_table(das, ep->attributes, btp, suffix);
    }
    else {
	AttrTable &at = btp->get_attr_table();
	string n = ep->name + suffix;
	string t = AttrType_to_String(ep->type);
	vector<string> *attrs = ep->attr;
	for (vector<string>::iterator i = attrs->begin(); i!=attrs->end(); ++i)
	    at.append_attr(n, t, *i);
    }
}

/** Transfer a container to a BaseType. If the container's name is the same
    as the BaseType's, then copy the individual attributes from the container
    into the BaseType's container. If the names are different, install the
    container itself inside the BaseType's.

    @param das Pointer to the DAS instance which holds the attribtue table \e
    at. 
    @param at The attribute container
    @param btp The destination 
    @param suffix Append \e suffix to the attribute name when transferring.
    Defaults tot eh empty string. Useful when collapsing nested attributes. */
void
DDS::transfer_attr_table(DAS *das, AttrTable *at, BaseType *btp, 
                         const string &suffix)
{
    if (at->get_name() == btp->name()) {
	// for each entry in the table, call transfer_attr()
	for (AttrTable::Attr_iter i = at->attr_begin(); i!=at->attr_end(); ++i)
	    transfer_attr(das, *i, btp, suffix);
    }
    else {
	// Clone at because append_container does not and at may be deleted
	// before we're done with it. 05/22/03 jhrg
	AttrTable *new_at = new AttrTable(*at);
	btp->get_attr_table().append_container(new_at, at->get_name());
    }
}

/** Transfer an attribute container to a Constructor variable. */
void
DDS::transfer_attr_table(DAS *das, AttrTable *at, Constructor *c,
                         const string &suffix)
{
    for (AttrTable::Attr_iter i = at->attr_begin(); i != at->attr_end(); ++i) {
	AttrTable::entry *ep = *i;
	string n = ep->name;
	bool found = false;

	switch (c->type()) {
	  case dods_structure_c:
	  case dods_sequence_c: {
	      for (Constructor::Vars_iter j = c->var_begin(); j!=c->var_end(); 
		   ++j) {
		  if (n == (*j)->name()) { // found match
		      found = true;
		      transfer_attr(das, ep, *j, suffix);
		  }
	      }
	      break;
	  }

	  case dods_grid_c: {
	      Grid *g = dynamic_cast<Grid*>(c);
	      if (n == g->array_var()->name()) { // found match
		  found = true;
		  transfer_attr(das, ep, g->array_var(), suffix);
	      }
	      
	      for (Grid::Map_iter j = g->map_begin(); j!=g->map_end(); ++j) {
		  if (n == (*j)->name()) { // found match
		      found = true;
		      transfer_attr(das, ep, *j, suffix);
		  }
	      }
	      break;
	  }
	    
	  default:
	    throw InternalErr(__FILE__, __LINE__, "Unknown type.");
	}

	if (!found)
	    transfer_attr(das, ep, c);
    }
}

/** An attribute is global if it's name does not match any of the top-level
    variables. Assume that this function is called only for top-level
    attributes. 

    A private method.

    @param name Name of the attribute. 
    @return True if \i name is a global attribute. */
bool
DDS::is_global_attr(string name)
{
    for(Vars_iter i = var_begin(); i != var_end(); ++i)
	if ((*i)->name() == name)
	    return false;

    return true;
}

/** Should this attribute be thrown away? Some servers build extra attributes
    that don't fit back into the DDS. This checks for them by name.
    @param name The name of the attribute.
    @return True if the name fits a pattern of attributes known to not mesh
    well with DDS objects. */
static inline bool
is_in_kill_file(const string &name)
{
    static Regex dim(".*_dim_[0-9]*"); // HDF `dimension' attributes.

    return dim.match(name.c_str(), name.length()) != -1;
}

/** If a given AttrTable looks like it's a global attribute container, add it
    to this object's attributes. Heuristic on the best of days...
    
    A private method.

    @param at The candiate AttrTable */
void
DDS::add_global_attribute(AttrTable::entry *entry)
{
    string name = entry->name;

    if (is_global_attr(name) && !is_in_kill_file(name)) {
	if (entry->type == Attr_container) {
	    try {
		// Force the clone of table entry->attributes.
		// append_container just copies the pointer and
		// entry->attributes may be deleted before we're done with
		// it! 05/22/03 jhrg
		AttrTable *new_at = new AttrTable(*(entry->attributes));
		d_attr.append_container(new_at, name);
	    }
	    catch (Error &e) {
                DBG(cerr << "Error in DDS::global_attribute: " 
                         << e.get_error_message() << endl);
		// *** Ignore this error for now. We should probably merge
		// the attributes and this really is something we should for
		// before hand instead of letting an exception signal the
		// condition... 05/22/03 jhrg
	    }
        }
    }
}

/** Given a DAS object, scavenge attributes from it and load them into this
    object and the variables it contains. A set of heuristics is used to do
    this, including the AttrTable find algorithms and some weird stuff with
    regular expressions to weed out attributes created by some servers that
    don't fit into the DDS/Variable scheme of things. 

    @note This method is technically \e unnecessary because a server (or
    client) can easily add attributes directly using the DDS::get_attr_table
    or BaseType::get_attr_table methods and then poke values in using any
    of the methods AttrTable provides. This method exists to ease the
    transition to DDS objects which contain attribute information for the
    existing servers (Since they all make DAS objects separately from the
    DDS). They could be modified to use the same AttrTable methods but
    operate on the AttrTable instances in a DDS/BaseType instead of those in
    a DAS.

    @param das Get attribute information from this DAS. */
void
 DDS::transfer_attributes(DAS * das)
{
    for (AttrTable::Attr_iter i = das->attr_begin(); i != das->attr_end();
         ++i) {
        AttrTable::entry * ep = *i;

        // Look for attribute tables with <name>_dim_0, dim_1, et cetera. 
        // If found, seach for just the <name> since these tables are
        // created by the HDF4 server to hold information HDF4 binds to 
        // named dimensions. We should include this info in <name>'s 
        // attribute table.
        string::size_type dim_pos = ep->name.find("_dim_");
        string suffix = "";
        if (dim_pos != string::npos) {
            suffix = ep->name.substr(dim_pos);
            // Why remove the '_dim_?' part of the name? Because we need to 
            // find the variable by name _and_ when we add the attributes
            // if the container name matches the variable name exactly, the
            // attributes in the container are added to the variable's main
            // attribute container (i.e., the new attributes are merged into 
            // the variable's existing attribute container. So, we have to
            // make the name of the container match that of the variable. See
            // DDS::transfer_attr_table().
            ep->name = ep->name.substr(0, dim_pos);
        }

        BaseType *btp = var(ep->name);
        if (btp)
            transfer_attr(das, ep, btp, suffix);
        else
            add_global_attribute(*i);

    }
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

/** Get the attribute table for the global attributes. */
AttrTable &
DDS::get_attr_table()
{
    return d_attr;
}

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

    DBG(cerr << "In DDS::add_var(), bt's address is: " << bt << endl);

    BaseType *btp = bt->ptr_duplicate();
    DBG(cerr << "In DDS::add_var(), btp's address is: " << btp << endl);
    vars.push_back(btp);
}

/** Remove the named variable from the DDS. This method is not smart about
    looking up names. The variable must exist at the top level of the DDS and
    must match \e exactly the name given.
    
    @note Invalidates any iterators that reference the contents of the DDS.
    @param n The name of the variable to remove. */
void 
DDS::del_var(const string &n)
{ 
    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
    {
	if ((*i)->name() == n) {
	    BaseType *bt = *i ;
	    vars.erase(i) ;
	    delete bt ; bt = 0;
	    return;
	}
    }
}

/** Remove the variable referenced by the iterator and free its storage.

    @note Invalidates any iterators that reference the contents of the DDS.
    @param i The Vars_iter which refers to the variable. */
void
DDS::del_var(Vars_iter i)
{
    if( i != vars.end() )
    {
	BaseType *bt = *i ;
	vars.erase(i) ;
	delete bt ; bt = 0;
    }
}

/** Remove the variables referenced by the range of iterators and free their
    storage. 

    @note Invalidates any iterators that reference the contents of the DDS.
    @param i1 The start of the range.
    @param i2 The end of the range. */
void
DDS::del_var(Vars_iter i1, Vars_iter i2)
{
    for( Vars_iter i_tmp = i1; i_tmp != i2; i_tmp++ )
    {
	BaseType *bt = *i_tmp ;
	delete bt ; bt = 0;
    }
    vars.erase(i1, i2) ;
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
#if character
/** @brief Returns a pointer to the named variable.

    The following comment is no longer true; the implementation of Pix has
    been chnaged. 01/09/03 jhrg

    This is necessary because (char *) can be cast to Pix (because
    Pix is really (void *)). This must take precedence over the
    creation of a temporary object (the string). 

    @return A pointer to the variable or null if not found. */

BaseType *
DDS::var(const char *n, btp_stack *s)
{
    return var((string)n, s);
}
#endif

/** @brief Find the variable with the given name.

    Returns a pointer to the named variable. If the name contains one or
    more field separators then the function looks for a variable whose
    name matches exactly. If the name contains no field separators then
    the funciton looks first in the top level and then in all subsequent
    levels and returns the first occurrence found. In general, this
    function searches constructor types in the order in which they appear
    in the DDS, but there is no requirement that it do so. 

    @note If a dataset contains two constructor types which have field names
    that are the same (say point.x and pair.x) you should use fully qualified
    names to get each of those variables.

    @param n The name of the variable to find.
    @param s If given, this value-result parameter holds the path to the 
    returned BaseType. Thus, this method can return the FQN for the variable
    \e n.
    @return A BaseType pointer to the variable or null if not found. */
BaseType *
DDS::var(const string &n, btp_stack *s)
{
    string name = www2id(n);
    BaseType *v = exact_match(name, s);
    if (v)
	return v;

    return leaf_match(name, s);
}

BaseType *
DDS::leaf_match(const string &n, btp_stack *s) 
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
	BaseType *btp = *i;
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
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
	BaseType *btp = *i;
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


/** @brief Returns the first variable in the DDS. */

DDS::Vars_iter
DDS::var_begin()
{
    return vars.begin();
}

DDS::Vars_riter
DDS::var_rbegin()
{
    return vars.rbegin();
}

DDS::Vars_iter
DDS::var_end()
{
    return vars.end() ;
}

DDS::Vars_riter
DDS::var_rend()
{
    return vars.rend() ;
}

/** Return the iterator for the \e ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
DDS::Vars_iter
DDS::get_vars_iter(int i)
{
    return vars.begin() + i;
}

/** Return the \e ith variable.
    @param i the index
    @return The corresponding variable */
BaseType *
DDS::get_var_index(int i)
{
    return *(vars.begin() + i);
}

/** @brief Returns the number of variables in the DDS. */
int
DDS::num_var()
{
    return vars.size();
}

/////////////////////// Mostly CE evaluator below this point //////////////

void
DDS::timeout_on()
{
#ifndef WIN32
    alarm(d_timeout);
#endif
}

void 
DDS::timeout_off()
{
#ifndef WIN32
    d_timeout = alarm(0);
#endif
}

void
DDS::set_timeout(int t)
{
	//  Has no effect under win32
    d_timeout = t;
}

int
DDS::get_timeout()
{
	//  Has to effect under win32
    return d_timeout;
}

DDS::Clause_iter
DDS::clause_begin()
{
    return expr.begin() ;
}

DDS::Clause_iter
DDS::clause_end()
{
    return expr.end() ;
}

bool
DDS::clause_value(Clause_iter &iter, const string &dataset)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");

    return (*iter)->value(dataset, *this);
}

/** @brief Add a clause to a constraint expression.

    This function adds an operator clause to the constraint
    expression. 

    @param op An integer indicating the operator in use.  These
    values are generated by \c bison.
    @param arg1 A pointer to the argument on the left side of the
    operator. 
    @param arg2 A pointer to a list of the arguments on the right
    side of the operator.
*/
void
DDS::append_clause(int op, rvalue *arg1, rvalue_list *arg2)
{
    Clause *clause = new Clause(op, arg1, arg2);

    expr.push_back(clause);
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

    expr.push_back(clause);
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

    expr.push_back(clause);
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
    constants.push_back(btp);
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

/** @brief Add a boolean function to the list. */
void
DDS::add_function(const string &name, bool_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Add a BaseType function to the list. */
void
DDS::add_function(const string &name, btp_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Add a projection function to the list. */
void
DDS::add_function(const string &name, proj_func f)
{
    function func(name, f);
    functions.push_back(func);
}

/** @brief Find a Boolean function with a given name in the function list. */
bool
DDS::find_function(const string &name, bool_func *f) const
{
    if (functions.empty())
	return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
	{
	    if (name == (*i).name && (*f = (*i).b_func)) {
		return true;
	    }
	}

    return false;
}

/** @brief Find a BaseType function with a given name in the function list. */
bool
DDS::find_function(const string &name, btp_func *f) const
{
    if (functions.empty())
	return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
	{
	    if (name == (*i).name && (*f = (*i).bt_func)) {
		return true;
	    }
	}

    return false;
}

/** @brief Find a projection function with a given name in the function list. */
bool
DDS::find_function(const string &name, proj_func *f) const
{
    if (functions.empty())
	return false;

    for (Functions_citer i = functions.begin(); i != functions.end(); i++)
	if (name == (*i).name && (*f = (*i).p_func)) {
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

    Clause *cp = expr[0] ;
    return cp->value_clause();
}

/** @brief Evaluate a function-valued constraint expression. */
BaseType *
DDS::eval_function(const string &dataset)
{
    if (expr.size() != 1)
	throw InternalErr(__FILE__, __LINE__, 
			  "The length of the list of CE clauses is not 1.");

    Clause *cp = expr[0] ;
    BaseType *result;
    if (cp->value(dataset, *this, &result))
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
    for (Clause_iter i = expr.begin(); i != expr.end(); i++)
    {
	boolean = boolean && (*i)->boolean_clause();
    }
    
    return boolean;
}

/** @brief Traverse DDS, set Sequence leaf nodes. */
void
DDS::tag_nested_sequences()
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        if ((*i)->type() == dods_sequence_c)
            dynamic_cast<Sequence&>(**i).set_leaf_sequence();
        else if ((*i)->type() == dods_structure_c)
            dynamic_cast<Structure&>(**i).set_leaf_sequence();
    }
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
    for (Clause_iter i = expr.begin(); i != expr.end() && result; i++)
    {
	// A selection expression *must* contain only boolean clauses!
	if(!((*i)->boolean_clause()))
	    throw InternalErr(__FILE__, __LINE__, 
                "A selection expression must contain only boolean clauses.");
	result = result && (*i)->value(dataset, *this);
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

    try {
	parse(in);
	fclose(in);
    }
    catch (Error &e) {
	fclose(in);
	throw e;
    }
}


/** @brief Parse a DDS from a file indicated by the input file descriptor. */
void
DDS::parse(int fd)
{
#ifdef WIN32
    FILE *in = fdopen(_dup(fd), "r");
#else
    FILE *in = fdopen(dup(fd), "r");
#endif

    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    try {
	parse(in);
	fclose(in);
    }
    catch (Error &e) {
	fclose(in);
	throw e;
    }
}

/** @brief Parse a DDS from a file indicated by the input file descriptor. 
    Read the persistent representation of a DDS from the FILE *in, parse it
    and create a matching binary object. 
    @param in Read the persistent DDS from this FILE*.
    @exception InternalErr Thrown if \c in is null
    @exception Error Thrown if the parse fails. */
void
DDS::parse(FILE *in)
{
    if (!in) {
	throw InternalErr(__FILE__, __LINE__, "Null input stream.");
    }

    void *buffer = dds_buffer(in);
    dds_switch_to_buffer(buffer);

    parser_arg arg(this);

    bool status = ddsparse((void *)&arg) == 0;

    dds_delete_buffer(buffer);

    DBG(cout << "Status from parser: " << status << endl);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
	if (arg.error())
	  throw *arg.error();
    }
}

/** @brief Print the entire DDS to the specified file. */
void
DDS::print(FILE *out)
{
    fprintf( out, "Dataset {\n" ) ;

    for( Vars_citer i = vars.begin(); i != vars.end(); i++ )
    {
	(*i)->print_decl( out ) ;
    }

    fprintf( out, "} %s;\n", id2www(name).c_str() ) ;

    return ;
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
    fprintf( out, "Dataset {\n" ) ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++)
    {
	// for each variable, indent with four spaces, print a trailing
	// semi-colon, do not print debugging information, print only
	// variables in the current projection.
	(*i)->print_decl( out, "    ", true, false, true ) ;
    }

    fprintf( out, "} %s;\n", id2www(name).c_str() ) ;

    return;
}

class VariablePrintXML : public unary_function<BaseType *, void> {
    FILE *d_out;
    bool d_constrained;
public:
    VariablePrintXML(FILE *out, bool constrained) 
	: d_out(out), d_constrained(constrained) {}
    void operator()(BaseType *bt) {
	bt->print_xml(d_out, "    ", d_constrained);
    }
};

/** Print an XML represnetation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dodsBLOB tag.

    @param out Destination.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression. 
    @param blob The dodsBLOB URL. */
void
DDS::print_xml(FILE *out, bool constrained, const string &blob)
{
    fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    
    fprintf(out, "<Dataset name=\"%s\"\n", id2xml(name).c_str());

    fprintf(out, "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprintf(out, "xmlns=\"%s\"\n", dods_namespace.c_str());
    fprintf(out, "xsi:schemaLocation=\"%s  %s\">\n\n", 
	    dods_namespace.c_str(), default_schema_location.c_str());

    d_attr.print_xml(out, "    ", constrained);

    fprintf(out, "\n");
	
    for_each(var_begin(), var_end(), VariablePrintXML(out, constrained));
	
    fprintf(out, "\n");

    fprintf(out, "    <dodsBLOB URL=\"%s\"/>\n", blob.c_str());
	
    fprintf(out, "</Dataset>\n");
}

// Used by DDS::send() when returning data from a function call.
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
	for (Vars_iter i = vars.begin(); i != vars.end(); i++)
	    if (!(*i)->check_semantics(msg, true))
		return false;

    return true;
}

/** @brief Parse the constraint expression given the current DDS. 

    Evaluate the constraint expression; return the value of the expression.
    As a side effect, mark the DDS so that BaseType's mfuncs can be used to
    correctly read the variable's value and send it to the client.

    @param constraint A string containing the constraint expression.
    @exception Throws Error if the constraint does not parse. */
void
DDS::parse_constraint(const string &constraint)
{
    void *buffer = expr_string(constraint.c_str());
    expr_switch_to_buffer(buffer);

    parser_arg arg(this);

    // For all errors, exprparse will throw Error. 
    exprparse((void *)&arg);

    expr_delete_buffer(buffer);
}


// These three functions are defined here because they are used by the
// deprecated DDS::send method. 07/24/03 jhrg

// We start two sinks, one for regular data and one for XDR encoded data.
int
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
void
clean_sinks(int childpid, bool compressed, XDR *xdr_sink, FILE *comp_sink)
{
    delete_xdrstdio(xdr_sink);
    
    if (compressed) {
	if (fclose(comp_sink)) {
	    DBG(cerr << "clean_sinks - Failed to close " << (void *)comp_sink 
		<< endl);
	}

	int pid = 0 ;
#ifdef WIN32
	while ((pid = _cwait(NULL, childpid, NULL)) > 0)
#else
	while ((pid = waitpid(childpid, 0, 0)) > 0)
#endif
	DBG2(cerr << "pid: " << pid << endl);
    }
}

#if OLD_DDS_TRANS_CODE
static void
print_variable(FILE *out, BaseType *var, bool constrained = false)
{
    if(!out)
	throw InternalErr(__FILE__, __LINE__, 
			  "Invalid file descriptor, NULL pointer!");
    if(!var)
	throw InternalErr(__FILE__, __LINE__, 
     "Passing NULL variable to method DDS::print_variable in *this* object.");

    fprintf( out, "Dataset {\n" ) ;

    var->print_decl( out, "    ", true, false, constrained ) ;

    fprintf( out, "} function_value;\n" ) ;

    return;
}
#endif

#if OLD_DDS_TRANS_CODE
/** This function sends the variables described in the constrained DDS to
    the output described by the FILE pointer. This function calls
    <tt>parse_constraint()</tt>, <tt>BaseType::read()</tt>, and
    <tt>BaseType::serialize()</tt>.

    @deprecated Use method in DODSFilter instead. 07/24/03 jhrg

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

    parse_constraint(constraint);
  
    tag_nested_sequences();             // Must call this after CE parse.
    
    bool status = true;
  
    // Handle *functional* constraint expressions specially 
    if (functional_expression()) {
	BaseType *var = eval_function(dataset);
	if (!var)
	    throw Error(unknown_error, "Error calling the CE function.");

	set_mime_binary(out, dods_data, cgi_ver,
			(compressed) ? deflate : x_plain, lmt);
	fflush(out);
      
	FILE *comp_sink;
	XDR *xdr_sink;
	int childpid = get_sinks(out, compressed, &comp_sink, &xdr_sink);
      
	print_variable((compressed) ? comp_sink : out, var, true);
	fprintf((compressed) ? comp_sink : out, "Data:\n");
	fflush((compressed) ? comp_sink : out);
      
	// In the following call to serialize, suppress CE evaluation.
	status = var->serialize(dataset, *this, xdr_sink, false);
      
	clean_sinks(childpid, compressed, xdr_sink, comp_sink);
    }
    else {
	set_mime_binary(out, dods_data, cgi_ver,
			(compressed) ? deflate : x_plain, lmt);
	fflush(out);

	FILE *comp_sink;
	XDR *xdr_sink;
	int childpid = get_sinks(out, compressed, &comp_sink, &xdr_sink);

	// send constrained DDS	    
	print_constrained((compressed) ? comp_sink : out);
	fprintf((compressed) ? comp_sink : out, "Data:\n");
	fflush((compressed) ? comp_sink : out);

	for (Vars_iter i = vars.begin(); i != vars.end(); i++)
	    if ((*i)->send_p()) // only process projected variables
		status = status && (*i)->serialize(dataset, *this,
						   xdr_sink, true);

	clean_sinks(childpid, compressed, xdr_sink, comp_sink);
    }

    return status;
}
#endif

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

    @todo This should throw an exception on error!!!

    @todo These methods that use the btp_stack to keep track of the path from
    the top of a dataset to a particular variable can be rewritten to use the
    parent field instead. 

    @todo All the methods that use names to identify variables should have
    counterparts that take BaseType pointers.
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

    delete s ; s = 0;

    return true;
}

/** Mark the member variable <tt>send_p</tt> flags to
    <i>state</i>. 

    @return Void
*/
void
DDS::mark_all(bool state)
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
	(*i)->set_send_p(state);
}

