/*
 * AbstractFunction.cc
 *
 *  Created on: Feb 2, 2013
 *      Author: ndp
 */

#include "AbstractFunction.h"

namespace libdap {

AbstractFunction::AbstractFunction() {
	setName("abstract_function");
	setDescriptionString("This function does nothing.");
	d_bool_func = 0;
	d_btp_func  = 0;
	d_proj_func = 0;

}

AbstractFunction::~AbstractFunction() {
	// Nothing happens here...
	;
}



} /* namespace libdap */
