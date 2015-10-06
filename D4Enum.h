
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

#include "InternalErr.h"
#include "dods-datatypes.h"
#include "util.h"

namespace libdap
{

class D4EnumDef;
class ConstraintEvaluator;
class Marshaller;
class UnMarshaller;

/**
 * @brief Holds a DAP4 enumeration.
 *
 * @note When constructed a type for the Enum must be specified. If
 * it is not an integer type, the Enum will use unsigned int 64. This
 * is not the same as the enumeration type that is defined using the
 * Enumeration XML element in the DMR - that information is stored
 * in additional fields and used for checking values and printing the
 * variable's declaration, but not for the internal storage of values.
 *
 * @todo Note the hack to remove the union...
 */
class D4Enum: public BaseType
{
	friend class D4EnumTest;

public:
#if 0
    union enum_value {
    	int8_t i8;
    	uint8_t ui8;
    	int16_t i16;
    	uint16_t ui16;
    	int32_t i32;
    	uint32_t ui32;
    	int64_t i64;
    	uint64_t ui64;

    	enum_value() : ui64(0) { }

    	enum_value(int8_t i) : i8(i) {}
    	enum_value(uint8_t i) : ui8(i) {}
    	enum_value(int16_t i) : i16(i) {}
    	enum_value(uint16_t i) : ui16(i) {}
    	enum_value(int32_t i) : i32(i) {}
    	enum_value(uint32_t i) : ui32(i) {}
    	enum_value(int64_t i) : i64(i) {}
    	enum_value(uint64_t i) : ui64(i) {}

    	// cast operators; use by set_value()
    	operator int8_t() const { return i8; }
    	operator uint8_t() const { return ui8; }
    	operator int16_t() const { return i16; }
    	operator uint16_t() const { return ui16; }
    	operator int32_t() const { return i32; }
    	operator uint32_t() const { return ui32; }
    	operator int64_t() const { return i64; }
    	operator uint64_t() const { return ui64; }
    };
#endif
private:
#if 0
    enum_value d_buf;
#endif

    uint64_t d_buf;

    Type d_element_type;
    D4EnumDef *d_enum_def;	// The enumeration defined in the DMR, not an integer type
    bool d_is_signed;

    void m_duplicate(const D4Enum &src);

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
            	assert(!"illegal type for D4Enum");
            	return 0;
        }
    }

    D4Enum();	// No empty constructor

public:
	// TODO add a way to set the EnumDef to these
    D4Enum(const string &name, const string &enum_type) :
        BaseType(name, dods_enum_c, true /*is_dap4*/), d_buf(/*(uint64_t)*/ 0), d_element_type(dods_null_c), d_enum_def(0)
    {
        d_element_type = get_type(enum_type.c_str());

        if (!is_integer_type(d_element_type)) d_element_type = dods_uint64_c;
        set_is_signed(d_element_type);
    }

    D4Enum(const string &name, Type type) :
        BaseType(name, dods_enum_c, true /*is_dap4*/), d_buf(/*(uint64_t)*/ 0), d_element_type(type), d_enum_def(0)
    {
        if (!is_integer_type(d_element_type)) d_element_type = dods_uint64_c;
        set_is_signed(d_element_type);
    }

    D4Enum(const string &name, const string &dataset, Type type) :
        BaseType(name, dataset, dods_enum_c, true /*is_dap4*/), d_buf(/*(uint64_t)*/ 0), d_element_type(type), d_enum_def(0)
    {
        if (!is_integer_type(d_element_type)) d_element_type = dods_uint64_c;
        set_is_signed(d_element_type);
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

    virtual D4EnumDef *enumeration() const { return d_enum_def; }
    virtual void set_enumeration(D4EnumDef *enum_def);

    virtual BaseType *ptr_duplicate() { return new D4Enum(*this); }

    Type element_type() { return d_element_type; }
    void set_element_type(Type type) { d_element_type = type; }

    bool is_signed() const { return d_is_signed; }
    void set_is_signed(Type t) {
    	switch (t) {
    	case dods_byte_c:
    	case dods_uint8_c:
    	case dods_uint16_c:
    	case dods_uint32_c:
    	case dods_uint64_c:
    		d_is_signed = false;
    		break;

    	case dods_int8_c:
    	case dods_int16_c:
    	case dods_int32_c:
    	case dods_int64_c:
    		d_is_signed =  true;
    		break;

    	default:
    		assert(!"illegal type for D4Enum");
    		throw InternalErr(__FILE__, __LINE__, "Illegal type");
    	}
    }
	/**
	 * @todo Hack!
	 *
	 * @param v Value-result parameter; return the value of the Enum
	 * in this variable.
	 */
	template<typename T> void value(T *v) const {
		*v = static_cast<T>(d_buf);
	}


    /**
     * @brief Set the value of the Enum
     * Template member function to set the value of the Enum. The libdap library
     * contains versions of this member function for dods_byte, ..., dods_uint64
     * types for the parameter \c v.
     *
     * @param v Set the Enum to this value.
     */
    template <typename T> void set_value(T v)
    {
    	d_buf = v;
#if 0
    	// save this for use later for type checking
    	switch (d_element_type) {
		case dods_byte_c:
		case dods_uint8_c:
			if (v > 255 || v < 0)
				blah...
			break;
		case dods_uint16_c:
			d_buf.ui16 = v;
			break;
		case dods_uint32_c:
			d_buf.ui32 = v;
			break;
		case dods_uint64_c:
			d_buf.ui64 = v;
			break;

		case dods_int8_c:
			d_buf.i8 = v;
			break;
		case dods_int16_c:
			d_buf.i16 = v;
			break;
		case dods_int32_c:
			d_buf.i32 = v;
			break;
		case dods_int64_c:
			d_buf.i64 = v;
			break;
		default:
			assert(!"illegal type for D4Enum");
		}
#endif
    }

    /**
     * @brief Return the number of bytes in an instance of an Enum.
     * This returns the number of bytes an instance of Enum will use
     * either in memory or on the wire (i.e., in a serialization of
     * the type).
     *
     * @note This version of the method works for scalar Enums only.
     * @return The number of bytes used by a value.
     */
    virtual unsigned int width(bool /* constrained */ = false) const { return m_type_width(); }

    // DAP4
    virtual void compute_checksum(Crc32 &checksum);
    virtual void serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter = false);
    virtual void deserialize(D4StreamUnMarshaller &um, DMR &dmr);

    virtual void print_val(ostream &out, string space = "", bool print_decl_p = true);

    virtual void print_xml_writer(XMLWriter &xml, bool constrained);

    virtual bool ops(BaseType *b, int op);

    virtual void dump(ostream &strm) const;

    unsigned int val2buf(void *, bool);
    unsigned int buf2val(void **);
};

} // namespace libdap

#endif // _D4Enum_h

