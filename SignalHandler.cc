
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

// (c) COPYRIGHT URI/MIT 1994-2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#include "config.h"

#include <cstdlib>

#include <signal.h>
#include <pthread.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h> //for _exit
#endif

#include "SignalHandler.h"
#include "util.h"

namespace libdap {

EventHandler *SignalHandler::d_signal_handlers[NSIG];
Sigfunc *SignalHandler::d_old_handlers[NSIG];
SignalHandler *SignalHandler::d_instance = 0;

// instance_control is used to ensure that in a MT environment d_instance is
// correctly initialized.
static pthread_once_t instance_control = PTHREAD_ONCE_INIT;

/// Private static void method.
void
SignalHandler::initialize_instance()
{
    // MT-Safe if called via pthread_once or similar
    SignalHandler::d_instance = new SignalHandler;
    atexit(SignalHandler::delete_instance);
}

/// Private static void method.
void
SignalHandler::delete_instance()
{
    if (SignalHandler::d_instance) {
        for (int i = 0; i < NSIG; ++i) {
        	// Fortify warns about a leak because the EventHandler objects
        	// are not deleted, but that's OK - this is a singleton and
        	// so the 'leak' is really just a constant amount of memory that
        	// gets used.
        	d_signal_handlers[i] = 0;
            d_old_handlers[i] = 0;
        }

        delete SignalHandler::d_instance;
        SignalHandler::d_instance = 0;
    }
}

/** This private method is the adapter between the C-style interface of the
    signal subsystem and C++'s method interface. This uses the lookup table
    to find an instance of EventHandler and calls that instance's
    handle_signal method.

    @param signum The number of the signal. */
void
SignalHandler::dispatcher(int signum)
{
    // Perform a sanity check...
    if (SignalHandler::d_signal_handlers[signum] != 0)
        // Dispatch the handler's hook method.
        SignalHandler::d_signal_handlers[signum]->handle_signal(signum);

    Sigfunc *old_handler = SignalHandler::d_old_handlers[signum];
    if (old_handler == SIG_IGN || old_handler == SIG_ERR)
        return;
    else if (old_handler == SIG_DFL) {
        switch (signum) {
#if 0
#ifndef WIN32
        case SIGHUP:
        case SIGKILL:
        case SIGUSR1:
        case SIGUSR2:
        case SIGPIPE:
        case SIGALRM:
#endif
        case SIGINT:
        case SIGTERM: _exit(EXIT_FAILURE);

            // register_handler() should never allow any fiddling with
            // signals other than those listed above.
        default: abort();
#endif
        // Calling _exit() or abort() is not a good thing for a library to be
        // doing. This results in a warning from rpmlint
        default:
            throw Error("Signal handler operation on an unsupported signal.");
        }
    }
    else
        old_handler(signum);
}

/** Get a pointer to the single instance of SignalHandler. */
SignalHandler*
SignalHandler::instance()
{
    pthread_once(&instance_control, initialize_instance);

    return d_instance;
}

/** Register an event handler. By default run any previously registered
    action/handler such as those installed using \c sigaction(). For signals
    such as SIGALRM (the alarm signal) this may not be what you want; see the
    \e override parameter. See also the class description.

    @param signum Bind the event handler to this signal number. Limited to
    those signals that, according to POSIX.1, cause process termination.
    @param eh A pointer to the EventHandler for \c signum.
    @param override If \c true, do not run the default handler/action.
    Instead run \e eh and then treat the signal as if the original action was
    SIG_IGN. Default is false.
    @return A pointer to the old EventHandler or null. */
EventHandler *
SignalHandler::register_handler(int signum, EventHandler *eh, bool override)
{
    // Check first for improper use.
    switch (signum) {
#ifndef WIN32
    case SIGHUP:
    case SIGKILL:
    case SIGUSR1:
    case SIGUSR2:
    case SIGPIPE:
    case SIGALRM:
#endif
    case SIGINT:
    case SIGTERM: break;

    default: throw InternalErr(__FILE__, __LINE__,
                                   string("Call to register_handler with unsupported signal (")
                                   + long_to_string(signum) + string(")."));
    }

    // Save the old EventHandler
    EventHandler *old_eh = SignalHandler::d_signal_handlers[signum];

    SignalHandler::d_signal_handlers[signum] = eh;

    // Register the dispatcher to handle this signal. See Stevens, Advanced
    // Programming in the UNIX Environment, p.298.
#ifndef WIN32
    struct sigaction sa;
    sa.sa_handler = dispatcher;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Try to suppress restarting system calls if we're handling an alarm.
    // This lets alarms block I/O calls that would normally restart. 07/18/03
    // jhrg
    if (signum == SIGALRM) {
#ifdef SA_INTERUPT
        sa.sa_flags |= SA_INTERUPT;
#endif
    }
    else {
#ifdef SA_RESTART
        sa.sa_flags |= SA_RESTART;
#endif
    }

    struct sigaction osa; // extract the old handler/action

    if (sigaction(signum, &sa, &osa) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not register a signal handler.");

    // Take care of the case where this interface is used to register a
    // handler more than once. We want to make sure that the dispatcher is
    // not installed as the 'old handler' because that results in an infinite
    // loop. 02/10/04 jhrg
    if (override)
        SignalHandler::d_old_handlers[signum] = SIG_IGN;
    else if (osa.sa_handler != dispatcher)
        SignalHandler::d_old_handlers[signum] = osa.sa_handler;
#endif

    return old_eh;
}

/** Remove the event hander.
    @param signum The signal number of the handler to remove.
    @return The old event handler */
EventHandler *
SignalHandler::remove_handler(int signum)
{
    EventHandler *old_eh = SignalHandler::d_signal_handlers[signum];

    SignalHandler::d_signal_handlers[signum] = 0;

    return old_eh;
}

} // namespace libdap
