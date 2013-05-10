// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2011 OPeNDAP, Inc.
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

#include "config.h"

#include <sys/stat.h>

#include <iostream>
#include <string>
#include <fstream>

#include "DDS.h"
#include "ConstraintEvaluator.h"
#include "DDXParserSAX2.h"

#include "ResponseCache.h"
#include "ResponseBuilder.h"

#include "XDRStreamMarshaller.h"
#include "XDRFileUnMarshaller.h"

#include "DAPCache3.h"

#include "debug.h"
#include "mime_util.h"	// for last_modified_time() and rfc_822_date()
#include "util.h"

#define FUNCTION_CACHE "/tmp/dap_functions_cache/"
#define FUNCTION_CACHE_PREFIX "f"
// Cache size in megabytes; 20,000M -> 20GB
#define FUNCTION_CACHE_SIZE 20000

using namespace std;

namespace libdap {

/** Called when initializing a ResponseCache that's not going to be passed
 command line arguments. */
void ResponseCache::initialize()
{
    // The directory is a low-budget config param since
    // the cache will only be used if the directory exists.
    if (dir_exists(FUNCTION_CACHE)) {
        d_cache = DAPCache3::get_instance(FUNCTION_CACHE, FUNCTION_CACHE_PREFIX, FUNCTION_CACHE_SIZE);
    }
}

/**
 * Use the dataset name and the function-part of the CE to build a name
 * that can be used to index the result of that CE on the dataset. This
 * name can be used both to store a result for later (re)use or to access
 * a previously-stored result.
 *
 */
static string
build_cache_file_name(const string &dataset, const string &ce)
{
    DBG(cerr << "build_cache_file_name: dataset: " << dataset << ", ce: " << ce << endl);

    string name = dataset + "#" + ce;
    string::size_type pos = name.find_first_of("/(),\"\'");
    while (pos != string::npos) {
        name.replace(pos, 1, "#", 1);
        pos = name.find_first_of("/()\"\'");
    }

    DBG(cerr << "build_cache_file_name: name: " << name << endl);

    return name;
}

/**
 * Is the item named by cache_entry_name valid? This code tests that the
 * cache entry is non-zero in size (returns false if that is the case, although
 * that might not be correct) and that the dataset associated with this
 * ResponseBulder instance is at least as old as the cached entry.
 *
 * @param cache_file_name File name of the cached entry
 * @return True if the thing is valid, false otherwise.
 */
bool ResponseCache::is_valid(const string &cache_file_name, const string &dataset)
{
    // If the cached response is zero bytes in size, it's not valid.
    // (hmmm...)

    off_t entry_size = 0;
    time_t entry_time = 0;
    struct stat buf;
    if (stat(cache_file_name.c_str(), &buf) == 0) {
        entry_size = buf.st_size;
        entry_time = buf.st_mtime;
    }
    else {
        return false;
    }

    if (entry_size == 0)
        return false;

    time_t dataset_time = entry_time;
    if (stat(dataset.c_str(), &buf) == 0) {
        dataset_time = buf.st_mtime;
    }

    // Trick: if the d_dataset is not a file, stat() returns error and
    // the times stay equal and the code uses the cache entry.

    // TODO Fix this so that the code can get a LMT from the correct
    // handler.
    if (dataset_time > entry_time)
        return false;

    return true;
}

/**
 * Read the data from the saved response document.
 *
 * @note this method is made of code copied from Connect (process_data(0)
 * but this copy assumes it is reading a DDX with data written using the
 * code in ResponseCache::cache_data_ddx().
 *
 * @param data The input stream
 * @parma fdds Load this DDS object with the variables, attributes and
 * data values from the cached DDS.
 */
void ResponseCache::read_data_from_cache(FILE *data, DDS *fdds)
{
    // Rip off the MIME headers from the response if they are present
    string mime = get_next_mime_header(data);
    while (!mime.empty()) {
        mime = get_next_mime_header(data);
    }

    // Parse the DDX; throw an exception on error.
    DDXParser ddx_parser(fdds->get_factory());

    // Read the MPM boundary and then read the subsequent headers
    string boundary = read_multipart_boundary(data);
    DBG(cerr << "MPM Boundary: " << boundary << endl);

    read_multipart_headers(data, "text/xml", dap4_ddx);

    // Parse the DDX, reading up to and including the next boundary.
    // Return the CID for the matching data part
    string data_cid;
    ddx_parser.intern_stream(data, fdds, data_cid, boundary);

    // Munge the CID into something we can work with
    data_cid = cid_to_header_value(data_cid);
    DBG(cerr << "Data CID: " << data_cid << endl);

    // Read the data part's MPM part headers (boundary was read by
    // DDXParse::intern)
    read_multipart_headers(data, "application/octet-stream", dap4_data, data_cid);

    // Now read the data

    XDRFileUnMarshaller um(data);
    for (DDS::Vars_iter i = fdds->var_begin(); i != fdds->var_end(); i++) {
        (*i)->deserialize(um, fdds);
    }
}

/**
 * Read data from cache. Allocates a new DDS using the given factory.
 */
DDS *
ResponseCache::get_cached_data_ddx(const string &cache_file_name, BaseTypeFactory *factory, const string &dataset)
{
    DBG(cerr << "Reading cache for " << d_dataset + "?" + d_btp_func_ce << endl);

    DDS *fdds = new DDS(factory);

    fdds->filename(dataset) ;
    fdds->set_dataset_name( "function_result_" + name_path(dataset) ) ;

    // fstream data(cache_file_name.c_str());
    FILE *data = fopen( cache_file_name.c_str(), "r" );
    read_data_from_cache(data, fdds);
    fclose(data);

    fdds->set_factory( 0 ) ;

    // mark everything as read.
    DDS::Vars_iter i = fdds->var_begin() ;
    DDS::Vars_iter e = fdds->var_end() ;
    for( ; i != e; i++ ) {
        BaseType *b = (*i) ;
        b->set_read_p( true ) ;
    }

    return fdds;
}

/**
 * Get the cache DDS pointer - which will contain both attributes
 * and data values.
 *
 * @param dds The DDS of the dataset
 * @param eval The ConstraintEvaluator to use if the dataset is not in the
 * cache.
 * @param responseBuilder A ResponseBuilder to use to build the response if
 * the dataset is not in the cache.
 * @param cache_token A value-result parameter that contains an opaque
 * reference to the file in the cache. Used to release the lock on the cached file.
 * @return The DDS that resulted from calling the server functions
 * in the original CE.
 */
DDS *ResponseCache::read_cached_dataset(DDS &dds, ConstraintEvaluator &eval, ResponseBuilder &responseBuilder, string &cache_token)
{
    DBG(cerr << "Found function(s) in CE: " << responseBuilder.get_btp_func_ce() << endl);

    // These are used for the cached or newly created DDS object
    BaseTypeFactory factory;
    DDS *fdds;

    // Get the cache filename for this thing. Do not use the default
    // name mangling; instead use what build_cache_file_name() does.
    string cache_file_name = d_cache->get_cache_file_name(
    		build_cache_file_name(responseBuilder.get_dataset_name(), responseBuilder.get_btp_func_ce()),
    		false);
    int fd;
    try {
        // If the object in the cache is not valid, remove it. The read_lock will
        // then fail and the code will drop down to the create_and_lock() call.
        // is_valid() tests for a non-zero object and for d_dateset newer than
        // the cached object.
        if (!is_valid(cache_file_name, responseBuilder.get_dataset_name()))
            d_cache->purge_file(cache_file_name);

        if (d_cache->get_read_lock(cache_file_name, fd)) {
            DBG(cerr << "function ce - cached hit: " << cache_file_name << endl);
            fdds = get_cached_data_ddx(cache_file_name, &factory, responseBuilder.get_dataset_name());
        }
        else if (d_cache->create_and_lock(cache_file_name, fd)) {
            // If here, the cache_file_name could not be locked for read access;
            // try to build it. First make an empty file and get an exclusive lock on it.
            DBG(cerr << "function ce - caching " << cache_file_name << endl);

            eval.parse_constraint(responseBuilder.get_btp_func_ce(), dds);
            fdds = eval.eval_function_clauses(dds);

            responseBuilder.cache_data_ddx(cache_file_name, *fdds);

            // Change the exclusive lock on the new file to a shared lock. This keeps
            // other processes from purging the new file and ensures that the reading
            // process can use it.
            d_cache->exclusive_to_shared_lock(fd);

            // Now update the total cache size info and purge if needed. The new file's
            // name is passed into the purge method because this process cannot detect its
            // own lock on the file.
            unsigned long long size = d_cache->update_cache_info(cache_file_name);
            if (d_cache->cache_too_big(size))
                d_cache->update_and_purge(cache_file_name);
        }
        // get_read_lock() returns immediately if the file does not exist,
        // but blocks waiting to get a shared lock if the file does exist.
        else if (d_cache->get_read_lock(cache_file_name, fd)) {
            DBG(cerr << "function ce - cached hit: " << cache_file_name << endl);
            fdds = get_cached_data_ddx(cache_file_name, &factory, responseBuilder.get_dataset_name());
        }
        else {
            throw InternalErr(__FILE__, __LINE__, "Cache error during function invocation.");
        }
    }
    catch (...) {
        DBG(cerr << "caught exception, unlocking cache and re-throw." << endl );
        // I think this call is not needed. jhrg 10/23/12
        d_cache->unlock_cache();
        throw;
    }

    cache_token = cache_file_name;  // Set this value-result parameter
    return fdds;
}

/**
 * Given that a DDS has been extracted from the cache, use this to
 * unlock that item. Once the code has ths DDS object, it no longer
 * needs to hold onto the cached item.
 *
 * @param cache_token Opaque token used by the cache.
 */
void
ResponseCache::unlock_and_close(const string &cache_token)
{
	d_cache->unlock_and_close(cache_token);
}


} // namespace libdap

