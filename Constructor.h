
// -*- C++ -*-

#ifndef _constructor_h
#define _constructor_h 1

#ifdef _GNUG_
#pragma interface
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

class Constructor: public BaseType {
private:
    Constructor();		// No default ctor.

protected:
    void _duplicate(const Constructor &s);

public:
    Constructor(const string &n, const Type &t);

    Constructor(const Constructor &copy_from);
    virtual ~Constructor();

    Constructor &operator=(const Constructor &rhs);

    /** True if the instance can be flattened and printed as a single table
	of values. For Arrays and Grids this is always false. For Structures
	and Sequences the conditions are more complex. The implementation
	provided by this class always returns false. Other classes should
	override this implementation.

	@return True if the instance can be printed as a single table of
	values, false otherwise. */
    bool is_linear();
};

#endif // _constructor_h 
