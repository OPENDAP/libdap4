// -*- C++ -*-


// Types for the expr parser.
//
// 11/4/95 jhrg

// $Log: expr.h,v $
// Revision 1.5  1996/05/14 15:38:58  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.4  1996/04/05 00:22:18  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.3  1996/03/02 01:33:25  jimg
// Added to the value typedef to support the complete STL syntax.
// Added support for the rvalue and btp_func_value classes.
//
// Revision 1.2  1996/02/01 17:43:15  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.1  1996/01/31 18:55:15  jimg
// Added to repository.

#ifndef _expr_h
#define _expr_h

#include <String.h>
#include <SLList.h>

#include "config_dap.h"

#include "BaseType.h"

// value is used to return constant values within the parser. Constants are
// packaged in BaseType *s for evaluation.

typedef struct {
    Type type;			// Type is an enum defined in BaseType.h
    union {
	dods_int32 i;
	dods_float64 f;
	String *s;
    } v;
} value;

// Either a BaseType * XOR a btp_func_ptr and arguments. Note that the
// arguments can themselves be function calls.

struct rvalue;			// forward declaration

typedef bool (*bool_func_ptr)(int argc, BaseType *argv[]);
typedef BaseType *(*btp_func_ptr)(int argc, BaseType *argv[]);

struct btp_func_rvalue;

struct rvalue {
    BaseType *btp;
    btp_func_rvalue *btp_f_rvp;

    rvalue(BaseType *bt): btp(bt), btp_f_rvp(0) {}
    rvalue(btp_func_rvalue *f): btp(0), btp_f_rvp(f) {}
    rvalue(): btp(0), btp_f_rvp(0) {}

    ~rvalue();

    // Return the BaseType * that contains a value for a given rvalue. If the
    // rvalue is a BaseType *, ensures that the read mfunc has been
    // called. If the rvalue is a Func_rvalue, evaluates the func_rvalue.
    // NB: The functions referenced by func_rvalues must encapsulate their
    // values in BaseType *s.
    BaseType *bvalue(const String &dataset);
};

typedef SLList<rvalue *> rvalue_list;

struct btp_func_rvalue {
    btp_func_ptr btp_f_ptr;	// pointer to a function returning BaseType *
    rvalue_list *args;		// arguments to the function

    btp_func_rvalue(btp_func_ptr f, rvalue_list *a): btp_f_ptr(f), args(a) {}
    btp_func_rvalue(): btp_f_ptr(0) {}

    ~btp_func_rvalue() {
	delete args;
    }

    BaseType *bvalue(const String &dataset) {
	int argc = args->length();
	BaseType *argv[argc];

	int i = 0;
	for (Pix p = args->first(); p; args->next(p))
	    argv[i++] = (*args)(p)->bvalue(dataset);

	return (*btp_f_ptr)(argc, argv);
    }
};

typedef SLList<int> IntList;
typedef SLList<IntList *> IntListList;

#endif /* _expr_h */
