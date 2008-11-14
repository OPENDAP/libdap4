/*! \file xstream/common.h
 *
 * \brief common objects
 */

#ifndef XSTREAM_COMMON_H
#define XSTREAM_COMMON_H

#include "config.h"

#include <streambuf>

namespace xstream{


/*!
 * \internal
 * \brief common base for ostreambufs
 *
 */
class ostreambuf: public std::streambuf
{
	protected:
		/*!
		 * \brief remaining characters in the buffer
		 *
		 */
		std::streamsize inline available() const {
			return (epptr () - pptr ());
		}

		/*!
		 * \brief number of characters in the buffer
		 *
		 */
		std::streamsize inline taken() const {
			return (pptr () - pbase ());
		}
};

/*!
 * \brief buffer management
 *
 */

class buffer
{
	public:
		char* buf; /*!< buffer where data is kept */
		size_t size; /*!< size of buffer */

		/*
		 * \brief allocates a buffer of size \c size
		 *
		 * \param size length of buffer
		 *
		 */
		buffer(const size_t size);

		/*!
		 * \brief increases the size of buffer
		 *
		 * \param factor size is updated according to \f$ size'=size*factor \f$
		 * \note old data is copied to the new buffer
		 *
		 */
		void grow(const unsigned int factor=2);

		/*!
		 * \brief resets the size of the buffer
		 *
		 * \param size new length of the buffer
		 *
		 * \note no copying of data is done
		 *
		 */
		void resize(const size_t size);

		/*!
		 * \brief deallocates buffer
		 *
		 */
		~buffer();
};

/*!
 * \brief common base for objects that manage input and output buffers
 * to use with zlib and bzlib
 *
 */
class common_buffer
{
	protected:
		std::streambuf *_sb; /*!< streambuf to read/write from/to */
		buffer in; /*!< input buffer */
		buffer out; /*!< output buffer */

	public:
		/*!
		 * \brief construct using a streambuf
		 */
		common_buffer(std::streambuf* sb);

		~common_buffer();

};

}//namespace xstream

#endif
