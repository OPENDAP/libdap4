
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// The Grid Selection Expression Clause class.

// $Log: GSEClause.cc,v $
// Revision 1.1  1999/01/21 02:07:43  jimg
// Created
//

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

static char __unused__ id[] = {"$Id: GSEClause.cc,v 1.1 1999/01/21 02:07:43 jimg Exp $"};

#include <Pix.h>

#include "Error.h"
#include "GSEClause.h"

// Private methods

GSEClause::GSEClause()
{
    assert(false && "Default constructor for GSEClause called");
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

void
GSEClause::compute_indices()
{
    Pix p = _map->first_dim();
    assert(p);

    switch (_map->var()->type()) {
      case dods_byte_c:
	set_start_stop<char>();
	break;
      case dods_int32_c:
	set_start_stop<int>();
	break;
      case dods_uint32_c:
	set_start_stop<unsigned int>();
	break;
      case dods_float64_c:
	set_start_stop<double>();
	break;
    default:
	throw Error(malformed_expr, 
"Grid selection using non-numeric map vectors is not supported");
    }
}

// Public methods

#if 0
GSEClause::GSEClause(Grid *grid, const char *clause)
{
    // Initialize the start and stop indices.
    Pix p = _map->first_dim();
    assert(p);
    _start = _map->dimension_start(p);
    _stop = _map->dimension_stop(p);
    _expression = clause;

    compute_indices();
}

GSEClause::GSEClause(Grid *grid, const String &clause)
{
    // Initialize the start and stop indices.
    Pix p = _map->first_dim();
    assert(p);
    _start = _map->dimension_start(p);
    _stop = _map->dimension_stop(p);
    _expression = clause;

    compute_indices();
}
#endif

GSEClause::GSEClause(Grid *grid, const String &map, const double value,
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

GSEClause::GSEClause(Grid *grid, const String &map, const double value1,
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

String
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
