/*
 * oubuf.cc
 *
 *  Created on: Oct 5, 2008
 *      Author: jimg
 */


#include <iostream>
#include "outbuf.h"

int main()
{
    fdostream out(1);

    out << "31 hex:" << std::hex << 31 << std::endl;

    fdistream in(0);

    char c;
    for (int i = 0; i < 20; ++i) {
	in.get(c);

	std::cout << c << std::flush;

	if (i == 8) {
	    in.unget();
	    in.unget();
	}
    }

    std::cout << std::endl;

    return 0;
}
