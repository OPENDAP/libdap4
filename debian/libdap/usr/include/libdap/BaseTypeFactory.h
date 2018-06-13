
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2005 OPeNDAP, Inc.
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

#ifndef base_type_factory_h
#define base_type_factory_h

#include <string>

#include "Type.h"
#include "InternalErr.h"

// Class declarations; Make sure to include the corresponding headers in the
// implementation file.

namespace libdap
{

class Byte;
class Int16;
class UInt16;
class Int32;
class UInt32;
class Float32;
class Float64;
class Str;
class Url;
class Array;
class Structure;
class Sequence;
class Grid;
class BaseType;

/** A factory to create instances of the leaf nodes of BaseType (Byte, ...
    Grid). Clients of libdap++ which require special behavior for the types
    should subclass this factory and provide an implementation which creates
    instances of those specializations. Make sure to pass a reference to the
    new factory to DDS's constructor since by default it uses this factory.

    To define and use your own factory, first make sure that you are not
    using the compile time constant 'DEFAULT_BASETYPE_FACTORY.' Then pass a
    pointer to an instance of your factory to the DDS/DataDDS constructors.
    When the parser is used to build a DDS from a DAP response, the factory
    will be used to instantiate the different variable-type classes.

    @note The easiest way to subclass this is to follow the pattern of using
    a separate class declaration and implementation. It's possible to use one
    file to hold
    both, but that is complicated somewhat because DDS.h, which includes this
    class, also includes many of the type classes (Array.h, ..., Grid.h) and
    the order of their inclusion can create compilation problems where the
    Vector and/or Constructor base classes are not defined. It's easiest to
    split the declaration and implementation and include forward declarations
    of the type classes in the declaration (\c .h) file and then include the
    type class' headers in the implementation (\c .cc) file.

    @author James Gallagher
    @see DDS */
class BaseTypeFactory
{
public:
    BaseTypeFactory()
    {}
    virtual ~BaseTypeFactory()
    {}

    /**
     * Build a new variable and return it using a BaseType pointer. The
     * type of the variable is given using  Type enumeration.
     *
     * @note Added for DAP4
     *
     * @param t The type of the variable to create
     * @parma name The (optional) name of the variable.
     */
    virtual BaseType *NewVariable(Type t, const string &name = "") const;

    /**
     * Clone this object and return a pointer to the clone.
     *
     * @note added for DAP4
     */
    virtual BaseTypeFactory *ptr_duplicate() const {
        throw InternalErr(__FILE__, __LINE__, "Not Implemented.");
    }

    virtual Byte *NewByte(const string &n = "") const;
    virtual Int16 *NewInt16(const string &n = "") const;
    virtual UInt16 *NewUInt16(const string &n = "") const;
    virtual Int32 *NewInt32(const string &n = "") const;
    virtual UInt32 *NewUInt32(const string &n = "") const;
    virtual Float32 *NewFloat32(const string &n = "") const;
    virtual Float64 *NewFloat64(const string &n = "") const;

    virtual Str *NewStr(const string &n = "") const;
    virtual Url *NewUrl(const string &n = "") const;

    virtual Array *NewArray(const string &n = "", BaseType *v = 0) const;
    virtual Structure *NewStructure(const string &n = "") const;
    virtual Sequence *NewSequence(const string &n = "") const;
    virtual Grid *NewGrid(const string &n = "") const;
};

} // namespace libdap

#endif // base_type_factory_h
