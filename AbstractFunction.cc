/*
 * AbstractFunction.cc
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#include "AbstractFunction.h"

namespace libdap {

AbstractFunction::AbstractFunction() {
	setType(null);
	setName("abstract_function");
	setDescriptionString("This function does nothing.");
}

AbstractFunction::~AbstractFunction() {
	// Nothing happens here...
	;
}



} /* namespace libdap */
