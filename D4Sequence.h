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

#ifndef _sequence_h
#define _sequence_h 1

#include <stack>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

#ifndef S_XDRUtils_h
#include "XDRUtils.h"
#endif

namespace libdap
{

/** The type BaseTypeRow is used to store single rows of values in an
    instance of D4Sequence. Values are stored in instances of BaseType. */
typedef vector<BaseType *> BaseTypeRow;

/** This type holds all of the values of a D4Sequence. */
typedef vector<BaseTypeRow *> SequenceValues;

/** This is the interface for the class D4Sequence. A sequence contains
    a single set of variables, all at the same lexical level just like
    a Structure.  Like a Structure, a D4Sequence may contain other
    compound types, including other D4Sequences.  Unlike a Structure, a
    D4Sequence defines a pattern that is repeated N times for a sequence
    of N elements. It is useful to think of a D4Sequence as representing
    a table of values (like a relational database), with each row of
    the table corresponding to a D4Sequence ``instance.''  (This usage
    can be confusing, since ``instance'' also refers to a particular
    item of class D4Sequence.)  For example:

    <pre>
    D4Sequence {
      String name;
      Int32 age;
    } person;
    </pre>

    This represents a Sequence of ``person'' records, each instance of
    which contains a name and an age:

    <pre>
    Fred       34
    Ralph      23
    Andrea     29
    ...
    </pre>

    A D4Sequence can be arbitrarily long, which is to say that its
    length is not part of its declaration.  A D4Sequence can contain
    other D4Sequences:

    <pre>
    D4Sequence {
      String name;
      Int32 age;
      D4Sequence {
        String friend;
      } friend_list;
    } person;
    </pre>

    This is still represented as a single table, but each row contains
    the elements of both the main D4Sequence and the nested one:

    <pre>
    Fred       34     Norman
    Fred       34     Andrea
    Fred       34     Ralph
    Fred       34     Lisa
    Ralph      23     Norman
    Ralph      23     Andrea
    Ralph      23     Lisa
    Ralph      23     Marth
    Ralph      23     Throckmorton
    Ralph      23     Helga
    Ralph      23     Millicent
    Andrea     29     Ralph
    Andrea     29     Natasha
    Andrea     29     Norman
    ...        ..     ...
    </pre>

    Internally, the D4Sequence is represented by a vector of vectors. The
    members of the outer vector are the members of the D4Sequence. This
    includes the nested D4Sequences, as in the above example.

    NB: Note that in the past this class had a different behavior. It held
    only one row at a time and the deserialize(...) method had to be called
    from within a loop. This is <i>no longer true</i>. Now the
    deserailize(...) method should be called once and will read the entire
    sequence's values from the server. All the values are now stored in an
    instance of D4Sequence, not just a single row's.

    Because the length of a D4Sequence is indeterminate, there are
    changes to the behavior of the functions to read this class of
    data.  The <tt>read()</tt> function for D4Sequence must be written so that
    successive calls return values for successive rows of the D4Sequence.

    Similar to a C structure, you refer to members of D4Sequence
    elements with a ``.'' notation.  For example, if the D4Sequence has
    a member D4Sequence called ``Tom'' and Tom has a member Float32
    called ``shoe_size'', you can refer to Tom's shoe size as
    ``Tom.shoe_size''.

    @note This class contains the 'logic' for both the server- and client-side
    behavior. The field \e d_values is used by the client-side methods to store
    the entire D4Sequence. On the server-side, the read() method uses an underlying
    data system to read one row of data values which are then serialized using the
    serialize() methods of each variable.

    @todo Add an isEmpty() method which returns true if the D4Sequence is
    empty. This should work before and after calling deserialize().

    @brief Holds a sequence. */

class D4Sequence: public Constructor
{
private:
    // This holds the values of the sequence. Values are stored in
    // instances of BaseTypeRow objects which hold instances of BaseType.
	//
	// TODO Optimize
    SequenceValues d_values;

    // The number of the row that has just been deserialized. Before
    // deserialized has been called, this field is -1.
    int d_row_number;

    // If a client asks for certain rows of a sequence using the bracket
    // notation (<tt>[<start>:<stride>:<stop>]</tt>)
    // record that information in the next three fields.
    // Values of -1 indicate that these have not yet been set.
    int d_starting_row_number;
    int d_row_stride;
    int d_ending_row_number;

    bool is_end_of_rows(int i);

    friend class D4SequenceTest;

protected:
    void m_duplicate(const D4Sequence &s);
    typedef stack<SequenceValues*> sequence_values_stack_t;
#if 0
    virtual bool serialize_parent_part_one(DDS &dds,
                                           ConstraintEvaluator &eval,
					   Marshaller &m);
    virtual void serialize_parent_part_two(DDS &dds,
                                           ConstraintEvaluator &eval,
					   Marshaller &m);
    virtual bool serialize_leaf(DDS &dds,
                                ConstraintEvaluator &eval,
				Marshaller &m, bool ce_eval);

    virtual void intern_data_private( ConstraintEvaluator &eval,
                                      DDS &dds,
                                      sequence_values_stack_t &sequence_values_stack);
    virtual void intern_data_for_leaf(DDS &dds,
                                      ConstraintEvaluator &eval,
                                      sequence_values_stack_t &sequence_values_stack);

    virtual void intern_data_parent_part_one(DDS &dds,
            ConstraintEvaluator &eval,
            sequence_values_stack_t &sequence_values_stack);

    virtual void intern_data_parent_part_two(DDS &dds,
            ConstraintEvaluator &eval,
            sequence_values_stack_t &sequence_values_stack);
#endif
public:

    D4Sequence(const string &n);
    D4Sequence(const string &n, const string &d);

    D4Sequence(const D4Sequence &rhs);

    virtual ~D4Sequence();

    D4Sequence &operator=(const D4Sequence &rhs);

    virtual BaseType *ptr_duplicate();

    virtual bool is_linear();

    virtual int length();

    virtual int number_of_rows();

    virtual bool read_row(int row, DDS &dds,
                          ConstraintEvaluator &eval, bool ce_eval = true);
#if 0
    virtual void intern_data(ConstraintEvaluator &eval, DDS &dds) {
    	throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }
#endif
    virtual bool serialize(ConstraintEvaluator &, DDS &, Marshaller &, bool ) {
    	throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }
    virtual bool deserialize(UnMarshaller &, DDS *, bool ) {
    	throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }
#if 0
    // DAP4
    virtual void serialize(D4StreamMarshaller &m, DMR &dmr, ConstraintEvaluator &eval, bool filter = false) {
    	throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }
    virtual void deserialize(D4StreamUnMarshaller &um, DMR &dmr) {
    	throw InternalErr(__FILE__, __LINE__, "Not implemented for DAP4");
    }
#endif
    /// Rest the row number counter
    void reset_row_number();

    int get_starting_row_number();

    virtual int get_row_stride();

    virtual int get_ending_row_number();

    virtual void set_row_number_constraint(int start, int stop, int stride = 1);

    virtual void set_value(SequenceValues &values);
    virtual SequenceValues value();

    virtual BaseType *var_value(size_t row, const string &name);

    virtual BaseType *var_value(size_t row, size_t i);

    virtual BaseTypeRow *row_value(size_t row);

    virtual void print_one_row(ostream &out, int row, string space,
                               bool print_row_num = false);
    virtual void print_val_by_rows(ostream &out, string space = "",
                                   bool print_decl_p = true,
                                   bool print_row_numbers = true);
    virtual void print_val(ostream &out, string space = "",
                           bool print_decl_p = true);

    virtual void print_one_row(FILE *out, int row, string space,
                               bool print_row_num = false);
    virtual void print_val_by_rows(FILE *out, string space = "",
                                   bool print_decl_p = true,
                                   bool print_row_numbers = true);
    virtual void print_val(FILE *out, string space = "",
                           bool print_decl_p = true);

    virtual void dump(ostream &strm) const ;
};

} // namespace libdap

#endif //_sequence_h
