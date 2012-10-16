/*
 * D4EnumDef.cc
 *
 *  Created on: Oct 9, 2012
 *      Author: jimg
 */

#include "D4EnumDef.h"

#include <sstream>

#include "util.h"

namespace libdap {

#if 0
string D4EnumDef::type_name() {
    switch (d_type) {
    case dods_int8_c:
        return string("Int8");
    case dods_uint8_c:
        return string("UInt8");
    case dods_byte_c:
        return string("Byte");
    case dods_int16_c:
        return string("Int16");
    case dods_uint16_c:
        return string("UInt16");
    case dods_int32_c:
        return string("Int32");
    case dods_uint32_c:
        return string("UInt32");
    case dods_int64_c:
        return string("Int64");
    case dods_uint64_c:
        return string("UInt64");

    default:
        throw InternalErr(__FILE__, __LINE__, "Invalid enumeration basetype.");
    }
}
#endif

void D4EnumDef::print_enum_const(XMLWriter *xml, const enum_val ev)
{
    if (xmlTextWriterStartElement(xml->get_writer(), (const xmlChar*)"EnumConst") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write EnumConst element");

    if (xmlTextWriterWriteAttribute(xml->get_writer(), (const xmlChar*) "name", (const xmlChar*)ev.d_item.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    ostringstream oss;
    oss << ev.d_val;
    if (xmlTextWriterWriteAttribute(xml->get_writer(), (const xmlChar*) "value", (const xmlChar*)oss.str().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for value");

    if (xmlTextWriterEndElement(xml->get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end EnumConst element");

}

void D4EnumDef::print_xml_writer(XMLWriter &xml)
{
    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)"Enumeration") < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write Enumeration element");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)d_name.c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "basetype", (const xmlChar*)type_name(d_type).c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    for_each(d_values.begin(), d_values.end(), bind1st(ptr_fun(print_enum_const), &xml));

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end Enumeration element");
}

} /* namespace libdap */
