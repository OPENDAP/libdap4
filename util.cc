
// Utility functions used by the api.
//
// jhrg 9/21/94

// $Log: util.cc,v $
// Revision 1.2  1994/10/18 00:25:36  jimg
// Fixed error in char * [] allocation.
// Added debugging code.
//

static char rcsid[]={"$Id: util.cc,v 1.2 1994/10/18 00:25:36 jimg Exp $"};

#include <string.h>
#include <stdlib.h>

#include <SLList.h>
#include "BaseType.h"
#include "errmsg.h"
#include "debug.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

// Compare elements in a SLList of (BaseType *)s and return true if there are
// no duplicate elements, otherwise return false. Uses the same number of
// compares as qsort. (Guess why :-)
//
// NB: The elements of the array to be sorted are pointers to chars; the
// compare function gets pointers to those elements, thus the cast to (const
// char **) and the dereference to get (const char *) for strcmp's arguments.

static int
char_cmp(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

bool
unique(SLList<BaseTypePtr> l, const char *var_name, const char *type_name)
{
    // copy the identifier names to an array of char
    char **names = new char *[l.length()];
    if (!names)
	err_quit("util.cc:unique - Could not allocate array NAMES.");

    int nelem = 0;
    String s;
    for (Pix p = l.first(); p; l.next(p)) {
	names[nelem++] = strdup((const char *)l(p)->get_var_name());
	DBG(cerr << "NAMES[" << nelem-1 << "]=" << names[nelem-1] << endl);
    }
    
    // sort the array of names
    qsort(names, nelem, sizeof(char *), char_cmp);
	
#ifdef DEBUG2
    cout << "unique:" << endl;
    for (int ii = 0; ii < nelem; ++ii)
	cout << "NAMES[" << ii << "]=" << names[ii] << endl;
#endif

    // look for any instance of consecutive names that are ==
    for (int i = 1; i < nelem; ++i)
	if (!strcmp(names[i-1], names[i])) {
	    cerr << "The variable `" << names[i] 
		 << "' is used more than once in " << type_name << " `"
		 << var_name << "'" << endl;
	    for (i = 0; i < nelem; i++)
		free(names[i]);	// strdup uses malloc
	    delete [] names;
	    return false;
	}

    for (i = 0; i < nelem; i++)
	free(names[i]);		// strdup uses malloc
    delete [] names;

    return true;
}
