
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Declarations for CE functions.
//
// 1/15/99 jhrg

#ifndef _ce_functions_h
#define _ce_functions_h

bool func_member(int argc, BaseType *argv[], DDS &dds);
bool func_null(int argc, BaseType *argv[], DDS &dds);
BaseType *func_nth(int argc, BaseType *argv[], DDS &dds);
BaseType *func_length(int argc, BaseType *argv[], DDS &dds);

void func_grid_select(int argc, BaseType *argv[], DDS &dds);

// $Log: ce_functions.h,v $
// Revision 1.3  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.2  1999/01/21 02:52:01  jimg
// Fixed the stupid comments...
//
// Revision 1.1  1999/01/15 22:07:08  jimg
// Moved code from util.h.

#endif // _ce_functions_h
