// -*- C++ -*-

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

/* $Log: DDS.h,v $
/* Revision 1.8  1995/12/09 01:06:39  jimg
/* Added changes so that relational operators will work properly for all the
/* datatypes (including Sequences). The relational ops are evaluated in
/* DDS::eval_constraint() after being parsed by DDS::parse_constraint().
/*
 * Revision 1.7  1995/12/06  21:05:08  jimg
 * Added print_constrained(): prints only those parts of the DDS that satisfy
 * the constraint expression (projection + array selection).
 * Added eval_constraint(): given the text of a constraint expression, evaluate
 * it in the environment of the current DDS.
 * Added mark*(): add the named variables to the current projection.
 * Added send(): combines many functions like reading and serializing variables
 * with constraint evaluation.
 *
 * Revision 1.6  1995/02/10  02:30:49  jimg
 * Misc comment edits.
 *
 * Revision 1.5  1994/11/03  04:58:03  reza
 * Added two overloading for function parse to make it consistent with DAS
 * class. 
 *
 * Revision 1.4  1994/10/18  00:20:47  jimg
 * Added copy ctor, dtor, duplicate, operator=.
 * Added var() for const char * (to avoid confusion between char * and
 * Pix (which is void *)).
 * Switched to errmsg library.
 * Added formatting to print().
 *
 * Revision 1.3  1994/09/23  14:42:23  jimg
 * Added mfunc check_semantics().
 * Replaced print mfunc stub with real code.
 * Fixed some errors in comments.
 *
 * Revision 1.2  1994/09/15  21:09:00  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.1  1994/09/08  21:09:42  jimg
 * First version of the Dataset descriptor class.
 */

#ifndef _DDS_h
#define _DDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"

class DDS {
private:
    struct rel_clause {
	int op;			// operator code from parser
	BaseType *arg1;		// argument 1 for OP
	BaseType *arg2;		// argument 2; must be same type as ARG1
    };
	
    String name;		// the dataset name
    SLList<BaseTypePtr> vars;	// variables at the top level 
    
    SLList<rel_clause> expr;

    void duplicate(const DDS &dds);

public:
    DDS(const String &n = (char *)0);
    DDS(const DDS &dds);
    ~DDS();

    DDS & operator=(const DDS &rhs);

    String get_dataset_name();
    void set_dataset_name(const String &n);

    void add_var(BaseType *bt);
    void del_var(const String &n);
    BaseType *var(const String &n);
    BaseType *var(const char *n); // to avoid cast of char * to Pix.

    Pix first_var();
    void next_var(Pix &p);
    BaseType *var(Pix p);

    // Interface to the parsed expression
    Pix first_clause();
    void next_clause(Pix &p);
    int clause_op(Pix p);
    BaseType *clause_arg1(Pix p);
    BaseType *clause_arg2(Pix p);
    void append_clause(int op, BaseType *arg1, BaseType *arg2);

    // evaluate the current constraint
    bool eval_constraint();

    // evaluate the projectons
    bool mark_all(bool state);
    bool mark(const String &name, bool state);

    // Interface to the parser
    bool parse(String fname);
    bool parse(int fd);
    bool parse(FILE *in=stdin);

    // Print the entire DDS.
    bool print(ostream &os = cout);
    bool print(FILE *out);

    // Print only those parts of the DDS marked for transmission after
    // evaluating a constraint expression.
    bool print_constrained(ostream &os = cout);
    bool print_constrained(FILE *out);

    // Check the semantics of the variables in the DDS. if ALL is true,
    // recursively descend aggregate variables.
    bool check_semantics(bool all = false);

    // Evaluate the constraint expression CONSTRAINT given the current DDS.
    bool parse_constraint(const String &constraint);

    // Send VAR_NAME from DATASET given CONSTRAINT. if FLUSH is true, flush
    // the output buffer upon completion. Use OUT as the output buffer if not
    // null, otherwise use STDOUT. This mfunc uses eval_constraint(),
    // BaseType::read() and BaseType::serailize() as well as other mfuncs.
    bool send(const String &dataset, const String &constraint, FILE *out, 
	      bool flush = false);
};

#endif
