#ifndef _testsequence_h
#define _testsequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"

class TestSequence: public Sequence {
private:
    void _duplicate(const TestSequence &ts);

public:
    TestSequence(const string &n = "");
    TestSequence(const TestSequence &rhs);

    virtual ~TestSequence();
 
    TestSequence &operator=(const TestSequence &rhs);
    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);

    virtual int length();
};

#endif // _testsequence_h
