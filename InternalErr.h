
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

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for the InternalErr class
//
// 5/3/99 jhrg

#ifndef _internalerr_h
#define _internalerr_h 1


#include <string>

#ifndef _error_h
#include "Error.h"
#endif

namespace libdap
{


/** The InternalErr class is used to signal that somewhere inside libdap a
    software fault was found. This class wraps the message text in some
    boiler plate that asks the error be reported to us (tech support).

    NB: This class Adds some text to the message and might, in the future,
    hold information not also held in Error. However, all Error objects
    thrown on the server-side of libdap that cannot be resolved (and that's all
    of them for now, 5/3/99 jhrg) will be sent to the client-side <i>using
    Error objects</i>. That is, the information recorded in an InternalErr
    object will be sent by calling the <tt>print(...)</tt> mfunc of Error.

    @brief A class for software fault reporting.
    @author jhrg */

class InternalErr: public Error
{

public:
    /** These constructors always set the error code to <tt>internal_error</tt>.

    Note that there is no way to specify an error correction program for
    an internal error.

    @brief Constructors for the Error object
    @name Constructors */
    //@{
    ///
    InternalErr(const string &msg);
    ///
    InternalErr(const string &file, const int &line, const string &msg);
    ///
    InternalErr();
    ///
    InternalErr(const InternalErr &copy_from);
    //@}

    virtual ~InternalErr() throw();

    bool OK();
};

} // namespace libdap

#endif // _internalerr_h
