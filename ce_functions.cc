
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// These functions are used by the CE evaluator
//
// 1/15/99 jhrg

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: ce_functions.cc,v 1.9 2001/08/24 17:46:22 jimg Exp $"};

#include <iostream>
#include <vector>

#include "BaseType.h"
#include "List.h"
#include "Array.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"
#include "GSEClause.h"

#include "parser.h"
#include "gse.tab.h"
#include "debug.h"
#include "util.h"

using std::cerr;
using std::endl;

int gse_parse(void *arg);
void gse_restart(FILE *in);

// Glue routines declared in expr.lex
void gse_switch_to_buffer(void *new_buffer);
void gse_delete_buffer(void * buffer);
void *gse_string(const char *yy_str);

string
extract_string_argument(BaseType *arg)
{
    if (arg->type() != dods_str_c)
	throw Error(malformed_expr, 
	      "The Projection function requires a DODS string-type argument.");
    
    // Use String until conversion of String to string is complete. 9/3/98
    // jhrg
    string *sp = 0;
    arg->buf2val((void **)&sp);
    string s = *sp;
    delete sp; sp = 0;

    DBG(cerr << "s: " << s << endl);

    return s;
}

bool
func_member(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 2) {
	cerr << "Wrong number of arguments." << endl;
	return false;
    }
    
    switch(argv[0]->type()) {
      case dods_list_c: {
		List *var = (List *)argv[0];
		BaseType *btp = (BaseType *)argv[1];
		bool result = var->member(btp, dds);
    
		return result;
      }
      
      default:
		cerr << "Wrong argument type." << endl;
		return false;
    }

}

bool
func_null(int argc, BaseType *argv[], DDS &)
{
    if (argc != 1) {
		cerr << "Wrong number of arguments." << endl;
		return false;
    }
    
    switch(argv[0]->type()) {
      case  dods_list_c: {
	List *var = (List *)argv[0];
	bool result = var->null();
    
	return result;
      }

      default:
		cerr << "Wrong argument type." << endl;
	return false;
    }

}

BaseType *
func_length(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 1) {
	cerr << "Wrong number of arguments." << endl;
	return 0;
    }
    
    switch (argv[0]->type()) {
      case dods_list_c: {
	  List *var = (List *)argv[0];
	  dods_int32 result = var->length();
    
	  BaseType *ret = (BaseType *)NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);
	  dds.append_constant(ret); // DDS deletes in its dtor

	  return ret;
      }

      case dods_sequence_c: {
	  Sequence *var = dynamic_cast<Sequence *>(argv[0]);
	  if (!var)
	      throw Error(unknown_error, "Expected a Sequence variable");
	  dods_int32 result = var->length();
    
	  BaseType *ret = (BaseType *)NewInt32("constant");
	  ret->val2buf(&result);
	  ret->set_read_p(true);
	  ret->set_send_p(true);
	  dds.append_constant(ret); 
    
	  return ret;
      }

      default:
	cerr << "Wrong type argument to list operator `length'" << endl;
	return 0;
    }
}

BaseType *
func_nth(int argc, BaseType *argv[], DDS &)
{
    if (argc != 2) {
	cerr << "Wrong number of arguments." << endl;
	return 0;
    }
    
    switch (argv[0]->type()) {
	case dods_list_c: {
	    if (argv[1]->type() != dods_int32_c) {
		cerr << "Second argument to NTH must be an integer." << endl;
		return 0;
	    }
	    List *var = (List *)argv[0];
	    dods_int32 n;
	    dods_int32 *np = &n;
	    argv[1]->buf2val((void **)&np);

	    return var->var(n);
	}

      default:
	cerr << "Wrong type argument to list operator `nth'" << endl;
	return 0;
    }
}

// Assume the following arguments are sent to func_grid_select:
// Grid name, 1 or more strings which contain relational expressions.

void 
func_grid_select(int argc, BaseType *argv[], DDS &dds)
{
	Pix p = NULL;
	Pix q = NULL;

    if (argc <= 1)
	throw Error(unknown_error, "Wrong number of arguments to grid()");

    Grid *grid = dynamic_cast<Grid*>(argv[0]);

    // Mark this grid as part of the current projection.
    dds.mark(grid->name(), true);

    // Now read the map vectors. In theory we could do this after the parse
    // if the GSEClause used lazy evaluation (which should be the case) but
    // that means this code would depend on the inner workings of GSEClause.
    // Of course, maybe GSEClause should have the DDS and handle these reads
    // itself when the information is really needed. 1/20/99 jhrg
    for (p = grid->first_map_var(); p; grid->next_map_var(p)) {
	Array *map = dynamic_cast<Array *>(grid->map_var(p));
	if (!map->read_p()) {
	    map->read(dds.filename());
	}
    }

    vector<GSEClause *> clauses;
    for (int i = 1; i < argc; ++i) {
	gse_restart(0);
	void *cls = gse_string(extract_string_argument(argv[i]).c_str());
	gse_switch_to_buffer(cls);
	gse_arg *arg = new gse_arg(grid);
	bool status = gse_parse((void *)arg) == 0;
	gse_delete_buffer(cls);
	if (!status)
	    throw Error(malformed_expr, "Error parsing grid selection.");

	clauses.push_back(arg->get_gsec());
    }

    for (p = grid->first_map_var(); p; grid->next_map_var(p)) {
	Array *map = dynamic_cast<Array *>(grid->map_var(p));
	string map_name = map->name();
	// Init start and stop to the whole vector.
	// For each instance of map_name in the vector<GSEClause>
	//     if clause-instance (CI) start >= current map start
	//     map start = CI start, else error
	//     Same for stop except use <=
	q = map->first_dim();// a valid Grid Map is a vector.
	int start = map->dimension_start(q);
	int stop = map->dimension_stop(q);

	for (unsigned int i = 0; i < clauses.size(); ++i) {
	    if (clauses[i]->get_map_name() == map_name) {
		if (clauses[i]->get_start() >= start)
		    start = clauses[i]->get_start();
		else
		    throw Error(unknown_error,
"Improper starting Grid selection value.");
		if (clauses[i]->get_stop() >= stop)
		    stop = clauses[i]->get_stop();
		else
		    throw Error(unknown_error,
"Improper ending Grid selection value.");
	    }
	}		
	
	map->add_constraint(q, start, 1, stop);	// don't know about stride...
    }
}

// $Log: ce_functions.cc,v $
// Revision 1.9  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.8.4.2  2001/08/18 00:13:03  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.8.4.1  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.8  2000/09/22 02:17:22  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.7  2000/09/21 16:22:09  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.6  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.5  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.4.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
//
// Revision 1.4.14.1  2000/02/17 05:03:16  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.4  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.3  1999/04/22 22:30:52  jimg
// Uses dynamic_cast
//
// Revision 1.2  1999/01/21 02:52:52  jimg
// Added extract_string_argument function.
// Added grid_selection projection function.
//
// Revision 1.1  1999/01/15 22:06:44  jimg
// Moved code from util.cc
//

