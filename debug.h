/*
  This header defines macros which enable compile-time program
  instrumentation. These macros work for both C and C++. Enclose the entire
  statement to be debugged within the DBG() macro *and* put the semicolon
  after the macro. (e.g., DBG(cerr << "Bad program" << endl); ). Statements
  should not span lines unless they include `\'s to escape the newlines.

  jhrg 10/13/94
*/
  
#ifndef _debug_h
#define _debug_h

#ifdef __cplusplus
#define FILE_N_LINE cerr << __FILE__ << ": " << __LINE__ << ":"
#else
#define FILE_N_LINE fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);
#endif /* cplusplus */

#ifdef DODS_DEBUG
#define DBG(x) FILE_N_LINE; x
#else
#define DBG(x) /* x */
#endif

#ifdef DODS_DEBUG2
#define DBG2(x) FILE_N_LINE; x
#else
#define DBG2(x) /* x */
#endif

#ifdef DODS_PERF
#error "Deprecated macro!"
#endif

/* 
 * $Log: debug.h,v $
 * Revision 1.8  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.7  1998/09/08 21:34:41  jimg
 * Fixed up this file for use with C as well as C++.
 * Removed the old PERF macro since it was based on a non-POSIX time function
 * (systime()) and was never used!
 *
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

#endif /* _debug_h */
