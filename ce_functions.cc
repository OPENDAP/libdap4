
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)


// These functions are used by the CE evaluator
//
// 1/15/99 jhrg

// $Log: ce_functions.cc,v $
// Revision 1.1  1999/01/15 22:06:44  jimg
// Moved code from util.cc
//

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: ce_functions.cc,v 1.1 1999/01/15 22:06:44 jimg Exp $"};

#include "BaseType.h"
#include "List.h"

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
	  Sequence *var = (Sequence *)argv[0];
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

void 
func_grid_select(int argc, BaseType *argv[], DDS &)
{
}
