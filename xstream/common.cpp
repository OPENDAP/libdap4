#include "common.h"
#include <algorithm>
#define DODS_DEBUG
#include "debug.h"

namespace xstream {

    static const size_t buffer_size     = 4*1024;
	static const size_t out_buffer_size = static_cast < size_t > (buffer_size * 1.02 + 12);	//see manual

	buffer::buffer(const size_t s)
		:buf(0),size(0)
	{
		DBG(cerr << "buffer::buffer ("<<s<<")" << endl);
		resize(s);
	}

	void buffer::grow(const unsigned int f)
	{
		DBG(cerr << "buffer::grow "<<f << endl);
		if(f<1){
			DBG(cerr << "\tERROR: just tried to grow to a smaller size" << endl);
			return;
		}
		const size_t new_s = size*f;
		char* new_b = new char[new_s];

		std::copy(buf,buf+size,new_b);
		delete[] buf;

		size=new_s;
		buf=new_b;
	}

	void buffer::resize(const size_t s)
	{
		DBG(cerr << "buffer::resize "<<s << endl);
		if(buf){
			DBG(cerr << "\tdeleting buf" << endl);
			delete[] buf;
		}
		size=s;
		buf = new char[size];
	}


	buffer::~buffer()
	{
		DBG(cerr << "buffer::~buffer" << endl);
		delete[] buf;
	}

	common_buffer::common_buffer(std::streambuf * sb)
		:_sb (sb), in(buffer_size), out(out_buffer_size)
	{
		DBG(cerr << "common_buffer" << endl);
	}

	common_buffer::~common_buffer() {
			DBG(cerr << "~common_buffer" << endl);
	}

}//namespace xstream
