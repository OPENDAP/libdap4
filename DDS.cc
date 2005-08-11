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

#include "config_dap.h"

static char rcsid[] not_used = {"$Id$"};

#ifdef __GNUG__
// #pragma implementation
#endif

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

#include <assert.h>
#include <iostream>
#include <algorithm>
#include <functional>

#include "GNURegex.h"

#include "expr.h"
#include "Clause.h"
#include "DAS.h"
#include "AttrTable.h"
#include "DDS.h"
#include "Error.h"
#include "InternalErr.h"
#include "BTIterAdapter.h"
#include "ClauseIterAdapter.h"

#include "parser.h"
#include "debug.h"
#include "util.h"
#include "escaping.h"
#include "ce_functions.h"
#include "cgi_util.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

const string default_schema_location = "http://argon.coas.oregonstate.edu/ndp/dods.xsd";
const string dods_namespace = "http://www.dods.org/ns/DODS";

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
    : d_factory(factory), d_local_basetype_factory(false), 
      name(n), d_timeout(0)
{
    add_function("length", func_length);
    add_function("grid", func_grid_select);
}

// #ifdef DEFAULT_BASETYPE_FACTORY
/** Creates a DDS with the given string for its name. Uses the defaut
    BaseType Factory (which instantiates the Byte, ..., Grid classes defined
    here. 
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method. */
DDS::DDS(const string &n) : d_factory(new BaseTypeFactory), 
			    d_local_basetype_factory(true), 
			    name(n), d_timeout(0)
{
    add_function("length", func_length);
    add_function("grid", func_grid_select);
}
// #endif

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

    if (d_local_basetype_factory) {
	delete d_factory; d_factory = 0;
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
static bool
is_in_kill_file(const string &name)
{
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dimension' attributes.

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
DDS::transfer_attributes(DAS *das)
{
    for (AttrTable::Attr_iter i = das->attr_begin(); i!=das->attr_end(); ++i) {
	AttrTable::entry *ep = *i;
	bool found = false;

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
        
        // Look for a variable with the same as the attribute container. 
        // Assume broken servers may provide attribute container names that
        // match variables within Structures, et c., without correctly 
        // nesting the attributes. So, use DDS::var() which will recurrsively
        // search nested constructor types using only a leaf name.
        BaseType *btp = var(ep->name);
        if (btp) {
            found = true;
            transfer_attr(das, ep, btp, suffix);
        }
        
	if (!found)
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
Pix 
DDS::first_var()
{
    BTIterAdapter *i = new BTIterAdapter( vars ) ;
    i->first();
    return i;
}

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

/** @brief Increments the DDS variable counter to point at the next
    variable. */
void 
DDS::next_var(Pix p)
{ 
    p.next() ;
}

/** Returns a pointer to the indicated variable. */
BaseType *
DDS::var(Pix p)
{
    BTIterAdapter *i = (BTIterAdapter *)p.getIterator() ;
    if( i ) {
	return i->entry() ;
    }
    return 0 ;
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

/** Returns a pointer to the first clause in a parsed constraint
    expression. */
Pix
DDS::first_clause()
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");

    ClauseIterAdapter *i = new ClauseIterAdapter( expr ) ;

    i->first() ;
    return i ;
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

/** Increments a pointer to indicate the next clause in a parsed
    constraint expression. */
void
DDS::next_clause(Pix p)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");
    p.next() ;
}

/** Returns a clause of a parsed constraint expression. 

    The return type of this method was changed on the release-3-2 branch. */
Clause *
DDS::clause(Pix p)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");
    ClauseIterAdapter *i = (ClauseIterAdapter *)p.getIterator() ;
    if( !i )
	throw InternalErr(__FILE__, __LINE__, 
			 "No ClauseIterAdapter defined for *this* DDS object.");

    return i->entry() ;
}

/** Returns the value of the indicated clause of a constraint
    expression. */
bool
DDS::clause_value(Pix p, const string &dataset)
{
    if(expr.empty())
	throw InternalErr(__FILE__, __LINE__, 
			  "There are no CE clauses for *this* DDS object.");

    ClauseIterAdapter *i = (ClauseIterAdapter *)p.getIterator() ;
    if( !i )
	throw InternalErr(__FILE__, __LINE__, 
			 "No ClauseIterAdapter defined for *this* DDS object.");

    return i->entry()->value(dataset, *this);
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
#if 0
	BaseType *btp = *i;

	if (btp->type() == dods_sequence_c) {	  
	    DBG(cerr << "Found Sequence" << name << " in: " << btp << endl);

	    ((Sequence *)btp)->set_leaf_sequence();
	}
#endif
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

/** @brief Print the entire DDS on the specified output stream. 

    Write structure from tables to OUT (which defaults to stdout). 

    @return TRUE
*/
void
DDS::print(ostream &os)
{
    os << "Dataset {" << endl;

    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
	(*i)->print_decl(os);

    os << "} " << id2www(name) << ";" << endl;
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

    for (Vars_iter i = vars.begin(); i != vars.end(); i++)
	// for each variable, indent with four spaces, print a trailing
	// semi-colon, do not print debugging information, print only
	// variables in the current projection.
	(*i)->print_decl(os, "    ", true, false, true);

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

/** @brief Parse the constraint expression given the current DDS. 

    Evaluate the constraint expression; return the value of the
    expression. As a side effect, mark the DDS so that BaseType's
    mfuncs can be used to correctly read the variable's value and
    send it to the client. 

    @deprecated Use the single argument version.
    @return void
    @param constraint A string containing the constraint
    expression. 
    @param os The output stream on which to write error objects and
    messages. 
    @param server If true, send errors back to client instead of
    displaying errors on the default output stream. */ 
void
DDS::parse_constraint(const string &constraint, ostream &, bool)
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

    @deprecated This version requires the second param even though it's never
    used. Use the single argument version.

    @return void
    @param constraint A string containing the constraint
    expression. 
    @param out A FILE pointer to which error objects should be wrtten. 
    @param server If true, send errors back to client instead of
    displaying errors on the default output stream. 
*/
void
DDS::parse_constraint(const string &constraint, FILE *, bool)
{
    void *buffer = expr_string( constraint.c_str( ) ) ;
    expr_switch_to_buffer( buffer ) ;

    parser_arg arg( this ) ;

    // For all errors, exprparse will throw Error. 
    exprparse( (void *)&arg ) ;

    expr_delete_buffer( buffer ) ;

    return;
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

    parse_constraint(constraint, out, true);
  
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

// $Log: DDS.cc,v $
// Revision 1.78  2005/04/07 22:32:47  jimg
// Updated doxygen comments: fixed errors; updated comments about set_read_p.
// Removed the VirtualCtor classes. Added a README about the factory
// classes.
//
// Revision 1.77  2005/03/30 21:35:54  jimg
// Now uses the BaseTypeFactory class.
//
// Revision 1.76  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.62.2.16  2005/01/19 17:16:32  jimg
// Added set_leaf_sequence() to Structure to mirror its use in DDS. This
// code will now correctly mark Sequences that are parts of Structures.
//
// Revision 1.62.2.15  2005/01/18 23:12:40  jimg
// FIxed documentation. Added instrumentation. minor changes to
// tag_nested_sequences().
//
// Revision 1.62.2.14  2005/01/14 20:57:54  jimg
// Added a call to tag_nested_sequnece() to DDS::send().
//
// Revision 1.62.2.13  2004/12/23 20:53:10  dan
// Added method tag_nested_sequences() that traverses Sequence
// members to set the leaf_node for the innermost Sequence.
//
// Revision 1.75  2004/11/16 17:57:05  jimg
// Added accessors for reverse iterators.
//
// Revision 1.74  2004/08/25 23:34:48  jimg
// I modified DDS::transfer_attributes() so that the _dim_? attributes
// generated by the HDF4 server will be added to their variable's attr
// table directly and not result in a nested table. This seems closer to
// the organization clients expect.
//
// Revision 1.73  2004/08/03 23:09:45  jimg
// I fixed a number of problems with the transfer_attributes() method and its
// support functions. First, instead of searching the top level variables, the
// code now searches using the var() method, which can find variables when only
// the leaf node name is given. The code that processes global attributes is now
// much smarter about what a global attribute container really looks like (it
// ignores empty containers if another candidate is present and actually has
// some attributes). Finally, the _dim_0, _dim_1, et c., attributes generated by
// the HDF4 server are now grouped with the variable to which they refer. Before
// they were lost.
//
// Revision 1.72  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.62.2.12  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.71  2004/06/28 16:59:43  pwest
// Inherit DDS and DAS from DODSResponseObject
//
// Revision 1.62.2.11  2004/04/09 15:48:37  jimg
// I had some assert()s in the code that tested otherwise unused variables. When
// NDEBUG was defined this generated warnings. I decided the assert()s we less
// useful than the warnings were annoying.
//
// Revision 1.70  2004/03/10 16:29:18  jimg
// Repairs to the methods which provide access using iterators. These
// were using '*_iter &' type params and that made newer versions of g++
// gag. I'm not absolutely sure what the problem was, but making the
// parameters regular value params and not references fixed it.
//
// Revision 1.69  2004/03/01 22:32:10  jimg
// Bring the trunk up to date with the code in my working directory...
//
// Revision 1.68  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.62.2.10  2004/02/16 12:42:14  rmorris
// Well, everybody really needs assert.
//
// Revision 1.62.2.9  2004/02/15 21:17:21  rmorris
// Win32 needs assert.h when one uses assert.
//
// Revision 1.62.2.8  2004/02/13 18:22:28  jimg
// I added code to catch throws in the parse() methods and close open files.
// DDS::parse(FILE *in) does NOT do this because 'in' defaults to stdin.
//
// Revision 1.62.2.7  2004/02/11 22:26:46  jimg
// Changed all calls to delete so that whenever we use 'delete x' or
// 'delete[] x' the code also sets 'x' to null. This ensures that if a
// pointer is deleted more than once (e.g., when an exception is thrown,
// the method that throws may clean up and then the catching method may
// also clean up) the second, ..., call to delete gets a null pointer
// instead of one that points to already deleted memory.
//
// Revision 1.62.2.6  2004/02/04 00:05:11  jimg
// Memory errors: I've fixed a number of memory errors (leaks, references)
// found using valgrind. Many remain. I need to come up with a systematic
// way of running the tests under valgrind.
//
// Revision 1.67  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.66  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.65  2003/09/25 22:33:39  jimg
// Made local functions static.
//
// Revision 1.62.2.5  2003/09/06 22:27:26  jimg
// Updated the documentation. Removed some old code.
//
// Revision 1.62.2.4  2003/08/17 01:37:54  rmorris
// Removed "smart timeout" functionality under win32.  alarm() and
// SIGALRM are not supported under win32 - plus that functionality
// appears to be server-side only and win32 is client-side only.
//
// Revision 1.62.2.3  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.62.2.2  2003/07/23 23:56:36  jimg
// Now supports a simple timeout system.
//
// Revision 1.62.2.1  2003/07/16 04:22:39  jimg
// Fixed a bug (part of #635) where compressed data responses needed to have the
// stream flushed. I added calls to make sure the stream was flushed at two
// critical spots, one where the headers are written and one where the DDS is
// written.
//
// Revision 1.64  2003/05/30 16:51:45  jimg
// Modified transfer_attrs() so that aliases are supported. Unfortunately,
// aliases are broken in the C++ code and in their design, so this addition/fix
// is of limited use.
//
// Revision 1.63  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.62  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.60.2.3  2003/04/18 03:09:58  jimg
// I combined some of the win32 #ifdef code and removed some old code.
//
// Revision 1.60.2.2  2003/04/15 01:17:12  jimg
// Added a method to get the iterator for a variable (or map) given its
// index. To get the iterator for the ith variable/map, call
// get_vars_iter(i).
//
// Revision 1.61  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.60.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.60  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.59  2003/01/15 19:24:39  pwest
// Removing IteratorAdapterT and replacing with non-templated versions.
//
// Revision 1.58  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.53.4.22  2002/12/31 16:43:20  rmorris
// Patches to handle some of the fancier template code under VC++ 6.0.
//
// Revision 1.53.4.21  2002/12/27 19:34:42  jimg
// Modified the var() methods so that www2id() is called before looking
// up identifier names. See bug 563.
//
// Revision 1.53.4.20  2002/12/20 00:53:52  jimg
// I removed the static void print_variable(...) because it's no longer
// used by our code.
//
// Revision 1.53.4.19  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.53.4.18  2002/11/21 21:24:17  pwest
// memory leak cleanup and file descriptor cleanup
//
// Revision 1.53.4.17  2002/11/06 22:56:52  pwest
// Memory delete errors and uninitialized memory read errors corrected
//
// Revision 1.53.4.16  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.53.4.15  2002/10/02 17:50:36  pwest
// Added two new del_vars methods. The first takes an iterator and deltes the
// variable referenced by that iterator. The iterator now points to the element
// after the deleted element. The second method takes two iterators and will
// delete the variables starting from the first iterator and up to, not
// including the second iterator.
//
// Revision 1.53.4.14  2002/09/22 14:27:51  rmorris
// VC++ doesn't allow consider x in 'for(int x,...)' to be only for that
// scope of the block for that loop - therefor multiple of these in the
// same function are illegal because the 'x' is considered multiply
// declared.  Removed.
//
// Revision 1.53.4.13  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.53.4.12  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.53.4.11  2002/08/22 21:23:23  jimg
// Fixes for the Win32 Build made at ESRI by Vlad Plenchoy and myslef.
//
// Revision 1.53.4.10  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.53.4.9  2002/06/20 03:18:48  jimg
// Fixes and modifications to the Connect and HTTPConnect classes. Neither
// of these two classes is complete, but they should compile and their
// basic functions should work.
//
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

