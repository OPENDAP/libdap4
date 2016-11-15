// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2017 OPeNDAP, Inc.
// Author: Nathan Potter <ndp@opendap.org>
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

#ifndef DAPXMLNAMSPACES_H_
#define DAPXMLNAMSPACES_H_

#include <string>

namespace libdap {

enum DAPVersion { DAP_2_0, DAP_3_2, DAP_4_0 };

class DapXmlNamspaces {

public:

	virtual ~DapXmlNamspaces() {};

	/**
	 * Returns the XML Schema-instance namespace string.
	 */
	static std::string getXmlXsiNamespace(){
		return "http://www.w3.org/2001/XMLSchema-instance";
	}

	/**
	 * Returns the XML  namespace string.
	 */
	static std::string getXmlNamespace(){
		return "http://www.w3.org/XML/1998/namespace";
	}

	/**
	 * Returns the GRDDL transformation URL for the passed DAP version.
	 */
	static std::string getGrddlTransformation(DAPVersion version){

		switch(version) {
		case DAP_2_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 2.0");
			break;

		case DAP_3_2:
			return "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";
			break;

		case DAP_4_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 4.0");
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
	}




	/**
	 * Returns the DAP XML namespace string for the passed DAP version.
	 */
	static std::string getDapNamespaceString(DAPVersion version){

		switch(version) {
		case DAP_2_0:
			return "http://xml.opendap.org/ns/DAP2";
			break;

		case DAP_3_2:
			return "http://xml.opendap.org/ns/DAP/3.2#";
			break;

		case DAP_4_0:
			return "http://xml.opendap.org/ns/DAP/4.0#";
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
	}

	/**
	 * Returns the schema location URI string for the passed DAP version.
	 */
	static std::string getSchemaLocationString(DAPVersion version){

		switch(version) {
		case DAP_2_0:
			return "http://xml.opendap.org/dap/dap2.xsd";
			break;

		case DAP_3_2:
			return "http://xml.opendap.org/dap/dap3.2.xsd";
			break;

		case DAP_4_0:
			return "http://xml.opendap.org/dap/dap4.0.xsd";
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getSchemaLocationString() - Unrecognized namespace version.");
			break;
		}
	}



	/**
	 * Returns the schema location declaration (the namespace string followed by a
	 * space followed by the schema location string) for the passed DAP version.
	 */
	static std::string getSchemaLocationDeclarationString(DAPVersion version){
		return getDapNamespaceString(version).append(" ").append(getSchemaLocationString(version));
	}

};


} /* namespace libdap */
#endif /* DAPXMLNAMSPACES_H_ */
