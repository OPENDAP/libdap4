
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Declarations for CE functions.
//
// 1/15/99 jhrg

/* $Log: ce_functions.h,v $
/* Revision 1.1  1999/01/15 22:07:08  jimg
/* MOved code from util.h.
/* */


bool func_member(int argc, BaseType *argv[], DDS &dds);
bool func_null(int argc, BaseType *argv[], DDS &dds);
BaseType *func_nth(int argc, BaseType *argv[], DDS &dds);
BaseType *func_length(int argc, BaseType *argv[], DDS &dds);

void func_grid_select(int argc, BaseType *argv[], DDS &dds);

