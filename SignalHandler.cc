
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
 
// (c) COPYRIGHT URI/MIT 1994-2002
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#include "config_dap.h"

static char rcsid[] not_used =
    { "$Id: SignalHandler.cc,v 1.2 2003/12/08 18:02:29 edavis Exp $" };

#include <signal.h>

#ifndef WIN32
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "SignalHandler.h"

EventHandler *SignalHandler::d_signal_handlers[NSIG];
SignalHandler *SignalHandler::d_instance = 0;
// This variable (instance_control) is used to ensure that in a MT
// environment _instance is correctly initialized.
#if HAVE_PTHREAD_H
static pthread_once_t instance_control = PTHREAD_ONCE_INIT;
#endif

void
initialize_signal_handler()
{
    // MT-Safe if called via pthread_once or similar
    SignalHandler::d_instance = new SignalHandler;
}

/** This private method is the adapter between the C-style interface of the
    signal sub-system and C++'s method interface. This uses the lookup table
    to find an instance of EventHandler and call's that instance's
    handle_signal method.

    @param signum The number of the signal. */
void 
SignalHandler::dispatcher(int signum)
{
    // Perform a sanity check...
    if (SignalHandler::d_signal_handlers[signum] != 0)
	// Dispatch the handler's hook method.
	SignalHandler::d_signal_handlers[signum]->handle_signal(signum);
}

/** Get a pointer to the single instance of SignalHandler. */
SignalHandler* 
SignalHandler::instance()
{
#if HAVE_PTHREAD_H
    pthread_once(&instance_control, initialize_signal_handler);
#else
    if (!d_instance)
	initialize_signal_handler();
#endif

    return d_instance;
}

/** Register an event handler 
    @param signum Bind the event handler to this signal number.
    @param eh A pointer to the EventHandler for \c signum. 
    @return A pointer to the old EventHandler or null. */
EventHandler *
SignalHandler::register_handler(int signum, EventHandler *eh) 
    throw(InternalErr)
{
    // Save the old EventHandler
    EventHandler *old_eh = SignalHandler::d_signal_handlers[signum];

    SignalHandler::d_signal_handlers[signum] = eh;
 
    // Register the dispatcher to handle this signal. See Stevens, Advanced
    // Programming in the UNIX Environment, p.298.
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

    if (sigaction(signum, &sa, 0) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not register a signal handler.");

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

// $Log: SignalHandler.cc,v $
// Revision 1.2  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.1.2.3  2003/12/05 15:59:52  dan
// Fixed compile error in #if HAVE_PTHREAD_H, else clause.  Was using
// '_instance' when it should be 'd_instance'.
//
// Revision 1.1.2.2  2003/07/19 01:48:02  jimg
// Fixed up some comments.
//
// Revision 1.1.2.1  2003/07/19 01:47:43  jimg
// Added.
//
