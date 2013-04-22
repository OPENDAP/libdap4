
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
 */
class D4Enum: public BaseType
{
protected:
    char *d_buf;
    Type d_element_type;
    string d_enum_type;

    void m_duplicate(const D4Enum &src) {
        if (is_integer_type(d_element_type)) {
            d_buf = new char[m_type_width()];
            memcpy(d_buf, src.d_buf, m_type_width());
        }

        d_element_type = src.d_element_type;
    }

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
    : BaseType(name, dods_enum_c, true /*is_dap4*/),
      d_buf(0), d_element_type(dods_null_c), d_enum_type(enum_type) {
        if (is_integer_type(type)) {
            d_buf = new char[m_type_width()];
        }
    }
    D4Enum(const string &name, const string &dataset, Type type) :
        BaseType(name, dataset, dods_enum_c, true /*is_dap4*/), d_buf(0), d_element_type(type) {
        if (is_integer_type(type)) {
            d_buf = new char[m_type_width()];
        }
    }

    D4Enum(const D4Enum &src) : BaseType(src) { m_duplicate(src); }

    D4Enum &operator=(const D4Enum &rhs) {
        if (this == &rhs)
            return *this;
        static_cast<BaseType &>(*this) = rhs;
        m_duplicate(rhs);
        return *this;
    }

    virtual ~D4Enum() {
        delete d_buf;
    }

    virtual BaseType *ptr_duplicate() { return new D4Enum(*this); }

    Type element_type() { return d_element_type; }
    void set_element_type(Type type) { d_element_type = type; }

    virtual void value(dods_byte *v) const;
    virtual void value(dods_int16 *v) const;
    virtual void value(dods_uint16 *v) const;
    virtual void value(dods_int32 *v) const;
    virtual void value(dods_uint32 *v) const;
    virtual void value(dods_int64 *v) const;
    virtual void value(dods_uint64 *v) const;

    virtual void set_value(dods_byte v) ;
    virtual void set_value(dods_int16 v) ;
    virtual void set_value(dods_uint16 v) ;
    virtual void set_value(dods_int32 v) ;
    virtual void set_value(dods_uint32 v) ;
    virtual void set_value(dods_int64 v) ;
    virtual void set_value(dods_uint64 v) ;

    virtual unsigned int width() const { return m_type_width(); }

    virtual bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval = true);
    virtual bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false);

    virtual void print_val(ostream &out, string space = "",
                           bool print_decl_p = true);

    virtual void print_dap4(XMLWriter &xml, bool constrained = false);

    virtual bool ops(BaseType *b, int op);

    virtual void dump(ostream &strm) const ;

    unsigned int val2buf(void *, bool)  { throw InternalErr(__FILE__, __LINE__, "Not implemented for D4Enum"); }
    unsigned int buf2val(void **) { throw InternalErr(__FILE__, __LINE__, "Not implemented for D4Enum"); }
};

} // namespace libdap

#endif // _D4Enum_h

