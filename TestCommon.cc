#include "TestCommon.h"
#include "InternalErr.h"

TestCommon::TestCommon()
{
}

TestCommon::~TestCommon()
{
}

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
