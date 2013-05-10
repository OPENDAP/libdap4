
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef _response_cache_h
#define _response_cache_h

#include <string>

namespace libdap
{

class ResponseBuilder;
class DDS;
class DAPCache3;

/**
 * This class is used to cache DAP2 response objects.
 * @author jhrg 5/3/13
 */

class ResponseCache
{
private:
	ResponseCache(const ResponseCache &src);

    bool is_valid(const std::string &cache_file_name, const std::string &dataset);
    //void cache_data_ddx(const std::string &cache_file_name, DDS &dds);
    void read_data_from_cache(FILE *data, DDS *fdds);
    DDS *get_cached_data_ddx(const std::string &cache_file_name, BaseTypeFactory *factory, const std::string &dataset);

    DAPCache3 *d_cache;

    void initialize();

public:
    ResponseCache() { initialize(); }
    virtual ~ResponseCache() {}

    // This method is uses the above three and is used by send_das(), send_dds(), and send_data().
    virtual DDS *read_cached_dataset(DDS &dds, ConstraintEvaluator &eval, ResponseBuilder &responseBuilder, std::string &cache_token);
    virtual void unlock_and_close(const std::string &cache_token);
};

} // namespace libdap

#endif // _response_cache_h
