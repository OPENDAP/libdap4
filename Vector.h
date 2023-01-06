
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// This is the interface definition file for the abstract class
// Vector. Vector is the parent class for List and Array.

#ifndef _vector_h
#define _vector_h 1

#include <cassert>

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

#ifndef constraint_evaluator_h
#include "ConstraintEvaluator.h"
#endif

class Crc32;

namespace libdap
{

/** Holds a one-dimensional array of DAP2 data types.  This class
    takes two forms, depending on whether the elements of the vector
    are themselves simple or compound objects. This class contains
    common functionality for the List and Array classes, and should
    rarely be used directly.

    When each element of the class is a simple data type, the Vector
    is implemented as a simple array of C types, rather than as an
    array of BaseType data types.  A single private ``template''
    BaseType instance (<tt>_var</tt>) is used to hold information in common
    to all the members of the array.  The template is also used as a
    container to pass values back and forth to an application
    program, as in <tt>var()</tt>.

    If the elements of the vector are themselves compound data
    types, the array is stored as a vector of BaseType pointers (see
    the libdap class <b>BaseTypePtrVec</b>). The template is still used to
    hold information in common to all the members of the array, but
    is not used to pass information to and from the application
    program.

    @brief Holds a one-dimensional collection of DAP2 data types.
    @see BaseType
    @see Array
*/
class Vector: public BaseType
{
private:
    // Add d_length_ll. This uses -1 as a sentinel value. jhrg 7/25/22
    // If we decide to add a bool for 'no values yet' do that as a
    // separate refactor. jhrg 7/25/22
    int64_t d_length_ll = -1;  	// number of elements in the vector

    int d_length = -1;  		    // number of elements in the vector
    BaseType *d_proto = nullptr;    // element prototype for the Vector

    // _buf was a pointer to void; delete[] complained. 6/4/2001 jhrg
    char *d_buf = nullptr;   		// storage for cardinal data
    vector<string> d_str;		    // special storage for strings. jhrg 2/11/05
    vector<BaseType *> d_compound_buf; 	// storage for data in compound types (e.g., Structure)

    // the number of elements we have allocated memory to store.
    // This should be either the sizeof(buf)/width(bool constrained = false) for cardinal data
    // or the capacity of d_str for strings or capacity of _vec.
    unsigned int d_capacity = 0;
    uint64_t d_capacity_ll = 0;

    bool d_too_big_for_dap2 = false;    /// Conditionally set to true in set_length_ll()

    friend class MarshallerTest;

    // Made these template methods private because they can't be
    // overridden anyways (because c++...) - ndp 08/14/2015
    template <typename T> void value_worker(T *v) const;
    template <typename T> void value_ll_worker(T *v) const;
    template <typename T> void value_worker(vector<unsigned int> *indices, T *b) const;
    template <typename T> void value_ll_worker(vector<uint64_t> *indices, T *b) const;

    template <typename T> bool set_value_worker(T *v, int sz);
    template <typename T> bool set_value_ll_worker(T *v, int64_t sz);
    template <typename T> bool set_value_worker(vector<T> &v, int sz);
    template <typename T> bool set_value_ll_worker(vector<T> &v, int64_t sz);

    bool m_is_cardinal_type() const;
    int64_t m_create_cardinal_data_buffer_for_type(int64_t num_elements);
    void m_delete_cardinal_data_buffer();
    template <class CardType> void m_set_cardinal_values_internal(const CardType* fromArray, int64_t num_elements);

    // This function copies the private members of Vector.
    void m_duplicate(const Vector &v);

public:
    Vector(const string &n, BaseType *v, const Type &t, bool is_dap4 = false);
    Vector(const string &n, const string &d, BaseType *v, const Type &t, bool is_dap4 = false);
    Vector(const Vector &rhs);

    virtual ~Vector();

    Vector &operator=(const Vector &rhs);
    // FIXME BaseType *ptr_duplicate() = 0 override;

    /**
     * Provide access to internal data. Callers cannot delete this
     * but can pass them to other methods.
     *
     * @note Added so that the NCML handler can code some optimizations in its
     * specialized versions of Array. jhrg 8/14/15
     * @return A pointer to the data buffer for Vectors/Arrays of the cardinal types.
     */
    char *get_buf() {
        return d_buf;
    }

    /**
     * Provide access to internal string data by reference. Callers cannot delete this
     * but can pass them to other methods.
     *
     * @return A reference to a vector of strings
     */
    vector<string> &get_str() {
        return d_str;
    }

    /**
     * Provide access to internal data by reference. Callers cannot delete this
     * but can pass them to other methods.
     *
     * @return A reference to a vector of BaseType pointers. Treat with care; never
     * delete these!
     */
    vector<BaseType*> &get_compound_buf() {
        return d_compound_buf;
    }

    virtual BaseType *prototype() const { return d_proto; }

    /**
     * @brief Change the Vector/Array element type
     * @param btp
     * @return The previous template, calling code is responsible for the returned BaseType lifecycle.
     */
    virtual BaseType *set_prototype(BaseType *btp) {  BaseType *orig = d_proto; d_proto = btp; return orig; }

    void set_name(const std::string& name) override;

    int element_count(bool leaves) override;

    void set_send_p(bool state) override;

    void set_read_p(bool state) override;

    /** Returns the number of bytes needed to hold the entire
        array.  This is equal to \c length() (the number of elements in
        in the array) times the width of each
        element.

        @brief Returns the width of the data, in bytes.
        @deprecated Use width_ll() instead */
    unsigned int width(bool constrained = false) const override
    {
        // Jose Garcia
        assert(d_proto);

        return length() * d_proto->width(constrained);
    }

    /**
     * @brief Return the number of bytes needed to hold the array data
     * @param constrained If true, return the number of bytes given the current constraint
     * @return The number of bytes needed to hold the array data, as a 64-bit integer
     */
    int64_t width_ll(bool constrained = false) const override
    {
        return length_ll() * d_proto->width_ll(constrained);
    }

    /** @brief Returns the number of elements in the vector.
     * Note that some child classes of Vector use the length of -1 as a flag value.
     * @return The number of elements in the vector
     * @deprecated Use length_ll() instead
     */
    int length() const override { return d_length; }

    /** @brief Get the number of elements in this Vector/Array
     * This version of the function deprecates length() which is limited to
     * 32-bit sizes. The field uses -1 as a sentinel value indicating that
     * the Vector/Array holds no values yet (as opposed to zero values).
     * @return The number of elements in this Vector/Array
     */
    int64_t length_ll() const override { return d_length_ll; }

    void set_length(int64_t l) override;

    void set_length_ll(int64_t l) override;


    // DAP2
    void intern_data(ConstraintEvaluator &eval, DDS &dds) override;
    bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval = true) override;
    bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false) override;

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    void intern_data(/*Crc32 &checksum*/) override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    unsigned int val2buf(void *val, bool reuse = false) override;
    unsigned int buf2val(void **val) override;

    uint64_t val2buf_ll(void *val, bool reuse = false);
    uint64_t buf2val_ll(void **val);


    void set_vec(unsigned int i, BaseType *val);
    void set_vec_nocopy(unsigned int i, BaseType * val);

    void set_vec_ll(uint64_t i, BaseType *val);
    void set_vec_nocopy_ll(uint64_t i, BaseType * val);

    void vec_resize(int l);
    void vec_resize_ll(int64_t l);

    void clear_local_data() override;


    virtual unsigned int get_value_capacity() const;
    virtual uint64_t get_value_capacity_ll() const;

    void set_value_capacity(uint64_t l);
    virtual void reserve_value_capacity(unsigned int numElements);
    virtual void reserve_value_capacity();
    virtual void reserve_value_capacity_ll(uint64_t numElements);
    virtual void reserve_value_capacity_ll();


    //virtual unsigned int set_value_slice_from_row_major_vector(const Vector& rowMajorData, unsigned int startElement);
    virtual uint64_t set_value_slice_from_row_major_vector(const Vector& rowMajorData, uint64_t startElement);

    virtual bool set_value(dods_byte *val, int sz);
    virtual bool set_value(dods_int8 *val, int sz);
    virtual bool set_value(dods_int16 *val, int sz);
    virtual bool set_value(dods_uint16 *val, int sz);
    virtual bool set_value(dods_int32 *val, int sz);
    virtual bool set_value(dods_uint32 *val, int sz);
    virtual bool set_value(dods_int64 *val, int sz);
    virtual bool set_value(dods_uint64 *val, int sz);
    virtual bool set_value(dods_float32 *val, int sz);
    virtual bool set_value(dods_float64 *val, int sz);
    virtual bool set_value(string *val, int sz);


    virtual bool set_value_ll(dods_byte *val, int64_t sz);
    virtual bool set_value_ll(dods_int8 *val, int64_t sz);
    virtual bool set_value_ll(dods_int16 *val, int64_t sz);
    virtual bool set_value_ll(dods_uint16 *val, int64_t sz);
    virtual bool set_value_ll(dods_int32 *val, int64_t sz);
    virtual bool set_value_ll(dods_uint32 *val, int64_t sz);
    virtual bool set_value_ll(dods_int64 *val, int64_t sz);
    virtual bool set_value_ll(dods_uint64 *val, int64_t sz);
    virtual bool set_value_ll(dods_float32 *val, int64_t sz);
    virtual bool set_value_ll(dods_float64 *val, int64_t sz);
    virtual bool set_value_ll(string *val, int64_t sz);

    virtual bool set_value(vector<dods_byte> &val, int sz);
    virtual bool set_value(vector<dods_int8> &val, int sz);
    virtual bool set_value(vector<dods_int16> &val, int sz);
    virtual bool set_value(vector<dods_uint16> &val, int sz);
    virtual bool set_value(vector<dods_int32> &val, int sz);
    virtual bool set_value(vector<dods_uint32> &val, int sz);
    virtual bool set_value(vector<dods_int64> &val, int sz);
    virtual bool set_value(vector<dods_uint64> &val, int sz);
    virtual bool set_value(vector<dods_float32> &val, int sz);
    virtual bool set_value(vector<dods_float64> &val, int sz);
    virtual bool set_value(vector<string> &val, int sz);

    virtual bool set_value_ll(vector<dods_byte> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_int8> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_int16> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_uint16> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_int32> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_uint32> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_int64> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_uint64> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_float32> &val, int64_t sz);
    virtual bool set_value_ll(vector<dods_float64> &val, int64_t sz);
    virtual bool set_value_ll(vector<string> &val, int64_t sz);



    virtual void value(dods_byte *b) const;
    virtual void value(dods_int8 *b) const;
    virtual void value(dods_int16 *b) const;
    virtual void value(dods_uint16 *b) const;
    virtual void value(dods_int32 *b) const;
    virtual void value(dods_uint32 *b) const;
    virtual void value(dods_int64 *b) const;
    virtual void value(dods_uint64 *b) const;
    virtual void value(dods_float32 *b) const;
    virtual void value(dods_float64 *b) const;
    virtual void value(vector<string> &b) const;

    virtual void value(vector<unsigned int> *indices, dods_byte *b) const;
    virtual void value(vector<unsigned int> *indices, dods_int8 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_int16 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_uint16 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_int32 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_uint32 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_int64 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_uint64 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_float32 *b) const;
    virtual void value(vector<unsigned int> *indices, dods_float64 *b) const;
    virtual void value(vector<unsigned int> *index, vector<string> &b) const;

    virtual void value_ll(vector<uint64_t> *indices, dods_byte *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_int8 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_int16 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_uint16 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_int32 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_uint32 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_int64 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_uint64 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_float32 *b) const;
    virtual void value_ll(vector<uint64_t> *indices, dods_float64 *b) const;
    virtual void value_ll(vector<uint64_t> *index, vector<string> &b) const;


    virtual void *value();

    BaseType *var(const string &name = "", bool exact_match = true, btp_stack *s = nullptr) override;
    BaseType *var(const string &name, btp_stack &s) override;

    virtual BaseType *var(unsigned int i);
    virtual BaseType *var_ll(uint64_t i);

    void add_var(BaseType *v, Part p = nil) override;
    void add_var_nocopy(BaseType *v, Part p = nil) override;

    bool check_semantics(string &msg, bool all = false) override;

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif /* _vector_h */
