/*
 * D4AsyncUtil.cc
 *
 *  Created on: Feb 18, 2014
 *      Author: ndp
 */



#include "config.h"

#include <sstream>

#include "XMLWriter.h"

#include "Error.h"
#include "InternalErr.h"
#include "util.h"

#include "D4AsyncUtil.h"
#include "DapXmlNamespaces.h"


namespace libdap {

const string D4AsyncUtil::STYLESHEET_REFERENCE_KEY = "DAP.Async.StyleSheet.Ref";


D4AsyncUtil::D4AsyncUtil()  {}

D4AsyncUtil::~D4AsyncUtil() {}




/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD4AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime, string *stylesheet_ref) {

	// ------ AsynchronousResponse Element and Attributes - BEGIN

	/*
	int	xmlTextWriterWriteAttributeNS	(xmlTextWriterPtr writer,
						 const xmlChar * prefix,
						 const xmlChar * name,
						 const xmlChar * namespaceURI,
						 const xmlChar * content)
	*/

	if(stylesheet_ref){
		string href = "href='" + *stylesheet_ref +"'";
		if(xmlTextWriterStartPI(xml.get_writer(), (const xmlChar*) "xml-stylesheet") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not start XML Processing Instruction.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) "type='text/xsl'") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) " ") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) href.c_str()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterEndPI(xml.get_writer()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not Close XML Processing Instruction.");
	}

	DapXmlNamspaces dapns;
	if (xmlTextWriterStartElementNS(xml.get_writer(),
			(const xmlChar*)"dap",
			(const xmlChar*) "AsynchronousResponse",
			(const xmlChar*) dapns.getDapNamespaceString(DAP_4_0).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write AsynchronousResponse element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "status", (const xmlChar *) "required") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");


	// ------ expectedDelay Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:expectedDelay") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write expectedDelay element");
	ostringstream oss;
	oss << expectedDelay;
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "seconds", (const xmlChar*) oss.str().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end expectedDelay element");
	// ------ expectedDelay Element and Attributes - END


	// ------ responseLifetime Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:responseLifetime") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write expectedDelay element");
	ostringstream oss2;
	oss2 << responseLifetime;
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "seconds", (const xmlChar*) oss2.str().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'seconds'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end responseLifetime element");
	// ------ responseLifetime Element and Attributes - END


	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end AsynchronousResponse element");
	// ------ AsynchronousResponse Element and Attributes - END
}


/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD4AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl, string *stylesheet_ref)  {

	// ------ AsynchronousResponse Element and Attributes - BEGIN
	DapXmlNamspaces dapns;

	if(stylesheet_ref){
		string href = "href='" + *stylesheet_ref +"'";
		if(xmlTextWriterStartPI(xml.get_writer(), (const xmlChar*) "xml-stylesheet") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not start XML Processing Instruction.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) "type='text/xsl'") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) " ") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) href.c_str()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterEndPI(xml.get_writer()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not Close XML Processing Instruction.");
	}

	if (xmlTextWriterStartElementNS(xml.get_writer(),
			(const xmlChar*)"dap",
			(const xmlChar*) "AsynchronousResponse",
			(const xmlChar*) dapns.getDapNamespaceString(DAP_4_0).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write AsynchronousResponse element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "status", (const xmlChar *) "accepted") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");


	// ------ expectedDelay Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:expectedDelay") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write expectedDelay element");
	ostringstream oss;
	oss << expectedDelay;
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "seconds", (const xmlChar*) oss.str().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'seconds'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end expectedDelay element");
	// ------ expectedDelay Element and Attributes - END


	// ------ responseLifetime Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:responseLifetime") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write expectedDelay element");
	ostringstream oss2;
	oss2 << responseLifetime;
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "seconds", (const xmlChar*) oss2.str().c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'seconds'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end responseLifetime element");
	// ------ responseLifetime Element and Attributes - END


	// ------ link Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:link") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write expectedDelay element");

	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "href", (const xmlChar*) asyncResourceUrl.c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'href'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end link element");
	// ------ link Element and Attributes - END


	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end AsynchronousResponse element");
	// ------ AsynchronousResponse Element and Attributes - END
}

/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD4AsyncPending(XMLWriter &xml, string *stylesheet_ref)  {

	// ------ AsynchronousResponse Element and Attributes - BEGIN
	DapXmlNamspaces dapns;


	if(stylesheet_ref){
		string href = "href='" + *stylesheet_ref +"'";
		if(xmlTextWriterStartPI(xml.get_writer(), (const xmlChar*) "xml-stylesheet") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not start XML Processing Instruction.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) "type='text/xsl'") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) " ") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) href.c_str()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterEndPI(xml.get_writer()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not Close XML Processing Instruction.");
	}

	if (xmlTextWriterStartElementNS(xml.get_writer(),
			(const xmlChar*)"dap",
			(const xmlChar*) "AsynchronousResponse",
			(const xmlChar*) dapns.getDapNamespaceString(DAP_4_0).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write AsynchronousResponse element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "status", (const xmlChar *) "pending") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end AsynchronousResponse element");
	// ------ AsynchronousResponse Element and Attributes - END
}


/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD4AsyncResponseGone(XMLWriter &xml, string *stylesheet_ref)  {

	// ------ AsynchronousResponse Element and Attributes - BEGIN
	DapXmlNamspaces dapns;


	if(stylesheet_ref){
		string href = "href='" + *stylesheet_ref +"'";
		if(xmlTextWriterStartPI(xml.get_writer(), (const xmlChar*) "xml-stylesheet") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not start XML Processing Instruction.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) "type='text/xsl'") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) " ") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) href.c_str()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterEndPI(xml.get_writer()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not Close XML Processing Instruction.");
	}

	if (xmlTextWriterStartElementNS(xml.get_writer(),
			(const xmlChar*)"dap",
			(const xmlChar*) "AsynchronousResponse",
			(const xmlChar*) dapns.getDapNamespaceString(DAP_4_0).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write AsynchronousResponse element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "status", (const xmlChar *) "gone") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end AsynchronousResponse element");
	// ------ AsynchronousResponse Element and Attributes - END
}


/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD4AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description, string *stylesheet_ref) {

	// ------ AsynchronousResponse Element and Attributes - BEGIN
	DapXmlNamspaces dapns;


	if(stylesheet_ref){
		string href = "href='" + *stylesheet_ref +"'";
		if(xmlTextWriterStartPI(xml.get_writer(), (const xmlChar*) "xml-stylesheet") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not start XML Processing Instruction.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) "type='text/xsl'") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) " ") < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterWriteString(xml.get_writer(), (const xmlChar*) href.c_str()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not write Processing Instruction content.");
		if(xmlTextWriterEndPI(xml.get_writer()) < 0)
			throw InternalErr(__FILE__, __LINE__, "Could not Close XML Processing Instruction.");
	}

	if (xmlTextWriterStartElementNS(xml.get_writer(),
			(const xmlChar*)"dap",
			(const xmlChar*) "AsynchronousResponse",
			(const xmlChar*) dapns.getDapNamespaceString(DAP_4_0).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write AsynchronousResponse element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "status", (const xmlChar *) "rejected") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'status'");

	// ------ reason Element and Attributes
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "dap:reason") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write reason element");
	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "code", (const xmlChar*) getRejectReasonCodeString(code).c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for 'code'");
	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end reason element");
	// ------ reason Element and Attributes - END


	// ------ description Element and Attributes
	if (xmlTextWriterWriteElement(xml.get_writer(), (const xmlChar*) "dap:description", (const xmlChar*) description.c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write description element");

	// ------ description Element and Attributes - END

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end AsynchronousResponse element");
	// ------ AsynchronousResponse Element and Attributes - END


}

string D4AsyncUtil::getRejectReasonCodeString(RejectReasonCode code){

	string codeStr;
	switch(code){
	case TIME:
		codeStr = "time";
		break;

	case UNAVAILABLE:
		codeStr = "unavailable";
		break;

	case PRIVILEGES:
		codeStr = "privileges";
		break;

	case OTHER:
		codeStr = "other";
		break;

	default:
		throw InternalErr(__FILE__, __LINE__, "D4AsyncUtil::getRejectReasonCodeString() - Unrecognized reject_reason_code.");
		break;

	}
	return codeStr;
}





/**
 * @brief Print the AsyncRequired response to the.
 * Print the AsyncRequired in XML form.
 * @param xml Print to this XMLWriter instance
 */
void D4AsyncUtil::writeD2AsyncRequired(XMLWriter &xml, long expectedDelay, long responseLifetime)  {
	throw InternalErr(__FILE__, __LINE__, "DAP2 Doesn't handle Async.");
}



void D4AsyncUtil::writeD2AsyncAccepted(XMLWriter &xml, long expectedDelay, long responseLifetime, string asyncResourceUrl)  {
	throw InternalErr(__FILE__, __LINE__, "DAP2 Doesn't handle Async.");
}


void D4AsyncUtil::writeD2AsyncPending(XMLWriter &xml)  {
	throw InternalErr(__FILE__, __LINE__, "DAP2 Doesn't handle Async.");
}

void D4AsyncUtil::writeD2AsyncResponseGone(XMLWriter &xml)  {
	throw InternalErr(__FILE__, __LINE__, "DAP2 Doesn't handle Async.");
}

void D4AsyncUtil::writeD2AsyncResponseRejected(XMLWriter &xml, RejectReasonCode code, string description)  {
	throw InternalErr(__FILE__, __LINE__, "DAP2 Doesn't handle Async.");
}









} /* namespace libdap */
