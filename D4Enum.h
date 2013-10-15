
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

#ifndef _D4Enum_h
#define _D4Enum_h 1

#include <cassert>

#include "BaseType.h"

#include "dods-datatypes.h"
#include "util.h"

namespace libdap
{

class ConstraintEvaluator;
class Marshaller;
class UnMarshaller;

/**
 * @brief Holds a DAP4 enumeration.
 * For a single enumeration, it makes little sense to optimize it's
 * in-memory storage. This class uses an unsigned 64-bit int to hold
 * the value and casts it to a signed value if needed. The various
 * set_value() methods ensure that the value stored never exceeds the
 * range of values associated with the declared type.
 *
 * @note When constructed a type for the Enum must be specified. If
 * it is not an integer type, the Enum will use unsigned int 64.
 */
class D4Enum: public BaseType
{
	friend class D4EnumTest;

private:
	/**
	 * @brief The empty constructor is not supported.
	 */
	D4Enum();

protected:
    uint64_t d_buf;
    Type d_element_type;

    void m_duplicate(const D4Enum &src) {
        d_buf = src.d_buf;
        d_element_type = src.d_element_type;
    }

    // TODO Use this only for an Array version of this type? jhrg 8/19/13
    unsigned int m_type_width() const {
        switch(d_element_type) {
            case dods_byte_c:
            case dods_int8_c:
            case dods_uint8_c:
                return 1;
            case dods_int16_c:
            case dods_uint16_c:
                return 2;
            case dods_int32_c:
            case dods_uint32_c:
                return 4;
            case dods_int64_c:
            case dods_uint64_c:
                return 8;
            case dods_null_c:
            default:
                return 0;
        }
    }

public:
    D4Enum(const string &name, const string &enum_type)
    : BaseType(name, dods_enum_c, true /*is_dap4*/), d_buf(0), d_element_type(dods_null_c)//, d_enum_type(enum_type)
	{
    	d_element_type = get_type(enum_type.c_str());

    	assert(is_integer_type(d_element_type));

        if (!is_integer_type(d_element_type))
        	d_element_type = dods_uint64_c;
    }

    D4Enum(const string &name, Type type)
		: BaseType(name, dods_enum_c, true /*is_dap4*/), d_buf(0), d_element_type(type)
	{
    	assert(is_integer_type(d_element_type));

        if (!is_integer_type(d_element_type))
        	d_element_type = dods_uint64_c;
    }

    D4Enum(const string &name, const string &dataset, Type type)
		: BaseType(name, dataset, dods_enum_c, true /*is_dap4*/), d_buf(0), d_element_type(type)
	{
    	assert(is_integer_type(d_element_type));

        if (!is_integer_type(d_element_type))
        	d_element_type = dods_uint64_c;
    }

    D4Enum(const D4Enum &src) : BaseType(src) { m_duplicate(src); }

    D4Enum &operator=(const D4Enum &rhs) {
        if (this == &rhs)
            return *this;
        static_cast<BaseType &>(*this) = rhs;
        m_duplicate(rhs);
        return *this;
    }

    virtual ~D4Enum() { }

    virtual BaseType *ptr_duplicate() { return new D4Enum(*this); }

    Type element_type() { return d_element_type; }
    void set_element_type(Type type) { d_element_type = type; }

    /**
     * @brief Copy the value of this Enum into \c v.
     * Template member function that can be used to read the value of the
     * Enum. This template is explicitly instantiated so libdap includes
     * D4Enum::value(dods_byte* v), ..., value(dods_uint64) (i.e., all
     * of the integer types).
     *
     * @param v Value-result parameter; return the value of the Enum
     * in this variable.
     */
    template <typename T> void value(T *v) const { *v = static_cast<T>(d_buf); }

    /**
     * @brief Set the value of the Enum
     * Template member function to set the value of the Enum. The libdap library
     * contains versions of this member function for dods_byte, ..., dods_uint64
     * types for the parameter \c v.
     *
     * @param v Set the Enum to this value.
     */
    template <typename T> void set_value(T v) { d_buf = static_cast<unsigned long long>(v); }

    /**
     * @brief Return the number of bytes in an instance of an Enum.
     * This returns the number of bytes an instance of Enum will use
     * either in memory or on the wire (i.e., in a serialization of
     * the type).
     *
     * @note This version of the method works for scalar Enums only.
     * @return The number of bytes used by a value.
     */
    virtual unsigned int width(bool /* constrained */ = false) {
    	return sizeof(d_buf); //return m_type_width();
    }

    // DAP4
    virtual void compute_checksum(Crc32 &checksum);
    virtual void serialize(D4StreamMarshaller &m, DMR &dmr, ConstraintEvaluator &eval, bool filter = false);

    virtual void print_val(ostream &out, string space = "", bool print_decl_p = true);

    virtual void print_xml_writer(XMLWriter &xml, bool constrained);

    //virtual void print_dap4(XMLWriter &xml, bool constrained = false);

    virtual bool ops(BaseType *b, int op);

    virtual void dump(ostream &strm) const ;

    unsigned int val2buf(void *, bool)  { throw InternalErr(__FILE__, __LINE__, "Not implemented for D4Enum"); }
    unsigned int buf2val(void **) { throw InternalErr(__FILE__, __LINE__, "Not implemented for D4Enum"); }
};

} // namespace libdap

#endif // _D4Enum_h

