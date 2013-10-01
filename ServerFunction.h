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
 * AbstractFunction.h
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#ifndef ABSTRACTFUNCTION_H_
#define ABSTRACTFUNCTION_H_

#include <iostream>
#include "expr.h"

using std::endl;

#include "BaseType.h"

namespace libdap {


class ServerFunction {

private:
    string name;
    string description;
    string usage;
    string doc_url; // @TODO 'doc_url' Should be a URL object.
    string role;    // @TODO 'role' Should be a URI object.
    string version;

    libdap::bool_func d_bool_func;
    libdap::btp_func  d_btp_func;
    libdap::proj_func d_proj_func;

public:
    ServerFunction();
    ServerFunction(string name, string version, string description, string usage, string doc_url, string role, bool_func f);
    ServerFunction(string name, string version, string description, string usage, string doc_url, string role, btp_func f);
    ServerFunction(string name, string version, string description, string usage, string doc_url, string role, proj_func f);
    virtual ~ServerFunction();



	string getName() { return name; }
	void   setName(const string &n){ name = n; }

	string getUsageString() { return usage; }
	void   setUsageString(const string &u){ usage = u; }

	string getDocUrl() { return doc_url; }
	void   setDocUrl(const string &url){ doc_url = url; }

	string getRole() { return role; }
	void   setRole(const string &r){ role = r; }

	string getDescriptionString(){ return description; }
	void   setDescriptionString(const string &desc){ description = desc; }

	string getVersion(){ return version; }
	void   setVersion(const string &ver){ version = ver; }

	/**
	 * If you are writing a function that can only operate on a particular kind of data, or one that relies on the presence
	 * of particular metadata, then you might orride this method in order to stop the server from
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

	void setFunction(bool_func bf){
		d_bool_func = bf;
		d_btp_func  = 0;
		d_proj_func = 0;
	}

	void setFunction(btp_func btp){
		d_bool_func = 0;
		d_btp_func  = btp;
		d_proj_func = 0;
	}

	void setFunction(proj_func pf){
		d_bool_func = 0;
		d_btp_func  = 0;
		d_proj_func = pf;
	}

	string getTypeString(){
		if(d_bool_func)
			return "boolean";
		if(d_btp_func)
			return "basetype";
		if(d_proj_func)
			return "projection";
		return "null";
	}


	bool_func get_bool_func(){ return d_bool_func; }
	btp_func  get_btp_func() { return d_btp_func;  }
	proj_func get_proj_func(){ return d_proj_func; }

};

} /* namespace libdap */
#endif /* ABSTRACTFUNCTION_H_ */
