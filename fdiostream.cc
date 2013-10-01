// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2009 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
//
// Portions of this code were taken verbatim from  Josuttis,
// "The C++ Standard Library," p.672

#include "config.h"

#include "fdiostream.h"
#include <cstring> // for memcpy

//#define DODS_DEBUG
#include "debug.h"

namespace libdap {

/** Make an instance of fdoutbuf. To write to a file using an open file
    descriptor, use fdostream instead.
    @param _fd The open file descriptor
    @param _close If true, close the file when the stream buffer is deleted.
    False by default.
    @see fdostream */
fdoutbuf::fdoutbuf(int _fd, bool _close) :
    fd(_fd), close(_close)
{
    setp(buffer, buffer + (bufferSize - 1));
}

/** When deleting the stream buffer, be sure to call sync(). Close the
    associated file if specified. */
fdoutbuf::~fdoutbuf()
{
    sync();
    if (close)
	::close(fd);
}

// flush the characters in the buffer
int fdoutbuf::flushBuffer()
{
    int num = pptr() - pbase();
    if (write(1, buffer, num) != num) {
	return EOF;
    }
    pbump(-num);
    return num;
}

/** Buffer full, write c and all previous characters */
int fdoutbuf::overflow(int c)
{
    if (c != EOF) {
	*pptr() = c;
	pbump(1);
    }
    // flush the buffer
    if (flushBuffer() == EOF) {
	//Error
	return EOF;
    }

    return c;
}

/** synchronize with file/destination */
int fdoutbuf::sync()
{
    if (flushBuffer() == EOF) {
	// Error
	return -1;
    }
    return 0;
}

/** write multiple characters */
std::streamsize fdoutbuf::xsputn(const char *s, std::streamsize num)
{
    return write(fd, s, num);
}

/*
How the buffer works for input streams:

Initialized:
     eback() --\
     gptr()  --|
     egptr() --|
               |
---------------------------
| | | | | | | | | | | | | |
---------------------------

After the first call to read, the buffer is filled:
   eback() --\
   gptr()  --|
             | egptr() --|
             |           |
---------------------------
| | | | | | |h|a|l|l|o|w|e|
---------------------------

After 'hallowe' is read from the stream, gptr() reaches egptr() and that
triggers the second read, which first must shuffle the characters 'hallowe'
to the 'put back' area of the buffer and then read more characters from the
underlying input source (fle descriptor or FILE*).

   eback() --\
             |  gptr() --|
             |           |
             |           |
---------------------------
| | | | | | |h|a|l|l|o|w|e|
---------------------------
                         |
	       egptr() --|

After each read, gptr() is advanced until it hits egptr, which triggers a
read. However, before the read takes place, characters are moved into the
put back part of teh buffer. IE when a character is 'read' using the stream
all the really happens is the gptr is advanced, the character is still in the
buffer

   gptr()  --|
             |    egptr()
 /-eback()   |           |
---------------------------
|h|a|l|l|o|w|e|e|n| |c|o|s|
---------------------------

*/

/** Make a stream buffer for reading from an open file using its file
    descriptor.
    @param _fd The open file descriptor
    @param _close If true, close the open file when deleting the stream buffer.
    False by default.
    @see fdistream */
fdinbuf::fdinbuf(int _fd, bool _close) :
    fd(_fd), close(_close)
{
    setg(buffer + putBack, // beginning of put back area
	    buffer + putBack, // read position
	    buffer + putBack); // end position
}

/** Close the file if specified. */
fdinbuf::~fdinbuf()
{
    if (close)
	::close(fd);
}

/** Insert new characters into the buffer */
int fdinbuf::underflow()
{
    if (gptr() < egptr()) {
	DBG(std::cerr << "underflow, no read" << std::endl);
	return *gptr();
    }

    // How many characters are in the 'put back' part of the buffer? Cap
    // this number at putBack, which is nominally 128.
    int numPutBack = gptr() - eback();
    if (numPutBack > putBack)
	numPutBack = putBack;

    // copy characters previously read into the put back area of the
    // buffer. In a typical call, putBack is 128 and numPutBack is 128 too.
    // In this case the destination of memcpy is the start of the buffer and
    // gptr() - numPutBack (the source of the copy) points to the last 128
    // characters in the buffer.
    memcpy(buffer + (putBack - numPutBack), gptr() - numPutBack, numPutBack);

    // read new characters
    int num = read(fd, buffer + putBack, bufferSize - putBack);
    DBG(std::cerr << "underflow, read returns: " << num << std::endl);
    if (num <= 0) {
	// Error or EOF; error < 0; EOF == 0
	return EOF;
    }

    setg(buffer + (putBack - numPutBack), // beginning of put back area
	    buffer + putBack, // read position
	    buffer + putBack + num); // end of buffer

    // return next character
#ifdef DODS_DEBUG
    char c = *gptr();
    DBG(std::cerr << "returning :" << c << std::endl);
    return c;
#else
    return *gptr();
#endif
}

/** Make a stream buffer for reading from an open file using a FILE pointer.

    @param _fp The open FILE pointer
    @param _close If true, close the open file when deleting the stream buffer.
    False by default.
    @see fpistream */
fpinbuf::fpinbuf(FILE *_fp, bool _close) :
    fp(_fp), close(_close)
{
    setg(buffer + putBack, // beginning of put back area
	    buffer + putBack, // read position
	    buffer + putBack); // end position
}

/** Close the file if specified. */
fpinbuf::~fpinbuf()
{
    if (close)
	fclose(fp);
}

/** Insert new characters into the buffer */
int fpinbuf::underflow()
{
    if (gptr() < egptr()) {
	DBG(std::cerr << "underflow, no read" << std::endl);
	return *gptr();
    }

    // process size of putBack area
    // use the number of characters read, but a maximum of putBack
    int numPutBack = gptr() - eback();
    if (numPutBack > putBack)
	numPutBack = putBack;

    // copy characters previously read into the put back area of the
    // buffer.
    memcpy(buffer + (putBack - numPutBack), gptr() - numPutBack, numPutBack);

    // read new characters
    int num = fread(buffer + putBack, 1, bufferSize - putBack, fp);
    DBG(std::cerr << "underflow, read returns: " << num << std::endl);
    if (num == 0) {
	// Error or EOF; use feof() or ferror() to test
	return EOF;
    }

    setg(buffer + (putBack - numPutBack), // beginning of put back area
	    buffer + putBack, // read position
	    buffer + putBack + num); // end of buffer

    // return next character
    return *gptr();
}

}
