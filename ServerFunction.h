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
