/*
 * D4Maps.cc
 *
 *  Created on: Sep 26, 2012
 *      Author: jimg
 */

#include "config.h"

#include "XMLWriter.h"
#include "InternalErr.h"
#include "D4Maps.h"

using namespace libdap;

void
D4Map::print_dap4(XMLWriter &xml)
{
	if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*) "Map") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write Map element");

	if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

	if (xmlTextWriterEndElement(xml.get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end Map element");

}
