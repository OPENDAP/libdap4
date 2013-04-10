/*
 * io_test.cc
 *
 *  Created on: Sep 29, 2008
 *      Author: jimg
 */

#include "config.h"

#define DODS_DEBUG

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>

#include "xstream/fd.h"

using namespace std;

int
main(int argc, char *argv[])
{
#if 0
    // Build a loopback pipe that uses c++ streams
    int fd[2];
    if (pipe(fd) < 0) {
        fprintf(stderr, "Could not open pipe\n");
        return 1;
    }

    xstream::fd::streambuf out(fd[1], true);
    xstream::fd::streambuf in(fd[0], true);
#endif
    xstream::fd::streambuf out(1, true);
    xstream::fd::streambuf in(0, true);

    std::ostream pout(&out);
    pout.exceptions(ios::badbit);
    std::istream pin(&in);
    pin.exceptions(ios::badbit);
#if 0
    FILE *pin = fdopen(fd[0], "rb");
    if (!pin)
        cerr << "Could not open input side of loopback pipe." << endl;
#endif

    pout << "This is a test." << endl;
    pout << flush;
#if 0
    string info;

    pin >> info;

    cout << "info: " << info << endl;
#endif
}

