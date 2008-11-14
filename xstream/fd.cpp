#include "config.h"
#include "posix.h"
#include "fd.h"

#include <algorithm>
#include <streambuf>

#define DODS_DEBUG

#include "debug.h"

namespace xstream
{
	namespace fd
	{
		static const int eof = std::streambuf::traits_type::eof();
		static const size_t buflen = 4*1024;

		streambuf::streambuf(const int f, const bool c)
			:xstream::posix::fd(f,c), rbuf(buflen), wbuf(buflen)
		{
			DBG(cerr << "fd::streambuf (int fd)" << endl);

			//next read/write calls will call uflow overflow
		}

		void streambuf::flush_write(){
			DBG(cerr << "fd::streambuf::flush, taken: " << taken() << " bytes." << endl);
			write(wbuf.buf,taken());
			reset_write();
		}

		int streambuf::sync() {
			DBG(cerr << "fd::streambuf::sync" << endl);

			//write remaining data
			flush_write();

			//physical sync data
			xstream::posix::fd::sync();
			//if there's an error an exception is thrown
			//and it doesn't get here
			return 1;
		}

		int streambuf::overflow(const int c) {
			DBG(cerr << "fd::streambuf::overflow ("<<c<<")" << endl);

			if(eof==c){
				return eof;
			}

			flush_write();
			*pptr() = static_cast < char >(c);
			pbump (1);
			return c;
		}

		std::streamsize streambuf::xsputn(const char* buffer, std::streamsize n)
		{
            DBG(cerr << "fd::streambuf::xsputn "<<n << endl);
			flush_write();
			std::streamsize count = write(buffer, n);
			return count;
		}

		int streambuf::underflow() {
			DBG(cerr << "fd::streambuf::underflow" << endl);

			std::streamsize nread = read(rbuf.buf,rbuf.size);

			setg(rbuf.buf, rbuf.buf, rbuf.buf+nread);

			if(0==nread){
				return eof;
			}else{
				//return static_cast<int>(*rbuf.buf);
				return 0;
			}
		}

		std::streamsize streambuf::xsgetn(char *buffer, std::streamsize n){
			DBG(cerr << "fd::streambuf::xsgetn "<<n << endl);
			std::streamsize av=available();

			std::streamsize nread=0;

			char* beg=gptr();
			char* end = egptr();

			bool need_read=true;

			if(av>=n){
				end=beg+n;
				need_read=false;
			}

			//copy buffered data to suplied buffer
			std::copy(beg,end,buffer);
			nread=end-beg;
			setg(rbuf.buf,end,rbuf.buf+rbuf.size);

			if(need_read){
				nread+=read(buffer+av,n-av);
			}
			DBG(cerr << "\tread "<<nread);
			return nread;
		}

		void streambuf::reset_write()
		{
			setp(wbuf.buf,wbuf.buf+wbuf.size);
		}

		streambuf::~streambuf() {
			DBG(cerr << "fd::~streambuf" << endl);

			if (taken()>0){
				sync();
			}
		}
	}	//namespace fd
}	//namespace xstream
