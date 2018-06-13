
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

#ifndef event_handler_h
#define event_handler_h

#include <iostream>

namespace libdap
{

/** An abstract class which provides a hook method used by SignalHandler.

    Based on "Applying Design Patterns to Simplify Signal Handling", Douglas
    C. Schmidt, 1998, http://www.cs.wustl.edu/~schmidt/signal-patterns.html.

    @see SignalHandler
    @see AlarmHandler
    @author James Gallagher <jgallagher@opendap.org> */
class EventHandler
{
public:
    /** Hook method for SignalHandler. If a concrete instance of this class
    is registered with SignalHandler, this method will be called when \c
    signum is received. */
    virtual void handle_signal(int signum) = 0;

    /** We don't need a destructor, but including one might stave off an
    error later on... */
    virtual ~EventHandler()
    {}
};

#if 0
// moved to SignalHandlerTest jhrg 4/26/13

/** Test Handler. This is used with the SignalHandlerTest unit tests. */
class TestHandler : public EventHandler
{
public:
    int flag;

    TestHandler() : flag(0)
    {}

    virtual void handle_signal(int ) {
        flag = 1;
    }
};
#endif

} // namespace libdap

#endif // event_handler_h
