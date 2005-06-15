
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
    { "$Id: SignalHandler.cc,v 1.6 2005/04/21 17:48:59 jimg Exp $" };

#include <signal.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <pthread.h>

#include "SignalHandler.h"
#include "util.h"

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
	    d_signal_handlers[i] = 0;
	    d_old_handlers[i] = 0;
	}

	delete SignalHandler::d_instance; 
	SignalHandler::d_instance = 0;
    }
}

/** This private method is the adapter between the C-style interface of the
    signal sub-system and C++'s method interface. This uses the lookup table
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
	  case SIGHUP:
	  case SIGINT:
	  case SIGKILL:
	  case SIGPIPE:
	  case SIGALRM:
	  case SIGTERM:
	  case SIGUSR1:
	  case SIGUSR2: _exit(EXIT_FAILURE);

	    // register_handler() should never allow any fiddling with
	    // signals other than those listed above.
	  default: abort();
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
    throw(InternalErr)
{
    // Check first for improper use.
    switch (signum) {
      case SIGHUP:
      case SIGINT:
      case SIGKILL:
      case SIGPIPE:
      case SIGALRM:
      case SIGTERM:
      case SIGUSR1:
      case SIGUSR2: break;

      default: throw InternalErr(__FILE__, __LINE__,
		  string("Call to register_handler with unsupported signal (")
		  + long_to_string(signum) + string(")."));
    }

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

    struct sigaction osa;	// extract the old handler/action

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
// Revision 1.6  2005/04/21 17:48:59  jimg
// Removed PTHREADS compile-time switch. Also, checkpoint for the build
// work.
//
// Revision 1.5  2004/07/07 21:08:48  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.4  2004/06/28 16:57:53  pwest
// unix compiler issues
//
// Revision 1.1.2.12  2004/04/02 16:46:36  dan
// Fixed aproblem with the Alpha build. Code didn't compile when HAVE_PTHREAD
// was not defined.
//
// Revision 1.1.2.11  2004/03/11 18:21:59  jimg
// Removed unneeded extra pthread_once_t mutex (was used for
// re-initialization).
//
// Revision 1.1.2.10  2004/03/11 18:11:36  jimg
// Ripped out the code in delete_instance that (tries to) reset(s) the
// pthread_once_t mutex. We cannot do this in a portable way and it's needed
// only for the unit tests, which I claim to have fixed so they don't require
// it anymore.
//
// Revision 1.1.2.9  2004/03/07 23:17:44  rmorris
// Make static initialization of PTHREAD_ONCE_INIT compatible cross platform.
//
// Revision 1.1.2.8  2004/02/27 17:25:12  edavis
//  adding { } for PTHREAD_ONCE_INIT parm
//
// Revision 1.1.2.7  2004/02/26 22:44:50  edavis
// remove the platform dependent parm PTHREAD_ONCE_INIT
//
// Revision 1.1.2.6  2004/02/22 23:35:03  rmorris
// Solved some problems regarding the differences in the pthread
// implementation across OSX, Win32 and non-OSX unixes. Either we are using
// pthreads in a manner that was not intended by the pthread 'standard' (??)
// or pthread implementations vary across platform or (finally) perhaps we
// are encountering different implementations of pthreads as a result of its
// development over time. Regardless our pthread code is starting to become
// less portable. See how pthread_once_t varies across the above-mentioned
// platforms. These changes get it to compile. I'm crossing my fingers that
// it will run correctly everywhere.
//
// Revision 1.3  2004/02/19 19:42:52  jimg
// Merged with release-3-4-2FCS and resolved conflicts.
//
// Revision 1.1.2.5  2004/02/11 17:12:01  jimg
// Changed how this class creates its instance. It now does the same thing as
// RCReader and is much more in line with HTTPCache.
//
// Revision 1.1.2.4  2004/02/10 20:48:13  jimg
// Added support for running old handlers/actions once the newly registered
// handlers are executed. Also limited the signals to those that POSIX.1 defines
// as terminating the process by default. The code won't try to work with other
// signals. It's possible to get it to do so, but fairly involved. See Steven's
// "Advanced Programming ..." book for the lowdown on signals.
//
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
