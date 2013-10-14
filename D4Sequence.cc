// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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

#include "config.h"

#include <algorithm>
#include <string>
#include <sstream>

#define DODS_DEBUG
//#define DODS_DEBUG2

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "D4Sequence.h"

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"

#include "debug.h"
#include "Error.h"
#include "InternalErr.h"
#include "util.h"
#include "escaping.h"

using namespace std;

namespace libdap {

#if 0
// Keep this stuff around in case we decide to switch back to sentinels

static const unsigned char end_of_sequence = 0xA5;// binary pattern 1010 0101
static const unsigned char start_of_instance = 0x5A;// binary pattern 0101 1010

static void
write_end_of_sequence(Marshaller &m)
{
	m.put_opaque( (char *)&end_of_sequence, 1 );
}

static void
write_start_of_instance(Marshaller &m)
{
	m.put_opaque( (char *)&start_of_instance, 1 );
}

static unsigned char
read_marker(UnMarshaller &um)
{
	unsigned char marker;
	um.get_opaque( (char *)&marker, 1 );

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
#endif

// Private member functions

// A reminder of these type defs
//
// typedef vector<BaseType *> D4SeqRow;
// typedef vector<D4SeqRow *> D4SeqValues;
// D4SeqValues d_values;

void D4Sequence::m_duplicate(const D4Sequence &s)
{
	d_row_number = s.d_row_number;
	d_starting_row_number = s.d_starting_row_number;
	d_ending_row_number = s.d_ending_row_number;
	d_row_stride = s.d_row_stride;

	// Deep copy for the values
	for (D4SeqValues::const_iterator i = s.d_values.begin(), e = s.d_values.end(); i != e; ++i) {
		D4SeqRow &row = **i;
		D4SeqRow *dest = new D4SeqRow;
		for (D4SeqRow::const_iterator j = row.begin(), e = row.end(); j != e; ++j) {
			// *j is a BaseType*
			dest->push_back((*j)->ptr_duplicate());
		}

		d_values.push_back(dest);
	}
#if 0
	D4Sequence &cs = const_cast<D4Sequence &>(s);

	// Copy the BaseType objects used to hold values.
	// TODO Review this
	// Deep copy of d_values

	for (vector<D4SeqRow *>::iterator rows_iter = cs.d_values.begin();
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
#endif
}

// Public member functions

/** The Sequence constructor requires only the name of the variable
 to be created.  The name may be omitted, which will create a
 nameless variable.  This may be adequate for some applications.

 @param n A string containing the name of the variable to be
 created.

 @brief The Sequence constructor. */
D4Sequence::D4Sequence(const string &n) : Constructor(n, dods_sequence_c, true /* is dap4 */),
		d_row_number(-1), d_starting_row_number(-1), d_row_stride(1), d_ending_row_number(-1)
{
}

/** The Sequence server-side constructor requires the name of the variable
 to be created and the dataset name from which this variable is being
 created.

 @param n A string containing the name of the variable to be
 created.
 @param d A string containing the name of the dataset from which this
 variable is being created.

 @brief The Sequence server-side constructor. */
D4Sequence::D4Sequence(const string &n, const string &d) : Constructor(n, d, dods_sequence_c, true /* is dap4 */),
		d_row_number(-1), d_starting_row_number(-1), d_row_stride(1), d_ending_row_number(-1)
{
}

/** @brief The Sequence copy constructor. */
D4Sequence::D4Sequence(const D4Sequence &rhs) : Constructor(rhs)
{
	m_duplicate(rhs);
}

BaseType *
D4Sequence::ptr_duplicate()
{
	return new D4Sequence(*this);
}

static inline void delete_bt(BaseType *bt_ptr)
{
	delete bt_ptr;
}

static inline void delete_rows(D4SeqRow *bt_row_ptr)
{
	for_each(bt_row_ptr->begin(), bt_row_ptr->end(), delete_bt);

	delete bt_row_ptr;
}

D4Sequence::~D4Sequence()
{
	for_each(d_values.begin(), d_values.end(), delete_rows);
}

D4Sequence &
D4Sequence::operator=(const D4Sequence &rhs)
{
	if (this == &rhs) return *this;

	dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

	m_duplicate(rhs);

	return *this;
}

/**
 * @brief Read the next instance of the sequence
 * While the rest of the variables' read() methods are assumed to return the entire
 * variable in one call (modulo enhancements of the library to support streaming
 * large variables), this class assumes that the underlying data store is return
 * data from a table of unknown size. Thus, D4Sequence::read() is assumed to return
 * one instance (or element or row) of the sequence per call and return true when the
 * EOF (end of the sequence) is reached.
 *
 * For each call to read, the values for each of the sequence's members
 * are expected to have been loaded into the member's BaseType variables; this
 * method will copy them out and store then in the D4Sequence's internal storage.
 * This method always returns the next instance that satisfies the CE when 'filter'
 * is true.
 *
 * @note this method is called by D4Sequence::serialize() and it will evaluate the
 * CE for each set of values read.
 *
 * @param dmr
 * @param eval
 * @param filter
 * @return False when read() indicates that the EOF was found, true otherwise.
 */
bool D4Sequence::read_next_instance(DMR &/*dmr*/, ConstraintEvaluator &/*eval*/, bool filter)
{
    bool eof = false;
    bool done = false;

    do {
        eof = read();
        // Advance the row number if ce_eval is false (we're not supposed to
        // evaluate the selection) or both filter and the selection are
        // true.
        // FIXME CE's not supported for DAP4 yet. jhrg 10/11/13
        filter = false;
        if (!eof && (!filter /*|| eval.eval_selection(dmr, dataset()*/)) {
            d_row_number++;
            done = true;
        }
    } while (!eof && !done);

    DBG(cerr << "D4Sequence::read_next_instance eof: " << eof << endl);
    return !eof;
}

/**
 * @brief Serialize the values of a D4Sequence
 * This method assumes that the underlying data store cannot/does not return a count
 * of items separately from the items themselves. For a data store that does, this
 * method should probably be specialized to take advantage of that. Because the DAP4
 * spec requires that a sequence be prefixed by a count, this method reads the entire
 * sequence into memory before sending it (and counts the number of elements in the
 * the process). For a data store where the count is available a priori, this could
 * be rewritten so that the count is sent and then each instance/element of the sequence
 * sent in succession.
 *
 * @param m Stream data sink
 * @param dmr DMR object for the evaluator
 * @param eval CE Evaluator object
 * @param filter True if the CE should be evaluated, false otherwise.
 */
void
D4Sequence::serialize(D4StreamMarshaller &m, DMR &dmr, ConstraintEvaluator &eval, bool filter)
{
	// Read the data values, then serialize.
	while (read_next_instance(dmr, eval, filter)) {
	    D4SeqRow *row = new D4SeqRow;
	    for (Vars_iter i = d_vars.begin(), e = d_vars.end(); i != e; i++) {
	        if ((*i)->send_p()) {
	            // store the variable's value.
	            row->push_back((*i)->ptr_duplicate());
	        }
	    }
	    d_values.push_back(row);
	    DBG(cerr << "D4Sequence::serialize Added row" << endl);
	}

    // write D4Sequecne::length(); don't include the length in the checksum
    int64_t count = length(); 	// TODO remove once length() gets normalized to int64_t
    m.put_count(count);
    DBG(cerr << "D4Sequence::serialize count: " << count << endl);

    // By this point the d_values object holds all and only the values to be sent;
    // use the serialize methods to send them (but no need to test send_p).
    for (D4SeqValues::iterator i = d_values.begin(), e = d_values.end(); i != e; ++i) {
        for (D4SeqRow::iterator j = (*i)->begin(), f = (*i)->end(); j != f; ++j) {
            (*j)->serialize(m, dmr, eval, false);
        }
    }
}

void D4Sequence::deserialize(D4StreamUnMarshaller &um, DMR &dmr)
{
    int64_t count = um.get_count();
    set_length(count);
    DBG(cerr << "D4Sequence::deserialize count: " << count << endl);

    while (count-- > 0) {
        D4SeqRow *row = new D4SeqRow;
        for (Vars_iter i = d_vars.begin(), e = d_vars.end(); i != e; ++i) {
            (*i)->deserialize(um, dmr);
            row->push_back((*i)->ptr_duplicate());
        }
        d_values.push_back(row);
    }
}

#if 0
/** Set the start, stop and stride for a row-number type constraint.
 This should be used only when the sequence is constrained using the
 bracket notation (which supplies start, stride and stop information).
 If omitted, the stride defaults to 1.

 @param start The starting row number. The first row is row zero.
 @param stop The ending row number. The 20th row is row 19.
 @param stride The stride. A stride of two skips every other row. */
virtual void set_row_number_constraint(int start, int stop, int stride)
{
    if (stop < start)
    throw Error(malformed_expr, "Starting row number must precede the ending row number.");

    d_starting_row_number = start;
    d_row_stride = stride;
    d_ending_row_number = stop;
}
#endif

/** @brief Get a whole row from the sequence.
 @param row Get row number <i>row</i> from the sequence.
 @return A BaseTypeRow object (vector<BaseType *>). Null if there's no such
 row number as \e row. */
D4SeqRow *
D4Sequence::row_value(size_t row)
{
	if (row >= d_values.size()) return 0;
	return d_values[row];
}

static bool
base_type_name_eq(BaseType *btp, const string name)
{
	return btp->name() == name;
}

/** @brief Get the BaseType pointer to the named variable of a given row.
 @param row Read from <i>row</i> in the sequence.
 @param name Return <i>name</i> from <i>row</i>.
 @return A BaseType which holds the variable and its value.
 @see number_of_rows */
BaseType *
D4Sequence::var_value(size_t row_num, const string &name)
{
	D4SeqRow *row = row_value(row_num);
	if (!row) return 0;

	D4SeqRow::iterator elem = find_if(row->begin(), row->end(), bind2nd(ptr_fun(base_type_name_eq), name));
	return (elem != row->end()) ? *elem: 0;
}

/** @brief Get the BaseType pointer to the $i^{th}$ variable of <i>row</i>.
 @param row Read from <i>row</i> in the sequence.
 @param i Return the $i^{th}$ variable from <i>row</i>.
 @return A BaseType which holds the variable and its value.
 @see number_of_rows */
BaseType *
D4Sequence::var_value(size_t row_num, size_t i)
{
	D4SeqRow *row = row_value(row_num);
	if (!row) return 0;

	if (i >= row->size()) return 0;

	return (*row)[i];
}

void D4Sequence::print_one_row(ostream &out, int row, string space, bool print_row_num)
{
	if (print_row_num) out << "\n" << space << row << ": ";

	out << "{ ";

	int elements = element_count();
	int j = 0;
	BaseType *bt_ptr = 0;

	// This version of print_one_row() works for both data read with
	// deserialize(), where each variable is assumed to have valid data, and
	// intern_data(), where some/many variables do not. Because of that, it's
	// not correct to assume that all of the elements will be printed, which
	// is what the old code did.

	// Print the first value
	while (j < elements && !bt_ptr) {
		bt_ptr = var_value(row, j++);
		if (bt_ptr) {  // data
			if (bt_ptr->type() == dods_sequence_c)
				static_cast<D4Sequence*>(bt_ptr)->print_val_by_rows(out, space + "    ", false, print_row_num);
			else
				bt_ptr->print_val(out, space, false);
		}
	}

	// Print the remaining values
	while (j < elements) {
		bt_ptr = var_value(row, j++);
		if (bt_ptr) {  // data
			out << ", ";
			if (bt_ptr->type() == dods_sequence_c)
				static_cast<D4Sequence*>(bt_ptr)->print_val_by_rows(out, space + "    ", false, print_row_num);
			else
				bt_ptr->print_val(out, space, false);
		}
	}

	out << " }";
}

void D4Sequence::print_val_by_rows(ostream &out, string space, bool print_decl_p, bool print_row_numbers)
{
	if (print_decl_p) {
		print_decl(out, space, false);
		out << " = ";
	}

	out << "{ ";

	int rows = length();
	int i;
	for (i = 0; i < rows; ++i) {
		print_one_row(out, i, space, print_row_numbers);
		out << ", ";
	}
	print_one_row(out, i, space, print_row_numbers);

	out << " }";

	if (print_decl_p) out << ";\n";
}

void D4Sequence::print_val(ostream &out, string space, bool print_decl_p)
{
	print_val_by_rows(out, space, print_decl_p, false);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and information about this
 * instance.
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void D4Sequence::dump(ostream &strm) const
{
	strm << DapIndent::LMarg << "Sequence::dump - (" << (void *) this << ")" << endl;
	DapIndent::Indent();
	Constructor::dump(strm);
	strm << DapIndent::LMarg << "# rows deserialized: " << d_row_number << endl;
	strm << DapIndent::LMarg << "bracket notation information:" << endl;

	DapIndent::Indent();
	strm << DapIndent::LMarg << "starting row #: " << d_starting_row_number << endl;
	strm << DapIndent::LMarg << "row stride: " << d_row_stride << endl;
	strm << DapIndent::LMarg << "ending row #: " << d_ending_row_number << endl;
	DapIndent::UnIndent();

	DapIndent::UnIndent();
}

} // namespace libdap

