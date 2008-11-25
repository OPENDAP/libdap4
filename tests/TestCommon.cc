#include "TestCommon.h"
#include "InternalErr.h"

using namespace libdap ;

TestCommon::TestCommon()
{
}

TestCommon::~TestCommon()
{
}

#if 0
// FILE_METHODS
void 
TestCommon::output_values(FILE *out)
{
    throw InternalErr(__FILE__, __LINE__, "Not implemented.");
}
#endif

#if 0
void 
TestCommon::output_values(std::ostream &out)
{
    // print_val() is a BaseType method, so this can only be called in one
    // of the child classes which inherits from BaseType
    print_val(out, "", false);
}
#endif

void
TestCommon::set_series_values(bool)
{
    throw InternalErr(__FILE__, __LINE__, "Unimplemented");
}

bool
TestCommon::get_series_values()
{
    throw InternalErr(__FILE__, __LINE__, "Unimplemented");
}
