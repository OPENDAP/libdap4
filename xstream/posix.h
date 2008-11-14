/*! \file xstream/posix.h
 *
 * \brief POSIX helper objects and functions
 */

#ifndef __XSTREAM_POSIX_H
#define __XSTREAM_POSIX_H

#include "config.h"

#include <string>
#include <iosfwd>

namespace xstream{
/*!
 * \brief POSIX objects and functions
 */
namespace posix{
#if 0
	/*!
	 * \brief stores a \c strftime format and returns a string representation of the current date
	 *
	 */
	class date_format
	{
		public:
			std::string format; /*!< format string for strftime */

			/*!
			 * \brief construct specifying strftime format
			 */

			date_format(const std::string& format);
			std::string now() const;
	};
#endif
	/*!
	 * \brief checks the return code of a syscall and raises apropriate exception if needed
	 *
	 * \param code error code
	 * \param call name of syscall
	 *
	 */
	void check_return(const int code, const std::string& call);



	/*!
	 * \brief encapsulates a file descriptor
	 *
	 */

	class fd
	{
		protected:
			int fdn; /*!< the actual file descriptor */
			bool dest_close; /*<! if close should be called at destruction time */

		public:

			/*!
			 * \brief constructs a fd object and specifies properties
			 *
			 * \param fd filedescriptor to operate on
			 * \param close if true closes the file descriptor at destruction, otherwise doesn't do anything
			 *
			 */
			fd(const int fd, const bool close=false);

			/*!
			 * \brief reads at most \c len bytes from fd and stores then in \c buffer
			 *
			 * \param buffer where read data is stored
			 * \param len number of bytes to read
			 *
			 * \return number of read bytes
			 *
			 */
			std::streamsize read(char* buffer, const std::streamsize len);

			/*!
			 * \brief reads at most \c len bytes
			 *
			 * \return a string with the read data
			 *
			 */
			std::string read(std::streamsize len);

			/*!
			 * \brief writes at most \c len bytes from \c buffer to \c fd
			 *
			 * \param buffer where data is read from
			 * \param len number of bytes to write
			 *
			 * \return number of written bytes
			 *
			 */
			std::streamsize write(const char* buffer, const std::streamsize len);

#if 0
			/*!
			 * \brief writes a string to the fd
			 *
			 * \param string data to write
			 *
			 * \return number of written bytes
			 *
			 */
			void write(const std::string& string);
#endif

			/*!
			 * \brief tries to flush data to disk (or whatever the case)
			 *
			 * \note internally calls \c fdatasync so \c stat data is not updated
			 */
			void sync();

			/*!
			 * \brief destructor
			 *
			 * if specified at construct time, fd is closed
			 *
			 */
			~fd();

	};

}//namespace posix
}//namespace xstream

#endif
