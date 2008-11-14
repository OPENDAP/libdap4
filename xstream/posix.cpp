#include "config.h"
#include "posix.h"
#include "except_posix.h"

#include <iosfwd>

#include <string>
#include <ctime>

//for read write, etc
#include <unistd.h>

//for errno
#include <errno.h>

//for strerror and strerror_r
#include <cstring>
#define DODS_DEBUG
#include "debug.h"

/*
 * I don't prefix time localtime_r and strftime with std:: because lot's of implementaions are broken and it's an unnecessary purism
 *
 */

namespace xstream {
namespace posix{
	void check_return(const int code, const std::string& call){
		DBG(cerr << "posix::check_return "<<call<<" => "<<code << endl);
		if(-1==code) {
			//XXX please try to use strerror_r instead
			const std::string desc(strerror(errno));
			DBG(cerr << "\tthrowing "<<errno<<" => "<<desc << endl);
			throw general_error(call,errno,desc);
		}
	}

	fd::fd(const int f, const bool c):
		fdn(f),dest_close(c)
	{
		DBG(cerr << "posix::fd ("<<f<<","<<c<<")" << endl);
	}

	std::streamsize fd::read(char* buf, const std::streamsize len)
	{
		DBG(cerr << "posix::fd::read "<<len << endl);

		ssize_t count;

		do{
			count = ::read(fdn, buf, len);
		}while(-1==count && EINTR==errno);

		check_return(count,"read");

		return count;
	}

	std::string fd::read(std::streamsize len)
	{
		char buf[len];
		ssize_t count = read(buf,len);

		return std::string(buf,buf+count);
	}

	std::streamsize fd::write(const char* buf, const std::streamsize len)
	{
		DBG(cerr << "posix::fd::write "<<len << endl);

		ssize_t count;

		do{
			count = ::write(fdn, buf, len);
		}while(-1==count && EINTR==errno);

		check_return(count,"write");

		return count;
	}

	void fd::sync()
	{
		DBG(cerr << "posix::fd::sync" << endl);
		int cret=fsync(fdn);
		check_return(cret,"fsync");
	}

	fd::~fd()
	{
		DBG(cerr << "posix::fd::~fd" << endl);
		if(dest_close){
		DBG(cerr << "\tclosing" << endl);
			int cret = ::close(fdn);
			check_return(cret,"close");
		}
	}

}//namespace posix
}//namespace xstream
