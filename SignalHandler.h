
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

typedef void Sigfunc(int);	// Plauger, 1992

/** Singleton to handle signals. This class adapts the C-style function call
    interface to one suited for C++. This class records a signal's old
    action/handler when it installs new handler. When a signal is caught, the
    new handler (registered with this class) is run and then the old
    action/handler is performed. This ensures that when libdap++ is embedded
    in code which has a handler for a signal such as SIGINT which does
    something other than the default, that thing, whatever it may be, gets
    done. 

    This class treats signals it registers (using the EventHandler abstract
    class) differently than ones registered using the \c signal() or \c
    sigaction() system interfaces. If the register_handler() method is called
    and an instance of EventHandler is already bound to \e signum, then the
    old EventHandler is returned. However, if there's an exisitng handler
    that was set up with \c sigaction(), ..., it won't be returned. Instead
    it will either be run after the newly registered EventHandler or ignored,
    depending on register_handler()'s \e override parameter. This feature may
    be used only for POSIX.1 signals which cause process termination. They
    are: SIGHUP, SIGINT, SIGKILL, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, and
    SIGUSR2.

    @note Based on "Applying Design Patterns to Simplify Signal Handling",
    Douglas C. Schmidt, 1998,
    http://www.cs.wustl.edu/~schmidt/signal-patterns.html.

    @see EvenHandler
    @author James Gallagher <jgallagher@opendap.org> */
class SignalHandler {
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

    // This array holds the old signal handlers. Once the handler in
    // d_signal_handler[signum] is run, look here to see what the original
    // action was. This is important since libdap++ is often embedded in code
    // that already has a non-default signal handler for things like SIGINT.
    static Sigfunc *d_old_handlers[NSIG];

    // Entry point adapter installed into sigaction(). This must be static
    // method (or a regular C-function) to conform to sigaction's interface.
    // this is the part of SignalHandler that uses the Adapter pattern.
    static void dispatcher(int signum);

    // Delete the global instance. Call this with atexit().
    static void delete_instance();

    // Call this using pthread_once() to ensure there's only one instance
    // when running in a MT world.
    static void initialize_instance();

    friend class SignalHandlerTest;
    friend class HTTPCacheTest;

public:
    static SignalHandler *instance();

    ///
    virtual ~SignalHandler() {}

    EventHandler *register_handler(int signum, EventHandler *eh, 
				   bool override = false) throw(InternalErr);

    EventHandler *remove_handler(int signum);
};

// $Log: SignalHandler.h,v $
// Revision 1.3  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.1.2.4  2004/02/11 17:13:33  jimg
// Changed how this class creates its instance. It is now very similar to
// RCReader and much more in line with HTTPCache. Also, made this a friend of
// HTTPCacheTest to smooth over some testing woes.
//
// Revision 1.1.2.3  2004/02/10 20:44:33  jimg
// Added support for remembering and running old handlers/actions.
//
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
