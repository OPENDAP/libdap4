
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// The Grid Selection Expression Clause class.

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

static char id[] not_used = {"$Id: GSEClause.cc,v 1.5 2000/09/22 02:17:20 jimg Exp $"};

#include <assert.h>
#include <Pix.h>

#include "Error.h"
#include "InternalErr.h"
#include "GSEClause.h"

// Private methods

GSEClause::GSEClause()
{
  throw InternalErr(__FILE__, __LINE__, "default ctor called for GSEClause");
}

GSEClause::GSEClause(const GSEClause &param)
{
  throw InternalErr(__FILE__, __LINE__, "copy ctor called for GSEClause");
}

GSEClause &GSEClause::operator=(GSEClause &rhs)
{
  throw InternalErr(__FILE__, __LINE__, "assigment called for GSEClause");
}

template<class T>
static bool
compare(T elem, relop op, double value)
{
    switch (op) {
      case dods_greater_op:
	return elem > value;
      case dods_greater_equal_op:
	return elem >= value;
      case dods_less_op:
	return elem < value;
      case dods_less_equal_op:
	return elem <= value;
      case dods_nop_op:
	throw Error(malformed_expr, "Attempt to use NOP in Grid selection.");
      default:
	throw Error(malformed_expr, "Unknown relational operator");
    }
}

#ifndef WIN32
template<class T>
void
GSEClause::set_start_stop()
{
   // Read the byte array, scan, set start and stop.
   T *vals = 0;
   _map->buf2val((void **)&vals);

   int i = _start;
   int end = _stop;
   while(i <= end && !compare<T>(vals[i], _op1, _value1))
           i++;
   _start = i;

   i = end;
   while(i >= 0 && !compare<T>(vals[i], _op1, _value1))
   i--;
   _stop = i;

   // Every clause must have one operator but the second is optional since
   // the more complex for of a clause is optional.
   if (_op2 != dods_nop_op) {
   int i = _start;
   int end = _stop;
   while(i <= end && !compare<T>(vals[i], _op2, _value2))
                  i++;
   _start = i;

   i = end;
   while(i >= 0 && !compare<T>(vals[i], _op2, _value2))
           i--;
   _stop = i;
   }
}
#endif

void
GSEClause::compute_indices()
{
    Pix p = _map->first_dim();
    assert(p);

#ifdef WIN32
	//  Allows us to get around short-comming with MS Visual C++ 6.0
	//  templates
	char dummy;

    switch (_map->var()->type()) {
      case dods_byte_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_int16_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_uint16_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_int32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_uint32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_float32_c:
	set_start_stop((char *)(&dummy));
	break;
      case dods_float64_c:
	set_start_stop((char *)(&dummy));
	break;
    default:
	throw Error(malformed_expr, 
"Grid selection using non-numeric map vectors is not supported");
    }

#else
    switch (_map->var()->type()) {
      case dods_byte_c:
	set_start_stop<char>();
	break;
      case dods_int16_c:
	set_start_stop<int>();
	break;
      case dods_uint16_c:
	set_start_stop<unsigned int>();
	break;
      case dods_int32_c:
	set_start_stop<int>();
	break;
      case dods_uint32_c:
	set_start_stop<unsigned int>();
	break;
      case dods_float32_c:
	set_start_stop<double>();
	break;
      case dods_float64_c:
	set_start_stop<double>();
	break;
    default:
	throw Error(malformed_expr, 
"Grid selection using non-numeric map vectors is not supported");
    }
#endif

}

// Public methods

GSEClause::GSEClause(Grid *grid, const string &map, const double value,
		     const relop op) 
    : _map((Array *)grid->var(map)), _value1(value), _value2(0), _op1(op), 
      _op2(dods_nop_op)
{
    // Initialize the start and stop indices.
    Pix p = _map->first_dim();
    assert(p);
    _start = _map->dimension_start(p);
    _stop = _map->dimension_stop(p);
    _expression = "none";

    compute_indices();
}

GSEClause::GSEClause(Grid *grid, const string &map, const double value1,
		     const relop op1, const double value2, const relop op2) 
    : _map((Array *)grid->var(map)), _value1(value1), _value2(value2), 
      _op1(op1), _op2(op2)
{
    // Initialize the start and stop indices.
    Pix p = _map->first_dim();
    assert(p);
    _start = _map->dimension_start(p);
    _stop = _map->dimension_stop(p);
    _expression = "none";

    compute_indices();
}

bool
GSEClause::OK() const
{
    if (!_map)
	return false;
    
    // More ...

    return true;
}

Array *
GSEClause::get_map() const
{
    return _map;
}

string
GSEClause::get_map_name() const
{
    return _map->name();
}

int
GSEClause::get_start() const
{
    return _start;
}

int
GSEClause::get_stop() const
{
    return _stop;
}

// $Log: GSEClause.cc,v $
// Revision 1.5  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.4  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.3.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.3  1999/04/29 02:29:30  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/03/24 23:37:14  jimg
// Added support for the Int16, UInt16 and Float32 types
//
// Revision 1.1  1999/01/21 02:07:43  jimg
// Created
//

