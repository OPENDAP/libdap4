
// Methods for class DDS
//
// jhrg 9/7/94

// $Log: DDS.cc,v $
// Revision 1.9  1994/12/09 21:37:24  jimg
// Added <unistd.h> to the include files.
//
// Revision 1.8  1994/12/07  21:23:16  jimg
// Removed config
//
// Revision 1.7  1994/11/22  14:05:40  jimg
// Added code for data transmission to parts of the type hierarchy. Not
// complete yet.
// Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
//
// Revision 1.6  1994/11/03  04:58:02  reza
// Added two overloading for function parse to make it consistent with DAS class.
//
// Revision 1.5  1994/10/18  00:20:46  jimg
// Added copy ctor, dtor, duplicate, operator=.
// Added var() for const cahr * (to avoid confusion between char * and
// Pix (which is void *)).
// Switched to errmsg library.
// Added formatting to print().
//
// Revision 1.4  1994/10/05  16:34:14  jimg
// Fixed bug in the parse function(s): the bison generated parser returns
// 1 on error, 0 on success, but parse() was not checking for this.
// Instead it returned the value of bison's parser function.
// Changed types of `status' in print and parser functions from int to bool.
//
// Revision 1.3  1994/09/23  14:42:22  jimg
// Added mfunc check_semantics().
// Replaced print mfunc stub with real code.
// Fixed some errors in comments.
//
// Revision 1.2  1994/09/15  21:08:39  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/08  21:09:40  jimg
// First version of the Dataset descriptor class.
// 

static char rcsid[]="$Id: DDS.cc,v 1.9 1994/12/09 21:37:24 jimg Exp $";

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>

#include "DDS.h"
#include "errmsg.h"
#include "util.h"

#include "config.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

void ddsrestart(FILE *yyin);
int ddsparse(DDS &table);	// defined in dds.tab.c

// Copy the stuff in DDS to THIS. The mfunc returns void because THIS gets
// the `result' of the mfunc.
//
// NB: This can't define the formal param to be const since SLList<>first()
// (which is what DDS::first_var() calls) does not define THIS to be const.

void
DDS::duplicate(DDS &dds)
{
    name = dds.name;
    // copy the things pointed to by the list, not just the pointers
    for (Pix src = dds.first_var(); src; dds.next_var(src)) {
	BaseType *btp = dds.var(src)->ptr_duplicate();
	add_var(btp);
    }
}

DDS::DDS(const String &n) : name(n)
{
}

DDS::DDS(DDS &rhs)
{
    duplicate(rhs);
}

DDS::~DDS()
{
    for (Pix p = first_var(); p; next_var(p))
	delete var(p);
}

DDS &
DDS::operator=(DDS &rhs)
{
    if (this == &rhs)
	return *this;

    duplicate(rhs);

    return *this;
}

String 
DDS::get_dataset_name()
{ 
    return name; 
}

void
DDS::set_dataset_name(const String &n) 
{ 
    name = n; 
}

void
DDS::add_var(BaseType *bt)
{ 
    vars.append(bt); 
}

void 
DDS::del_var(const String &n)
{ 
    Pix pp = 0;			// previous Pix

    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == n) {
	    vars.del_after(pp);	// pp points to the pos before p
	    return;
	}
	else
	    pp = p;
}

BaseType *
DDS::var(const String &n)
{ 
    for (Pix p = vars.first(); p; vars.next(p))
	if (vars(p)->get_var_name() == n)
	    return vars(p);

    return 0;
}

// This is necessary because (char *) can be cast to Pix (because PIX is
// really (void *)). This must take precedence over the creation of a
// temporary object (the String).

BaseType *
DDS::var(const char *n)
{
    return var((String)n);
}

Pix 
DDS::first_var()
{ 
    return vars.first(); 
}

void 
DDS::next_var(Pix &p)
{ 
    if (!vars.empty())
	vars.next(p); 
}

BaseType *
DDS::var(Pix p)
{ 
    if (!vars.empty() && p)
	return vars(p); 
}

bool
DDS::parse(String fname)
{
    FILE *in = fopen(fname, "r");

    if (!in) {
        cerr << "Could not open: " << fname << endl;
        return false;
      }

    bool status = parse(in);

    fclose(in);

    return status;
  }


bool
DDS::parse(int fd)
{
    FILE *in = fdopen(dup(fd), "r");

    if (!in) {
        cerr << "Could not access file" << endl;
        return false;
      }

    bool status = parse(in);

    fclose(in);

    return status;
  }

// Read structure from IN (which defaults to stdin). If ddsrestart() fails,
// return false, otherwise return the status of ddsparse().

bool
DDS::parse(FILE *in)
{
    if (!in) {
	err_print("DDS::parse: NULL file pointer");
	return false;
    }

    ddsrestart(in);

    return ddsparse(*this) == 0;
}

// Write strucutre from tables to OUT (which defaults to stdout). Return
// true. 

bool
DDS::print(ostream &os)
{
    os << "Dataset {" << endl;

    for (Pix p = vars.first(); p; vars.next(p))
	vars(p)->print_decl(os);

    os << "} " << name << ";" << endl;
					   
    return true;
}

// Check the semantics of the DDS describing a complete dataset. If ALL is
// true, check not only the semantics of THIS->TABLE, but also recurrsively
// all ctor types in the THIS->TABLE. By default, ALL is false since parsing
// a DDS input file runns semantic checks on all variables (but not the
// dataset itself.
//
// Returns: true if the conventions for the DDS are not violated, false
// otherwise. 

bool
DDS::check_semantics(bool all)
{
    // The dataset must have a name
    if (name == (char *)0) {
	cerr << "A dataset must have a name" << endl;
	return false;
    }

    if (!unique(vars, (const char *)name, (const char *)"Dataset"))
	return false;

    if (all) 
	for (Pix p = vars.first(); p; vars.next(p))
	    if (!vars(p)->check_semantics(true))
		return false;

    return true;
}
