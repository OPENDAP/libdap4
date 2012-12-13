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

#include <sstream>
#include <vector>

#include <gridfields/type.h>
#include <gridfields/gridfield.h>
#include <gridfields/grid.h>
#include <gridfields/implicit0cells.h>
#include <gridfields/gridfieldoperator.h>
#include <gridfields/restrict.h>
#include <gridfields/array.h>


#include "BaseType.h"
#include "Int32.h"
#include "Float64.h"
#include "Array.h"
#include "util.h"


#include "ugrid_utils.h"
#include "MeshDataVariable.h"
#include "TwoDMeshTopology.h"

#include "debug.h"

using namespace std;
using namespace libdap;

namespace libdap {

TwoDMeshTopology::TwoDMeshTopology():myVar(0),gridTopology(0),inputGridField(0),resultGridField(0),sharedNodeArray(0)
{
	rangeDataArrays = new vector<MeshDataVariable *>();
	sharedIntArrays = new vector<int *>();
	sharedFloatArrays = new vector<float *>();
}

TwoDMeshTopology::~TwoDMeshTopology()
{
    DBG(cerr << "Entering ~TwoDMeshTopology (" << this << ")" << endl);

	DBG(cerr << "~TwoDMeshTopology() - Deleting resultGridField." << endl);
	delete resultGridField;

	DBG(cerr << "~TwoDMeshTopology() - Deleting inputGridField." << endl);
	delete inputGridField;

	DBG(cerr << "releaseTDMT() - Deleting gridTopology." << endl);
	delete gridTopology;

	DBG(cerr << "~TwoDMeshTopology() - Deleting sharedIntArrays..." << endl);
	for (vector<int *>::iterator it = sharedIntArrays->begin(); it != sharedIntArrays->end(); ++it) {
		int *i = *it;
		DBG(cerr << "~TwoDMeshTopology() - Deleting int array '" << i << "'" << endl);
		delete [] i;
	}
	delete sharedIntArrays;

	DBG(cerr << "~TwoDMeshTopology() - Deleting sharedFloatArrays..." << endl);
	for (vector<float *>::iterator it = sharedFloatArrays->begin(); it != sharedFloatArrays->end(); ++it) {
		float *f = *it;
		DBG(cerr << "~TwoDMeshTopology() - Deleting float array '" << f << "'" << endl);
		delete [] f;
	}
	delete sharedFloatArrays;

	DBG(cerr << "~TwoDMeshTopology() - Deleting MeshDataVariables..." << endl);
	vector<MeshDataVariable *>::iterator mdvIt;
	for (mdvIt = rangeDataArrays->begin(); mdvIt != rangeDataArrays->end(); ++mdvIt) {
		MeshDataVariable *mdv = *mdvIt;
		DBG(cerr << "~TwoDMeshTopology() - Deleting MeshDataVariable '"<< mdv->meshDataVar->name()<< "'" << endl);
		delete mdv;
	}
	delete rangeDataArrays;

	DBG(cerr << "~TwoDMeshTopology() - Deleting GF::Node array." << endl);
	delete sharedNodeArray;


    DBG(cerr << "Exiting ~TwoDMeshTopology" << endl);
}

void TwoDMeshTopology::init(string meshVarName, DDS &dds)
{

	myVar = dds.var(meshVarName);

	dimension = getAttributeValue(myVar,UGRID_DIMENSION);

	// Retrieve the node coordinate arrays for the mesh
	nodeCoordinateArrays = getNodeCoordinateArrays(myVar, dds);

	// Retrieve the face node connectivity array for the mesh
	faceNodeConnectivityArray = getFaceNodeConnectivityArray(myVar,dds);

	nodeCount = (*nodeCoordinateArrays)[0]->length();


}

void TwoDMeshTopology::addDataVariable(MeshDataVariable *mdv)
{

	Array *dapArray = mdv->getDapArray();

	// Make sure that the requested range variable is the same shape as the node coordinate arrays
	// We only need to test the first nodeCoordinate array against the first rangeVar array
	// because we have already made sure all of the node coordinate arrays are the same size and
	// that all the rangeVar arrays are the same size. This is just to compare the two collections.
	Array *firstCoordinate = (*(nodeCoordinateArrays))[0];
	if (!same_dimensions(dapArray, firstCoordinate))
		throw Error(
				"The dimensions of the requested range variable "
						+ mdv->getName() + " does not match the shape "
						+ " of the node coordinate array "
						+ firstCoordinate->name());


	rangeDataArrays->push_back(mdv);
}






/**
 * Locates the the DAP variable identified by the face_node_connectivity attribute of the
 * meshTopology variable.
 */
Array *TwoDMeshTopology::getFaceNodeConnectivityArray(BaseType *meshTopology, DDS &dds)
{

    DBG(cerr << "getFaceNodeConnectivityArray() - "  << "Locating FNCA" << endl);

	string face_node_connectivity_var_name;
    AttrTable at = meshTopology->get_attr_table();

    AttrTable::Attr_iter iter_fnc = at.simple_find(UGRID_FACE_NODE_CONNECTIVITY);
    if (iter_fnc != at.attr_end()) {
    	face_node_connectivity_var_name = at.get_attr(iter_fnc, 0);
    }
    else {
    	throw Error("Could not locate the "  UGRID_FACE_NODE_CONNECTIVITY  " attribute in the "   UGRID_MESH_TOPOLOGY   " variable! "
    			"The mesh_topology variable is named " + meshTopology->name());
    }

	// Find the variable using the name

    BaseType *btp = dds.var(face_node_connectivity_var_name);

    if(btp==0)
    	throw Error("Could not locate the " UGRID_FACE_NODE_CONNECTIVITY " variable named '" + face_node_connectivity_var_name + "'! "+
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
 * Returns the coordinate variables identified in the meshTopology variable's node_coordinates attribute.
 * throws an error if the node_coordinates attribute is missing, if the coordinates are not arrays, and
 * if the arrays are not all the same shape.
 */
vector<Array *> *TwoDMeshTopology::getNodeCoordinateArrays(BaseType *meshTopology, DDS &dds)
{
	DBG(cerr << "getNodeCoordinatesArrays() - " << "BEGIN Gathering node coordinate arrays..." << endl);

	string node_coordinates;
	AttrTable at = meshTopology->get_attr_table();

	AttrTable::Attr_iter iter_nodeCoors = at.simple_find(UGRID_NODE_COORDINATES);
	if (iter_nodeCoors != at.attr_end()) {
		node_coordinates = at.get_attr(iter_nodeCoors, 0);
	} else {
		throw Error(
				"Could not locate the " UGRID_NODE_COORDINATES " attribute in the " UGRID_MESH_TOPOLOGY
				" variable! The mesh_topology variable is named " + meshTopology->name());
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
					"Could not locate the "  UGRID_NODE_COORDINATES  " variable named '"
					        + nodeCoordinateName + "'! "
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

void TwoDMeshTopology::buildGridFieldsTopology()
{

	DBG(cerr << "buildGridFieldsTopology() - Building GridFields objects for mesh_topology variable "<< tdmt->myVar->name() << endl);
	// Start building the Grid for the GridField operation.
	// This is, I think essentially a representation of the
	// mesh_topology
	DBG(cerr << "buildGridFieldsTopology() - Constructing new GF::Grid for "<< name << endl);
	gridTopology = new GF::Grid(name());

	// 1) Make the implicit nodes - same size as the range and the coordinate node arrays
	DBG(cerr << "buildGridFieldsTopology() - Building and adding implicit range Nodes to the GF::Grid" << endl);
	GF::AbstractCellArray *nodes = new GF::Implicit0Cells(nodeCount);
	// Attach the implicit nodes to the grid at rank 0
	gridTopology->setKCells(nodes, node);

	// Attach the Mesh to the grid.
	// Get the face node connectivity cells (i think these correspond to the GridFields K cells of Rank 2)
	// FIXME Read this array once! It is read again below..
	DBG(cerr << "buildGridFieldsTopology() - Building face node connectivity Cell array from the DAP version" << endl);
	GF::CellArray *faceNodeConnectivityCells = getFaceNodeConnectivityCells();

	// Attach the Mesh to the grid at rank 2
	// TODO Is this 2 the same as the value of the "dimension" attribute in the "mesh_topology" variable?
	// This 2 stands for rank 2, or faces.
	DBG(cerr << "buildGridFieldsTopology() - Attaching Cell array to GF::Grid" << endl);
	gridTopology->setKCells(faceNodeConnectivityCells, face);

	// The Grid is complete. Now we make a GridField from the Grid
	DBG(cerr << "buildGridFieldsTopology() - Construct new GF::GridField from GF::Grid" << endl);
	inputGridField = new GF::GridField(gridTopology);
	// TODO Question for Bill: Can we delete the GF::Grid (tdmt->gridTopology) here?

	// We read and add the coordinate data (using GridField->addAttribute() to the GridField at
	// grid dimension/rank/dimension 0 (a.k.a. node)
	vector<Array *>::iterator ncit;
	for (ncit = nodeCoordinateArrays->begin(); ncit != nodeCoordinateArrays->end(); ++ncit) {
		Array *nca = *ncit;
		DBG(cerr << "buildGridFieldsTopology() - Adding node coordinate "<< nca->name() << " to GF::GridField at rank 0" << endl);
		GF::Array *gfa = extractGridFieldArray(nca,sharedIntArrays,sharedFloatArrays);
		inputGridField->AddAttribute(node, gfa);
	}

	// For each range data variable associated with this MeshTopology read and add the  data to the GridField
	// At the appropriate rank.
	// They are added at Rank 0 because they're nodes, at least for now.
	for (vector<MeshDataVariable *>::iterator mdv_it = rangeDataArrays->begin(); mdv_it != rangeDataArrays->end(); ++mdv_it) {
		MeshDataVariable *mdVar = *mdv_it;
		GF::Array *gfa = extractGridFieldArray(mdVar->getDapArray(),sharedIntArrays,sharedFloatArrays);
		DBG(cerr << "buildGridFieldsTopology() - Adding mesh data variable '"<< mdVar->name() <<"' to GF::GridField at rank 0" << endl);
		inputGridField->AddAttribute(node, gfa);
	}

}



/**
 * Takes a row major 3xN Face node connectivity DAP array
 * and converts it to a collection GF::Nodes organized as
 * 0,N,2N; 1,1+N,1+2N;
 *
 * This is the inverse operation to getGridFieldCellArrayAsDapArray()
 *
 *FIXME Make this use less memory. Certainly consider reading the values directly from
 *FIXME the DAP array (after it's read method has been called)
 */
GF::Node *TwoDMeshTopology::getFncArrayAsGFNodes(Array *fncVar)
{

	DBG(cerr << "getFncArrayAsGFNodes() - BEGIN" << endl);

	int N = getNfrom3byNArray(fncVar);

	// interpret the array data as triangles
	GF::Node *cellids = new GF::Node[fncVar->length()];

	DBG(cerr << "getFncArrayAsGFNodes() - Reading DAP data into GF::Node array." << endl);
	GF::Node *cellids2 = extract_array<GF::Node>(fncVar);

	// Reorganize the cell ids so that cellids contains
	// the cells in three consecutive values (0,1,2; 3,4,5; ...).
	// The the values from  fncVar now in cellids2 and ar organized
	// as 0,N,2N; 1,1+N,1+2N; ...
	DBG(cerr << "getFncArrayAsGFNodes() - Re-packing and copying GF::Node array to result." << endl);
	for (int j = 0; j < N; j++) {
		cellids[3 * j] = cellids2[j];
		cellids[3 * j + 1] = cellids2[j + N];
		cellids[3 * j + 2] = cellids2[j + 2 * N];
	}


	DBG(cerr << "getFncArrayAsGFNodes() - Deleting intermediate GF::Node array." << endl);
	delete [] cellids2;

	DBG(cerr << "getFncArrayAsGFNodes() - DONE" << endl);

	return cellids;
}

/**
 * Returns the value of the "start_index" attribute for the passed Array. If the start_index
 * is missing the value 0 is returned.
 */
int TwoDMeshTopology::getStartIndex(Array *array)
{
	AttrTable &at = array->get_attr_table();
	AttrTable::Attr_iter start_index_iter = at.simple_find(UGRID_START_INDEX);
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
GF::CellArray *TwoDMeshTopology::getFaceNodeConnectivityCells()
{
	DBG(cerr << "getFaceNodeConnectivityCells() - Building face node connectivity Cell " <<
			"array from the Array "<< faceNodeConnectivityArray->name() << endl);

	int rank2CellCount = getNfrom3byNArray(faceNodeConnectivityArray);

	int total_size = 3 * rank2CellCount;

	DBG(cerr << "getFaceNodeConnectivityCells() - Retrieving shared GF::Node array 'cellids' "<< cellids << endl);
	sharedNodeArray = getFncArrayAsGFNodes(faceNodeConnectivityArray);


	// adjust for the start_index (cardinal or ordinal array access)
	int startIndex = getStartIndex(faceNodeConnectivityArray);
	if (startIndex != 0) {
		DBG(cerr << "getFaceNodeConnectivityCells() - Applying startIndex to GF::Node array 'cellids'." << endl);
		for (int j = 0; j < total_size; j++) {
			sharedNodeArray[j] -= startIndex;
		}
	}
	// Create the cell array
	// Is this '3' the same as the '3' in '3xN'? YES! The 3 here is the number of nodes per cell (aka face)
	// This is where we extend the code for faces with more vertices (nodes).
	GF::CellArray *rankTwoCells = new GF::CellArray(sharedNodeArray, rank2CellCount, 3);


	DBG(cerr << "getFaceNodeConnectivityCells() - DONE" << endl);
	return rankTwoCells;

}



void TwoDMeshTopology::applyRestrictOperator(locationType loc, string filterExpression)
{
	// Build the restriction operator;
	DBG(cerr << "applyOperator() - Constructing new GF::RestrictOp using user "<<
			"supplied 'dimension' value and filter expression combined with the GF:GridField " << endl);
	GF::RestrictOp op = GF::RestrictOp(filterExpression, loc, inputGridField);

	// Apply the operator and get the result;
	DBG(cerr << "applyOperator() - Applying GridField operator." << endl);
	GF::GridField *resultGF = op.getResult();
	resultGridField = resultGF;
}




/**
 * Builds the DAP response content from the GF::GridField result object.
 */
vector<BaseType *> *TwoDMeshTopology::convertResultGridFieldToDapObjects()
{

	DBG(cerr << "convertResultGridFieldToDapObject() - BEGIN" << endl);

	vector<BaseType *> *results = new vector<BaseType *>();

	DBG(cerr << "convertResultGridFieldToDapObject() - Normalizing Grid." << endl);
	resultGridField->GetGrid()->normalize();

	// Add the coordinate node arrays to the response.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the coordinate node arrays to the response." << endl);
	vector<Array *>::iterator it;
	for (it = nodeCoordinateArrays->begin(); it != nodeCoordinateArrays->end(); ++it) {
		Array *sourceCoordinateArray = *it;
		Array *resultCoordinateArray = getRankZeroAttributeNodeSetAsDapArray(resultGridField, sourceCoordinateArray);
		results->push_back(resultCoordinateArray);
	}

	// Add the range variable data arrays to the response.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the range variable data arrays to the response." << endl);
	vector<MeshDataVariable *>::iterator mdvIt;
	for (mdvIt = rangeDataArrays->begin(); mdvIt != rangeDataArrays->end(); ++mdvIt) {
		MeshDataVariable *mdv = *mdvIt;
		Array *resultRangeVar = getRankZeroAttributeNodeSetAsDapArray(resultGridField, mdv->getDapArray());
		results->push_back(resultRangeVar);
	}

	// Add the new face node connectivity array - make sure it has the same attributes as the original.
	DBG(cerr << "convertResultGridFieldToDapObject() - Adding the new face node connectivity array to the response." << endl);
	Array *resultFaceNodeConnectivityDapArray = getGridFieldCellArrayAsDapArray(resultGridField, faceNodeConnectivityArray);
	results->push_back(resultFaceNodeConnectivityDapArray);

	DBG(cerr << "convertResultGridFieldToDapObject() - END" << endl);
	return results;

}




/**
 * Get a new 3xN DAP Array of Int32 with the same name, attributes, and dimension names
 * as the templateArray. Make the new array's second dimension size N.
 * Returns a DAP Array with an Int32 type template.
 */
Array *TwoDMeshTopology::getNewFcnDapArray(Array *templateArray, int N)
{

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
Array *TwoDMeshTopology::getGridFieldCellArrayAsDapArray(GF::GridField *resultGridField, Array *sourceFcnArray)
{

	DBG(cerr << "getGridFieldCellArrayAsDapArray() - BEGIN" << endl);

	// Get the rank 2 k-cells from the GridField object.
	GF::CellArray* gfCellArray = (GF::CellArray*) (resultGridField->GetGrid()->getKCells(2));

	// This is a vector of size N holding vectors of size 3
	vector<vector<int> > nodes2 = gfCellArray->makeArrayInts();

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

#if 0
	DBG(
		cerr << "getGridFieldCellArrayAsDapArray() - rowMajorNodes: " << endl << "{";
		for (unsigned int j=0; j < rowMajorNodes.size(); j++) {
			dods_int32 val = rowMajorNodes.at(j);
			cerr << val << ", ";
		}
		cerr << "}" << endl;
	)
#endif

	// Add them to the DAP array.
	resultFcnDapArray->set_value(rowMajorNodes, rowMajorNodes.size());

#if 0
	DBG(
			cerr << "getGridFieldCellArrayAsDapArray() - DAP Array: "<< endl;
			resultFcnDapArray->print_val(cerr);
	)
#endif
	DBG(cerr << "getGridFieldCellArrayAsDapArray() - DONE" << endl);

	return resultFcnDapArray;

}

/**
 * Retrieves a single dimensional rank 0 GF attribute array from a GF::GridField and places the data into
 * DAP array of the appropriate type.
 */
Array *TwoDMeshTopology::getRankZeroAttributeNodeSetAsDapArray(GF::GridField *resultGridField, Array *sourceArray)
{

	DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - BEGIN" << endl);

	// The result variable is assumed to be bound to the GridField with rank 0
	// Try to get the Attribute from rank 0 with the same name as the source array
	DBG(cerr << "getRankZeroAttributeNodeSetAsDapArray() - Retrieving GF::GridField Attribute '" <<
			sourceArray->name() << "'" << endl);
	GF::Array* gfa = resultGridField->GetAttribute(node, sourceArray->name());

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





} // namespace libdap
