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


#include "config.h"

#include <algorithm>
#include <string>
#include <sstream>

//#define DODS_DEBUG

#include "debug.h"
#include "Error.h"
#include "InternalErr.h"
#include "Sequence.h"
#include "DDS.h"
#include "DataDDS.h"
#include "util.h"
#include "InternalErr.h"
#include "escaping.h"

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
    d_leaf_sequence = s.d_leaf_sequence;
    d_unsent_data = s.d_unsent_data;
    d_wrote_soi = s.d_wrote_soi;
    d_top_most = s.d_top_most;

    Sequence &cs = const_cast<Sequence &>(s);

    // Copy the template BaseType objects.
    for (Vars_iter i = cs.var_begin(); i != cs.var_end(); i++) {
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

static void
write_end_of_sequence(Marshaller &m)
{
    m.put_opaque( (char *)&end_of_sequence, 1 ) ;
}

static void
write_start_of_instance(Marshaller &m)
{
    m.put_opaque( (char *)&start_of_instance, 1 ) ;
}

static unsigned char
read_marker(UnMarshaller &um)
{
    unsigned char marker;
    um.get_opaque( (char *)&marker, 1 ) ;

    return marker;
}

static bool
is_start_of_instance(unsigned char marker)
{
    return (marker == start_of_instance);
}

static bool
is_end_of_sequence(unsigned char marker)
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
        d_row_stride(1), d_ending_row_number(-1),
        d_unsent_data(false), d_wrote_soi(false),
        d_leaf_sequence(false), d_top_most(false)
{}

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
    DBG2(cerr << "In delete_bt: " << bt_ptr << endl);
    delete bt_ptr; bt_ptr = 0;
}

static inline void
delete_rows(BaseTypeRow *bt_row_ptr)
{
    DBG2(cerr << "In delete_rows: " << bt_row_ptr << endl);

    for_each(bt_row_ptr->begin(), bt_row_ptr->end(), delete_bt);

    delete bt_row_ptr; bt_row_ptr = 0;
}

Sequence::~Sequence()
{
    DBG2(cerr << "Entering Sequence::~Sequence" << endl);
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        BaseType *btp = *i ;
        delete btp ; btp = 0;
    }

    for_each(d_values.begin(), d_values.end(), delete_rows);
    DBG2(cerr << "exiting Sequence::~Sequence" << endl);
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

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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
        for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
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
    for (Vars_iter iter = _vars.begin(); linear && iter != _vars.end(); iter++) {
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
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
        (*i)->set_send_p(state);
    }

    BaseType::set_send_p(state);
}

void
Sequence::set_read_p(bool state)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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

    @param bt A pointer to the DAP2 type variable to add to this Sequence.
    @param part defaults to nil */
void
Sequence::add_var(BaseType *bt, Part)
{
    if (!bt)
        throw InternalErr(__FILE__, __LINE__,
                          "Cannot add variable: NULL pointer");
    // Jose Garcia
    // We append a copy of bt so the owner
    // of bt is free to deallocate as he wishes.
    DBG2(cerr << "In Sequence::add_var(), bt: " << bt << endl);
    DBG2(cerr << bt->toString() << endl);

    BaseType *bt_copy = bt->ptr_duplicate();
    bt_copy->set_parent(this);
    _vars.push_back(bt_copy);

    DBG2(cerr << "In Sequence::add_var(), bt_copy: " << bt_copy << endl);
    DBG2(cerr << bt_copy->toString() << endl);
}

// Deprecated
BaseType *
Sequence::var(const string &n, btp_stack &s)
{
    string name = www2id(n);

    BaseType *btp = m_exact_match(name, &s);
    if (btp)
        return btp;

    return m_leaf_match(name, &s);
}

BaseType *
Sequence::var(const string &name, bool exact_match, btp_stack *s)
{
    string n = www2id(name);

    if (exact_match)
        return m_exact_match(n, s);
    else
        return m_leaf_match(n, s);
}

BaseType *
Sequence::m_leaf_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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
Sequence::m_exact_match(const string &name, btp_stack *s)
{
    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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
            return 0;  // qualified names must be *fully* qualified
    }

    return 0;
}

/** @brief Get a whole row from the sequence.
    @param row Get row number <i>row</i> from the sequence.
    @return A BaseTypeRow object (vector<BaseType *>). Null if there's no such
    row number as \e row. */
BaseTypeRow *
Sequence::row_value(size_t row)
{
    if (row >= d_values.size())
        return 0;
    return d_values[row];
}

/** Set value of this Sequence. This does not perform a deep copy, so data
    should be allocated on the heap and freed only when the Sequence dtor is
    called.
    @see SequenceValues
    @see BaseTypeRow
    @param values Set the value of this Sequence. */
void
Sequence::set_value(SequenceValues &values)
{
    d_values = values;
}

/** Get the value for this sequence.
    @return The SequenceValues object for this Sequence. */
SequenceValues
Sequence::value()
{
    return d_values;
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

unsigned int
Sequence::width()
{
    unsigned int sz = 0;

    for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
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

/** When reading a nested sequence, use this method to reset the internal
    row number counter. This is necessary so that the second, ... instances
    of the inner/nested sequence will start off reading row zero. */
void
Sequence::reset_row_number()
{
    d_row_number = -1;
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

/** Read row number <i>row</i> of the Sequence. The values of the row
    are obtained by calling the read() method of the sequence. The
    current \e row just read is stored in the Sequence instance
    along with its row number. If a selection expression has been
    supplied, rows are counted only if they satisfy that expression.

    Note that we can only advance in a Sequence. It is not possible to back up
    and read a row numbered lower than the current row. If you need that
    you will need to replace the serialize() method with one of your own.

    Used on the server side.

    @note The first row is row number zero. A Sequence with 100 rows will
    have row numbers 0 to 99.

    @return A boolean value, with TRUE indicating that read_row
    should be called again because there's more data to be read.
    FALSE indicates the end of the Sequence.
    @param row The row number to read.
    @param dataset A string, often a file name, used to refer to t he
    dataset.
    @param dds A reference to the DDS for this dataset.
    @param eval Use this as the constraint expression evaluator.
    @param ce_eval If True, evaluate any CE, otherwise do not.
*/
bool
Sequence::read_row(int row, const string &dataset, DDS &dds,
                   ConstraintEvaluator &eval, bool ce_eval)
{
    DBG2(cerr << "Entering Sequence::read_row for " << name() << endl);
    if (row < d_row_number)
        throw InternalErr("Trying to back up inside a sequence!");

    DBG2(cerr << "read_row: row number " << row << ", current row " << d_row_number
         << endl);
    if (row == d_row_number)
        return true;

    dds.timeout_on();

    int eof = 0;  // Start out assuming EOF is false.
    while (!eof && d_row_number < row) {
        if (!read_p()) {
            eof = (read(dataset) == false);
        }

        // Advance the row number if ce_eval is false (we're not supposed to
        // evaluate the selection) or both ce_eval and the selection are
        // true.
        if (!eof && (!ce_eval || eval.eval_selection(dds, dataset)))
            d_row_number++;

        set_read_p(false); // ...so that the next instance will be read
    }

    // Once we finish the above loop, set read_p to true so that the caller
    // knows that data *has* been read. This is how the read() methods of the
    // elements of the sequence know to not call read() but instead look for
    // data values inside themselves.
    set_read_p(true);

    dds.timeout_off();

    // Return true if we have valid data, false if we've read to the EOF.
    return eof == 0;
}

// Private. This is used to process constraints on the rows of a sequence.
// Starting with 3.2 we support constraints like Sequence[10:2:20]. This
// odd-looking logic first checks if d_ending_row_number is the sentinel
// value of -1. If so, the sequence was not constrained by row number and
// this method should never return true (which indicates that we're at the
// end of a row-number constraint). If d_ending_row_number is not -1, then is
// \e i at the end point? 6/1/2001 jhrg
inline bool
Sequence::is_end_of_rows(int i)
{
    return ((d_ending_row_number == -1) ? false : (i > d_ending_row_number));
}

/** Serialize a Sequence.

    Leaf Sequences must be marked as such (see DDS::tag_nested_sequence()),
    as must the top most Sequence.

    How the code works. Methods called for various functions are named in
    brackets:
    <ol>
    <li>Sending a one-level sequence:
    <pre>
    Dataset {
        Sequence {
     Int x;
     Int y;
 } flat;
    } case_1;
    </pre>

    Serialize it by reading successive rows and sending all of those that
    satisfy the CE. Before each row, send a start of instance (SOI) marker.
    Once all rows have been sent, send an End of Sequence (EOS)
    marker.[serialize_leaf].</li>

    <li>Sending a nested sequence:
    <pre>
    Dataset {
        Sequence {
     Int t;
     Sequence {
         Int z;
     } inner;
 } outer;
    } case_2;
    </pre>

    Serialize by reading the first row of outer and storing the values. Do
    not evaluate the CE [serialize_parent_part_one]. Call serialize() for inner
    and read each row for it, evaluating the CE for each row that is read.
    After the first row of inner is read and satisfies the CE, write out the
    SOI marker and values for outer [serialize_parent_part_two], then write
    the SOI and values for the first row of inner. Continue to read and send
    rows of inner until the last row has been read. Send EOS for inner
    [serialize_leaf]. Now read the next row of outer and repeat. Once outer\
    is completely read, send its EOS marker.</li>
    </ol>

    Notes:
    <ol>
    <li>For a nested Sequence, the child sequence must follow all other types
    in the parent sequence (like the example). There may be only one nested
    Sequence per level.</li>

    <li>CE evaluation happens only in a leaf sequence.</li>

    <li>When no data statisfies a CE, the empty Sequence is signalled by a
    single EOS marker, regardless of the level of nesting of Sequences. That
    is, the EOS marker is sent for only the outer Sequence in the case of a
    completely empty response.</li>
    </ol>
*/
bool
Sequence::serialize(const string &dataset, ConstraintEvaluator &eval, DDS &dds,
                    Marshaller &m, bool ce_eval)
{
    DBG2(cerr << "Entering Sequence::serialize for " << name() << endl);

    // Special case leaf sequences!
    if (is_leaf_sequence())
        return serialize_leaf(dataset, dds, eval, m, ce_eval);
    else
        return serialize_parent_part_one(dataset, dds, eval, m);
}

// We know this is not a leaf Sequence. That means that this Sequence holds
// another Sequence as one of its fields _and_ that child Sequence triggers
// the actual transmission of values.

bool
Sequence::serialize_parent_part_one(const string &dataset, DDS &dds,
                                    ConstraintEvaluator &eval, Marshaller &m)
{
    DBG2(cerr << "Entering serialize_parent_part_one for " << name() << endl);

    int i = (d_starting_row_number != -1) ? d_starting_row_number : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    // Since this is a parent sequence, read the row ignoring the CE (all of
    // the CE clauses will be evaluated by the leaf sequence).
    bool status = read_row(i, dataset, dds, eval, false);
    DBG2(cerr << "Sequence::serialize_parent_part_one::read_row() status: " << status << endl);

    while (status && !is_end_of_rows(i)) {
        i += d_row_stride;

        // DBG(cerr << "Writing Start of Instance marker" << endl);
        // write_start_of_instance(sink);

        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
            // Only call serialize for child Sequences; the leaf sequence
            // will trigger the transmission of values for its parents (this
            // sequence and maybe others) once it gets soem valid data to
            // send.
            // Note that if the leaf sequence has no variables in the current
            // projection, its serialize() method will never be called and that's
            // the method that triggers actually sending values. Thus the leaf
            // sequence must be the lowest level sequence with values whose send_p
            // property is true.
            if ((*iter)->send_p() && (*iter)->type() == dods_sequence_c)
                (*iter)->serialize(dataset, eval, dds, m);
        }

        set_read_p(false); // ...so this will read the next instance

        status = read_row(i, dataset, dds, eval, false);
        DBG(cerr << "Sequence::serialize_parent_part_one::read_row() status: " << status << endl);
    }
    // Reset current row number for next nested sequence element.
    d_row_number = -1;

    // Always write the EOS marker? 12/23/04 jhrg
    // Yes. According to DAP2, a completely empty response is signalled by
    // a return value of only the EOS marker for the outermost sequence.
    if (d_top_most || d_wrote_soi) {
        DBG(cerr << "Writing End of Sequence marker" << endl);
        write_end_of_sequence(m);
        d_wrote_soi = false;
    }

    return true;  // Signal errors with exceptions.
}

// If we are here then we know that this is 'parent sequence' and that the
// leaf seq has found valid data to send. We also know that
// serialize_parent_part_one has been called so data are in the instance's
// fields. This is wheree we send data. Whereas ..._part_one() contains a
// loop to iterate over all of rows in a parent sequence, this does not. This
// method assumes that the serialize_leaf() will call it each time it needs
// to be called.
//
// NB: This code only works if the child sequences appear after all other
// variables.
void
Sequence::serialize_parent_part_two(const string &dataset, DDS &dds,
                                    ConstraintEvaluator &eval, Marshaller &m)
{
    DBG(cerr << "Entering serialize_parent_part_two for " << name() << endl);

    BaseType *btp = get_parent();
    if (btp && btp->type() == dods_sequence_c)
        dynamic_cast<Sequence&>(*btp).serialize_parent_part_two(dataset, dds,
                eval, m);

    if (d_unsent_data) {
        DBG(cerr << "Writing Start of Instance marker" << endl);
        d_wrote_soi = true;
        write_start_of_instance(m);

        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
            // Send all the non-sequence variables
            DBG(cerr << "Sequence::serialize_parent_part_two(), serializing "
                << (*iter)->name() << endl);
            if ((*iter)->send_p() && (*iter)->type() != dods_sequence_c) {
                DBG(cerr << "Send P is true, sending " << (*iter)->name() << endl);
                (*iter)->serialize(dataset, eval, dds, m, false);
            }
        }

        d_unsent_data = false;              // read should set this.
    }
}

// This code is only run by a leaf sequence. Note that a one level sequence
// is also a leaf sequence.
bool
Sequence::serialize_leaf(const string &dataset, DDS &dds,
                         ConstraintEvaluator &eval, Marshaller &m, bool ce_eval)
{
    DBG(cerr << "Entering Sequence::serialize_leaf for " << name() << endl);
    int i = (d_starting_row_number != -1) ? d_starting_row_number : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    bool status = read_row(i, dataset, dds, eval, ce_eval);
    DBG(cerr << "Sequence::serialize_leaf::read_row() status: " << status << endl);

    // Once the first valid (satisfies the CE) row of the leaf sequence has
    // been read, we know we're going to send data. Send the current instance
    // of the parent/ancestor sequences now, if there are any. We only need
    // to do this once, hence it's not inside the while loop, but we only
    // send the parent seq data _if_ there's data in the leaf to send, that's
    // why we wait until after the first call to read_row() here in the leaf
    // sequence.
    //
    // NB: It's important to only call serialize_parent_part_two() for a
    // Sequence that really is the parent of a leaf sequence. The fancy cast
    // will throw and exception if btp is not a Sequence, but doesn't test
    // that it's a parent sequence as we've defined them here.
    if (status && !is_end_of_rows(i)) {
        BaseType *btp = get_parent();
        if (btp && btp->type() == dods_sequence_c)
            dynamic_cast<Sequence&>(*btp).serialize_parent_part_two(dataset,
                    dds, eval, m);
    }

    d_wrote_soi = false;
    while (status && !is_end_of_rows(i)) {
        i += d_row_stride;

        DBG(cerr << "Writing Start of Instance marker" << endl);
        d_wrote_soi = true;
        write_start_of_instance(m);

        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
            DBG(cerr << "Sequence::serialize_leaf(), serializing "
                << (*iter)->name() << endl);
            if ((*iter)->send_p()) {
                DBG(cerr << "Send P is true, sending " << (*iter)->name() << endl);
                (*iter)->serialize(dataset, eval, dds, m, false);
            }
        }

        set_read_p(false); // ...so this will read the next instance

        status = read_row(i, dataset, dds, eval, ce_eval);
        DBG(cerr << "Sequence::serialize_leaf::read_row() status: " << status << endl);
    }

    // Only write the EOS marker if there's a matching Start Of Instnace
    // Marker in the stream.
    if (d_wrote_soi || d_top_most) {
        DBG(cerr << "Writing End of Sequence marker" << endl);
        write_end_of_sequence(m);
    }

    return true;  // Signal errors with exceptions.
}

/** This method is used to evaluate a constraint and based on those results
    load the Sequence variable with data. This simulates having a server call
    the serialize() method and a client call the deserialize() method without
    the overhead of any IPC. Use this method on the server-side to 'load the
    d_values field with data' so that other code and work with those data.

    The somewhat odd algorithm used by serialize() is largely copied here, so
    comments about logic in serialize() and the related methods apply here
    as well.

    @note Even though each Sequence variable has a \e values field, only the
    top-most Sequence in a hierarchy of Sequences holds values. The field
    accessed by the var_value() method is completely linked object; access
    the values of nested Sequences using the BaseType objects returned by
    var_value().

    @note Only call this method for top-most Sequences. Never call it for
    Sequences which have a parent (directly or indirectly) variable that is
    a Sequence.

    @param dataset The name of the data set
    @param eval Use this contraint evaluator
    @param dds This DDS holds the variables for the data source */
void
Sequence::intern_data(const string &dataset, ConstraintEvaluator &eval, DDS &dds)
{
    DBG(cerr << "Entering Sequence::transfer_data for " << name() << endl);

    DBG(cerr << "Sequence::transfer_data, values: " << &d_values << endl);

    sequence_values_stack_t sequence_values_stack;
    sequence_values_stack.push_back(&d_values);

    transfer_data_private(dataset, eval, dds, sequence_values_stack);
}

void
Sequence::transfer_data_private(const string &dataset,
                                ConstraintEvaluator &eval,
                                DDS &dds,
                                sequence_values_stack_t &sequence_values_stack)
{
    DBG(cerr << "Entering Sequence::transfer_data_private for " << name() << endl);

    if (is_leaf_sequence()) {
        transfer_data_for_leaf(dataset, dds, eval, sequence_values_stack);
    }
    else {
        transfer_data_parent_part_one(dataset, dds, eval, sequence_values_stack);
    }
}

void
Sequence::transfer_data_for_leaf(const string &dataset, DDS &dds,
                                 ConstraintEvaluator &eval,
                                 sequence_values_stack_t &sequence_values_stack)
{
    int i = (get_starting_row_number() != -1) ? get_starting_row_number() : 0;

    bool status = read_row(i, dataset, dds, eval, true);

    if (status && (get_ending_row_number() == -1 || i <= get_ending_row_number())) {
        BaseType *btp = get_parent();
        if (btp && btp->type() == dods_sequence_c) {
            dynamic_cast<Sequence&>(*btp).transfer_data_parent_part_two(dataset, dds, eval, sequence_values_stack);
        }
    }

    SequenceValues *values = sequence_values_stack.back();
    DBG(cerr << "In td_for_leaf, using values = " << values << endl);

    while (status && (get_ending_row_number() == -1 || i <= get_ending_row_number())) {
        i += get_row_stride();

        BaseTypeRow *row_data = new BaseTypeRow;

        // In this loop serialize will signal an error with an exception.
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++) {
            if ((*iter)->send_p()) {
                row_data->push_back((*iter)->ptr_duplicate());
            }
        }

        DBG(cerr << "Pushing values for " << name() << " to " << values << endl);
        // Save the row_data to the values().
        values->push_back(row_data);
        set_read_p(false);      // ...so this will read the next instance
        status = read_row(i, dataset, dds, eval, true);
    }

    sequence_values_stack.pop_back();
    DBG(cerr << "In td_for_leaf, Poping the top of the stack" << endl);;
}


void
Sequence::transfer_data_parent_part_one(const string & dataset, DDS & dds,
                                        ConstraintEvaluator & eval,
                                        sequence_values_stack_t &
                                        sequence_values_stack)
{
    DBG(cerr << "Entering transfer_data_parent_part_one for " << name() <<
        endl);

    int i =
        (get_starting_row_number() != -1) ? get_starting_row_number() : 0;

    // read_row returns true if valid data was read, false if the EOF was
    // found. 6/1/2001 jhrg
    // Since this is a parent sequence, read the row ignoring the CE (all of
    // the CE clauses will be evaluated by the leaf sequence).
    bool status = read_row(i, dataset, dds, eval, false);

    while (status
           && (get_ending_row_number() == -1
               || i <= get_ending_row_number())) {
        i += get_row_stride();
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++) {
            if ((*iter)->send_p()) {
                switch ((*iter)->type()) {
                case dods_sequence_c:
                    dynamic_cast <
                    Sequence & >(**iter).transfer_data_private(dataset,
                            eval,
                            dds,
                            sequence_values_stack);
                    break;

                case dods_structure_c:
                    dynamic_cast <Structure & >(**iter).intern_data(dataset, eval, dds);
                    break;

                default:
                    (*iter)->read(dataset);
                    break;
                }
            }
        }

        set_read_p(false);      // ...so this will read the next instance

        status = read_row(i, dataset, dds, eval, false);
    }

    // Reset current row number for next nested sequence element.
    reset_row_number();
    sequence_values_stack.pop_back();
    DBG(cerr << "In td_fparent_part_one, Poping the top of the stack" <<
        endl);;
}

void
Sequence::transfer_data_parent_part_two(const string &dataset, DDS &dds,
                                        ConstraintEvaluator &eval,
                                        sequence_values_stack_t &sequence_values_stack)
{
    DBG(cerr << "Entering transfer_data_parent_part_two for " << name() << endl);

    BaseType *btp = get_parent();
    if (btp && btp->type() == dods_sequence_c) {
        dynamic_cast<Sequence&>(*btp).transfer_data_parent_part_two(dataset, dds, eval, sequence_values_stack);
    }

    SequenceValues *values = sequence_values_stack.back();
    DBG(cerr << "In td_parent_part_two, using values = " << values << endl);

    if (get_unsent_data()) {
        BaseTypeRow *row_data = new BaseTypeRow;

        // In this loop transfer_data will signal an error with an exception.
        for (Vars_iter iter = var_begin(); iter != var_end(); iter++) {

            if ((*iter)->send_p() && (*iter)->type() != dods_sequence_c) {
                row_data->push_back((*iter)->ptr_duplicate());
            }
            else if ((*iter)->send_p()) { //Sequence, and it must be last the variable
                Sequence *tmp = dynamic_cast<Sequence*>((*iter)->ptr_duplicate());
                row_data->push_back(tmp);
                DBG(cerr << "In td_parent_part_two, pushing values = "
                    << &(dynamic_cast<Sequence&>(*tmp).d_values) << endl);
                sequence_values_stack.push_back(&(dynamic_cast<Sequence&>(*tmp).d_values));
            }
        }

        DBG(cerr << "Pushing values for " << name() << " to " << values << endl);
        values->push_back(row_data);
        set_unsent_data(false);
    }
}


/** @brief Deserialize (read from the network) the entire Sequence.

    This method used to read a single row at a time. Now the entire
    sequence is read at once. The method used to return True to indicate
    that more data needed to be deserialized and False when the sequence
    was completely read. Now it simply returns false. This might seem odd,
    but making this method return true breaks existing software the least.

    @param um An UnMarshaller that knows how to deserialize data
    @param dds A DataDDS from which to read.
    @param reuse Passed to child objects when they are deserialized.
    @exception Error if a sequence stream marker cannot be read.
    @exception InternalErr if the <tt>dds</tt> param is not a DataDDS.
    @return A return value of false indicates that an EOS ("end of
    Sequence") marker was found, while a value of true indicates
    that there are more rows to be read.
*/
bool
Sequence::deserialize(UnMarshaller &um, DDS *dds, bool reuse)
{
    DataDDS *dd = dynamic_cast<DataDDS *>(dds);
    if (!dd)
        throw InternalErr("Expected argument 'dds' to be a DataDDS!");

    DBG2(cerr << "Reading from server/protocol version: "
         << dd->get_protocol_major() << "." << dd->get_protocol_minor()
         << endl);

    // Check for old servers.
    if (dd->get_protocol_major() < 2) {
        throw Error(string("The protocl version (") + dd->get_protocol() 
		    + ") indicates that this\nis an old server which may not correctly transmit Sequence variables.\nContact the server administrator.");
    }

    while (true) {
        // Grab the sequence stream's marker.
        unsigned char marker = read_marker(um);
        if (is_end_of_sequence(marker))
            break;  // EXIT the while loop here!!!
        else if (is_start_of_instance(marker)) {
            d_row_number++;
            DBG2(cerr << "Reading row " << d_row_number << " of "
                 << name() << endl);
            BaseTypeRow *bt_row_ptr = new BaseTypeRow;
            // Read the instance's values, building up the row
            for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
                BaseType *bt_ptr = (*iter)->ptr_duplicate();
                bt_ptr->deserialize(um, dds, reuse);
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

/** Never use this interface for Sequence! To add data to the members of a
    Sequence, use BaseTypeRow variables and operate on them individually. */
unsigned int
Sequence::val2buf(void *, bool)
{
    throw InternalErr(__FILE__, __LINE__, "Never use this method; see the programmer's guide documentation.");
    return sizeof(Sequence);
}

/** Never use this interface for Sequence! Use Sequence::var_value() or
    Sequence::row_value().

    @deprecated */
unsigned int
Sequence::buf2val(void **)
{
    throw InternalErr(__FILE__, __LINE__, "Use Sequence::var_value() or Sequence::row_value() in place of Sequence::buf2val()");
    return sizeof(Sequence);
}

void
Sequence::print_one_row(FILE *out, int row, string space,
                        bool print_row_num)
{
    if (print_row_num)
        fprintf(out, "\n%s%d: ", space.c_str(), row) ;

    fprintf(out, "{ ") ;

    int elements = element_count() - 1;
    int j;
    BaseType *bt_ptr;
    // Print first N-1 elements of the row.
    for (j = 0; j < elements; ++j) {
        bt_ptr = var_value(row, j);
        if (bt_ptr) {  // data
            if (bt_ptr->type() == dods_sequence_c)
                dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
                (out, space + "    ", false, print_row_num);
            else
                bt_ptr->print_val(out, space, false);
            fprintf(out, ", ") ;
        }
    }

    // Print Nth element; end with a `}.'
    bt_ptr = var_value(row, j);
    if (bt_ptr) {  // data
        if (bt_ptr->type() == dods_sequence_c)
            dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
            (out, space + "    ", false, print_row_num);
        else
            bt_ptr->print_val(out, space, false);
    }

    fprintf(out, " }") ;
}

void
Sequence::print_one_row(ostream &out, int row, string space,
                        bool print_row_num)
{
    if (print_row_num)
	out << "\n" << space << row << ": " ;

    out << "{ " ;

    int elements = element_count() - 1;
    int j;
    BaseType *bt_ptr;
    // Print first N-1 elements of the row.
    for (j = 0; j < elements; ++j) {
        bt_ptr = var_value(row, j);
        if (bt_ptr) {  // data
            if (bt_ptr->type() == dods_sequence_c)
                dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
                (out, space + "    ", false, print_row_num);
            else
                bt_ptr->print_val(out, space, false);
	    out << ", " ;
        }
    }

    // Print Nth element; end with a `}.'
    bt_ptr = var_value(row, j);
    if (bt_ptr) {  // data
        if (bt_ptr->type() == dods_sequence_c)
            dynamic_cast<Sequence*>(bt_ptr)->print_val_by_rows
            (out, space + "    ", false, print_row_num);
        else
            bt_ptr->print_val(out, space, false);
    }

    out << " }" ;
}

void
Sequence::print_val_by_rows(FILE *out, string space, bool print_decl_p,
                            bool print_row_numbers)
{
    if (print_decl_p) {
        print_decl(out, space, false);
        fprintf(out, " = ") ;
    }

    fprintf(out, "{ ") ;

    int rows = number_of_rows() - 1;
    int i;
    for (i = 0; i < rows; ++i) {
        print_one_row(out, i, space, print_row_numbers);
        fprintf(out, ", ") ;
    }
    print_one_row(out, i, space, print_row_numbers);

    fprintf(out, " }") ;

    if (print_decl_p)
        fprintf(out, ";\n") ;
}

void
Sequence::print_val_by_rows(ostream &out, string space, bool print_decl_p,
                            bool print_row_numbers)
{
    if (print_decl_p) {
        print_decl(out, space, false);
	out << " = " ;
    }

    out << "{ " ;

    int rows = number_of_rows() - 1;
    int i;
    for (i = 0; i < rows; ++i) {
        print_one_row(out, i, space, print_row_numbers);
	out << ", " ;
    }
    print_one_row(out, i, space, print_row_numbers);

    out << " }" ;

    if (print_decl_p)
	out << ";\n" ;
}

void
Sequence::print_val(FILE *out, string space, bool print_decl_p)
{
    print_val_by_rows(out, space, print_decl_p, false);
}

void
Sequence::print_val(ostream &out, string space, bool print_decl_p)
{
    print_val_by_rows(out, space, print_decl_p, false);
}


bool
Sequence::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
        return false;

    if (!unique_names(_vars, name(), type_name(), msg))
        return false;

    if (all)
        for (Vars_iter i = _vars.begin(); i != _vars.end(); i++) {
            if (!(*i)->check_semantics(msg, true)) {
                return false;
            }
        }

    return true;
}

void
Sequence::set_leaf_p(bool state)
{
    d_leaf_sequence = state;
}

bool
Sequence::is_leaf_sequence()
{
    return d_leaf_sequence;
}

/** @brief Mark the Sequence which holds the leaf elements.

    In a nested Sequence, the Sequence which holds the leaf elements is special
    because it during the serialization of this Sequence's data that constraint
    Expressions must be evaluated. If CEs are evaluated at the upper levels,
    then valid data may not be sent because it was effectlively hidden from the
    serialization and evaluation code (see the documentatin for the serialize_leaf()
    method).

    The notion of the leaf Sequence needs to be modified to mean the lowest level
    of a Sequence where data are to be sent. Suppose there's a two level Sequence,
    but that only fields from the top level are to be sent. Then that top level
    is also the leaf Sequence and should be marked as such. If the lower level is
    marked as a leaf Sequence, then no values will ever be sent since the send_p
    property will always be false for each field and it's the call to
    serialize_leaf() that actually triggers transmission of values (because it's
    not until the code makes it into serialize_leaf() that it knows there are
    values to be sent.

    @note This method \e must not be called before the CE is parsed.

    @param lvl The current level of the Sequence. a \e lvl of 1 indicates the
    topmost Sequence. The default value is 1.
    @see Sequence::serialize_leaf() */
void
Sequence::set_leaf_sequence(int lvl)
{
    bool has_child_sequence = false;

    if (lvl == 1) d_top_most = true;

    DBG2(cerr << "Processing sequence " << name() << endl);

    for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
        // About the test for send_p(): Only descend into a sequence if it has
        // fields that might be sent. Thus if, in a two-level sequence, nothing
        // in the lower level is to be sent, the upper level is marked as the
        // leaf sequence. This ensures that values _will_ be sent (see the comment
        // in serialize_leaf() and serialize_parent_part_one()).
        if ((*iter)->type() == dods_sequence_c && (*iter)->send_p()) {
            if (has_child_sequence)
                throw Error("This implementation does not support more than one nested sequence at a level. Contact the server administrator.");

            has_child_sequence = true;
            dynamic_cast<Sequence&>(**iter).set_leaf_sequence(++lvl);
        }
        else if ((*iter)->type() == dods_structure_c) {
            dynamic_cast<Structure&>(**iter).set_leaf_sequence(lvl);
        }
    }

    if (!has_child_sequence)
        set_leaf_p(true);
    else
        set_leaf_p(false);

    DBG2(cerr << "is_leaf_sequence(): " << is_leaf_sequence() << " (" << name() << ")" << endl);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
Sequence::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Sequence::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    Constructor::dump(strm) ;
    strm << DapIndent::LMarg << "# rows deserialized: " << d_row_number
         << endl ;
    strm << DapIndent::LMarg << "bracket notation information:" << endl ;
    DapIndent::Indent() ;
    strm << DapIndent::LMarg << "starting row #: " << d_starting_row_number
         << endl ;
    strm << DapIndent::LMarg << "row stride: " << d_row_stride << endl ;
    strm << DapIndent::LMarg << "ending row #: " << d_ending_row_number
         << endl ;
    DapIndent::UnIndent() ;

    strm << DapIndent::LMarg << "data been sent? " << d_unsent_data << endl ;
    strm << DapIndent::LMarg << "start of instance? " << d_wrote_soi << endl ;
    strm << DapIndent::LMarg << "is leaf sequence? " << d_leaf_sequence
         << endl ;
    strm << DapIndent::LMarg << "top most in hierarchy? " << d_top_most
         << endl ;
    DapIndent::UnIndent() ;
}

