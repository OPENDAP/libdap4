/*
 * AbstractServerSideFunction.h
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#ifndef ABSTRACTSERVERSIDEFUNCTION_H_
#define ABSTRACTSERVERSIDEFUNCTION_H_

#include <iostream>

using std::endl;

#include "BaseType.h"

namespace libdap {

class AbstractServerSideFunction {

private:
    string name;
    string description;
    string usage;

public:
	AbstractServerSideFunction();
	virtual ~AbstractServerSideFunction();

	string getName(){ return name; }
	void setName(string n){ name = n; }

	string getDescriptionString(){ return description; }
	void setDescriptionString(string desc){ description = desc; }

	void ssf(int argc, libdap::BaseType *argv[], libdap::DDS &dds, libdap::BaseType **btpp);


};

} /* namespace libdap */
#endif /* ABSTRACTSERVERSIDEFUNCTION_H_ */
