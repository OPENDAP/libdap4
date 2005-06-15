/* 
 * trace_new.H - A memory management system.
 * 
 * Author:	Robert Mecklenburg
 * 		Computer Science Dept.
 * 		University of Utah
 * Date:	Fri Nov 19 1993
 *
 * Copyright (c) 1993,1994 The University of Utah and
 * the Center for Software Science (CSS).  All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software is hereby
 * granted provided that (1) source code retains these copyright, permission,
 * and disclaimer notices, and (2) redistributions including binaries
 * reproduce the notices in supporting documentation, and (3) all advertising
 * materials mentioning features or use of this software display the following
 * acknowledgement: ``This product includes software developed by the Center
 * for Software Science at the University of Utah.''
 *
 * THE UNIVERSITY OF UTAH AND CSS ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS
 * IS" CONDITION.  THE UNIVERSITY OF UTAH AND CSS DISCLAIM ANY LIABILITY OF
 * ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * CSS requests users of this software to return to css-dist@cs.utah.edu any
 * improvements that they make and grant CSS redistribution rights.
 */

#ifndef TRACE_NEW_H_
#define TRACE_NEW_H_

#if defined(__STDC__) || defined(DEBUG)
#ifndef __STDDEF_H
#include <stddef.h>
#endif

/*****************************************************************
 * operator new
 */
extern void *
operator new( size_t size, const char * file, int line );

#ifdef __GNUC__
extern void *
operator new[]( size_t size, const char * file, int line );
#endif

#define NEW_PASTE_( x, y ) x##y
#define new NEW_PASTE_(n,ew)( __FILE__, __LINE__ )
#endif /* defined(__STDC__) || defined(DEBUG) */

#endif /* TRACE_NEW_H_ */
