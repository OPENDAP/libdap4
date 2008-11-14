/*
 * oubuf.cc
 *
 *  Created on: Oct 5, 2008
 *      Author: jimg
 */


#include <iostream>
#include <outbuf.h>

int main()
{
    fdostream out(1);

    out << "31 hex:" << std::hex << 31 << std::endl;

    return 0;
}
