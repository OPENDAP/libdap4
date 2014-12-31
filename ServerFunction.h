// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: Nathan Potter <npotter@opendap.org>
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

/*
 * ServerFunction.h
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#ifndef SERVER_FUNCTION_H_
#define SERVER_FUNCTION_H_

#include <iostream>

#include <expr.h>
#include <D4Function.h>

namespace libdap {

class ServerFunction {

private:
    std::string name;
    std::string description;
    std::string usage;
    std::string doc_url;
    std::string role;
    std::string version;

    // These are typedefs from DAP2 that are used with its CE parser
    // and are found in expr.h. jhrg 3/10/14
    bool_func d_bool_func;
    btp_func  d_btp_func;
    proj_func d_proj_func;

    D4Function d_d4_function;

public:
    ServerFunction();
    ServerFunction(std::string name, std::string version, std::string description, std::string usage,
    		std::string doc_url, std::string role, bool_func f);
    ServerFunction(std::string name, std::string version, std::string description, std::string usage,
    		std::string doc_url, std::string role, btp_func f);
    ServerFunction(std::string name, std::string version, std::string description, std::string usage,
    		std::string doc_url, std::string role, proj_func f);
    ServerFunction(std::string name, std::string version, std::string description, std::string usage,
    		std::string doc_url, std::string role, D4Function f);

    virtual ~ServerFunction() {	}

	std::string getName() { return name; }
	void setName(const std::string &n){ name = n; }

	std::string getUsageString() { return usage; }
	void setUsageString(const std::string &u){ usage = u; }

	std::string getDocUrl() { return doc_url; }
	void setDocUrl(const std::string &url){ doc_url = url; }

	std::string getRole() { return role; }
	void setRole(const std::string &r){ role = r; }

	std::string getDescriptionString(){ return description; }
	void setDescriptionString(const std::string &desc){ description = desc; }

	std::string getVersion(){ return version; }
	void setVersion(const std::string &ver){ version = ver; }

	/**
	 * If you are writing a function that can only operate on a particular kind of data, or one that relies on the presence
	 * of particular metadata, then you might override this method in order to stop the server from
	 * advertising the function in conjunction with datasets to which it cannot be applied.
	 *
	 * @param dds A DDS object for the dataset about which we will ask the question:
	 * Can this function operate on all or some portion of the contents of this dataset?
	 * CAUTION: Any implementation of this should be careful not to read data (or read as little as possible) from the
	 * passed DDS. Reading data in this method may have negative effect on overall performance. Examining metadata and
	 * dataset structure should be the basis for determining the applicability of a function to the dataset.
	 * @return true If this function operate on all or some portion of the contents of this dataset, false otherwise.
	 */
	virtual bool canOperateOn(DDS &) { return true; }

	/**
	 * @see canOperateOn(DDS &)
	 * @param The Dataset's DMR.
	 * @return True if the function can work with the dataset, false otherwise.
	 */
	virtual bool canOperateOn(DMR &) { return true; }

	/**
	 * Set the C function pointer for this function object.
	 * @note This does not alter any of the other function pointers,
	 * so the same name can be used for all four different kinds of
	 * functions without conflict.
	 *
	 * @param bf
	 */
	void setFunction(bool_func bf) {
		d_bool_func = bf;
	}

	void setFunction(btp_func btp) {
		d_btp_func  = btp;
	}

	void setFunction(proj_func pf) {
		d_proj_func = pf;
	}

	void setFunction(D4Function pf) {
		d_d4_function = pf;
	}

	std::string getTypeString() {
		if (d_bool_func) return "boolean";
		if (d_btp_func) return "basetype";
		if (d_proj_func) return "projection";
		if (d_d4_function) return "D4Function";
		return "null";
	}

	bool_func get_bool_func(){ return d_bool_func; }
	btp_func  get_btp_func() { return d_btp_func;  }
	proj_func get_proj_func(){ return d_proj_func; }
	D4Function get_d4_function() { return d_d4_function; }
};

} /* namespace libdap */
#endif /* SERVER_FUNCTION_H_ */
