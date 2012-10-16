/*
 * D4Group.cc
 *
 *  Created on: Sep 27, 2012
 *      Author: jimg
 */

#include "D4Group.h"

#include "BaseType.h"

namespace libdap {

/** The D4Group constructor requires only the name of the variable
    to be created. The name may be omitted, which will create a
    nameless variable. This may be adequate for some applications.

    @param n A string containing the name of the variable.
*/
D4Group::D4Group(const string &n) : Constructor(n, dods_group_c, /*is_dap4*/true)
{}

/** The D4Group server-side constructor requires the name of the variable
    to be created and the dataset name from which this variable is being
    created. Used on server-side handlers.

    @param n A string containing the name of the variable.
    @param d A string containing the name of the dataset.
*/
D4Group::D4Group(const string &n, const string &d)
    : Constructor(n, d, dods_group_c, /*is_dap4*/true)
{}

/** The D4Group copy constructor. */
D4Group::D4Group(const D4Group &rhs) : Constructor(rhs)
{
    m_duplicate(rhs);
}

static void enum_del(D4EnumDef *ed)
{
    delete ed;
}

D4Group::~D4Group()
{
    for_each(d_enums.begin(), d_enums.end(), enum_del);
}

BaseType *
D4Group::ptr_duplicate()
{
    return new D4Group(*this);
}

D4Group &
D4Group::operator=(const D4Group &rhs)
{
    if (this == &rhs)
        return *this;

    dynamic_cast<Constructor &>(*this) = rhs; // run Constructor=

    m_duplicate(rhs);

    return *this;
}

void
D4Group::add_enumeration_nocopy(D4EnumDef *enum_def)
{
    d_enums.push_back(enum_def);
}

class PrintVariable : public unary_function<BaseType *, void>
{
    XMLWriter &d_xml;
    bool d_constrained;
public:
    PrintVariable(XMLWriter &x, bool c) : d_xml(x), d_constrained(c) {}

    void operator()(BaseType *btp)
    {
        btp->print_xml_writer(d_xml, d_constrained);
    }
};

class PrintEnum : public unary_function<D4EnumDef *, void>
{
    XMLWriter &d_xml;

public:
    PrintEnum(XMLWriter &x) : d_xml(x){}

    void operator()(D4EnumDef *e)
    {
        e->print_xml_writer(d_xml);
    }
};

void
D4Group::print_xml_writer(XMLWriter &xml, bool constrained)
{
    if (constrained && !send_p())
        return;

    if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar*)type_name().c_str()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

    if (!name().empty())
        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar*) "name", (const xmlChar*)name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");

    // If the Group has Enumeration definitions
    if (d_enums.size() > 0)
        for_each(d_enums.begin(), d_enums.end(), PrintEnum(xml));

    // If it has attributes
    if (get_attr_table().get_size() > 0)
        get_attr_table().print_xml_writer(xml);

    // If it has variables
    if (var_begin() != var_end())
        for_each(var_begin(), var_end(), PrintVariable(xml, constrained));

    if (xmlTextWriterEndElement(xml.get_writer()) < 0)
        throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
}

} /* namespace libdap */
