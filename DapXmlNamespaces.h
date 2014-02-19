/*
 * DapXmlNamspaces.h
 *
 *  Created on: Feb 19, 2014
 *      Author: ndp
 */

#ifndef DAPXMLNAMSPACES_H_
#define DAPXMLNAMSPACES_H_


namespace libdap {

enum DAPVersion { DAP_2_0, DAP_3_2, DAP_4_0 };

class DapXmlNamspaces {
private:

	 string c_xml_xsi;

	 string c_xml_namespace;

	 string grddl_transformation_dap32;

	 string c_default_dap20_schema_location;
	 string c_default_dap32_schema_location;
	 string c_default_dap40_schema_location;

	 string c_dap20_namespace;
	 string c_dap32_namespace;
	 string c_dap40_namespace;

public:

	DapXmlNamspaces(){
		 c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
		 c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

		 grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

		 c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
		 c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
		 c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

		 c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
		 c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
		 c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";

	};
	virtual ~DapXmlNamspaces(){};

	/**
	 * Returns the XML Schema-instance namespace string.
	 */
	string getXmlXsiNamespace(){
		return c_xml_xsi;
	}

	/**
	 * Returns the XML  namespace string.
	 */
	string getXmlNamespace(){
		return c_xml_namespace;
	}

	/**
	 * Returns the GRDDL transformation URL for the passed DAP version.
	 */
	string getGrddlTransformation(DAPVersion version){
		switch(version) {
		case DAP_2_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 2.0");
			break;

		case DAP_3_2:
			return grddl_transformation_dap32;
			break;

		case DAP_4_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 4.0");
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
		return 0;
	}



	/**
	 * Returns the DAP XML namespace string for the passed DAP version.
	 */
	string getDapNamespaceString(DAPVersion version){
		switch(version) {
		case DAP_2_0:
			return c_dap20_namespace;
			break;

		case DAP_3_2:
			return c_dap32_namespace;
			break;

		case DAP_4_0:
			return c_dap40_namespace;
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
		return 0;
	}

	/**
	 * Returns the schema location URI string for the passed DAP version.
	 */
	string getSchemaLocationString(DAPVersion version){
		switch(version) {
		case DAP_2_0:
			return c_default_dap20_schema_location;
			break;

		case DAP_3_2:
			return c_default_dap32_schema_location;
			break;

		case DAP_4_0:
			return c_default_dap40_schema_location;
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getSchemaLocationString() - Unrecognized namespace version.");
			break;
		}
		return 0;
	}



	/**
	 * Returns the schema location declaration (the namespace string followed by a
	 * space followed by the schema location string) for the passed DAP version.
	 */
	string getSchemaLocationDeclarationString(DAPVersion version){
		switch(version) {
		case DAP_2_0:
			return c_dap20_namespace + " " + c_default_dap20_schema_location;
			break;

		case DAP_3_2:
			return c_dap32_namespace + " " + c_default_dap32_schema_location;
			break;

		case DAP_4_0:
			return c_dap40_namespace + " " + c_default_dap40_schema_location;
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getSchemaLocationDeclarationString() - Unrecognized namespace version.");
			break;
		}
		return 0;
	}

};

} /* namespace libdap */
#endif /* DAPXMLNAMSPACES_H_ */
