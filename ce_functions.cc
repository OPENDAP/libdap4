
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// These functions are used by the CE evaluator
//
// 1/15/99 jhrg

// $Log: ce_functions.cc,v $
// Revision 1.5  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.4.20.1  2000/06/02 18:36:38  rmorris
// Mod's for port to Win32.
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

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: ce_functions.cc,v 1.5 2000/06/07 18:06:59 jimg Exp $"};

#include <iostream.h>
#ifdef WIN32
#include <vector>
#else
#include <vector.h>
#endif

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

#ifdef WIN32
using namespace std;
#endif

int		gse_parse(void *arg);
void	gse_restart(FILE *in);

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
    delete sp;

#ifdef WIN32
    DBG(std::cerr << "s: " << s << endl);
#else
    DBG(cerr << "s: " << s << endl);
#endif

    return s;
}

bool
func_member(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 2) {
#ifdef WIN32
	std::cerr << "Wrong number of arguments." << endl;
#else
	cerr << "Wrong number of arguments." << endl;
#endif
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
#ifdef WIN32
		std::cerr << "Wrong argument type." << endl;
#else
		cerr << "Wrong argument type." << endl;
#endif
		return false;
    }

}

bool
func_null(int argc, BaseType *argv[], DDS &)
{
    if (argc != 1) {
#ifdef WIN32
		std::cerr << "Wrong number of arguments." << endl;
#else
		cerr << "Wrong number of arguments." << endl;
#endif
		return false;
    }
    
    switch(argv[0]->type()) {
      case  dods_list_c: {
	List *var = (List *)argv[0];
	bool result = var->null();
    
	return result;
      }

      default:
#ifdef WIN32
		std::cerr << "Wrong argument type." << endl;
#else
		cerr << "Wrong argument type." << endl;
#endif
	return false;
    }

}

BaseType *
func_length(int argc, BaseType *argv[], DDS &dds)
{
    if (argc != 1) {
#ifdef WIN32
	std::cerr << "Wrong number of arguments." << endl;
#else
	cerr << "Wrong number of arguments." << endl;
#endif
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
#ifdef WIN32
	std::cerr << "Wrong type argument to list operator `length'" << endl;
#else
	cerr << "Wrong type argument to list operator `length'" << endl;
#endif
	return 0;
    }
}

BaseType *
func_nth(int argc, BaseType *argv[], DDS &)
{
    if (argc != 2) {
#ifdef WIN32
	std::cerr << "Wrong number of arguments." << endl;
#else
	cerr << "Wrong number of arguments." << endl;
#endif
	return 0;
    }
    
    switch (argv[0]->type()) {
	case dods_list_c: {
	    if (argv[1]->type() != dods_int32_c) {
#ifdef WIN32
		std::cerr << "Second argument to NTH must be an integer." << endl;
#else
		cerr << "Second argument to NTH must be an integer." << endl;
#endif
		return 0;
	    }
	    List *var = (List *)argv[0];
	    dods_int32 n;
	    dods_int32 *np = &n;
	    argv[1]->buf2val((void **)&np);

	    return var->var(n);
	}

      default:
#ifdef WIN32
	std::cerr << "Wrong type argument to list operator `nth'" << endl;
#else
	cerr << "Wrong type argument to list operator `nth'" << endl;
#endif
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
	int error = 0;
	if (!map->read_p()) {
	    map->read(dds.filename(), error);
	    if (error)
		throw Error(malformed_expr, "Could not read map vector data.");
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
