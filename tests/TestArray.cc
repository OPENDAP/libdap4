#ifdef __GNUG__
#pragma implementation
#endif

#include "TestArray.h"
#include "dods-datatypes.h"

Array *
NewArray(const string &n, BaseType *v)
{
    return new TestArray(n, v);
}

BaseType *
TestArray::ptr_duplicate()
{
    return new TestArray(*this);
}

TestArray::TestArray(const string &n, BaseType *v) : Array(n, v)
{
}

TestArray::~TestArray()
{
}

// This read mfunc does some strange things to get a value - a real program
// would never get values this way. For testing this is OK.

bool
TestArray::read(const string &dataset)
{
    if( read_p( ) )
    {
	return true ;
    }

    dods_int16 thearr[ 4 ][ 3 ][ 2 ] = { { {1,2},
					   {3,4},
					   {5,6} },
				         { {7,8},
					   {9,10},
					   {11,12} },
				         { {13,14},
					   {15,16},
					   {17,18} },
				         { {19,20},
					   {21,22},
					   {23,24} } } ;

    val2buf( (void *)&thearr ) ;
    set_read_p( true ) ;

    return true ;
}

