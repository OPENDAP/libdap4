/*
  This header defines macros which enable compile-time program
  instrumentation. These macros work for both C and C++. Enclose the entire
  statement to be debugged within the DBG() macro *and* put the semicolon
  after the macro. (e.g., DBG(cerr << "Bad program" << endl); ). Statements
  should not span lines unless they include `\'s to escape the newlines.

  jhrg 10/13/94
*/
  
/* 
 * $Log: debug.h,v $
 * Revision 1.6  1997/08/11 18:19:35  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.5  1996/11/25 03:43:08  jimg
 * Added DODS_PERF macro.
 *
 * Revision 1.4  1996/08/09 18:20:46  jimg
 * Changed DEBUG and DEBUG2 to DODS_DEBUG and DODS_DEBUG2 to avoid conflicts
 * with the WWW libraries use of that name (which it always defines...).
 *
 * Revision 1.3  1996/06/18 23:50:13  jimg
 * Added file and line number information to the macros.
 *
 * Revision 1.2  1996/03/05 00:59:47  jimg
 * Added _DEBUG_H to prevent multiple definition messages.
 *
 * Revision 1.1  1995/09/13 21:58:05  jimg
 * Added to src/dap in keeping with the plan to store all include files in src
 * dirs and `build' the include directory during `make depend' from those
 * inlcudes.
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DODS_DEBUG
#define DBG(x) cerr << __FILE__ << ":" << __LINE__ << ":"; x
#else
#define DBG(x) /* x */
#endif

#ifdef DODS_DEBUG2
#define DBG2(x) cerr << __FILE__ << ":" << __LINE__ << ":"; x
#else
#define DBG2(x) /* x */
#endif

#ifdef DODS_PERF
#define PERF(x) cerr << __FILE__ << ":" << __LINE__ << ":" << systime() << ":"; x
#else
#define PERF(x) /* x */
#endif

#endif /* _DEBUG_H */
