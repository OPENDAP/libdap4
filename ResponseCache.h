
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#ifndef _response_cache_h
#define _response_cache_h

#include <string>

#define FUNCTION_CACHE "/tmp/dap_functions_cache/"
#define FUNCTION_CACHE_PREFIX "f"
// Cache size in megabytes; 20,000M -> 20GB
#define FUNCTION_CACHE_SIZE 20000

namespace libdap
{

class DDS;
class DAPCache3;
class ResponseBuilder;
class ConstraintEvaluator;
class BaseTypeFactory;

/**
 * This class is used to cache DAP2 response objects.
 * @author jhrg 5/3/13
 */

class ResponseCache
{
private:
	ResponseCache(const ResponseCache &src);

    bool is_valid(const std::string &cache_file_name, const std::string &dataset);
    void read_data_from_cache(FILE *data, DDS *fdds);
    DDS *get_cached_data_ddx(const std::string &cache_file_name, BaseTypeFactory *factory, const std::string &dataset);

    DAPCache3 *d_cache;

    void initialize(const std::string &cache_path, const std::string &prefix, unsigned long size_in_megabytes);

    friend class ResponseCacheTest;

public:
    /** Initialize the cache using the default values for the cache. */
    ResponseCache() : d_cache(0) {
    	initialize(FUNCTION_CACHE, FUNCTION_CACHE_PREFIX, FUNCTION_CACHE_SIZE);
    }

    /** Initialize the cache.
     *
     * @note Once the underlying cache object is made, calling this has no effect. To change the cache
     * parameters, first delete the cache.
     * @todo Write the delete method.
     *
     * @param cache_path The pathname where responses are stored. If this does not exist, the cache is not
     * initialized
     * @param prefix Use this to prefix each entry in the cache. This is used to differentiate the response
     * cache entries from other entries if other things are cached in the same pathame.
     * @param size_in_megabytes Cache size.
     */
    ResponseCache(const std::string &cache_path, const std::string &prefix,
    		unsigned long size_in_megabytes) : d_cache(0) {
    	initialize(cache_path, prefix, size_in_megabytes);
    }

    virtual ~ResponseCache() {}

    /** Is the ResponseCache configured to cache objects? It is possible
     * to make a ResponseCache object even though the underlying cache
     * software has not been configured (or is intentionally turned off).
     *
     * @return True if the cache can be used, false otherwise.
     */
    bool is_available() { return d_cache != 0; }

    virtual DDS *read_cached_dataset(DDS &dds, const std::string &constraint, ResponseBuilder *rb,
    		ConstraintEvaluator *eval, std::string &cache_token);

    virtual void unlock_and_close(const std::string &cache_token);
};

} // namespace libdap

#endif // _response_cache_h
