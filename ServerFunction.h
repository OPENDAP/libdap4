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
	void   setName(string n){ name = n; }

	string getUsageString() { return usage; }
	void   setUsageString(string u){ usage = u; }

	string getDocUrl() { return doc_url; }
	void   setDocUrl(string url){ doc_url = url; }

	string getRole() { return role; }
	void   setRole(string r){ role = r; }

	string getDescriptionString(){ return description; }
	void   setDescriptionString(string desc){ description = desc; }

	string getVersion(){ return version; }
	void   setVersion(string ver){ version = ver; }

	bool canOperateOn(DDS &dds) { return true; }

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

	bool_func get_bool_func(){ return d_bool_func; }
	btp_func  get_btp_func() { return d_btp_func;  }
	proj_func get_proj_func(){ return d_proj_func; }

};

} /* namespace libdap */
#endif /* ABSTRACTFUNCTION_H_ */
