
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
 
#ifndef signal_handler_h
#define signal_handler_h

#include <signal.h>

#include "EventHandler.h"
#include "InternalErr.h"

/** Singleton to handle signals. This class also adapts the C-style function
    call interface to one suited for C++.

    Based on "Applying Design Patterns to Simplify Signal Handling", Douglas
    C. Schmidt, 1998, http://www.cs.wustl.edu/~schmidt/signal-patterns.html.

    @see EvenHandler
    @author James Gallagher <jgallagher@opendap.org> */
class SignalHandler
{
private:
    // Ensure we're a Singleton.
    SignalHandler() {}

    // Singleton pointer.
    static SignalHandler *d_instance;

    // Table of pointers to instances of EventHandlers. Since EventHandler is
    // abstract, the pointers will actually reference instances that are
    // children of EventHandler. NSIG is defined in signal.h but this may be
    // a portability issue. 
    static EventHandler *d_signal_handlers[NSIG];

    // Entry point adapter installed into sigaction(). This must be static
    // method (or a regular C-function) to conform to sigaction's interface.
    // this is part of SignalHandler that uses the Adapter pattern.
    static void dispatcher(int signum);

    friend void initialize_signal_handler();

    friend class SignalHandlerTest;

public:
    static SignalHandler *instance();

    EventHandler *register_handler(int signum, EventHandler *eh)
	throw(InternalErr);

    EventHandler *remove_handler(int signum);
};

// $Log: SignalHandler.h,v $
// Revision 1.2  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.1.2.2  2003/07/19 01:46:34  jimg
// Fixed up some comments.
//
// Revision 1.1.2.1  2003/07/19 01:45:26  jimg
// Added.
//

#endif // signal_handler_h
