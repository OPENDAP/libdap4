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

static char rcsid[] not_used =
    {"$Id$"
    };

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

//#define DODS_DEBUG

#include "GNURegex.h"

#include "DAS.h"
#include "Clause.h"
#include "Error.h"
#include "InternalErr.h"

#include "parser.h"
#include "debug.h"
#include "util.h"
#include "escaping.h"

const string default_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
const string dods_namespace = "http://xml.opendap.org/ns/DAP2";

using namespace std;

void ddsrestart(FILE *yyin); // Defined in dds.tab.c
int ddsparse(void *arg);

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
    for (Vars_iter i = dds_tmp.var_begin(); i != dds_tmp.var_end(); i++) {
        add_var(*i); // add_var() dups the BaseType.
    }
}

/** Make a DDS which uses the given BaseTypeFactory to create variables.
    @param n The name of the dataset. Can also be set using the
    set_dataset_name() method.
    @param factory BaseTypeFactory which instantiates Byte, ..., Grid. The
    caller is responsible for freeing the object \e after deleting this DDS.
    Can also be set using set_factory(). Never delete until just before
    deleting the DDS itself unless you intend to replace the factory with a
    new instance.
    @param n The name of the data set. Can also be set using
    set_dataset_name(). */
DDS::DDS(BaseTypeFactory *factory, const string &n)
        : d_factory(factory), name(n), d_timeout(0)
{}

/** The DDS copy constructor. */
DDS::DDS(const DDS &rhs) : DapObj()
{
    duplicate(rhs);
}

DDS::~DDS()
{
    // delete all the variables in this DDS
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ; btp = 0;
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

/** @brief Look for the parent of an HDF4 dimension attribute

    If this attribute container's name ends in the '_dim_?' suffix, look
    for the variable to which it's attributes should be bound: For an array,
    they should be held in a sub-table of the array; for a Structure or
    Sequence, I don't think the HDF4 handler ever makes these (since those
    types don't have 'dimension' in hdf-land);  and for a Grid, the attributes
    belong with the map variables.

    @note This method does check that the \e source really is an hdf4 dimension
    attribute.

    @param source The attribute container, an AttrTable::entry instance.
    @return the BaseType to which these attributes belong or null if none
    was found. */
BaseType *
DDS::find_hdf4_dimension_attribute_home(AttrTable::entry *source)
{
    BaseType *btp;
    string::size_type i = source->name.find("_dim_");
    if (i != string::npos && (btp = var(source->name.substr(0, i)))) {
        if (btp->is_vector_type()) {
            return btp;
        }
        else if (btp->type() == dods_grid_c) {
            // For a Grid, the hdf4 handler uses _dim_n for the n-th Map
            // i+5 points to the character holding 'n'
            int n = atoi(source->name.substr(i + 5).c_str());
            DBG(cerr << "Found a Grid (" << btp->name() << ") and "
                << source->name.substr(i) << ", extracted n: " << n << endl);
            return *(dynamic_cast<Grid&>(*btp).map_begin() + n);
        }
    }

    return 0;
}

/** Given an attribute container from a DAS, find or make a destination
    for its contents in the DDS.
    @param source Attribute table
    @param dest_variable result param that holds the BaseType
    @return Destination Attribute Table */
AttrTable *
DDS::find_matching_container(AttrTable::entry *source, BaseType **dest_variable)
{
    // The attribute entry 'source' must be a container
    if (source->type != Attr_container)
        throw InternalErr(__FILE__, __LINE__, "DDS::find_matching_container");

    // Use the name of the attribute container 'source' to figure out where
    // to put its contents.
    BaseType *btp;
    if ((btp = var(source->name))) {
        // ... matches a variable name? Use var's table
        *dest_variable = btp;
        return &btp->get_attr_table();
    }
    else if ((btp = find_hdf4_dimension_attribute_home(source))) {
        // ... hdf4 dimension attribute? Make a sub table and use that.
        // btp can only be an Array or a Grid Map (which is an array)
        if (btp->get_parent() && btp->get_parent()->type() == dods_grid_c) {
            DBG(cerr << "Found a Grid, assigning to the map" << endl);
            *dest_variable = btp;
            return &btp->get_attr_table();
        }
        else { // must ba a plain Array
            string::size_type i = source->name.find("_dim_");
            string ext = source->name.substr(i + 1);
            *dest_variable = btp;
            return btp->get_attr_table().append_container(ext);
        }
    }
    else {
        // ... otherwise assume it's a global attribute.
        AttrTable *at = d_attr.find_container(source->name);
        if (!at) {
            at = new AttrTable();       // Make a new global table if needed
            d_attr.append_container(at, source->name);
        }

        *dest_variable = 0;
        return at;
    }
}

/** Given a DAS object, scavenge attributes from it and load them into this
    object and the variables it contains.

    If a DAS contans attributes from the current (8/2006) HDF4 server with
    names like var_dim_0, var_dim_1, then make those attribute tables
    sub tables of the \e var table.

    @todo Generalize the code that treats the _dim_? attributes or make
    is obsolete by fixing the HDF4 server.

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
    // foreach container at the outer level
    AttrTable::Attr_iter das_i = das->attr_begin();
    while (das_i != das->attr_end()) {
        DBG(cerr << "Working on the '" << (*das_i)->name << "' container."
            << endl);

        AttrTable *source = (*das_i)->attributes;
        // Variable that holds 'dest'; null for a global attribute.
        BaseType *dest_variable = 0;
        AttrTable *dest = find_matching_container(*das_i, &dest_variable);

        // foreach source attribute in the das_i container
        AttrTable::Attr_iter source_p = source->attr_begin();
        while (source_p != source->attr_end()) {
            DBG(cerr << "Working on the '" << (*source_p)->name << "' attribute"
                << endl);

            // If this is container, we must have a container (this one) within
            // a container (the 'source'). Look and see if the variable is a
            // Constructor. If so, pass that container into
            // Constructor::transfer_attributes()
            if ((*source_p)->type == Attr_container) {
                if (dest_variable && dest_variable->is_constructor_type()) {
                    dynamic_cast<Constructor&>(*dest_variable).transfer_attributes(*source_p);
                }
                else {
                    dest->append_container(new AttrTable(*(*source_p)->attributes),
                                           (*source_p)->name);
                }
            }
            else {
                dest->append_attr(source->get_name(source_p),
                                  source->get_type(source_p),
                                  source->get_attr_vector(source_p));
            }

            ++source_p;
        }

        ++das_i;
    }
}

/** Get and set the dataset's name.  This is the name of the dataset
    itself, and is not to be confused with the name of the file or
    disk on which it is stored.

    @name Dataset Name Accessors */

//@{

/** Returns the dataset's name. */
string
DDS::get_dataset_name() const
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

/** @brief Adds a copy of the variable to the DDS.
    Using the ptr_duplicate() method, perform a deep copy on the variable
    \e bt and adds the result to this DDS.
    @note The copy will not copy data values.
    @param bt Source variable. */
void
DDS::add_var(BaseType *bt)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__,
                          "Trying to add a BaseType object with a NULL pointer.");

    DBG2(cerr << "In DDS::add_var(), bt's address is: " << bt << endl);

    BaseType *btp = bt->ptr_duplicate();
    DBG2(cerr << "In DDS::add_var(), btp's address is: " << btp << endl);
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
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
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
    if (i != vars.end()) {
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
    for (Vars_iter i_tmp = i1; i_tmp != i2; i_tmp++) {
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
        DBG2(cerr << "Looking at " << n << " in: " << btp << endl);
        // Look for the name in the dataset's top-level
        if (btp->name() == n) {
            DBG2(cerr << "Found " << n << " in: " << btp << endl);
            return btp;
        }
        if (btp->is_constructor_type() && (btp = btp->var(n, false, s))) {
            return btp;
        }
    }

    return 0;   // It is not here.
}

BaseType *
DDS::exact_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = vars.begin(); i != vars.end(); i++) {
        BaseType *btp = *i;
        DBG2(cerr << "Looking for " << name << " in: " << btp << endl);
        // Look for the name in the current ctor type or the top level
        if (btp->name() == name) {
            DBG2(cerr << "Found " << name << " in: " << btp << endl);
            return btp;
        }
    }

    string::size_type dot_pos = name.find(".");
    if (dot_pos != string::npos) {
        string aggregate = name.substr(0, dot_pos);
        string field = name.substr(dot_pos + 1);

        BaseType *agg_ptr = var(aggregate, s);
        if (agg_ptr) {
            DBG2(cerr << "Descending into " << agg_ptr->name() << endl);
            return agg_ptr->var(field, true, s);
        }
        else
            return 0;  // qualified names must be *fully* qualified
    }

    return 0;   // It is not here.
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

    bool status = ddsparse((void *) & arg) == 0;

    dds_delete_buffer(buffer);

    DBG2(cout << "Status from parser: " << status << endl);

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
    fprintf(out, "Dataset {\n") ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        (*i)->print_decl(out) ;
    }

    fprintf(out, "} %s;\n", id2www(name).c_str()) ;

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
    fprintf(out, "Dataset {\n") ;

    for (Vars_citer i = vars.begin(); i != vars.end(); i++) {
        // for each variable, indent with four spaces, print a trailing
        // semi-colon, do not print debugging information, print only
        // variables in the current projection.
        (*i)->print_decl(out, "    ", true, false, true) ;
    }

    fprintf(out, "} %s;\n", id2www(name).c_str()) ;

    return;
}

class VariablePrintXML : public unary_function<BaseType *, void>
{
    FILE *d_out;
    bool d_constrained;
public:
    VariablePrintXML(FILE *out, bool constrained)
            : d_out(out), d_constrained(constrained)
    {}
    void operator()(BaseType *bt)
    {
        bt->print_xml(d_out, "    ", d_constrained);
    }
};

/** Print an XML represnetation of this DDS. This method is used to generate
    the part of the DDX response. The \c Dataset tag is \e not written by
    this code. The caller of this method must handle writing that and
    including the \c dataBLOB tag.

    @param out Destination.
    @param constrained True if the output should be limited to just those
    variables that are in the projection of the current constraint
    expression.
    @param blob The dataBLOB href. */
void
DDS::print_xml(FILE *out, bool constrained, const string &)
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

    fprintf(out, "    <dataBLOB href=\"\"/>\n");

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

    DBG2(cerr << "Looking for " << n << endl);

    BaseType *variable = var(n, s);
    if (!variable) {
        DBG2(cerr << "Could not find variable " << n << endl);
        return false;
    }
    variable->set_send_p(state);
    DBG2(cerr << "Set variable " << variable->name() << endl);

    // Now check the btp_stack and run BaseType::set_send_p for every
    // BaseType pointer on the stack.
    while (!s->empty()) {
        s->top()->BaseType::set_send_p(state);
        DBG2(cerr << "Set variable " << s->top()->name() << endl);
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

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DDS::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "DDS::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    strm << DapIndent::LMarg << "name: " << name << endl ;
    strm << DapIndent::LMarg << "filename: " << _filename << endl ;
    strm << DapIndent::LMarg << "protocol major: " << d_protocol_major << endl;
    strm << DapIndent::LMarg << "protocol minor: " << d_protocol_minor << endl;
    strm << DapIndent::LMarg << "factory: " << (void *)d_factory << endl ;

    strm << DapIndent::LMarg << "global attributes:" << endl ;
    DapIndent::Indent() ;
    d_attr.dump(strm) ;
    DapIndent::UnIndent() ;

    if (vars.size()) {
        strm << DapIndent::LMarg << "vars:" << endl ;
        DapIndent::Indent() ;
        Vars_citer i = vars.begin() ;
        Vars_citer ie = vars.end() ;
        for (; i != ie; i++) {
            (*i)->dump(strm) ;
        }
        DapIndent::UnIndent() ;
    }
    else {
        strm << DapIndent::LMarg << "vars: none" << endl ;
    }

    DapIndent::UnIndent() ;
}

///////// bone yard for old transfer attributes code /////////


// Old content of the transfer_attributes() method
#if 0
AttrTable::Attr_iter i = das->attr_begin();
while (i != das->attr_end())
{
    // NB: (*i) == AttrTable::entry*;

    // This code should be making the <name>_dim_0, ..., attributes
    // into 'dim_0' containers within the <name> container. Let specific
    // clients handle the nested attributes however they want to. See
    // ticket #480.
#if 0
    string::size_type dim_pos = (*i)->name.find("_dim_");
#endif
    string sub_table = "";
#if 0
    if (dim_pos != string::npos) {
        sub_table = (*i)->name.substr(dim_pos);
        (*i)->name = (*i)->name.substr(0, dim_pos);
    }
#endif
    DBG(cerr << "DDS::transfer_attributes(DAS * das): sub table: "
        << sub_table << endl);

    BaseType *btp = var((*i)->name);
    if (btp)
        transfer_attr(das, (*i), btp, sub_table);
    else
        add_global_attribute(*i);

    ++i;
}
#endif


#if 0
/** Transfer a single attribute to the table held by a BaseType. If the
    attribute turns out to be a container, call transfer_attr_table. If not
    load the attribute's values into the BaseType's attribute table. Both
    this function and transfer_attr_table() \e assume that you know that
    the attributes are destined for the particular BaseType.

    @param das Pointer to the DAS instance which holds the attribute \e ep.
    @param ep The attribute
    @param btp The destination
    @param sub_table Not used by this code but passed to transfer_attr_table()
    where is might be used. */

void
DDS::transfer_attr(DAS *das, const AttrTable::entry *ep, BaseType *btp,
                   const string &sub_table)
{
    DBG(cerr << "DDS::transfer_attr: sub_table: " << sub_table << endl);

    if (ep->is_alias) {
        AttrTable *source_table = das->get_attr_table(ep->aliased_to);
        AttrTable &dest = btp->get_attr_table();
        if (source_table)
            dest.add_container_alias(ep->name /*+ sub_table*/, source_table);
        else
            dest.add_value_alias(das, ep->name /*+ sub_table*/, ep->aliased_to);
    }
    else if (ep->type == Attr_container) {
        DBG(cerr << "ep-type == container, ep-<name: " << ep->name << endl);
        //use sub_table here to make the new stuff a 'sub table'. I think this
        // is wrong. jhrg 10/24/06
        ep->attributes->set_name(ep->name);
        Constructor *c = dynamic_cast<Constructor*>(btp);
        if (c)
            transfer_attr_table(das, ep->attributes, c, sub_table);
        else
            transfer_attr_table(das, ep->attributes, btp, sub_table);
    }
    else {
        btp->get_attr_table().append_attr(ep->name, AttrType_to_String(ep->type),
                                          ep->attr);
#if 0
        AttrTable &at = btp->get_attr_table();
        string n = ep->name /*+ sub_table*/;
        string t = AttrType_to_String(ep->type);
        vector<string> *attrs = ep->attr;
        for (vector<string>::iterator i = attrs->begin(); i != attrs->end(); ++i)
            at.append_attr(n, t, *i);
#endif
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
    @param sub_table If \e sub_table is not empty, then make a sub-table named
    \e sub_table and add that to the \e btp attribute table. This is used to
    bind things like the HDF4 dimension attributes in a place where older
    clients will find them or where theay will not get in the way. */
void
DDS::transfer_attr_table(DAS *das, AttrTable *at, BaseType *btp,
                         const string &sub_table)
{
    DBG(cerr << "DDS::transfer_attr_table (BseType): sub_table: " << sub_table << endl);

    if (at->get_name() == btp->name()) {
        // If the name matches and sub_table is not null, make a new table
        // called 'sub_table' and add that to btp's table.
        if (!sub_table.empty()) {
            string tsub_table = sub_table;
            AttrTable *new_at = new AttrTable(*at); //clone; see below
            // If the sub_table has a leading undescore, remove it.
            if (sub_table.find('_') != string::npos) {
                tsub_table = tsub_table.substr(tsub_table.find('_') + 1);
            }
            btp->get_attr_table().append_container(new_at, tsub_table);
        }
        else {
            // for each entry in the table, call transfer_attr()
            for (AttrTable::Attr_iter i = at->attr_begin(); i != at->attr_end(); ++i)
                transfer_attr(das, *i, btp, "");
        }
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
                         const string &sub_table)
{
    DBG(cerr << "DDS::transfer_attr_table: (Constructor) sub_table: "
        << sub_table << endl);
    for (AttrTable::Attr_iter i = at->attr_begin(); i != at->attr_end(); ++i) {
        AttrTable::entry *ep = *i;
        string n = ep->name;
        bool found = false;

        switch (c->type()) {
        case dods_structure_c:
        case dods_sequence_c: {
                for (Constructor::Vars_iter j = c->var_begin(); j != c->var_end();
                     ++j) {
                    if (n == (*j)->name()) { // found match
                        found = true;
                        transfer_attr(das, ep, *j, sub_table);
                    }
                }
                break;
            }

        case dods_grid_c: {
                Grid *g = dynamic_cast<Grid*>(c);
                if (n == g->get_array()->name()) { // found match
                    found = true;
                    transfer_attr(das, ep, g->get_array(), sub_table);
                }

                for (Grid::Map_iter j = g->map_begin(); j != g->map_end(); ++j) {
                    if (n == (*j)->name()) { // found match
                        found = true;
                        transfer_attr(das, ep, *j, sub_table);
                    }
                }
                break;
            }

        default:
            throw InternalErr(__FILE__, __LINE__, "Unknown type.");
        }

        if (!found) {
            DBG(cerr << "Could not find a place in a constructor for " << sub_table
                << ", calling transfer_attr() without it." << endl);
            transfer_attr(das, ep, c);
        }
    }
}
#endif
#if 0
/** An attribute is global if it's name does not match any of the top-level
    variables. Assume that this function is called only for top-level
    attributes.

    A private method.

    @param name Name of the attribute.
    @return True if \i name is a global attribute. */
bool
DDS::is_global_attr(string name)
{
    for (Vars_iter i = var_begin(); i != var_end(); ++i)
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
#endif

// This code should be making the <name>_dim_0, ..., attributes
// into 'dim_0' containers within the <name> container. Let specific
// clients handle the nested attributes however they want to. See
// ticket #480.
//
// Refactor: Instead of matching each table to variable, adopt the
// opposite approach. Scan the DDS (this) and for each variable,
// look for its matching attribute table. Once that code works, add
// special cases for things like the HDF4 _dim_? attribute tables.
// One approach is to use the existing AttrTable::find methods to
// locate tables and remove them from the DAS once they have been
// transferred. Then the remaining AttrTable objects can be made the
// Global attributes. Between those two operations, special cases can
// be considered. jhrg 8/16/06

// The AttrTable::find_container() method (which is what
// DAS::get_attr_table(string) uses) will look in the current table only
// unless the 'dot notation' is used. Since each variable must have an
// AttrTable, we can scan the DDS for attributes at the top level and do the
// same for the DAS. When a variable is not a simple type
//(!BaseType::is_simple_type()), we can recur.

// Instead of adding a method to BaseType and then to the Constructor and
// Grid classes, use functions/private methods here to keep the DAS object
// Out of the BaseType interface.

#if 0
// This is a failed attempt to rewrite the transfer_attributes() method.
// Instead I fixed the original code; a better fix is to build servers that
// use the DDX object.

/** This function looks for attributes first in a container (AttrTable) and
    then in the entire DAS object. Sometimes servers fail to include
    the containers they should or fail to properly nest them. This function
    is an attempt to compensate for that errant behavior.

    @ note This function checks to see if \e das is null before using it.
    This was done to make testing the transfer_attr_to_constructor() and
    transfer_attributes() methods easier. */
static AttrTable *
search_for_attributes(const string &name, AttrTable *at, DAS *das)
{
    // if 'at' is null, set ptable to null.
    AttrTable *ptable = (at) ? at->find_container(name) : 0;
    if (!ptable && das)
        ptable = das->get_attr_table(name);

    return ptable;
}

/** This private, recurrsive, method looks at each variable in the constructor
    and searches the attribute table \e at for a container with its attributes.
    if found, it loads them into the varaible and then removes that table
    from \e at. If not found, it looks in the DAS \e das because some servers
    don't properly nest the attribute tables. Then it takes whatever
    attributes remain and loads those into the Constructor \e cp.
 */
void
DDS::transfer_attr_to_constructor(Constructor *cp, AttrTable *at, DAS *das)
{
    if (cp->type() != dods_grid_c) {
        // Look at each variable held in the Constructor
        for (Constructor::Vars_iter i = cp->var_begin(); i != cp->var_end(); ++i) {
            AttrTable *ptable = search_for_attributes((*i)->name(), at, das);
            if (!ptable)
                continue;

            if ((*i)->is_simple_type() || (*i)->is_vector_type()) {
                (*i)->set_attr_table(*ptable);  // Performs a deep copy
            }
            else { // a constructor
                transfer_attr_to_constructor(dynamic_cast<Constructor*>(*i), ptable, das);
            }
        }

        // Now transfer all the regular attributes in 'at' to the Constructor.
        if (at) {
            AttrTable tmp;
            for (AttrTable::Attr_iter p = at->attr_begin(); p != at->attr_end(); ++p) {
                if (!at->is_container(p)) {
                    cp->get_attr_table().append_attr(at->get_name(p),
                                                     at->get_type(p),
                                                     at->get_attr_vector(p));
                }
            }
        }
    }
    else { // it's a grid, first special case for the Array
        Grid *g = dynamic_cast<Grid*>(cp);
        AttrTable *ptable = search_for_attributes(g->get_array()->name(), at, das);
        if (ptable)
            g->get_array()->set_attr_table(*ptable);  // Performs a deep copy
        // Look at each map in the
        for (Grid::Map_iter i = g->map_begin(); i != g->map_end(); ++i) {
            AttrTable *ptable = search_for_attributes((*i)->name(), at, das);
            if (!ptable)
                continue;
            // Since this is a map inside a Grid, it must be an array
            (*i)->set_attr_table(*ptable);  // Performs a deep copy
        }

        // Now transfer all the regular attributes in at to the Grid.
        if (at) {
            AttrTable tmp;
            for (AttrTable::Attr_iter p = at->attr_begin(); p != at->attr_end(); ++p) {
                if (!at->is_container(p)) {
                    cp->get_attr_table().append_attr(at->get_name(p),
                                                     at->get_type(p),
                                                     at->get_attr_vector(p));
                }
            }
        }
    }
}

void
DDS::new_transfer_attributes(DAS * das)
{
    for (Vars_iter i = var_begin(); i != var_end(); ++i) {
        AttrTable *at = das->get_attr_table((*i)->name());

        // There is always supposed to be an attribute table for each variable,
        // but sometimes servers goof. That's why the code below tests 'at'.

        // Now we have the table that matches the top-level variable.
        // Decide how to add it to the variable. This will depend on
        // The type of variable.
        if (at && ((*i)->is_simple_type() || (*i)->is_vector_type())) {
            // *** Does not take into account vectors of constructors.
            // The same this is true in the above method. jhrg 8/17/06
            (*i)->set_attr_table(*at);  // Performs a deep copy
        }
        else { // a constructor; transfer_attr_to_constructor can deal with a
            // null 'at'.
            transfer_attr_to_constructor(dynamic_cast<Constructor*>(*i), at, das);
        }
    }
}
#endif
