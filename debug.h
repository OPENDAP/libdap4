/*
  This header defines macros which enable compile-time program
  instrumentation. These macros work for both C and C++. Enclose the entire
  statement to be debugged within the DBG() macro *and* put the semicolon
  after the macro. (e.g., DBG(cerr << "Bad program" << endl); ). Statements
  should not span lines unless they include `\'s to escape the newlines.

  jhrg 10/13/94
*/
  
/* $Log: debug.h,v $
/* Revision 1.2  1996/03/05 00:59:47  jimg
/* Added _DEBUG_H to prevent multiple definition messages.
/*
 * Revision 1.1  1995/09/13 21:58:05  jimg
 * Added to src/dap in keeping with the plan to store all include files in src
 * dirs and `build' the include directory during `make depend' from those
 * inlcudes.
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x) /* x */
#endif

#ifdef DEBUG2
#define DBG2(x) x
#else
#define DBG2(x) /* x */
#endif

#endif /* _DEBUG_H */
