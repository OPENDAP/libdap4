
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

  virtual bool is_linear();

};

#endif // _constructor_h 
