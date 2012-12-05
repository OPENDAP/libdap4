// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2011,2012 OPeNDAP, Inc.
// Authors: James Gallagher <jgallagher@opendap.org>
//         Scott Moe <smeest1@gmail.com>
//         Bill Howe <billhowe@cs.washington.edu>
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

// NOTE: This file is built only when the gridfields library is linked with
// the netcdf_handler (i.e., the handler's build is configured using the
// --with-gridfields=... option to the 'configure' script).

#include "config.h"

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

#include "debug.h"
#include "util.h"

#include <gridfields/restrict.h>
#include <gridfields/gridfield.h>
#include <gridfields/grid.h>
#include <gridfields/cell.h>
#include <gridfields/cellarray.h>
#include <gridfields/array.h>
#include <gridfields/implicit0cells.h>
#include <gridfields/gridfieldoperator.h>

//  We wrapped VC++ 6.x strtod() to account for a short coming
//  in that function in regards to "NaN".  I don't know if this
//  still applies in more recent versions of that product.
//  ROM - 12/2007
#ifdef WIN32
#include <limits>
double w32strtod(const char *, char **);
#endif

using namespace std;
using namespace libdap;

namespace libdap {

/** Given a BaseType pointer, extract the string value it contains and return
 it.

 @param arg The BaseType pointer
 @return A C++ string
 @exception Error thrown if the referenced BaseType object does not contain
 a DAP String. */
static string extract_string_argument(BaseType * arg) {
	if (arg->type() != dods_str_c)
		throw Error(malformed_expr,
				"The function requires a DAP string argument.");

	if (!arg->read_p())
		throw InternalErr(__FILE__, __LINE__,
				"The CE Evaluator built an argument list where some constants held no values.");

	Str &dapString = dynamic_cast<Str&>(*arg);
	string s = dapString.value();

	DBG(cerr << "extract_string_argument() - s: " << s << endl);

	return s;
}


template<typename DODS, typename T>
static T *extract_array_helper(Array *a) {
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
static GF::Array *extractGridFieldIntArray(Array *a) {
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

/** Given a pointer to an Array that holds a numeric type, extract the
 values and return in an array of T. This function allocates the
 array using 'new T[n]' so delete[] can be used when you are done
 the data. */
template<typename T>
static T *extract_array(Array * a) {
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

/** Given a BaseType pointer, extract the numeric value it contains and return
 it in a C++ double.

 @param arg The BaseType pointer
 @return A C++ double
 @exception Error thrown if the referenced BaseType object does not contain
 a DAP numeric value. */
static double extract_double_value(BaseType * arg) {
	// Simple types are Byte, ..., Float64, String and Url.
	if (!arg->is_simple_type() || arg->type() == dods_str_c
			|| arg->type() == dods_url_c)
		throw Error(malformed_expr,
				"The function requires a DAP numeric-type argument.");

	if (!arg->read_p())
		throw InternalErr(__FILE__, __LINE__,
				"The CE Evaluator built an argument list where some constants held no values.");

	// The types of arguments that the CE Parser will build for numeric
	// constants are limited to Uint32, Int32 and Float64. See ce_expr.y.
	// Expanded to work for any numeric type so it can be used for more than
	// just arguments.
	switch (arg->type()) {
	case dods_byte_c:
		return (double) (dynamic_cast<Byte&>(*arg).value());
	case dods_uint16_c:
		return (double) (dynamic_cast<UInt16&>(*arg).value());
	case dods_int16_c:
		return (double) (dynamic_cast<Int16&>(*arg).value());
	case dods_uint32_c:
		return (double) (dynamic_cast<UInt32&>(*arg).value());
	case dods_int32_c:
		return (double) (dynamic_cast<Int32&>(*arg).value());
	case dods_float32_c:
		return (double) (dynamic_cast<Float32&>(*arg).value());
	case dods_float64_c:
		return dynamic_cast<Float64&>(*arg).value();
	default:
		throw InternalErr(__FILE__, __LINE__,
				"The argument list built by the CE parser contained an unsupported numeric type.");
	}
}

/**
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 * *******************************************************************************************************
 */

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

/**
 *  UGrid attribute vocabulary
 */
const string _cfRole = "cf_role";
const string _standardName = "standard_name";
const string _meshTopology = "mesh_topology";
const string _nodeCoordinates = "node_coordinates";
const string _faceNodeConnectivity = "face_node_connectivity";
const string _dimension = "dimension";
const string _location = "location";
const string _gridLocation = "grid_location";
const string _node = "node";
const string _mesh = "mesh";
const string _start_index = "start_index";


enum locationType {
	node, edge, face
};

/**
 *
 */

struct MeshDataVariable {

	/**
	 * The DAP dataset variable that the user requested.
	 */
	Array *meshDataVar;

	/**
	 * REQUIRED
	 * The attribute mesh points to the mesh_topology variable containing the meta-data attributes
	 * of the mesh on which the variable has been defined.
	 */
	string meshName;

	/**
	 * REQUIRED
	 * The first DAP dataset variable in the dataset that has a 'cf_role' attribute whose value is equal the value of
	 * the string 'mesh' or an attribute named 'standard_name' whose value is the same as the value of the string 'mesh'.
	 */
	BaseType *meshTopologyVariable;

	/**
	 * REQUIRED
	 * The attribute location points to the (stagger) location within the mesh at which the
	 * variable is defined. (face or node)
	 */
	locationType location;

	/**
	 * OPTIONAL
	 * The use of the coordinates attribute is copied from the CF-conventions.
	 * It is used to map the values of variables defined on the unstructured
	 * meshes directly to their location: latitude, longitude and optional elevation.
	 *
	 * The attribute node_coordinates contains a list of the whitespace separated names of
	 * the auxiliary coordinate variables representing the node locations (latitude,
	 * longitude, and optional elevation or other coordinates). These auxiliary coordinate
	 * variables will have length nNodes or nFaces, depending on the value of the location attribute.
	 *
	 * It appears that the coordinates attribute is redundant since the coordinates could also be
	 * obtained by using the appropriate coordinates definition in the mesh topology.
	 *
	 */
	//vector<string> *coordinateNames;
	//vector<Array *> *coordinateArrays;
};

struct TwoDMeshTopology {

	/**
	 * REQUIRED
	 *
	 * The DAP dataset variable that defined the mesh_topology.
	 */
	BaseType *myVar;

	/**
	 * REQUIRED
	 * The attribute dimension indicates the highest dimensionality of the geometric
	 * elements; for a 2-dimensional (triangular) mesh this should be 2.
	 */
	int dimension;

	int nodeCount;

	/**
	 * REQUIRED
	 *
	 * The attribute node_coordinates contains a list of the whitespace separated names of
	 * the auxiliary coordinate variables representing the node locations (latitude,
	 * longitude, and optional elevation or other coordinates). These auxiliary coordinate
	 * variables will have length nNodes.
	 */
	vector<Array *> *nodeCoordinateArrays;


	/**
	 * REQUIRED
	 *
	 *  - - - - - -
	 * 2D triangular mesh
	 * The attribute face_node_connectivity points to an index variable identifying for every
	 * face (here consistently triangle) the indices of its three corner nodes.
	 * The corner nodes should be specified in anticlockwise (also referred to as counterclockwise)
	 * direction as viewed from above (consistent with the CF-convention for bounds of p-sided cells.
	 *
	 * The connectivity array will thus be a matrix of size nFaces x 3. For the indexing one may use
	 * either 0- or 1-based indexing; the convention used should be specified using a start_index
	 * attribute to the index variable (i.e. Mesh2_face_nodes in the example below). Consistent with
	 * the CF-conventions compression option, the connectivity indices are 0-based by default.
	 * See this section on 0-/1-based indexing for more details.
	 * http://publicwiki.deltares.nl/display/NETCDF/Deltares+CF+proposal+for+Unstructured+Grid+data+model#DeltaresCFproposalforUnstructuredGriddatamodel-0%2F1basedindexing
	 *
	 * - - - - - -
	 * 2D flexible mesh
	 * The attribute face_node_connectivity points to an index variable identifying for every face the
	 * indices of its corner nodes. The corner nodes should be specified in anticlockwise direction as
	 * viewed from above (consistent with the CF-convention for bounds of p-sided cells. The connectivity
	 * array will be a matrix of size nFaces x MaxNumNodesPerFace; if a face has less corner nodes than
	 * MaxNumNodesPerFace then the last node indices shall be equal to _FillValue (which should obviously
	 * be larger than the number of nodes in the mesh). For the indexing one may use either 0- or 1-based
	 * indexing; the convention used should be specified using a start_index attribute to the index
	 * variable (i.e. Mesh2_face_nodes in the example below). Consistent with the CF-conventions
	 * compression option, the connectivity indices are 0-based by default. See this section on 0-/1-based
	 * indexing for more details.
	 *
	 */
	Array *faceNodeConnectivityArray;

	vector<MeshDataVariable *> *rangeDataArrays;

	/**
	 * OPTIONAL
	 * The "Optionally required" attribute edge_node_connectivity is required only if you want to store data on
	 * the edges (i.e. if you mind the numbering order of the edges).
	 * The edge_node_connectivity attribute contains the name of the array that maps edges to nodes. Although the
	 * face to node mapping implicitly also defines the location of the edges, it does not specify the global
	 * numbering of the edges. Again the indexing convention of edge_node_connectivity should be specified
	 * using the start_index attribute to the index variable (i.e. Mesh2_edge_nodes in the example below)
	 * and 0-based indexing is the default.
	 *
	 */
	//string edgeNodeConnectivityArrayName;

	/**
	 * OPTIONAL
	 * The face_edge_connectivity attribute points to an index variable identifying for every face
	 * (here consistently triangle) the indices of its three edges. The edges should be specified
	 * in anticlockwise direction as viewed from above. This connectivity array will thus be a matrix
	 * of size nFaces x 3. Again the indexing convention of face_edge_connectivity should be specified
	 * using the start_index attribute to the index variable (i.e. Mesh2_face_edges in the example
	 * below) and 0-based indexing is the default.
	 */
	//string faceEdgeConnectivityArrayName;
	//Array *faceEdgeConnectivityArray;

	/**
	 * OPTIONAL
	 * The face_face_connectivity attribute points to an index variable identifying pairs of faces
	 * (here consistently triangle) that share an edge, i.e. are neighbors. TODO: CHECK DEFINITION
	 * This connectivity array will thus be a matrix of size nFacePairs x 2. Again the indexing
	 * convention of face_face_connectivity should be specified using the start_index attribute to
	 * the index variable (i.e. Mesh2_face_links in the example below) and 0-based indexing is the default.
	 *
	 *
	 */
	//string faceFaceConnectivityArrayName;
	//Array *faceFaceConnectivityArray;

	/**
	 * OPTIONAL
	 * The face_coordinates attribute points to the auxiliary coordinate variables
	 * associated with the characteristic location of the faces. These auxiliary coordinate
	 * variables will have length nFaces, and may have in turn a bounds
	 * attribute that specifies the bounding coordinates of the face or edge (thereby duplicating the
	 * data in the node_coordinates variables).
	 *
	 */
	//vector<string> *faceCoordinateNames;
	//vector<Array *> *faceCoordinateArrays;

	/**
	 * OPTIONAL
	 * The edge_coordinates attribute points to the auxiliary coordinate variables
	 * associated with the characteristic location of the  edges. These auxiliary coordinate
	 * variables will have length nEdges, and may have in turn a bounds
	 * attribute that specifies the bounding coordinates of the edge (thereby duplicating the
	 * data in the node_coordinates variables).
	 *
	 */
	//vector<string> *edgeCoordinateNames;
	//vector<Array *> *edgeCoordinateArrays;

	GF::Grid *gridTopology;
	GF::GridField *inputGridField;
	GF::GridField *resultGridField;

};

/**
 * Function syntax
 */
static string UgridRestrictSyntax =
		"ugr(dim:int32, rangeVariable:string, [rangeVariable:string, ... ] condition:string)";

/**
 * Function Arguments
 */
struct UgridRestrictArgs {
	int dimension;
	vector<Array *> rangeVars;
	string filterExpression;
};

// Returns true iff the submitted BaseType variable has an attribute called aName attribute whose value is aValue.
static bool checkAttributeValue(BaseType *bt, string aName, string aValue) {

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

// Returns the string value of the attribute called aName, 0 otherwise.
static string getAttributeValue(BaseType *bt, string aName) {

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
static bool matchesCfRoleOrStandardName(BaseType *bt, string aValue) {
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
static bool same_dimensions(Array *arr1, Array *arr2) {
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

/**
 * Returns the coordinate variables identified in the meshTopology variable's node_coordinates attribute.
 * throws an error if the node_coordinates attribute is missing, if the coordinates are not arrays, and
 * if the arrays are not all the same shape.
 */
static vector<Array *> *getNodeCoordinateArrays(BaseType *meshTopology,
		DDS &dds) {
	DBG(cerr << "getNodeCoordinatesArrays() - " << "BEGIN Gathering node coordinate arrays..." << endl);

	string node_coordinates;
	AttrTable at = meshTopology->get_attr_table();

	AttrTable::Attr_iter iter_nodeCoors = at.simple_find(_nodeCoordinates);
	if (iter_nodeCoors != at.attr_end()) {
		node_coordinates = at.get_attr(iter_nodeCoors, 0);
	} else {
		throw Error(
				"Could not locate the " + _nodeCoordinates
						+ " attribute in the " + _meshTopology + " variable! "
						+ "The mesh_topology variable is named "
						+ meshTopology->name());
	}

	vector<Array *> *nodeCoordinateArrays = new vector<Array *>();

	// Split the node_coordinates string up on spaces
	// TODO make this work on situations where multiple spaces in the node_coorindates string doesn't hose the split()
	vector<string> nodeCoordinateNames = split(node_coordinates, ' ');

	// Find each variable in the resulting list
	vector<string>::iterator coorName_it;
	for (coorName_it = nodeCoordinateNames.begin();
			coorName_it != nodeCoordinateNames.end(); ++coorName_it) {
		string nodeCoordinateName = *coorName_it;

		//Now that we have the name of the coordinate variable get it from the DDS!!
		BaseType *btp = dds.var(nodeCoordinateName);
		if (btp == 0)
			throw Error(
					"Could not locate the " + _nodeCoordinates
							+ " variable named '" + nodeCoordinateName + "'! "
							+ "The mesh_topology variable is named "
							+ meshTopology->name());

		Array *newNodeCoordArray = dynamic_cast<Array*>(btp);
		if (newNodeCoordArray == 0) {
			throw Error(malformed_expr,
					"Node coordinate variable '" + nodeCoordinateName
							+ "' is not an Array type. It's an instance of "
							+ btp->type_name());
		}

		// Make sure this node coordinate variable has the same shape as all the others on the list - error if not true.
		vector<Array *>::iterator cachedCoorVar_it;
		for (cachedCoorVar_it = nodeCoordinateArrays->begin();
				cachedCoorVar_it != nodeCoordinateArrays->end();
				++cachedCoorVar_it) {
			Array *cachedNodeCoordinateArray = *cachedCoorVar_it;
			if (!same_dimensions(newNodeCoordArray, cachedNodeCoordinateArray))
				throw Error(
						"The node coordinate array '" + nodeCoordinateName
								+ "' is not the same shape as the cached node coordinate "
								+ " array '" + cachedNodeCoordinateArray->name()
								+ "'! " + "The mesh_topology variable is named "
								+ meshTopology->name());
		}
		// Add variable to returned vector.
		nodeCoordinateArrays->push_back(newNodeCoordArray);

	}


	DBG(cerr << "getNodeCoordinatesArrays() - " << "DONE" << endl);

	return nodeCoordinateArrays;

}
/**
 * Locates the the DAP variable identified by the face_node_connectivity attribute of the
 * meshTopology variable.
 */
static Array *getFaceNodeConnectivityArray(BaseType *meshTopology, DDS &dds)
{

    DBG(cerr << "getFaceNodeConnectivityArray() - "  << "Locating FNCA" << endl);

	string face_node_connectivity_var_name;
    AttrTable at = meshTopology->get_attr_table();

    AttrTable::Attr_iter iter_fnc = at.simple_find(_faceNodeConnectivity);
    if (iter_fnc != at.attr_end()) {
    	face_node_connectivity_var_name = at.get_attr(iter_fnc, 0);
    }
    else {
    	throw Error("Could not locate the "+_faceNodeConnectivity+" attribute in the "+_meshTopology+" variable! "+
    			"The mesh_topology variable is named "+meshTopology->name());
    }

	// Find the variable using the name

    BaseType *btp = dds.var(face_node_connectivity_var_name);

    if(btp==0)
    	throw Error("Could not locate the "+_faceNodeConnectivity+" variable named '"+face_node_connectivity_var_name+"'! "+
    			"The mesh_topology variable is named "+meshTopology->name());

    // Additional QC??

    // Is it an array?
    Array *fncArray = dynamic_cast<Array*>(btp);
    if(fncArray == 0) {
        throw Error(malformed_expr,"Face Node Connectivity variable '"+face_node_connectivity_var_name+"' is not an Array type. It's an instance of " + btp->type_name());
    }


    DBG(cerr << "getFaceNodeConnectivityArray() - "  << "Got FCNA '"+fncArray->name()+"'" << endl);

    return fncArray;


}


/**
 * FIXME: Make this thing ingest the other parts of the TwoDMeshTopology
 */
static TwoDMeshTopology *getNewMeshTopology(DDS &dds, string meshVarName) {

	TwoDMeshTopology *tdmt = new TwoDMeshTopology();

	tdmt->myVar = dds.var(meshVarName);

	// Retrieve the node coordinate arrays for the mesh
	tdmt->nodeCoordinateArrays = getNodeCoordinateArrays(tdmt->myVar, dds);

	// Retrieve the face node connectivity array for the mesh
	tdmt->faceNodeConnectivityArray = getFaceNodeConnectivityArray(tdmt->myVar,dds);

	tdmt->nodeCount = (*tdmt->nodeCoordinateArrays)[0]->length();

	tdmt->rangeDataArrays = new vector<MeshDataVariable *>();

	return tdmt;

}

// map<string, TwoDMeshTopology *> rank_dimensions;
static void addRangeVar(DDS &dds, Array *rangeVar,
		map<string, TwoDMeshTopology *> &meshTopologies) {

	MeshDataVariable *mdv = new MeshDataVariable();
	mdv->meshDataVar = rangeVar;

	DBG(cerr << "getMeshDataVar() - The user submitted the range data array: " << rangeVar->name() << endl);

	/**
	 * TODO: STOP doing this check and deal with face nodes!
	 * Confirm that submitted variable has a 'location' attribute whose value is "node".
	 */
	if (!checkAttributeValue(rangeVar, _location, _node)) {
		// Missing the 'location' attribute? Check for a 'grid_location' attribute whose value is "node".
		if (!checkAttributeValue(rangeVar, _gridLocation, _node)) {
			throw Error(
					"The requested range variable '" + rangeVar->name()
							+ "' has neither a '" + _location + "' attribute "
							+ "or a " + _gridLocation
							+ " attribute whose value is equal to '" + _node
							+ "'.");
		}
	}
	mdv->location = node;

	string meshVarName = getAttributeValue(rangeVar, _mesh);

	DBG(cerr << "getMeshDataVar() - Range data array refers to 'mesh' variable:: " << meshVarName << endl);

	// Get the MeshTopology from the map.
	TwoDMeshTopology *meshTopology;
	map<string, TwoDMeshTopology *>::iterator mit = meshTopologies.find(meshVarName);
	if(mit == meshTopologies.end()){
		// Not there? Make a new one.
		DBG(cerr << "getMeshDataVar() - MeshTopology object for '" << meshVarName <<"' does NOT exist. Getting a new one... "  << endl);
		meshTopology = getNewMeshTopology(dds, meshVarName);
		meshTopologies[meshVarName] =  meshTopology;
	}
	else {
		// Sweet! Found it....
		DBG(cerr << "getMeshDataVar() - MeshTopology object for '" << meshVarName <<"' exists. Retrieving... "  << endl);
		meshTopology = mit->second;
	}


	// Make sure that the requested range variable is the same shape as the node coordinate arrays
	// We only need to test the first nodeCoordinate array against the first rangeVar array
	// because we have already made sure all of the node coordinate arrays are the same size and
	// that all the rangeVar arrays are the same size. This is just to compare the two collections.
	Array *firstCoordinate = (*(meshTopology->nodeCoordinateArrays))[0];
	if (!same_dimensions(rangeVar, firstCoordinate))
		throw Error(
				"The dimensions of the requested range variable "
						+ rangeVar->name() + " does not match the shape "
						+ " of the node coordinate array "
						+ firstCoordinate->name());

	meshTopology->rangeDataArrays->push_back(mdv);

}

/**
 * Process the functions arguments and return the structure containing their values.
 */
static UgridRestrictArgs processUgrArgs(int argc, BaseType * argv[]) {

	DBG(cerr << "processUgrArgs() - BEGIN" << endl);

	UgridRestrictArgs args;
	args.rangeVars = vector<Array *>();

	// Check number of arguments; DBG is a macro. Use #define
	// DODS_DEBUG to activate the debugging stuff.
	if (argc < 3)
		throw Error(malformed_expr,
				"Wrong number of arguments to ugrid restrict function: "
						+ UgridRestrictSyntax + " was passed "
						+ long_to_string(argc) + " argument(s)");

	BaseType * bt;

	// ---------------------------------------------
	// Process the first arg, which is "dimension" or something - WE DON'T REALLY KNOW. (see FIXME below)
	// FIXME Ask Bill/Scott what this is about. Eliminate if not needed.
	bt = argv[0];
	if (bt->type() != dods_int32_c)
		throw Error(malformed_expr,
				"Wrong type for first argument, expected DAP Int32. "
						+ UgridRestrictSyntax + "  was passed a/an "
						+ bt->type_name());
	//FIXME Tell James what dim is about...
	args.dimension = extract_double_value(bt);

	// ---------------------------------------------
	// Process the last argument, the relational expression used to restrict the ugrid content.
	bt = argv[argc - 1];
	if (bt->type() != dods_str_c)
		throw Error(malformed_expr,
				"Wrong type for third argument, expected DAP String. "
						+ UgridRestrictSyntax + "  was passed a/an "
						+ bt->type_name());
	args.filterExpression = extract_string_argument(bt);

	// --------------------------------------------------
	// Process the range variables selected by the user.
	// We know that argc>=3, because we checked so the
	// following loop will try to find at least one rangeVar,
	// and it won't try to process the first or last members
	// of argv.
	for (int i = 1; i < (argc - 1); i++) {
		bt = argv[i];
		if (bt->type() != dods_array_c)
			throw Error(malformed_expr,
					"Wrong type for second argument, expected DAP Array. "
							+ UgridRestrictSyntax + "  was passed a/an "
							+ bt->type_name());

		Array *newRangeVar = dynamic_cast<Array*>(bt);
		if (newRangeVar == 0) {
			throw Error(malformed_expr,
					"Wrong type for second argument. " + UgridRestrictSyntax
							+ "  was passed a/an " + bt->type_name());
		}
		args.rangeVars.push_back(newRangeVar);
	} DBG(cerr << "processUgrArgs() - END" << endl);

	return args;

}


/**
 * Retrieves the size of the second dimension from a 3xN array. Throws an
 * Error if the array is not the correct shape.
 */
static int getNfrom3byNArray(Array *array) {

	int dimCount = array->dimensions(true);

	if (dimCount != 2)
		throw Error(
				"Expected a 2 dimensional array. The array '" + array->name()
						+ "' has " + long_to_string(dimCount) + " dimensions.");

	// Check the first dimension to be sure it's size is 3.
	Array::Dim_iter di = array->dim_begin();
	if (di->c_size != 3) {
		string msg =
				"Expected a 2 dimensional array with shape of 3xN! The array "
						+ array->name() + " has a first " + "dimension of size "
						+ long_to_string(di->c_size);
		DBG(cerr << msg << endl);
		throw Error(malformed_expr, msg);
	}

	// Return the size of the second dimension;
	di++;
	return di->c_size;

}

/**
 * Takes a row major 3xN Face node connectivity DAP array
 * and converts it to a collection GF::Nodes organized as
 * 0,N,2N; 1,1+N,1+2N;
 *
 * This is the inverse operation to getGridFieldCellArrayAsDapArray()
 *
 */
static GF::Node *getFncArrayAsGFNodes(Array *fncVar) {

	int nodeCount = getNfrom3byNArray(fncVar);

	// interpret the array data as triangles
	GF::Node *cellids = new GF::Node[fncVar->length()];

	GF::Node *cellids2 = extract_array<GF::Node>(fncVar);

	// Reorganize the cell ids so that cellids contains
	// the cells in three consecutive values (0,1,2; 3,4,5; ...).
	// The the values from  fncVar now in cellids2 and ar organized
	// as 0,N,2N; 1,1+N,1+2N; ...
	for (int j = 0; j < nodeCount; j++) {
		cellids[3 * j] = cellids2[j];
		cellids[3 * j + 1] = cellids2[j + nodeCount];
		cellids[3 * j + 2] = cellids2[j + 2 * nodeCount];
	}
	return cellids;
}

/**
 * Returns the value of the "start_index" attribute for the passed Array. If the start_index
 * is missing the value 0 is returned.
 */
static int getStartIndex(Array *array) {
	AttrTable &at = array->get_attr_table();
	AttrTable::Attr_iter start_index_iter = at.simple_find(_start_index);
	if (start_index_iter != at.attr_end()) {
		DBG(cerr << "getStartIndex() - "<< "Found the "<< _start_index<<" attribute." << endl);
		AttrTable::entry *start_index_entry = *start_index_iter;
		if (start_index_entry->attr->size() == 1) {
			string val = (*start_index_entry->attr)[0];
			DBG(cerr << "getStartIndex() - " << "value: " << val << endl);
			stringstream buffer(val);
			// what happens if string cannot be converted to an integer?
			int start_index;
			;

			buffer >> start_index;
			return start_index;
		} else {
			throw Error(malformed_expr,
					"Index origin attribute exists, but either no value supplied, or more than one value supplied.");
		}
	}
	return 0;
}

/**
 * Converts a row major 3xN Face node connectivity DAP array into a GF::CellArray
 */
static GF::CellArray *getFaceNodeConnectivityCells(
		Array *faceNodeConnectivityArray) {
	DBG(cerr << "getFaceNodeConnectivityCells() - Building face node connectivity Cell " <<
			"array from the Array "<< faceNodeConnectivityArray->name() << endl);

	int rank2CellCount = getNfrom3byNArray(faceNodeConnectivityArray);

	int total_size = 3 * rank2CellCount;

	GF::Node *cellids = getFncArrayAsGFNodes(faceNodeConnectivityArray);

	// adjust for the start_index (cardinal or ordinal array access)
	int startIndex = getStartIndex(faceNodeConnectivityArray);
	if (startIndex != 0) {
		for (int j = 0; j < total_size; j++) {
			cellids[j] -= startIndex;
		}
	}
	// Create the cell array
	// TODO Is this '3' the same as the '3' in '3xN'?
	// If so, then this is where we extend the code for faces with more sides.
	GF::CellArray *rankTwoCells = new GF::CellArray(cellids, rank2CellCount, 3);

	DBG(cerr << "getFaceNodeConnectivityCells() - DONE" << endl);
	return rankTwoCells;

}

/**
 * Get a new 3xN DAP Array of Int32 with the same name, attributes, and dimension names
 * as the templateArray. Make the new array's second dimension size N.
 * Returns a DAP Array with an Int32 type template.
 */
static Array *getNewFcnDapArray(Array *templateArray, int N) {

	// Is the template array a 2D array?
	int dimCount = templateArray->dimensions(true);
	if (dimCount != 2)
		throw Error(
				"Expected a 2 dimensional array. The array '"
						+ templateArray->name() + "' has "
						+ long_to_string(dimCount) + " dimensions.");

	// Is the template array really 3xN?
	Array::Dim_iter di = templateArray->dim_begin();
	if (di->c_size != 3) {
		string msg =
				"Expected a 2 dimensional array with shape of 3xN! The array "
						+ templateArray->name() + " has a first "
						+ "dimension of size " + long_to_string(di->c_size);
		DBG(cerr << msg << endl);
		throw Error(malformed_expr, msg);
	}

	// Get a new template variable for our new array (should be just like the template for the source array)
	//BaseType *arrayTemplate = getDapVariableInstance(templateArray->var(0)->name(),templateArray->var(0)->type());
	Array *newArray = new Array(templateArray->name(),
			new Int32(templateArray->name()));

	//Add the first dimension (size 3 same same as template array's first dimension)
	newArray->append_dim(3, di->name);

	// Add the second dimension to the result array, but use only the name from the template array's
	// second dimension. The size will be from the passed parameter N
	di++;
	newArray->append_dim(N, di->name);

	newArray->set_attr_table(templateArray->get_attr_table());

	// make the new array big enough to hold all the values.
	newArray->reserve_value_capacity(3 * N);

#if 0
	DBG(cerr<<"getNewFcnDapArray() -"<<endl<<endl;
			cerr << "Newly minted Array: "<< endl;
			newArray->print_val(cerr);
			cerr<<endl<<endl;
	)
#endif

	return newArray;

}

/**
 * Takes a GF::GridField, extracts it's rank2 GF::CellArray. The GF::CellArray content is
 * extracted and re-packed into a 3xN DAP Array. This is the inverse operation to
 * getFncArrayAsGFNodes()
 */
static Array *getGridFieldCellArrayAsDapArray(GF::GridField *resultGridField,
		Array *sourceFcnArray) {

	DBG(cerr << "getGridFieldCellArrayAsDapArray() - BEGIN" << endl);

	// Get the rank 2 k-cells from the GridField object.
	GF::CellArray* Inb =
			(GF::CellArray*) (resultGridField->GetGrid()->getKCells(2));

	// This is a vector of size N holding vectors of size 3
	vector<vector<int> > nodes2 = Inb->makeArrayInts();

	Array *resultFcnDapArray = getNewFcnDapArray(sourceFcnArray, nodes2.size());

	// Make a vector to hold the re-packed cell nodes.
	vector<dods_int32> rowMajorNodes;

	// Re-pack the mesh nodes.
	for (unsigned int firstDim = 0; firstDim < 3; firstDim++) {
		for (unsigned int secondDim = 0; secondDim < nodes2.size();
				secondDim++) {
			dods_int32 val = nodes2.at(secondDim).at(firstDim);
			rowMajorNodes.push_back(val);
		}
	}

	DBG(
			cerr << "getGridFieldCellArrayAsDapArray() - rowMajorNodes: " << endl << "{";
			for (unsigned int j=0; j < rowMajorNodes.size(); j++) {
				dods_int32 val = rowMajorNodes.at(j);
				cerr << val << ", ";
			}
			cerr << "}" << endl;
	)

	// Add them to the DAP array.
	resultFcnDapArray->set_value(rowMajorNodes, rowMajorNodes.size());

	DBG(
			cerr << "getGridFieldCellArrayAsDapArray() - DAP Array: "<< endl;
			resultFcnDapArray->print_val(cerr);
	)

	DBG(cerr << "getGridFieldCellArrayAsDapArray() - DONE" << endl);

	return resultFcnDapArray;

}

/**
 * Retrieves a single dimensional rank 0 GF attribute array from a GF::GridField and places the data into
 * DAP array of the appropriate type.
 */
static Array *getRankZeroAttributeNodeSetAsDapArray(
		GF::GridField *resultGridField, Array *sourceArray) {

	DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - BEGIN" << endl);

	// The result variable is assumed to be bound to the GridField with rank 0
	// Try to get the Attribute from rank 0 with the same name as the source array
	DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - Retrieving GF::GridField Attribute '" <<
			sourceArray->name() << "'" << endl);
	GF::Array* gfa = resultGridField->GetAttribute(0, sourceArray->name());

	Array *dapArray;
	BaseType *templateVar = sourceArray->var();
	string dimName;

	switch (templateVar->type()) {
	case dods_byte_c:
	case dods_uint16_c:
	case dods_int16_c:
	case dods_uint32_c:
	case dods_int32_c: {
		// Get the data
		DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - GF::Array was made from some type of int, retrieve it as such." << endl);
		vector<dods_int32> GF_ints = gfa->makeArray();
		// Make a DAP array to put the data into.
		dapArray = new Array(sourceArray->name(), new Int32(sourceArray->name()));
		// Add the dimension
		dimName = sourceArray->dimension_name(sourceArray->dim_begin());
		dapArray->append_dim(GF_ints.size(), dimName);
		// Add the data
		dapArray->set_value(GF_ints, GF_ints.size());
		break;
	}
	case dods_float32_c:
	case dods_float64_c: {
		// Get the data
		DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - GF::Array was made from some type of float, retrieve it as such." << endl);
		vector<dods_float64> GF_floats = gfa->makeArrayf();
		// Make a DAP array to put the data into.
		dapArray = new Array(sourceArray->name(), new Float64(sourceArray->name()));
		// Add the dimension
		dimName = sourceArray->dimension_name(sourceArray->dim_begin());
		dapArray->append_dim(GF_floats.size(), dimName);
		// Add the data
		dapArray->set_value(GF_floats, GF_floats.size());
		break;
	}
	default:
		throw InternalErr(__FILE__, __LINE__,
				"Unknown DAP type encountered when converting to gridfields array");
	}

	// Copy the source objects attributes.
	dapArray->set_attr_table(sourceArray->get_attr_table());

	DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - DONE" << endl);

	return dapArray;
}



/**
 * Builds the DAP response content from the GF::GridField result object.
 */
static vector<BaseType *> *convertResultGridFieldToDapObjects(TwoDMeshTopology *tdmt){

	DBG(cerr << "convertResultGridFieldToDapObject() - BEGIN" << endl);

	vector<BaseType *> *results = new vector<BaseType *>();

	DBG(cerr << "convertResultGridFieldToDapObject() - Normalizing Grid." << endl);
	tdmt->resultGridField->GetGrid()->normalize();

	// Add the coordinate node arrays to the response.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the coordinate node arrays to the response." << endl);
	vector<Array *>::iterator it;
	for (it = tdmt->nodeCoordinateArrays->begin(); it != tdmt->nodeCoordinateArrays->end(); ++it) {
		Array *sourceCoordinateArray = *it;
		Array *resultCoordinateArray = getRankZeroAttributeNodeSetAsDapArray(tdmt->resultGridField, sourceCoordinateArray);
		results->push_back(resultCoordinateArray);
	}

	// Add the range variable data arrays to the response.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the range variable data arrays to the response." << endl);
	vector<MeshDataVariable *>::iterator mdvIt;
	for (mdvIt = tdmt->rangeDataArrays->begin(); mdvIt != tdmt->rangeDataArrays->end(); ++mdvIt) {
		MeshDataVariable *mdv = *mdvIt;
		Array *resultRangeVar = getRankZeroAttributeNodeSetAsDapArray(tdmt->resultGridField, mdv->meshDataVar);
		results->push_back(resultRangeVar);
	}

	// Add the new face node connectivity array - make sure it has the same attributes as the original.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the new face node connectivity array to the response." << endl);
	Array *resultFaceNodeConnectivityDapArray = getGridFieldCellArrayAsDapArray(
			tdmt->resultGridField, tdmt->faceNodeConnectivityArray);
	results->push_back(resultFaceNodeConnectivityDapArray);

	DBG(cerr << "convertResultGridFieldToDapObject() - END" << endl);
	return results;

}

static void release(TwoDMeshTopology *tdmt){

	delete tdmt->rangeDataArrays;

	delete tdmt->gridTopology;
	//delete tdmt->inputGridField;
	delete tdmt->resultGridField;

}

/**
 Subset an irregular mesh (aka unstructured grid).

 @param argc Count of the function's arguments
 @param argv Array of pointers to the functions arguments
 @param dds Reference to the DDS object for the complete dataset.
 This holds pointers to all of the variables and attributes in the
 dataset.
 @param btpp Return the function result in an instance of BaseType
 referenced by this pointer to a pointer. We could have used a
 BaseType reference, instead of pointer to a pointer, but we didn't.
 This is a value-result parameter.

 @return void

 @exception Error Thrown If the Array is not a one dimensional
 array. */
void function_ugr2(int argc, BaseType * argv[], DDS &dds, BaseType **btpp) {
	DBG(cerr << "function_ugr() - BEGIN" << endl);

	static string info = string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
			+ "<function name=\"ugrid_restrict\" version=\"0.1\">\n"
			+ "Server function for Unstructured grid operations.\n" + "usage: "
			+ UgridRestrictSyntax + "\n"
					"</function>";

	if (argc == 0) {
		Str *response = new Str("info");
		response->set_value(info);
		*btpp = response;
		return;
	}

	// Process and QC the arguments
	UgridRestrictArgs args = processUgrArgs(argc, argv);

	// For convenience, cache the pointer to the collection of user selected range variables
	//    Array *rangeVar =  args.rangeVar;
	//vector<Array *> &rangeVars = args.rangeVars;

	//vector<MeshDataVariable *> meshDataVars;
	map<string, TwoDMeshTopology *> meshTopologies;

	int rangeVarCount =0;
	vector<Array *>::iterator it;
	for (it = args.rangeVars.begin(); it != args.rangeVars.end(); ++it) {
		Array *rangeVar = *it;
	    addRangeVar(dds, rangeVar, meshTopologies);
	    rangeVarCount++;
	}

	DBG(cerr << "function_ugr() - The user requested "<< rangeVarCount << " range data variables." << endl);
	DBG(cerr << "function_ugr() - The user's request referenced "<< meshTopologies.size() << " mesh topology variables." << endl);

	// ----------------------------------
	// OK, so up to this point we have not read any data from the data set, but we have QC'd the inputs and verified that
	// it looks like the request is consistent with the semantics of the dataset.
	// Now it's time to read some data and pack it into the GridFields library...

	//Each mesh topology gets it's own GridField
	map<string, TwoDMeshTopology *>::iterator mit;
	for (mit = meshTopologies.begin(); mit != meshTopologies.end(); ++mit) {
		string name = mit->first;
		TwoDMeshTopology *tdmt = mit->second;

		DBG(cerr << "function_ugr() - Building GridFields objects for mesh_topology variable "<< tdmt->myVar->name() << endl);


		// Start building the Grid for the GridField operation.
		// This is, I think essentially a representation of the
		// mesh_topology
		// TODO Is the Grid G Leaked?
		// TODO This is the 'domain' data?
		DBG(cerr << "function_ugr() - Constructing new GF::Grid for "<< name << endl);
		tdmt->gridTopology = new GF::Grid(name);

		// 1) Make the implicit nodes - same size as the range and the coordinate node arrays
		DBG(cerr << "function_ugr() - Building and adding implicit range Nodes to the GF::Grid" << endl);
		GF::AbstractCellArray *nodes = new GF::Implicit0Cells(tdmt->nodeCount);
		// Attach the implicit nodes to the grid at rank 0
		tdmt->gridTopology->setKCells(nodes, 0);

		// Attach the Mesh to the grid.
		// Get the face node connectivity cells (i think these correspond to the GridFields K cells of Rank 2)
		// FIXME Read this array once! It is read again below..
		DBG(cerr << "function_ugr() - Building face node connectivity Cell array from the DAP version" << endl);
		GF::CellArray *faceNodeConnectivityCells = getFaceNodeConnectivityCells(tdmt->faceNodeConnectivityArray);

		// Attach the Mesh to the grid at rank 2
		// TODO Is this 2 the same as the value of the "dimension" attribute in the "mesh_topology" variable?
		DBG(cerr << "function_ugr() - Attaching Cell array to GF::Grid" << endl);
		tdmt->gridTopology->setKCells(faceNodeConnectivityCells, 2);

		// The Grid is complete. Now we make a GridField from the Grid
		DBG(cerr << "function_ugr() - Construct new GF::GridField from GF::Grid" << endl);
		tdmt->inputGridField = new GF::GridField(tdmt->gridTopology);

		// We read and add the coordinate data (using GridField->addAttribute() to the GridField at
		// grid dimension 0 ( key?, rank?? whatever this is)
		DBG(cerr << "function_ugr() - Adding node coordinates to GF::GridField at rank 0" << endl);
		vector<Array *> *nodeCoordinateArrays = tdmt->nodeCoordinateArrays;
		vector<Array *>::iterator ncit;
		for (ncit = nodeCoordinateArrays->begin(); ncit != nodeCoordinateArrays->end(); ++ncit) {
			Array *nca = *ncit;
			GF::Array *gfa = extract_gridfield_array(nca);
			tdmt->inputGridField->AddAttribute(node, gfa);
		}

		// FIXME Read this the array once! It has already been read above.
		// We read and add faceNodeConnectivity data to the grid at rank 2 for face.
		GF::Array *gfa = extract_gridfield_array(tdmt->faceNodeConnectivityArray);
		DBG(cerr << "function_ugr() - Adding face node connectivity Cell array to GF::GridField at rank 2" << endl);
		tdmt->inputGridField->AddAttribute(face, gfa);


		// For each range data variable associated with this MeshTopology read and add the  data to the GridField
		// At the appropriate rank.
		// They are added at Rank 0 because they're nodes, at least for now.
		for (vector<MeshDataVariable *>::iterator mdv_it = tdmt->rangeDataArrays->begin(); mdv_it != tdmt->rangeDataArrays->end(); ++mdv_it) {
			MeshDataVariable *mdVar = *mdv_it;
			GF::Array *gfa = extract_gridfield_array(mdVar->meshDataVar);
			DBG(cerr << "function_ugr() - Adding mesh data variable '"<< mdVar->meshDataVar->name() <<"' to GF::GridField at rank 0" << endl);
			tdmt->inputGridField->AddAttribute(node, gfa);
		}

	}




	Structure *dapResult = new Structure("ugr_result");
	try {

		for (mit = meshTopologies.begin(); mit != meshTopologies.end(); ++mit) {
			string meshTopologyName = mit->first;
			TwoDMeshTopology *tdmt = mit->second;

			// Build the restriction operator;
			DBG(cerr << "function_ugr() - Constructing new GF::RestrictOp using user "<<
					"supplied dimension value and filter expression combined with the GF:GridField " << endl);
			GF::RestrictOp op = GF::RestrictOp(args.filterExpression, args.dimension, tdmt->inputGridField);

			// Apply the operator and get the result;
			DBG(cerr << "function_ugr() - Applying GridField operator." << endl);
			tdmt->resultGridField = new GF::GridField(op.getResult());

			DBG(cerr << "function_ugr() - Adding mesh_topology variable '"<< tdmt->myVar->name() << "' to the DAP response." << endl);
			dapResult->add_var(tdmt->myVar);

			// Get the GridField back in a DAP representation of a ugrid.
			// TODO This returns a single structure but it would make better sense to the
			// world if it could return a vector of objects and have them appear at the
			// top level of the DDS.
			// FIXME Because the metadata attributes hold the key to understanding the response we
			// need to allow the user to request DAS and DDX for the function call.
			DBG(cerr << "function_ugr() - Converting result GF:GridField to DAP data structure.." << endl);
			vector<BaseType *> *dapResults = convertResultGridFieldToDapObjects(tdmt);

			// FIXME fix the names of the variables in the mesh_topology attributes
			// If the server side function can be made to return a DDS or a collection of BaseType's then the
			// names won't change and the original mesh_topology variable and it's metadata will be valid
			for (vector<BaseType *>::iterator btIt=dapResults->begin(); btIt != dapResults->end(); ++btIt) {
				BaseType *bt = *btIt;
				dapResult->add_var_nocopy(bt);
			}
		}
	} catch (std::bad_alloc &e) {
		throw Error(
				"Unable to construct GF::RestrictOp. Bad Allocation Exception. std::bad_alloc.where(): '"
						+ string(e.what()) + "'");
	}



	*btpp = dapResult;


	for (mit = meshTopologies.begin(); mit != meshTopologies.end(); ++mit) {
		string meshTopologyName = mit->first;
		TwoDMeshTopology *tdmt = mit->second;

		release(tdmt);
	}


	DBG(cerr << "function_ugr() - END" << endl);

	return;
}


} // namespace libdap
//#endif
