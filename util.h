
// -*- mode: C++ -*-

// declarations for utility functions
//
// jhrg 9/21/94

/* $Log: util.h,v $
/* Revision 1.2  1994/11/29 20:21:24  jimg
/* Added xdr_str and xdr_url functions (C linkage). These provide a way for
/* the Str and Url classes to en/decode strings (Urls are effectively strings)
/* with only two parameters. Thus the Array ad List classes might actually
/* work as planned.
/* */

#include <rpc/xdr.h>
#include <SLList.h>

bool unique(SLList<BaseTypePtr> l, const char *var, const char *type);
extern "C" bool_t xdr_str(XDR *xdrs, char **buf);
extern "C" bool_t xdr_url(XDR *xdrs, char **buf);
