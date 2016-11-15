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
	 /*

	 static string c_xml_xsi;

	 static string c_xml_namespace;

	 static string grddl_transformation_dap32;

	 static string c_default_dap20_schema_location;
	 static string c_default_dap32_schema_location;
	 static string c_default_dap40_schema_location;

	 static string c_dap20_namespace;
	 static string c_dap32_namespace;
	 static string c_dap40_namespace;

	 static string c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
	 static string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

	 static string grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

	 static string c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
	 static string c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
	 static string c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

	 static string c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
	 static string c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
	 static string c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";
*/

public:

	DapXmlNamspaces(){
		/*
		 c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
		 c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

		 grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

		 c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
		 c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
		 c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

		 c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
		 c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
		 c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";
*/
	};
	virtual ~DapXmlNamspaces(){};

	/**
	 * Returns the XML Schema-instance namespace string.
	 */
	static string getXmlXsiNamespace(){
		string c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
		return c_xml_xsi;
	}

	/**
	 * Returns the XML  namespace string.
	 */
	static string getXmlNamespace(){
		string c_xml_namespace = "http://www.w3.org/XML/1998/namespace";
		return c_xml_namespace;
	}

	/**
	 * Returns the GRDDL transformation URL for the passed DAP version.
	 */
	static string getGrddlTransformation(DAPVersion version){

		string grddl_transformation_string = 0;

		switch(version) {
		case DAP_2_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 2.0");
			break;

		case DAP_3_2:
			grddl_transformation_string = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";
			break;

		case DAP_4_0:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - GRDDL Transformation undefined for DAP 4.0");
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
		return grddl_transformation_string;
	}




	/**
	 * Returns the DAP XML namespace string for the passed DAP version.
	 */
	static string getDapNamespaceString(DAPVersion version){
		string dap_namespace_string = 0;
		switch(version) {
		case DAP_2_0:
			dap_namespace_string = "http://xml.opendap.org/ns/DAP2";
			break;

		case DAP_3_2:
			dap_namespace_string = "http://xml.opendap.org/ns/DAP/3.2#";
			break;

		case DAP_4_0:
			dap_namespace_string = "http://xml.opendap.org/ns/DAP/4.0#";
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getDapXmlNamespaceString() - Unrecognized namespace version.");
			break;
		}
		return dap_namespace_string;
	}

	/**
	 * Returns the schema location URI string for the passed DAP version.
	 */
	static string getSchemaLocationString(DAPVersion version){
		string dap_schema_location_string = 0;

		switch(version) {
		case DAP_2_0:
			dap_schema_location_string = "http://xml.opendap.org/dap/dap2.xsd";
			break;

		case DAP_3_2:
			dap_schema_location_string = "http://xml.opendap.org/dap/dap3.2.xsd";
			break;

		case DAP_4_0:
			dap_schema_location_string = "http://xml.opendap.org/dap/dap4.0.xsd";
			break;

		default:
			throw InternalErr(__FILE__, __LINE__, "DapXmlNamspaces::getSchemaLocationString() - Unrecognized namespace version.");
			break;
		}
		return dap_schema_location_string;
	}



	/**
	 * Returns the schema location declaration (the namespace string followed by a
	 * space followed by the schema location string) for the passed DAP version.
	 */
	static string getSchemaLocationDeclarationString(DAPVersion version){
		return getDapNamespaceString(version) + " " + getSchemaLocationString(version);
	}

};

/*
string DapXmlNamspaces::c_xml_xsi = "http://www.w3.org/2001/XMLSchema-instance";
string DapXmlNamspaces::c_xml_namespace = "http://www.w3.org/XML/1998/namespace";

string DapXmlNamspaces::grddl_transformation_dap32 = "http://xml.opendap.org/transforms/ddxToRdfTriples.xsl";

string DapXmlNamspaces::c_default_dap20_schema_location = "http://xml.opendap.org/dap/dap2.xsd";
string DapXmlNamspaces::c_default_dap32_schema_location = "http://xml.opendap.org/dap/dap3.2.xsd";
string DapXmlNamspaces::c_default_dap40_schema_location = "http://xml.opendap.org/dap/dap4.0.xsd";

string DapXmlNamspaces::c_dap20_namespace = "http://xml.opendap.org/ns/DAP2";
string DapXmlNamspaces::c_dap32_namespace = "http://xml.opendap.org/ns/DAP/3.2#";
string DapXmlNamspaces::c_dap40_namespace = "http://xml.opendap.org/ns/DAP/4.0#";
*/

} /* namespace libdap */
#endif /* DAPXMLNAMSPACES_H_ */
