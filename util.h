
// -*- mode: C++ -*-

// declarations for utility functions
//
// jhrg 9/21/94

/* $Log: util.h,v $
/* Revision 1.8  1996/02/02 00:31:25  jimg
/* Merge changes for DODS-1.1.0 into DODS-2.x
/*
 * Revision 1.7  1995/12/06  18:33:34  jimg
 * Added forward decalration of text_to_temp();
 *
 * Revision 1.6  1995/08/26  00:32:12  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.5  1995/08/23  00:41:57  jimg
 * xdr_str() now takes a String & instead of a String ** for arg 2.
 *
 * Revision 1.4.2.1  1995/09/29 19:28:06  jimg
 * Fixed problems with xdr.h on an SGI.
 * Fixed conflict of int32_t (which was in an enum type defined by BaseType) on
 * the SGI.
 *
 * Revision 1.4  1995/03/04  14:36:50  jimg
 * Fixed xdr_str so that it works with the new String objects.
 * Added xdr_str_array for use with arrays of String objects.
 *
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
#ifdef NEVER
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#include <SLList.h>
#include <String.h>
#endif

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
FILE *text_to_temp(String text);

extern "C" bool_t xdr_str(XDR *xdrs, String &buf);
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
