
#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

#include <string>
#include <strstream>

#include "Constructor.h"

#include "debug.h"
#include "Error.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

using std::endl;

// Private member functions

void
Constructor::_duplicate(const Constructor &s)
{
}

// Public member functions

Constructor::Constructor(const string &n, const Type &t) 
    : BaseType(n, t)
{
}

Constructor::Constructor(const Constructor &rhs) : BaseType(rhs)
{
}

Constructor::~Constructor()
{
}

Constructor &
Constructor::operator=(const Constructor &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<BaseType &>(*this) = rhs; // run BaseType=

    _duplicate(rhs);

    return *this;
}

    /** True if the instance can be flattened and printed as a single table
	of values. For Arrays and Grids this is always false. For Structures
	and Sequences the conditions are more complex. The implementation
	provided by this class always returns false. Other classes should
	override this implementation.

	@brief Check to see whether this variable can be printed simply.
	@return True if the instance can be printed as a single table of
	values, false otherwise. */
bool
Constructor::is_linear()
{
    return false;
}

// $Log: Constructor.cc,v $
// Revision 1.4  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.3  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.1.2.2  2001/09/25 20:35:28  jimg
// Added a default definition for is_linear().
//
// Revision 1.2  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.1.2.1  2001/06/05 16:04:39  jimg
// Created.
//
