
// Implementation for BaseType.
//
// jhrg 9/6/94

// $Log: BaseType.cc,v $
// Revision 1.1  1994/09/09 15:28:41  jimg
// Class for base type variables. Int32, ... inherit from this class.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <String.h>

#include "BaseType.h"

const int shape_clump = 10;	// alloc shape_clump ints at a time for shape

// Do not use the int * passed as `s' to directly initialize `shape'. Instead
// copy the values from s to shape. This ensures that shape is correcly
// allocated.
// NB: Space for shape is allocated even if dim == 0, however, values for
// shape are assigned only if s is non-zero. In that case, it is assumed to
// have dim values.

BaseType::BaseType(const String &n, const String &t, const unsigned int d, 
		   const int *s)
    : name(n), type(t), dimensions(d)
{
    // shape_size == d rounded up to the next multiple of shape_clump
    shape_size = (d + shape_clump) / shape_clump * shape_clump;
    shape = new int[shape_size];

    if (s)
	for (int i = 0; i < d; ++i)
	    shape[i] = s[i];
} 

BaseType::BaseType(const BaseType &copy_from)
{
    name = copy_from.name;
    type = copy_from.type;
    dimensions = copy_from.dimensions;
    shape_size = copy_from.shape_size;

    shape = new int[shape_size];
    for (int i = 0; i < dimensions; ++i)
	shape[i] = copy_from.shape[i];
}

BaseType::~BaseType()
{
    delete [] shape;
}

BaseType &BaseType::operator=(const BaseType &rhs)
{
    if (this == &rhs)
	return *this;

    if (rhs.shape_size > shape_size) {
	delete [] shape;
	shape = new int[rhs.shape_size];
    }
    for (int i = 0; i < rhs.dimensions; ++i)
	shape[i] = rhs.shape[i];

    name = rhs.name;
    type = rhs.type;
    dimensions = rhs.dimensions;
    shape_size = rhs.shape_size;

    return *this;
}

// Add another dimension onto shape. Size is the magnitude of the dimension,
// which must be > 0.

void
BaseType::add_shape_dim(unsigned int size)
{
    if (dimensions < shape_size) {
	shape[dimensions] = size;
	dimensions++;
    }
    else {
	shape_size += shape_clump;
	int *t_shape = new int[shape_size];
	
	for (int i = 0; i < dimensions; ++i) 
	    t_shape[i] = shape[i];
	t_shape[dimensions] = size;
	delete [] shape;
	shape = t_shape;
	++dimensions;
    }
}

void 
BaseType::print()
{
    cout << "Type: " << type << endl;
    cout << "Name: " << name << endl;
    cout << "Dimensions: " << dimensions << endl;

    cout << "Shape Size: " << shape_size << endl;

    cout << "Shape: ";
    for (int i = 0; i < dimensions; ++i)
	cout << shape[i] << " : ";
    cout << endl;
}
