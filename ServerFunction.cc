/*
 * AbstractFunction.cc
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

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
