/*! \file xstream/fd.h
 *
 * \brief C++ streambuf to work with file descriptors
 *
 */

#ifndef __XSTREAM_FD_H
#define __XSTREAM_FD_H

#include "config.h"

#include "common.h"
#include "posix.h"

#include <streambuf>
#include <set>

namespace xstream{

/*!
 * \brief  file descriptor streambuf objects
 *
 */
namespace fd{

/*!
 * \brief file descriptor streambuf
 *
 * allows to use a streambuf to read/write data to/from a file descriptor
 *
 * \todo implement seek operation when possible
 *
 */
class streambuf: public xstream::ostreambuf, private xstream::posix::fd {
	private:

		xstream::buffer rbuf; /*!< were read data is stored */
		xstream::buffer wbuf; /*!< were data to be written is stored */


		/*!
		 * \brief flush as much data as possible (overloaded from streambuf)
		 *
		 * */
		int sync();

		/*!
		 * \brief write a character that surpasses buffer end (overloaded from streambuf)
		 *
		 */
		int overflow(const int c);

		/*!
		 * \brief write an entire buffer (overloaded from streambuf)
		 *
		 */
		std::streamsize xsputn(const char* buffer, std::streamsize n);

		/*!
		 * \brief reads \c n characters to \c buffer (overloaded from streambuf)
		 *
		 */
		std::streamsize xsgetn(char *buffer, std::streamsize n);

		/*!
		 * \brief requests that input buffer be reloaded (overloaded from streambuf)
		 */
		int underflow();

		void reset_write();

		void flush_write();

	public:
		/*!
		 * \brief construct specifying the file descriptor
		 *
		 * \param fd filedescriptor
		 * \param close if true closes the file descriptor at destruction
		 */
		streambuf(const int fd, const bool close=true);

		/*!
		 * \brief closes the streambuf stream
		 *
		 */
		~streambuf();

};


}//namespace fd
}//namespace xstream

#endif
