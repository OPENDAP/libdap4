
// -*- mode: C++ -*-

// declarations for utility functions
//
// jhrg 9/21/94

/* $Log: util.h,v $
/* Revision 1.4  1995/03/04 14:36:50  jimg
/* Fixed xdr_str so that it works with the new String objects.
/* Added xdr_str_array for use with arrays of String objects.
/*
 * Revision 1.3  1995/01/19  20:14:59  jimg
 * Added declarations for new utility functions which return pointers to
 * objects in the BaseType hierarchy.
 *
 * Revision 1.2  1994/11/29  20:21:24  jimg
 * Added xdr_str and xdr_url functions (C linkage). These provide a way for
 * the Str and Url classes to en/decode strings (Urls are effectively strings)
 * with only two parameters. Thus the Array and List classes might actually
 * work as planned.
 */

#include <stdio.h>
#include <rpc/xdr.h>
#include <SLList.h>
#include <String.h>

#include "BaseType.h"
#include "Byte.h"
#include "Int32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "List.h"
#include "Structure.h"
#include "Sequence.h"
#include "Function.h"
#include "Grid.h"

bool unique(SLList<BaseTypePtr> l, const char *var, const char *type);
XDR *new_xdrstdio(FILE *stream, enum xdr_op xop);
void delete_xdrstdio(XDR *xdr);

extern "C" bool_t xdr_str(XDR *xdrs, String **buf);
extern "C" bool_t xdr_str_array(XDR *xdrs, String *buf);

Byte *NewByte(const String &n = (char *)0);
Int32 *NewInt32(const String &n = (char *)0);
Float64 *NewFloat64(const String &n = (char *)0);
Str *NewStr(const String &n = (char *)0);
Url *NewUrl(const String &n = (char *)0);
Array *NewArray(const String &n = (char *)0, BaseType *v = 0);
List *NewList(const String &n = (char *)0, BaseType *v = 0);
Structure *NewStructure(const String &n = (char *)0);
Sequence *NewSequence(const String &n = (char *)0);
Function *NewFunction(const String &n = (char *)0);
Grid *NewGrid(const String &n = (char *)0);
