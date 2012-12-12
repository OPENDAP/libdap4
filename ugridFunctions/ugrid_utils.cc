// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2011,2012 OPeNDAP, Inc.
// Authors: Nathan Potter <ndp@opendap.org>
//          James Gallagher <jgallagher@opendap.org>
//          Scott Moe <smeest1@gmail.com>
//          Bill Howe <billhowe@cs.washington.edu>
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#if 0
#include <limits.h>

#include <cstdlib>      // used by strtod()
#include <cerrno>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cxxabi.h>

#define DODS_DEBUG

#include "BaseType.h"
#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"
#include "Error.h"
#endif

#include <vector>
#include <sstream>

#include <gridfields/array.h>

#include "BaseType.h"
#include "Array.h"

#include "debug.h"

#include "ugrid_utils.h"

using namespace std;
using namespace libdap;

namespace libdap {

/**
 * DAP Array data extraction helper method.
 */
template<typename DODS, typename T>T *extract_array_helper(Array *a) {
	int length = a->length();

	DBG(
	int status;
	char *dodsTypeName = abi::__cxa_demangle(typeid(DODS).name(), 0, 0, &status);;
	char *tTypeName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);;
	)

	DBG(cerr << "extract_array_helper() - " << "Extracting data from DAP Array '" << a->name() <<"'"<< endl);
	DBG(cerr << "extract_array_helper() - " << "Allocating " << length << " of type "<< dodsTypeName << endl);
	DODS *src = new DODS[length];

	DBG(cerr << "extract_array_helper() - " << "Copying values from DAP Array "<< a->name() <<
			" to an array of type '" << dodsTypeName <<"'. targetAddress=" << src << endl);
	a->value(src);
	DBG(cerr << "extract_array_helper() - " << "Copy complete." << endl);

	DBG(cerr << "extract_array_helper() - " << "Allocating " << length << " of type "<< tTypeName << endl);
	T *dest = new T[length];

	DBG(cerr << "extract_array_helper() - " << "Casting/Copying array of type '" <<
			 dodsTypeName<<"' to an array of type '" << tTypeName << "'" << endl);
	for (int i = 0; i < length; ++i)
		dest[i] = (T) src[i];

	DBG(cerr << "extract_array_helper() - " << "Copy complete." << endl);


	// We're done with b, so get rid of it.
	DBG(cerr << "extract_array_helper() - " << "Releasing memory for an array of size "<< length <<
			" and type '" << dodsTypeName <<"'"<< endl);
	delete [] src;

	DBG(cerr << "extract_array_helper() - " << "Returning extracted values from DAP Array '" << a->name() <<"'"<< endl);

	return dest;
}

#if 0
/**
 * Extract data from a DAP array and return those values in a gridfields
 * array. This function sets the \e send_p property of the DAP Array and
 * uses its \e read() member function to get values. Thus, it should work
 * for values stored in any type of data source (e.g., file) for which the
 * Array class has been specialized.
 *
 * @param a The DAP Array. Extract values from this array
 * @return A GF::Array
 */
static GF::Array *extract_gridfield_array(Array *a) {
	if ((a->type() == dods_array_c && !a->var()->is_simple_type())
			|| a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
		throw Error(malformed_expr,
				"The function requires a DAP numeric-type array argument.");

	DBG(cerr << "extract_gridfield_array() - " << "Reading data values into DAP Array '" << a->name() <<"'"<< endl);
	a->set_send_p(true);
	a->read();

	// Construct a GridField array from a DODS array
	GF::Array *gfa;

	switch (a->var()->type()) {
	case dods_byte_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_byte, int>(a);
		gfa->shareIntData(values, a->length());
		break;
	}
	case dods_uint16_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_uint16, int>(a);
		gfa->shareIntData(values, a->length());
		break;
	}
	case dods_int16_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_int16, int>(a);
		gfa->shareIntData(values, a->length());
		break;
	}
	case dods_uint32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_uint32, int>(a);
		gfa->shareIntData(values, a->length());
		break;
	}
	case dods_int32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_int32, int>(a);
		gfa->shareIntData(values, a->length());
		break;
	}
	case dods_float32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::FLOAT);
		float *values = extract_array_helper<dods_float32, float>(a);
		gfa->shareFloatData(values, a->length());
		break;
	}
	case dods_float64_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::FLOAT);
		float *values = extract_array_helper<dods_float64, float>(a);
		gfa->shareFloatData(values, a->length());
		break;
	}
	default:
		throw InternalErr(__FILE__, __LINE__,
				"Unknown DAP type encountered when converting to gridfields array");
	}


	return gfa;
}

#endif

/**
 * Extract data from a DAP array and return those values in a gridfields
 * array. This function sets the \e send_p property of the DAP Array and
 * uses its \e read() member function to get values. Thus, it should work
 * for values stored in any type of data source (e.g., file) for which the
 * Array class has been specialized.
 *
 * @param a The DAP Array. Extract values from this array
 * @return A GF::Array
 */
GF::Array *extractGridFieldArray(Array *a, vector<int*> *sharedIntArrays, vector<float*> *sharedFloatArrays) {
	if ((a->type() == dods_array_c && !a->var()->is_simple_type())
			|| a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
		throw Error(malformed_expr,
				"The function requires a DAP numeric-type array argument.");

	DBG(cerr << "extract_gridfield_array() - " << "Reading data values into DAP Array '" << a->name() <<"'"<< endl);
	a->set_send_p(true);
	a->read();

	// Construct a GridField array from a DODS array
	GF::Array *gfa;

	switch (a->var()->type()) {
	case dods_byte_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_byte, int>(a);
		gfa->shareIntData(values, a->length());
		sharedIntArrays->push_back(values);
		break;
	}
	case dods_uint16_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_uint16, int>(a);
		gfa->shareIntData(values, a->length());
		sharedIntArrays->push_back(values);
		break;
	}
	case dods_int16_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_int16, int>(a);
		gfa->shareIntData(values, a->length());
		sharedIntArrays->push_back(values);
		break;
	}
	case dods_uint32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_uint32, int>(a);
		gfa->shareIntData(values, a->length());
		sharedIntArrays->push_back(values);
		break;
	}
	case dods_int32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::INT);
		int *values = extract_array_helper<dods_int32, int>(a);
		gfa->shareIntData(values, a->length());
		sharedIntArrays->push_back(values);
		break;
	}
	case dods_float32_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::FLOAT);
		float *values = extract_array_helper<dods_float32, float>(a);
		gfa->shareFloatData(values, a->length());
		sharedFloatArrays->push_back(values);
		break;
	}
	case dods_float64_c:
	{
		gfa = new GF::Array(a->var()->name(), GF::FLOAT);
		float *values = extract_array_helper<dods_float64, float>(a);
		gfa->shareFloatData(values, a->length());
		sharedFloatArrays->push_back(values);
		break;
	}
	default:
		throw InternalErr(__FILE__, __LINE__,
				"Unknown DAP type encountered when converting to gridfields array");
	}
	return gfa;
}

/** Given a pointer to an Array that holds a numeric type, extract the
 values and return in an array of T. This function allocates the
 array using 'new T[n]' so delete[] can be used when you are done
 the data. */
template<typename T> T *extract_array(Array * a) {

	// Simple types are Byte, ..., Float64, String and Url.
	if ((a->type() == dods_array_c && !a->var()->is_simple_type())
			|| a->var()->type() == dods_str_c || a->var()->type() == dods_url_c)
		throw Error(malformed_expr,
				"The function requires a DAP numeric-type array argument.");

	DBG(cerr << "extract_array() - " << "Reading data values into DAP Array '" << a->name() <<"'"<< endl);
	a->set_send_p(true);
	a->read();
	// This test should never pass due to the previous two lines;
	// reading here seems to make
	// sense rather than letting the caller forget to do so.
	// is read() idemopotent?
	if (!a->read_p())
		throw InternalErr(__FILE__, __LINE__,
				string("The Array '") + a->name()
						+ "'does not contain values. send_read_p() not called?");



	// The types of arguments that the CE Parser will build for numeric
	// constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
	// Expanded to work for any numeric type so it can be used for more than
	// just arguments.
	switch (a->var()->type()) {
	case dods_byte_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_byte_c'" << endl);
		return extract_array_helper<dods_byte, T>(a);

	case dods_uint16_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_uint16_c'" << endl);
		return extract_array_helper<dods_uint16, T>(a);

	case dods_int16_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_int16_c'" << endl);
		return extract_array_helper<dods_int16, T>(a);

	case dods_uint32_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_uint32_c'" << endl);
		return extract_array_helper<dods_uint32, T>(a);

	case dods_int32_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_int32_c'" << endl);
		return extract_array_helper<dods_int32, T>(a);

	case dods_float32_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_float32_c'" << endl);
		// Added the following line. jhrg 8/7/12
		return extract_array_helper<dods_float32, T>(a);

	case dods_float64_c:
		DBG(cerr << "extract_array() - extracting to an array of 'dods_float64_c'" << endl);
		return extract_array_helper<dods_float64, T>(a);

	default:
		throw InternalErr(__FILE__, __LINE__,
				"The argument list built by the CE parser contained an unsupported numeric type.");
	}
}

/**
 * Splits the string on the passed char. Returns vector of substrings.
 * TODO make this work on situations where multiple spaces doesn't hose the split()
 */
static vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
 * Splits the string on the passed char. Returns vector of substrings.
 */
static vector<string> split(const string &s, char delim) {
	vector<string> elems;
	return split(s, delim, elems);
}

// Returns the string value of the attribute called aName, 0 otherwise.
string getAttributeValue(BaseType *bt, string aName) {

	AttrTable &at = bt->get_attr_table();
	DBG(cerr << "getAttributeValue() - " << "Checking to see if the variable " << bt->name()
			<< "' has an attribute '"<< aName << "'"<<endl);

	// Confirm that submitted variable has an attribute called aName whose value is aValue.
	AttrTable::Attr_iter loc = at.simple_find(aName);
	if (loc != at.attr_end()) {
		DBG(cerr << "checkAttributeValue() - " << "'" << bt->name() << "' has a attribute named '" << aName << "'"<< endl);
		string value = at.get_attr(loc, 0);
		return value;
	}
	throw Error( "The variable "+bt->name()+" does not have the requested attribute '" + aName );
}

/**
 * Checks the passed BaseType attributes as follows: If the BaseType has a "cf_role" attribute and it's value is the same as
 * aValue return true. If it doesn't have a "cf_role" attribute, then if there is a "standard_name" attribute and it's value is
 * the same as aValue then  return true. All other outcomes return false.
 */
bool matchesCfRoleOrStandardName(BaseType *bt, string aValue) {
	// Confirm that submitted variable has a 'location' attribute whose value is "node".
	if (!checkAttributeValue(bt, _cfRole, aValue)) {
		// Missing the 'cf_role' attribute? Check for a 'standard_name' attribute whose value is "aValue".
		if (!checkAttributeValue(bt, _standardName, aValue)) {
			return false;
		}
	}
	return true;
}

/*
 If the two arrays have the exact dimensions in the same order, with the same name, size, start, stop, and stride values,
 return true.  Otherwise return false.
 */
bool same_dimensions(Array *arr1, Array *arr2) {
	Array::Dim_iter ait1;
	Array::Dim_iter ait2;
	DBG(cerr<< "same_dimensions() - " << "comparing array " << arr1->name() << " and array " << arr2->name() << endl);

	if (arr1->dimensions(true) != arr1->dimensions(true))
		return false;

	// We start walking both sets of ArrayDimensions at the beginning and increment each together.
	// We end the loop by testing for the end of one set of dimensions because we have already tested
	// that the two sets are the same size.
	for (ait1 = arr1->dim_begin(), ait2 = arr2->dim_begin();
			ait1 != arr1->dim_end(); ++ait1, ++ait2) {
		Array::dimension ad1 = *ait1;
		Array::dimension ad2 = *ait2;
		DBG(cerr << "same_dimensions() - " << "Comparing: "<< arr1->name() << "["<< ad1.name << "=" << ad1.size << "] AND "<< arr2->name() << "[" << ad2.name << "=" << ad2.size << "] "<< endl);
		if (ad2.name != ad1.name or ad2.size != ad1.size
				or ad2.stride != ad1.stride or ad2.stop != ad1.stop)
			return false;
	}
	if (ait2 != arr2->dim_end())
		return false;

	return true;
}


// Returns true iff the submitted BaseType variable has an attribute called aName attribute whose value is aValue.
bool checkAttributeValue(BaseType *bt, string aName, string aValue) {

	AttrTable &at = bt->get_attr_table();
	DBG(cerr << "checkAttributeValue() - " << "Checking to see if the variable " << bt->name()
			<< "' has an attribute '"<< aName << "' with value '" << aValue << "'"<<endl);

	// Confirm that submitted variable has an attribute called aName whose value is aValue.
	AttrTable::Attr_iter loc = at.simple_find(aName);
	if (loc != at.attr_end()) {
		DBG(cerr << "checkAttributeValue() - " << "'" << bt->name() << "' has a attribute named '" << aName << "'"<< endl);
		string value = at.get_attr(loc, 0);
		DBG(cerr << "checkAttributeValue() - " << "Attribute '"<< aName <<"' has value of '" << value << "'"<< endl);
		if (value != aValue) {
			return false;
		}
		return true;
	}
	return false;

}

} // namespace libdap
