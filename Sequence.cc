
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

// Implementation for the class Structure
//
// jhrg 9/14/94

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

#include <algorithm>
#include <string>
#include <sstream>

#include "debug.h"
#include "Error.h"
#include "InternalErr.h"
#include "Sequence.h"
#include "DDS.h"
#include "DataDDS.h"
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using namespace std;

static const unsigned char end_of_sequence = 0xA5; // binary pattern 1010 0101
static const unsigned char start_of_instance = 0x5A; // binary pattern 0101 1010

// Private member functions

void
Sequence::_duplicate(const Sequence &s)
{
    d_row_number = s.d_row_number;
    d_starting_row_number = s.d_starting_row_number;
    d_ending_row_number = s.d_ending_row_number;
    d_row_stride = s.d_row_stride;

    Sequence &cs = const_cast<Sequence &>(s);
    
    // Copy the template BaseType objects.
    for (Vars_iter i = cs.var_begin(); i != cs.var_end(); i++)
    {
	add_var((*i)) ;
    }

    // Copy the BaseType objects used to hold values.
    for (vector<BaseTypeRow *>::iterator rows_iter = cs.d_values.begin();
	 rows_iter != cs.d_values.end();
	 rows_iter++) {
	// Get the current BaseType Row
	BaseTypeRow *src_bt_row_ptr = *rows_iter;
	// Create a new row.
	BaseTypeRow *dest_bt_row_ptr = new BaseTypeRow;
	// Copy the BaseType objects from a row to new BaseType objects.
	// Push new BaseType objects onto new row.
	for (BaseTypeRow::iterator bt_row_iter = src_bt_row_ptr->begin();
	     bt_row_iter != src_bt_row_ptr->end();
	     bt_row_iter++) {
	    BaseType *src_bt_ptr = *bt_row_iter;
	    BaseType *dest_bt_ptr = src_bt_ptr->ptr_duplicate();
	    dest_bt_row_ptr->push_back(dest_bt_ptr);
	}
	// Push new row onto d_values.
	d_values.push_back(dest_bt_row_ptr);
    }
}

// Protected member functions

void
Sequence::write_end_of_sequence(XDR *sink)
{
    xdr_opaque(sink, (char *)&end_of_sequence, 1);
}

void
Sequence::write_start_of_instance(XDR *sink)
{
    xdr_opaque(sink, (char *)&start_of_instance, 1);
}

unsigned char
Sequence::read_marker(XDR *source)
{
    unsigned char marker;
    xdr_opaque(source, (char *)&marker, 1);

    return marker;
}

bool
Sequence::is_start_of_instance(unsigned char marker)
{
    return (marker == start_of_instance);
}

bool
Sequence::is_end_of_sequence(unsigned char marker)
{
    return (marker == end_of_sequence);
}

// Public member functions

/** The Sequence constructor requires only the name of the variable
    to be created.  The name may be omitted, which will create a
    nameless variable.  This may be adequate for some applications. 
      
    @param n A string containing the name of the variable to be
    created. 

    @brief The Sequence constructor. */
Sequence::Sequence(const string &n) : Constructor(n, dods_sequence_c), 
    d_row_number(-1), d_starting_row_number(-1),
    d_row_stride(1), d_ending_row_number(-1)
{
}

/** @brief The Sequence copy constructor. */
Sequence::Sequence(const Sequence &rhs) : Constructor(rhs)
{
    _duplicate(rhs);
}

BaseType *
Sequence::ptr_duplicate()
{
    return new Sequence(*this);
}

static inline void
delete_bt(BaseType *bt_ptr)
{
    DBG(cerr << "In delete_bt: " << bt_ptr << endl);
    delete bt_ptr;
    bt_ptr = 0;
}

static inline void
delete_rows(BaseTypeRow *bt_row_ptr)
{
    DBG(cerr << "In delete_rows: " << bt_row_ptr << endl);

    for_each(bt_row_ptr->begin(), bt_row_ptr->end(), delete_bt);

    delete bt_row_ptr;
    bt_row_ptr = 0;
}

Sequence::~Sequence()
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	BaseType *btp = *i ;
	delete btp ;
    }

    for_each(d_values.begin(), d_values.end(), delete_rows);
}

Sequence &
Sequence::operator=(const Sequence &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

string
Sequence::toString()
{
    ostringstream oss;

    oss << BaseType::toString();

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	oss << (*i)->toString();
    }

    oss << endl;

    return oss.str();
}

int
Sequence::element_count(bool leaves)
{
    if (!leaves)
	return _vars.size();
    else {
	int i = 0;
	for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++)
	{
	    i += (*iter)->element_count(true);
	}
	return i;
    }
}

bool
Sequence::is_linear()
{
    bool linear = true;
    bool seq_found = false;
    for (Vars_iter iter = _vars.begin(); linear && iter != _vars.end(); iter++)
    {
	if ((*iter)->type() == dods_sequence_c) {
	    // A linear sequence cannot have more than one child seq. at any
	    // one level. If we've already found a seq at this level, return
	    // false. 
	    if (seq_found) {
		linear = false;
		break;
	    }
	    seq_found = true;
	    linear = dynamic_cast<Sequence *>((*iter))->is_linear();
	}
	else if ((*iter)->type() == dods_structure_c) {
	    linear = dynamic_cast<Structure*>((*iter))->is_linear();
	}
	else {
	    // A linear sequence cannot have Arrays, Lists or Grids.
	    linear = (*iter)->is_simple_type();
	}
    }

    return linear;
}

void
Sequence::set_send_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Sequence::set_read_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->set_read_p(state);
    }

    BaseType::set_read_p(state);
}

void
Sequence::set_in_selection(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
	(*i)->set_in_selection(state);
    }

    BaseType::set_in_selection(state);
}

/** @brief Adds a variable to the Sequence.  
    
    Remember that if you wish to add a member to a nested
    Sequence, you must use the <tt>add_var()</tt> of that
    Sequence.  This means that variable names need not be unique
    among a set of nested Sequences. 

    @param bt A pointer to the DODS type variable to add to this Sequence.
    @param part defaults to nil */
void 
Sequence::add_var(BaseType *bt, Part)
{
   if(!bt)
       throw InternalErr(__FILE__, __LINE__, 
			 "Cannot add variable: NULL pointer");
   // Jose Garcia
   // We append a copy of bt so the owner
   // of bt is free to deallocate as he wishes.
   DBG(cerr << "In Sequence::add_var(), bt: " << bt <<endl);
   DBG2(cerr << bt->toString() << endl);

   BaseType *bt_copy = bt->ptr_duplicate();
   bt_copy->set_parent(this);
   _vars.push_back(bt_copy);

   DBG(cerr << "In Sequence::add_var(), bt_copy: " << bt_copy <<endl);
   DBG2(cerr << bt_copy->toString() << endl);
}

// Deprecated
BaseType *
Sequence::var(const string &n, btp_stack &s)
{
    string name = www2id(n);

    BaseType *btp = exact_match(name, &s);
    if (btp)
	return btp;

    return leaf_match(name, &s);
}

BaseType *
Sequence::var(const string &n, bool exact, btp_stack *s)
{
    string name = www2id(n);

    if (exact)
	return exact_match(name, s);
    else
	return leaf_match(name, s);
}

BaseType *
Sequence::leaf_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	if ((*i)->name() == name) {
	    if (s)
		s->push(static_cast<BaseType *>(this));
	    return *i;
	}
        if ((*i)->is_constructor_type()) {
	    BaseType *btp = (*i)->var(name, false, s);
	    if (btp) {
		if (s)
		    s->push(static_cast<BaseType *>(this));
		return btp;
	    }
	}
    }

    return 0;
}

BaseType *
Sequence::exact_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	if ((*i)->name() == name) {
	    if (s)
		s->push(static_cast<BaseType *>(this));
	    return *i;
	}
    }

    string::size_type dot_pos = name.find("."); // zero-based index of `.'
    if (dot_pos != string::npos) {
	string aggregate = name.substr(0, dot_pos);
	string field = name.substr(dot_pos + 1);

	BaseType *agg_ptr = var(aggregate);
	if (agg_ptr) {
	    if (s)
		s->push(static_cast<BaseType *>(this));
	    return agg_ptr->var(field, true, s); // recurse
	}
	else
	    return 0;		// qualified names must be *fully* qualified
    }

    return 0;
}

/** @brief Get a whole row from the sequence. 
    @param row Get <i>row</i> from the sequence.
    @return A BaseTypeRow object (vector<BaseType *>). */
BaseTypeRow *
Sequence::row_value(size_t row)
{
    if (row >= d_values.size())
	return 0;
    return d_values[row];
}

/** @brief Get the BaseType pointer to the named variable of a given row. 
    @param row Read from <i>row</i> in the sequence.
    @param name Return <i>name</i> from <i>row</i>.
    @return A BaseType which holds the variable and its value. 
    @see number_of_rows */
BaseType *
Sequence::var_value(size_t row, const string &name)
{
    BaseTypeRow *bt_row_ptr = row_value(row);
    if (!bt_row_ptr)
	return 0;

    BaseTypeRow::iterator bt_row_iter = bt_row_ptr->begin();
    BaseTypeRow::iterator bt_row_end = bt_row_ptr->end();    
    while (bt_row_iter != bt_row_end && (*bt_row_iter)->name() != name)
	++bt_row_iter;
    
    if (bt_row_iter == bt_row_end)
	return 0;
    else
	return *bt_row_iter;
}

/** @brief Get the BaseType pointer to the $i^{th}$ variable of <i>row</i>.
    @param row Read from <i>row</i> in the sequence.
    @param i Return the $i^{th}$ variable from <i>row</i>.
    @return A BaseType which holds the variable and its value.
    @see number_of_rows */
BaseType *
Sequence::var_value(size_t row, size_t i)
{
    BaseTypeRow *bt_row_ptr = row_value(row);
    if (!bt_row_ptr)
	return 0;

    if (i >= bt_row_ptr->size())
	return 0;

    return (*bt_row_ptr)[i];
}

#if 0
/** @brief Returns an index to the first variable in a Sequence instance.
    This corresponds to the item in the first column of the table
    the Sequence represents.  It is not the first row of the table. 
*/
Pix
Sequence::first_var()
{
    if (_vars.empty())
	return 0;

    BTIterAdapter *i = new BTIterAdapter( _vars ) ;
    i->first() ;
    return i ;
}
#endif

#if 0
Sequence::Vars_iter
Sequence::var_begin()
{
    return _vars.begin() ;
}

Sequence::Vars_iter
Sequence::var_end()
{
    return _vars.end() ;
}

/** Return the iterator for the \i ith variable.
    @param i the index
    @return The corresponding  Vars_iter */
Sequence::Vars_iter
Sequence::get_vars_iter(int i)
{
    return _vars.begin() + i;
}
#endif

#if 0
/** @brief Increments the Sequence instance.  
    This returns a pointer to the
    next ``column'' in the Sequence, not the next row. */
void
Sequence::next_var(Pix p)
{
    p.next() ;
}

/** @brief Returns a pointer to a Sequence member.  
    This may be another Sequence. */
BaseType *
Sequence::var(Pix p)
{
    BTIterAdapter *i = (BTIterAdapter *)p.getIterator() ;
    if( i ) {
	return i->entry() ;
    }
    return 0 ;
}
#endif

unsigned int
Sequence::width()
{
    unsigned int sz = 0;

    for(Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	sz += (*i)->width();
    }

    return sz;
}

// This version returns -1. Each API-specific subclass should define a more
// reasonable version. jhrg 5/24/96

/** Returns the number of elements in a Sequence object. Note that
    this is <i>not</i> the number of items in a row, but the number
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
    will have a more complete implementation. */
int
Sequence::length()
{
    return -1;
}

int
Sequence::number_of_rows()
{
    return d_values.size();
}

// Notes:
// Assume that read() is implemented so that, when reading data for a nested
// sequence, only the outer most level is *actually* read.
// This is a consequence of our current (12/7/99) implementation of
// the JGOFS server (which is the only server to actually use nested
// sequences). 12/7/99 jhrg
//
// Stop assuming this. This logic is being moved into the JGOFS server
// itself. 6/1/2001 jhrg

// The read() function returns a boolean value, with TRUE
// indicating that read() should be called again because there's
// more data to read, and FALSE indicating there's no more data
// to read. Note that this behavior is necessary to properly
// handle variables that contain Sequences. Jose Garcia If an
// error exists while reading, the implementers of the surrogate
// library SHOULD throw an Error object which will propagate
// beyond this point to to the original caller.
// Jose Garcia

/** Read row number <i>row</i> into the Sequence. The values of
    the row are obtained using the members of the sequence. This
    method calls the overloaded Sequence::read() method to read
    each row. The rows are counted using by the object (see
    <tt>get_row_number()</tt>). If a selection expression has been
    supplied, rows are counted only if they satisfy that
    expression.

    Note that we can only advance.  It is not possible to back up
    (though there is nothing theoretically against it).

    Used on the server side.

    NB: The first row is row number zero. A Sequence with 100 rows will
    have row numbers 0 to 99.

    @return A boolean value, with TRUE indicating that read_row
    should be called again because there's more data to be read.
    FALSE indicates the end of the Sequence.
    @param row The row number to read.
    @param dataset A string, often a file name, used to refer to t he
    dataset. 
    @param dds A reference to the DDS for this dataset.
    @param ce_eval If True, evaluate any CE, otherwise do not.
    @return True if there are more rows to read, False if the EOF was
    found. */
bool
Sequence::read_row(int row, const string &dataset, DDS &dds, bool ce_eval)
{
    if (row < d_row_number)
	throw InternalErr("Trying to back up inside a sequence!");

    if (row == d_row_number)
	return true;

    int eof = 0;		// Start out assuming EOF is false.
    while(!eof && d_row_number < row) {
	if (!read_p()) {
	    eof = (read(dataset) == false);
	}

	// Advance the row number if ce_eval is false (we're not supposed to
	// evaluate the selection) or both ce_eval and the selection are
	// true.
	if (!ce_eval || (ce_eval && dds.eval_selection(dataset)))
	    d_row_number++;

	set_read_p(false);	// ...so that the next instance will be read
    }

    // Once we finish te above loop, set read_p to true so that the caller
    // knows that data *has* been read. This is how the read() methods of the
    // elements of the sequence know to not call read() themselves but
    // instead look for data values inside the object.
    set_read_p(true);

    // Return true if we have valid data, false if we've read to the EOF.
    return eof == 0;
}

// Private. This is used to process constraints on the rows of a sequence.
// Starting with 3.2 we support constraints like Sequence[10:2:20]. This
// odd-looking logic first checks if d_ending_row_number is the sentinel
// value of -1. If so, the sequence was not constrained by row number and
// this method should never return true (which indicates that we're at the
// end of a row-number constraint). If d_ending_row_number is not -1, then is
// #i# at the end point? 6/1/2001 jhrg
inline bool
Sequence::is_end_of_rows(int i)
{
    return ((d_ending_row_number == -1) ? false : (i >= d_ending_row_number));
}

bool
Sequence::serialize(const string &dataset, DDS &dds, XDR *sink, bool ce_eval)
{
    int i = (d_starting_row_number != -1) ? d_starting_row_number : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    bool status = read_row(i, dataset, dds, ce_eval);

    while (status && !is_end_of_rows(i)) {
	i += d_row_stride;

	DBG(cerr << "Writing Start of Instance marker" << endl);
	write_start_of_instance(sink);

	// In this loop serialize will signal an error with an exception.
	for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++)
	{
	    if ((*iter)->send_p())
		(*iter)->serialize(dataset, dds, sink, false);
	}

	set_read_p(false);	// ...so this will read the next instance

	status = read_row(i, dataset, dds, ce_eval);
    }

    DBG(cerr << "Writing End of Sequence marker" << endl);
    write_end_of_sequence(sink);

    return true;		// Signal errors with exceptions.
}


/** @brief Deserialize (read from the network) the entire Sequence.

    This method used to read a single row at a time. Now the entire
    sequence is read at once. The method used to return True to indicate
    that more data needed to be deserialized and False when the sequence
    was completely read. Now it simply returns true. This might seem odd,
    but it is because all of the other implementation of this method
    return true when the XDR readers succeed and False otherwise. Making
    this method return true breaks existing software the least.

    @param source An XDR stream.
    @param dds A DataDDS from which to read.
    @param reuse Passed to child objects when they are deserialized.
    @exception Error if a sequence stream marker cannot be read.
    @exception InternalErr if the <tt>dds</tt> param is not a DataDDS.
    @return A return value of false indicates that an EOS ("end of
    Sequence") marker was found, while a value of true indicates
    that there are more rows to be read. 
*/
bool
Sequence::deserialize(XDR *source, DDS *dds, bool reuse)
{
    DataDDS *dd = dynamic_cast<DataDDS *>(dds);
    if (!dd)
	throw InternalErr("Expected argument 'dds' to be a DataDDS!");

    DBG2(cerr << "Reading from server version: " << dd->get_version_major() \
	 << "." << dd->get_version_minor() << endl);

    // Check for old servers.
    if (dd->get_version_major() < 2 
	|| dd->get_version_major() == 2 && dd->get_version_minor() < 15) {
	return old_deserialize(source, dd, reuse);
    }

    while (true) {
	// Grab the sequence stream's marker. 
	unsigned char marker = read_marker(source);
	if (is_end_of_sequence(marker))
	    break;		// EXIT the while loop here!!!
	else if (is_start_of_instance(marker)) {
	    d_row_number++;
	    DBG2(cerr << "Reading row " << d_row_number << " of "
		 << name() << endl);
	    BaseTypeRow *bt_row_ptr = new BaseTypeRow;
	    // Read the instance's values, building up the row
	    for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++)
	    {
		BaseType *bt_ptr = (*iter)->ptr_duplicate();
		bt_ptr->deserialize(source, dds, reuse);
		DBG2(cerr << "Deserialized " << bt_ptr->name() << " ("
		     << bt_ptr << ") = ");
		DBG2(bt_ptr->print_val(stderr, ""));
		bt_row_ptr->push_back(bt_ptr);
	    }
	    // Append this row to those accumulated.
	    d_values.push_back(bt_row_ptr);
	}
	else
	    throw Error("I could not read the expected Sequence data stream marker!");
    };

    return false;
}

// Return the current row number.

/** Return the starting row number if the sequence was constrained using
    row numbers (instead of, or in addition to, a relational constraint).
    If a relational constraint was also given, the row number corresponds
    to the row number of the sequence <i>after</i> applying the relational
    constraint.

    If the bracket notation was not used to constrain this sequence, this
    method returns -1.

    @brief Get the starting row number.
    @return The starting row number. */
int
Sequence::get_starting_row_number()
{
  return d_starting_row_number;
}

/** Return the row stride number if the sequence was constrained using
    row numbers (instead of, or in addition to, a relational constraint).
    If a relational constraint was also given, the row stride is applied
    to the sequence <i>after</i> applying the relational constraint.

    If the bracket notation was not used to constrain this sequence, this
    method returns -1.

    @brief Get the row stride.
    @return The row stride. */
int
Sequence::get_row_stride()
{
  return d_row_stride;
}

/** Return the ending row number if the sequence was constrained using
    row numbers (instead of, or in addition to, a relational constraint).
    If a relational constraint was also given, the row number corresponds
    to the row number of the sequence <i>after</i> applying the
    relational constraint.

    If the bracket notation was not used to constrain this sequence, this
    method returns -1.

    @brief Get the ending row number.
    @return The ending row number. */
int
Sequence::get_ending_row_number()
{
  return d_ending_row_number;
}

/** Set the start, stop and stride for a row-number type constraint.
    This should be used only when the sequence is constrained using the
    bracket notation (which supplies start, stride and stop information).
    If omitted, the stride defaults to 1.

    @param start The starting row number. The first row is row zero.
    @param stop The eding row number. The 20th row is row 19.
    @param stride The stride. A stride of two skips every other row. */
void
Sequence::set_row_number_constraint(int start, int stop, int stride)
{
  if (stop < start)
    throw Error(malformed_expr, "Starting row number must precede the ending row number.");

  d_starting_row_number = start;
  d_row_stride = stride;
  d_ending_row_number = stop;
}

// private mfunc. Use this to read from older servers.

bool
Sequence::old_deserialize(XDR *source, DDS *dds, bool reuse)
{
    bool stat = true;

    DBG2(cerr << "Entering old_deserialize()" << endl);

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	stat = (*i)->deserialize(source, dds, reuse);
	if (!stat) 
	    return false;
    }

    d_row_number++;

    return stat;
}

unsigned int
Sequence::val2buf(void *, bool)
{
    return sizeof(Sequence);
}

unsigned int
Sequence::buf2val(void **)
{
    return sizeof(Sequence);
}

#if 0
void
Sequence::print_decl(ostream &os, string space, bool print_semi,
		     bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name() << " {" << endl;
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->print_decl(os, space + "    ", true,
			 constraint_info, constrained);
    }
    os << space << "} " << id2www(name());

    if (constraint_info) {	// Used by test drivers only.
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
}

void
Sequence::print_decl(FILE *out, string space, bool print_semi,
		     bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    fprintf( out, "%s%s {\n", space.c_str(), type_name().c_str() ) ;
    for (Vars_citer i = _vars.begin(); i != _vars.end(); i++)
    {
	(*i)->print_decl(out, space + "    ", true,
			 constraint_info, constrained);
    }
    fprintf( out, "%s} %s", space.c_str(), id2www( name() ).c_str() ) ;

    if (constraint_info) {	// Used by test drivers only.
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	fprintf( out, ";\n" ) ;
}
#endif

/** Print the $i^{th}$ row of the sequence. This hopes in writing
    code that spits out sequences for testing. Sequences are now read all
    at once, this method does not change that. */
void 
Sequence::print_one_row(ostream &os, int row, string space, 
			bool print_row_num)
{
    if (print_row_num)
	os << endl << row << ": ";

    os << "{ ";

    int elements = element_count() - 1;
    int j;
    BaseType *bt_ptr;
    // Print first N-1 elements of the row.
    for (j = 0; j < elements; ++j) {
	bt_ptr = var_value(row, j);
	if (bt_ptr) {		// data
	    if (bt_ptr->type() == dods_sequence_c)
		dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
		    (os, space, false, print_row_num);
	    else
		bt_ptr->print_val(os, space, false);
	    os << ", ";
	}
    }

    // Print Nth element; end with a `}.'
    bt_ptr = var_value(row, j);
    if (bt_ptr) {		// data
	if (bt_ptr->type() == dods_sequence_c)
	    dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
		(os, space, false, print_row_num);
	else
	    bt_ptr->print_val(os, space, false);
    }

    os << " }";
}

void 
Sequence::print_one_row(FILE *out, int row, string space, 
			bool print_row_num)
{
    if (print_row_num)
	fprintf( out, "\n%d: ", row ) ;

    fprintf( out, "{ " ) ;

    int elements = element_count() - 1;
    int j;
    BaseType *bt_ptr;
    // Print first N-1 elements of the row.
    for (j = 0; j < elements; ++j) {
	bt_ptr = var_value(row, j);
	if (bt_ptr) {		// data
	    if (bt_ptr->type() == dods_sequence_c)
		dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
		    (out, space, false, print_row_num);
	    else
		bt_ptr->print_val(out, space, false);
	    fprintf( out, ", " ) ;
	}
    }

    // Print Nth element; end with a `}.'
    bt_ptr = var_value(row, j);
    if (bt_ptr) {		// data
	if (bt_ptr->type() == dods_sequence_c)
	    dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
		(out, space, false, print_row_num);
	else
	    bt_ptr->print_val(out, space, false);
    }

    fprintf( out, " }" ) ;
}
/** @brief Prints each row on its own line with a row number. 

    This is a special output method for sequences. Uses
    <tt>print_one_row</tt> with <tt>print_row_num</tt> True. 

    @param os The output stream on which to print the Sequence.
    @param space The leading spaces of the output.
    @param print_decl_p If TRUE, prints the declaration of the
    Sequence as well as its data.
    @param print_row_numbers If TRUE, prints the row numbers next
    to the Sequence instances (rows).
*/ 
void
Sequence::print_val_by_rows(ostream &os, string space, bool print_decl_p,
			    bool print_row_numbers)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";

    int rows = number_of_rows() - 1;
    int i;
    for (i = 0; i < rows; ++i) {
	print_one_row(os, i, space, print_row_numbers);
	    os << ", ";
    }
    print_one_row(os, i, space, print_row_numbers);

    os << " }";

    if (print_decl_p)
        os << ";" << endl;
}

void
Sequence::print_val_by_rows(FILE *out, string space, bool print_decl_p,
			    bool print_row_numbers)
{
    if (print_decl_p) {
	print_decl(out, space, false);
	fprintf( out, " = " ) ;
    }

    fprintf( out, "{ " ) ;

    int rows = number_of_rows() - 1;
    int i;
    for (i = 0; i < rows; ++i) {
	print_one_row(out, i, space, print_row_numbers);
	    fprintf( out, ", " ) ;
    }
    print_one_row(out, i, space, print_row_numbers);

    fprintf( out, " }" ) ;

    if (print_decl_p)
        fprintf( out, ";\n" ) ;
}

/** Print the values held by the sequence. This is used mostly for
    debugging.
    @param os Where should the text go?
    @param space Leading spaces, used by formatting code.
    @param print_decl_p If True, print the variable's declaration. */
void 
Sequence::print_val(ostream &os, string space, bool print_decl_p)
{
    print_val_by_rows(os, space, print_decl_p, false);
}

void 
Sequence::print_val(FILE *out, string space, bool print_decl_p)
{
    print_val_by_rows(out, space, print_decl_p, false);
}

// print_all_vals is from Todd Karakasian. 
// We need to integrate this into print_val somehow, maybe by adding an XDR *
// to Sequence? This can wait since print_val is mostly used for debugging...
//
// Deprecated. 

/** Prints a formatted version of an entire Sequence (all rows, all
    columns), including nested Sequences.  This is meant to be used
    on the client side of a DODS connection, and the source of the
    Sequence data to be printed is specified with an XDR pointer. 

    This function is no longer needed, since print_val_by_rows.

    @deprecated
    @brief Print the entire sequence.
    @param os The output stream on which to print the Sequence.
    @param src The external source from which the data is to come.
    This is passed to <tt>deserialize()</tt>.
    @param dds The Data Descriptor Structure object corresponding to
    this dataset.  See <i>The DODS User Manual</i> for information
    about this structure.  This would have been received from the
    server in an earlier transmission.
    @param space The leading spaces of the output.
    @param print_decl_p If TRUE, prints the declaration of the
    Sequence as well as its data.
*/
void
Sequence::print_all_vals(ostream& os, XDR *src, DDS *dds, string space,
			 bool print_decl_p)
{
    print_val(os, space, print_decl_p);
}

// print_all_vals is from Todd Karakasian. 
// We need to integrate this into print_val somehow, maybe by adding an XDR *
// to Sequence? This can wait since print_val is mostly used for debugging...
//
// Deprecated. No longer needed since print_vals does its job.

void
Sequence::print_all_vals(FILE *out, XDR *src, DDS *dds, string space,
			 bool print_decl_p)
{
#if 0
    deserialize(src, dds);
#endif
    print_val(out, space, print_decl_p);
}

bool
Sequence::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
	return false;

    if (!unique_names(_vars, name(), type_name(), msg))
	return false;

    if (all) 
	for (Vars_iter i = _vars.begin(); i != _vars.end(); i++)
	{
	    if (!(*i)->check_semantics(msg, true))
	    {
		return false;
	    }
	}

    return true;
}

// $Log: Sequence.cc,v $
// Revision 1.72  2003/12/10 21:11:58  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.71  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.70  2003/04/22 19:40:28  jimg
// Merged with 3.3.1.
//
// Revision 1.68.2.2  2003/04/15 01:17:12  jimg
// Added a method to get the iterator for a variable (or map) given its
// index. To get the iterator for the ith variable/map, call
// get_vars_iter(i).
//
// Revision 1.69  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.68.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.68  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.67  2003/01/15 19:24:39  pwest
// Removing IteratorAdapterT and replacing with non-templated versions.
//
// Revision 1.66  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.59.4.24  2002/12/31 16:43:20  rmorris
// Patches to handle some of the fancier template code under VC++ 6.0.
//
// Revision 1.59.4.23  2002/12/27 19:34:42  jimg
// Modified the var() methods so that www2id() is called before looking
// up identifier names. See bug 563.
//
// Revision 1.59.4.22  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.59.4.21  2002/12/01 14:37:52  rmorris
// Smalling changes for the win32 porting and maintenance work.
//
// Revision 1.59.4.20  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.59.4.19  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.59.4.18  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.59.4.17  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.65  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.64  2002/06/03 21:53:59  jimg
// Removed level stuff. The level() and set_level() methods were not being used
// anymore, so I removed them.
//
// Revision 1.59.4.16  2002/05/22 16:57:51  jimg
// I modified the `data type classes' so that they do not need to be
// subclassed for clients. It might be the case that, for a complex client,
// subclassing is still the best way to go, but you're not required to do
// it anymore.
//
// Revision 1.59.4.15  2002/03/29 18:36:40  jimg
// _duplicate() no longer calls Constructor::_duplicate.
//
// Revision 1.59.4.14  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.59.4.13  2002/02/20 19:01:25  jimg
// Changed some comments
//
// Revision 1.59.4.12  2001/11/09 15:11:09  rmorris
// *** empty log message ***
//
// Revision 1.59.4.11  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.63  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.59.4.10  2001/10/02 17:01:52  jimg
// Made the behavior of serialize and deserialize uniform. Both methods now
// use Error exceptions to signal problems with network I/O and InternalErr
// exceptions to signal other problems. The return codes, always true for
// serialize and always false for deserialize, are now meaningless. However,
// by always returning a code that means OK, old code should continue to work.
//
// Revision 1.62  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.59.4.9  2001/09/26 23:25:42  jimg
// Returned the set_level() and level() methods to the class.
//
// Revision 1.59.4.8  2001/09/25 20:30:15  jimg
// Added is_linear().
//
// Revision 1.59.4.7  2001/09/07 00:38:35  jimg
// Sequence::deserialize(...) now reads all the sequence values at once.
// Its call semantics are the same as the other classes' versions. Values
// are stored in the Sequence object using a vector<BaseType *> for each
// row (those are themselves held in a vector). Three new accessor methods
// have been added to Sequence (row_value() and two versions of var_value()).
// BaseType::deserialize(...) now always returns true. This matches with the
// expectations of most client code (the seqeunce version returned false
// when it was done reading, but all the calls for sequences must be changed
// anyway). If an XDR error is found, deserialize throws InternalErr.
//
// Revision 1.61  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.59.4.6  2001/08/18 00:16:22  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.59.4.5  2001/07/28 01:10:42  jimg
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
// Revision 1.60  2001/06/15 23:49:02  jimg
// Merged with release-3-2-4.
//
// Revision 1.59.4.4  2001/06/07 16:55:53  jimg
// Changed the logic of print_all_vals() so that it work when a sequence is
// returned that contains no values. This can happen if a Sequence is
// constrained to values that don't appear (e.g., date ranges).
// Changed the printing of the closing brace to match what happens in
// Structure::print_all_vals(). Braces will be balanced for Sequences held
// within Structures, et cetera.
//
// Revision 1.59.4.3  2001/06/05 06:49:19  jimg
// Added the Constructor class which is to Structures, Sequences and Grids
// what Vector is to Arrays and Lists. This should be used in future
// refactorings (I thought it was going to be used for the back pointers).
// Introduced back pointers so children can refer to their parents in
// hierarchies of variables.
// Added to Sequence methods to tell if a child sequence is done
// deserializing its data.
// Fixed the operator=() and copy ctors; removed redundency from
// _duplicate().
// Changed the way serialize and deserialize work for sequences. Now SOI and
// EOS markers are written for every `level' of a nested Sequence. This
// should fixed nested Sequences. There is still considerable work to do
// for these to work in all cases.
//
// Revision 1.59.4.2  2001/05/16 18:58:26  dan
// Modified serialize method to break out of while loop after
// serializing member elements if the current sequence is not
// at level 0.
//
// Revision 1.59.4.1  2001/05/12 00:05:07  jimg
// Fixed bugs in _duplicate(). The new fields associated with access by row
// number were not being copied.
// Added a toString() implementation.
//
// Revision 1.59  2000/10/06 01:26:05  jimg
// Changed the way serialize() calls read(). The status from read() is
// returned by the Structure and Sequence serialize() methods; ignored by
// all others. Any exceptions thrown by read() are caught and discarded.
// serialize() returns false if read() throws an exception. This should
// be fixed once all the servers build using the new read() definition.
//
// Revision 1.58  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.57  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.56  2000/09/11 16:31:48  jimg
// Added methods to make it simpler to access Sequences by row number. The new
// methods are: get_row(), get_row_number(), get_starting_row_number(),
// get_ending_row_number(), get_row_stride(), set_row_number_constraint(). The
// starting and ending row numbers refer to constraints placed on the sequence,
// to get the number of rows in the current sequence, use the length() method.
//
// Revision 1.55  2000/08/16 00:38:22  jimg
// Added the getRowNumber method.
//
// Revision 1.54  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.53  2000/06/16 18:15:00  jimg
// Merged with 3.1.7
//
// Revision 1.49.6.3  2000/06/07 23:05:30  jimg
// The first_*() methods return 0 if there are no variables
//
// Revision 1.51.2.2  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.51.2.1  2000/01/28 22:14:05  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.52  2000/01/27 06:29:57  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.49.6.2  2000/01/26 23:57:15  jimg
// Fixed the return type of string::find.
//
// Revision 1.51  1999/12/31 00:55:11  jimg
// Fixed up the progress indicator
//
// Revision 1.50  1999/09/03 22:07:44  jimg
// Merged changes from release-3-1-1
//
// Revision 1.49.6.1  1999/08/28 06:43:04  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.49  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.48  1999/04/22 22:28:26  jimg
// Uses dynamic_cast
//
// Revision 1.47  1998/09/17 17:18:39  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
//
// Revision 1.46.6.1  1999/02/02 21:57:01  jimg
// String to string version
//
// Revision 1.46  1998/04/03 17:41:42  jimg
// Patch from Jake Hamby to print_all_vals(). deserialize needed to be called
// before the first call to print_val.
//
// Revision 1.45  1998/03/26 00:20:04  jimg
// Removed DODS_DEBUG define.
//
// Revision 1.44  1998/03/17 17:39:24  jimg
// Added an implementation of element_count().
//
// Revision 1.43  1998/02/19 19:41:27  jimg
// Changed name of ...end_of_sequence to ...start_of_sequence since that is
// now how it is used. I hope this will reduce confusion.
// Changed the name of read_end_marker to read_marker (since they are not
// always end markers anymore).
//
// Revision 1.42  1998/02/18 01:00:48  jimg
// Reverted to the old transfer scheme for Sequences.
// Added EOI/EOS markers.
// The code supports (via version numbers in the header of the return
// documents) reading from the old-style servers (which do not have EOI/EOS
// markers.
//
// Revision 1.41  1998/02/05 20:13:55  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.40  1997/12/31 20:55:15  jimg
// Changed name of read_level() to level() to reduce confusion in child
// classes. Also changed return type from unsigned int to int. This allows
// callers to store flags, etc. in the _level field - not the best thing, but
// a realistic expectation
//
// Revision 1.39  1997/12/16 01:36:18  jimg
// Merged changes from release-2.14d.
//
// Revision 1.38  1997/12/16 00:41:22  jimg
// Added debugging code to help finder problems reading sequences from older
// servers.
//
// Revision 1.37  1997/09/22 22:48:17  jimg
// Added DDS * to deserialize parameters.
// Added End of instance and end of sequence marker constants.
// Change serialize() and deserialize() mfuncs so that the new markers are
// used and embedded sequences are sent without needlessly replicating the
// enclosing sequence's instance.
//
// Revision 1.36  1997/07/15 21:53:25  jimg
// Changed length member function to return -1 instead of 0. Thus the default,
// when a subclass does not define its own version of this member function, is
// to return -1. This value will be easy to differentiate from 0, which could
// mean that the sequence has zero elements.
//
// Revision 1.35  1997/03/08 19:02:05  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.34  1997/02/28 01:27:57  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.33  1997/02/10 02:32:42  jimg
// Added assert statements for pointers
//
// Revision 1.32  1996/10/08 17:07:59  jimg
// Fixed deserialize so that when called on a null Sequence it will return
// true.
//
// Revision 1.31  1996/09/16 18:09:16  jimg
// Fixed var(const String name) so that it would correctly descend names of the
// form <base>.<name> where <name> may itself contain `dots'.
//
// Revision 1.30  1996/08/26 21:12:59  jimg
// Changes for version 2.07
//
// Revision 1.29  1996/08/13 18:36:12  jimg
// Added void casts to values computed in print_val() for loops.
//
// Revision 1.28  1996/06/04 21:33:37  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.27  1996/05/31 23:29:58  jimg
// Updated copyright notice.
//
// Revision 1.26  1996/05/29 22:08:46  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.25  1996/05/22 18:05:15  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.24  1996/05/16 22:44:52  jimg
// Dan's changes for 2.0.
//
// Revision 1.23  1996/05/14 15:38:35  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.22  1996/04/05 00:21:38  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.21  1996/04/04 18:10:46  jimg
// Merged changes from version 1.1.1.
// Fixed a bug in serialize() which caused the sequence serialization to end
// when the current CE first evaluated to false
//
// Revision 1.20  1996/03/05 17:44:21  jimg
// Added ce_eval to serailize member function.
//
// Revision 1.19  1996/02/02 00:31:12  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.18  1995/12/09  01:06:54  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.17  1995/12/06  21:56:29  jimg
// Added `constrained' flag to print_decl.
// Removed third parameter of read.
// Modified print_decl() to print only those parts of a dataset that are
// selected when `constrained' is true.
//
// Revision 1.16  1995/10/23  23:21:01  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.15  1995/08/26  00:31:43  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.14  1995/08/23  00:11:06  jimg
// Changed old, deprecated member functions to new ones.
// Switched from String representation of type to enum.
//
// Revision 1.13.2.1  1995/09/14 20:58:15  jimg
// Moved some loop index variables out of the loop statement.
//
// Revision 1.13  1995/07/09  21:29:03  jimg
// Added copyright notice.
//
// Revision 1.12  1995/05/10  15:34:03  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:27  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:29:11  jimg
// Added include config_dap.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.9  1995/03/04  14:34:49  jimg
// Major modifications to the transmission and representation of values:
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
//
// Revision 1.8  1995/02/10  02:23:02  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.7  1995/01/19  20:05:26  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// references.
//
// Revision 1.6  1995/01/11  15:54:53  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/15  21:21:13  dan
// Modified Sequence class to directly inherit from class BaseType
// Modified constructors to reflect new inheritance.
//
// Revision 1.4  1994/12/08  15:55:58  dan
// Added data transmission member functions serialize() and deserialize()
// Modified size() member function to return cumulative size of all members.
// Fixed the duplicate() member function to correctly access the data members
// of the sequence being duplicated.
//
// Revision 1.3  1994/10/17  23:34:49  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:48:31  jimg
// Fixed some errors in comments.

