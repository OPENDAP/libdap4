/*
 * AbstractFunction.h
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#ifndef ABSTRACTFUNCTION_H_
#define ABSTRACTFUNCTION_H_

#include <iostream>

using std::endl;

#include "BaseType.h"

namespace libdap {

enum functionType {
    null, basetypePointer, boolean, projection
};

class AbstractFunction {

private:
    string name;
    string description;
    string usage;
    functionType fType;

public:
    AbstractFunction();
	virtual ~AbstractFunction();

	void setType(functionType ft){ fType = ft; }
	functionType getType(){ return fType; }

	string getName(){ return name; }
	void setName(string n){ name = n; }

	string getDescriptionString(){ return description; }
	void setDescriptionString(string desc){ description = desc; }

	bool canOperateOn(DDS &dds) { return true; }

	virtual void func(int argc, BaseType *argv[], DDS &dds, bool *result) = 0;
	virtual void func(int argc, BaseType *argv[], DDS &dds, BaseType **btpp) = 0;
	virtual void func(int argc, BaseType *argv[], DDS &dds, ConstraintEvaluator &ce) = 0;

};

} /* namespace libdap */
#endif /* ABSTRACTFUNCTION_H_ */
