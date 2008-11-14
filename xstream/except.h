/*! \file xstream/except.h
 *
 * \brief Main header file for exception related to xstream library
 *
 */

#ifndef __XSTREAM_EXCEPT_H
#define __XSTREAM_EXCEPT_H

#include "config.h"

#include <stdexcept>
#include <string>
#include <ios>

namespace xstream{

/*!
 * \brief general errors only detected at runtime, no solution possible
 *
 */

class fatal_error: public std::ios::failure
{
	public:
		fatal_error(const std::string& w):std::ios::failure(w){};

		/*!
		 * \brief describes the current library module
		 * (zlib,bzlib,base64,...)
		 *
		 */
		virtual std::string module() const
		{
			return "xstream";
		}

		virtual const char* what() const throw()
		{
			try{
				std::string w=module();
				w+=":: ";
				w+=(std::ios::failure::what());
				return w.c_str();
			}
			catch(...){
				return std::ios::failure::what();
			}
		}
};

#if 0

/*!
 * \brief general errors indicating bad usage of the library or unexpected situation, solution possible
 *
 */
class recoverable_error: public std::ios::failure {;
	recoverable_error(const std::string& w):std::ios::failure(w){};
};

#endif

}//namespace xstream

#endif
