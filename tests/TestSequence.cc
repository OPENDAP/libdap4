#include "TestSequence.h"

#include "debug.h"

using namespace std;

void
TestSequence::_duplicate(const TestSequence &ts)
{
}

Sequence *
NewSequence(const string &n)
{
    return new TestSequence(n);
}

BaseType *
TestSequence::ptr_duplicate()
{
    return new TestSequence(*this);
}

TestSequence::TestSequence(const string &n) : Sequence(n)
{
}

TestSequence::TestSequence(const TestSequence &rhs) : Sequence(rhs)
{
    _duplicate(rhs);
}

TestSequence::~TestSequence()
{
}

TestSequence &
TestSequence::operator=(const TestSequence &rhs)
{
    if (this == &rhs)
	return *this;

    dynamic_cast<Sequence &>(*this) = rhs; // run Constructor=

    _duplicate(rhs);

    return *this;
}

// Read values from text files. Sequence instances are stored on separate
// lines. Line can be no more than 255 characters long.

bool 
TestSequence::read(const string &dataset)
{
    if( read_p() )
    {
	return true;
    }

    set_read_p(true);
    
    return true;
}

int
TestSequence::length()
{
    return 0;
}
