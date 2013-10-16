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
 * AbstractFunction.cc
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#include "config.h"

#include "ServerFunction.h"

namespace libdap {

ServerFunction::ServerFunction() {
	setName("abstract_function");
	setDescriptionString("This function does nothing.");
	setUsageString("You can't use this function");
	setRole("http://services.opendap.org/dap4/server-side-function/null");
	setDocUrl("http://docs.opendap.org/index.php/Server_Side_Processing_Functions");
    d_bool_func = 0;
	d_btp_func  = 0;
	d_proj_func = 0;

}

ServerFunction::ServerFunction(string name, string version, string description, string usage, string doc_url, string role, bool_func f){
	setName(name);
	setVersion(version);
	setDescriptionString(description);
	setUsageString(usage);
	setRole(role);
	setDocUrl(doc_url);
	setFunction(f);
}

ServerFunction::ServerFunction(string name, string version, string description, string usage, string doc_url, string role, btp_func f){
	setName(name);
	setVersion(version);
	setDescriptionString(description);
	setUsageString(usage);
	setRole(role);
	setDocUrl(doc_url);
	setFunction(f);

}

ServerFunction::ServerFunction(string name, string version, string description, string usage, string doc_url, string role, proj_func f){
	setName(name);
	setVersion(version);
	setDescriptionString(description);
	setUsageString(usage);
	setRole(role);
	setDocUrl(doc_url);
	setFunction(f);
}


ServerFunction::~ServerFunction() {
    d_bool_func = 0;
	d_btp_func  = 0;
	d_proj_func = 0;
}



} /* namespace libdap */
