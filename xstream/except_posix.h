/*! \file xstream/except/posix.h
 *
 * \brief exceptions related to POSIX systems calls
 *
 */

#ifndef __XSTREAM_EXCEPT_POSIX_H
#define __XSTREAM_EXCEPT_POSIX_H

#include "config.h"

#include <string>
#include "except.h"
#include "posix.h"

namespace xstream{
	namespace posix{

/*!
 * \brief errors in POSIX usage
 *
 */
class general_error: public xstream::fatal_error
{
	private:
		std::string syscall; /*!< syscall that caused the error */
		int error_code; /*!< errno */

	public:
		general_error(const std::string& s, const int e, const std::string& d="general error"):
		xstream::fatal_error(d),syscall(s),error_code(e)
		{};

		virtual ~general_error() throw()
		{}

		virtual std::string module() const
		{
			return (xstream::fatal_error::module()+"::posix["+syscall+"]");
		}
};

}//namespace posix
}//namespace xstream

#endif
