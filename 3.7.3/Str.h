
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for Str type.
//
// jhrg 9/7/94

#ifndef _str_h
#define _str_h 1


#ifdef WIN32
#include <rpc.h>
#include <winsock2.h>
#include <xdr.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include <string>

#include "dods-limits.h"
#include "BaseType.h"


// max_str_len should be large since we always send strings with length bytes
// as a prefix (so xdr_string will always know how much memory to malloc) but
// if deserialize gets confused and thinks a ctor (in particular) is a string
// xdr_string in turn will max_str_len if it cannot get a length byte. A long
// term solution is to fix libdap, but strings should not routinely be > 32k
// for the time being... jhrg 4/30/97

const unsigned int max_str_len = 32767; // DODS_UINT_MAX-1; 

/** @brief Holds character string data.

    @see BaseType
    @see Url
    */
    
class Str: public BaseType {

protected:
    string _buf;

public:
    Str(const string &n = "");

    virtual ~Str() {}

    Str(const Str &copy_from);

    Str &operator=(const Str &rhs);

    virtual BaseType *ptr_duplicate();
    
    virtual unsigned int width();

    // Return the length of the stored string or zero if no string has been
    // stored in the instance's internal buffer.
    unsigned int length();

    virtual bool serialize(const string &dataset, ConstraintEvaluator &eval,
                           DDS &dds, XDR *sink, bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);
    
    virtual bool set_value(const string &value);
    virtual string value() const;
    
    virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true);

    virtual bool ops(BaseType *b, int op, const string &dataset);
};

#endif // _str_h

