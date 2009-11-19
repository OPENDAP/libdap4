/*
 * Dap4Group.cc
 *
 *  Created on: Nov 12, 2009
 *      Author: jimg
 */

#include "config.h"

#include <algorithm>

#include "Dap4Group.h"
#include "escaping.h"

#define XML_CHAR(x) reinterpret_cast<const xmlChar*>(x)

using namespace std;

namespace libdap {

void
Dap4Group::m_clone(const Dap4Group &g)
{
    Dap4Group &nc_g = const_cast<Dap4Group&>(g);

    // clear out the old stuff
    nc_g.d_shared_dims.clear();

    // copy the new stuff
    SharedDimsIter sdi = nc_g.dim_begin();
    while (sdi != nc_g.dim_end())
	add_dimension(sdi++);
}

Dap4Group::Dap4Group(const Dap4Group &rhs) : Structure(rhs) {
    m_clone(rhs);
}

Dap4Group::~Dap4Group() {
    SharedDimsIter sdi = dim_begin();
    while (sdi != dim_end())
	delete *sdi++;
}

Dap4Group &Dap4Group::operator=(const Dap4Group &rhs) {
    if (this == &rhs)
        return *this;

    dynamic_cast<Structure &>(*this) = rhs;

    m_clone(rhs);

    return *this;
}

BaseType *Dap4Group::ptr_duplicate() {
    return new Dap4Group(*this);
}

class isSharedDimName : public unary_function<Dap4SharedDimension*,bool>
 {
 private:
    string d_name;
    isSharedDimName() : d_name("") {}
 public:
    isSharedDimName(const string &name) : d_name(name) {}

    bool operator()(Dap4SharedDimension *shared_dim) const {
	return shared_dim->get_name() == d_name;
    }
};

int Dap4Group::dim_size(const string &name) const {
    SharedDimsCIter pos = find_if(dim_cbegin(), dim_cend(), isSharedDimName(name));

    if (pos != dim_cend())
	return (*pos)->get_size();
    else
	throw Error("Unknown dimension.");
}

void Dap4Group::add_dimension(const string &name, int size) {
    SharedDimsIter pos = find_if(dim_begin(), dim_end(), isSharedDimName(name));

    if (pos == dim_end())
	d_shared_dims.push_back(new Dap4SharedDimension(size, name));
    else
	throw Error("Attempt to add conflicting dimension name.");
}

void Dap4Group::add_dimension(SharedDimsIter pos) {
    // A pretty weak test...
    if (!(*pos)->get_name().empty())
	d_shared_dims.push_back(new Dap4SharedDimension(**pos));
    else
	throw Error("Attempt to add undefined dimension.");
}

/** @brief Insert a dimension at a specific position
 *
 * Inserts the dimension made up of name and size at the position \c pos.
 *
 * @param pos Insert at this position
 * @param name
 * @param size
 */
void Dap4Group::insert_dimension(SharedDimsIter pos, const string &name, int size) {
    if (pos == dim_end())
	d_shared_dims.push_back(new Dap4SharedDimension(size, name));
    else
	d_shared_dims.insert(pos, new Dap4SharedDimension(size, name));
}

/** @brief Insert a dimension
 *
 * Given a dimension referenced by an iterator, insert a copy into the list
 * of dimensions.
 *
 * @param pos
 * @param d
 */
void Dap4Group::insert_dimension(SharedDimsIter pos, SharedDimsIter d) {
    if (pos == dim_end())
	d_shared_dims.push_back(new Dap4SharedDimension(**d));
    else
	d_shared_dims.insert(pos, new Dap4SharedDimension(**d));
}

int Dap4Group::delete_dimension(SharedDimsIter pos) {
    SharedDimsIter tpos = find(dim_begin(), dim_end(), *pos);
    if (tpos != dim_end()) {
	int size = (*pos)->get_size();
	d_shared_dims.erase(pos);
	return size;
    }
    else
	throw Error("Attempt to delete a non-existent dimension.");
}

int Dap4Group::delete_dimension(const string &name) {
    SharedDimsIter pos = find_if(dim_begin(), dim_end(), isSharedDimName(name));

    return delete_dimension(pos);
}

/** @brief serialize variables in this Group
 *
 * A Group in DAP4 serializes any variable marked as part of the current
 * projection. However, this method does not trigger a read() call; effectively
 * it replaces the old top-level serialization loop in DAP2.
 *
 * @todo If the read() methods for the ctor types were implemented more
 * uniformly, then it could call read() and could actually probably use the
 * implementation in Structure::read()
 *
 * @param eval Evaluate constraints using this evaluator
 * @param dds This is the environment within which to evaluate the constraint
 * @param m Use this to marshal values (using XDR)
 * @param ce_eval If true (default) children will evaluate the constraint.
 * @return Always returns true
 */
bool Dap4Group::serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m,
	bool ce_eval) {

    Vars_iter i = var_begin();
    Vars_iter end = var_end();
    while (i != end)
	if ((*i)->send_p())
	    (*i++)->serialize(eval, dds, m, ce_eval);

    return true;
}

class PrintDimStrm : public unary_function<Dap4SharedDimension*, void>
{
    ostream &d_out;
    string d_space;
    bool d_constrained;
public:
    PrintDimStrm(ostream &o, string s, bool c)
            : d_out(o), d_space(s), d_constrained(c)
    {}

    void operator()(Dap4SharedDimension *sdp)
    {
        sdp->print_xml(d_out, d_space, d_constrained);
    }
};

class PrintFieldStrm : public unary_function<BaseType *, void>
{
    ostream &d_out;
    string d_space;
    bool d_constrained;
public:
    PrintFieldStrm(ostream &o, string s, bool c)
            : d_out(o), d_space(s), d_constrained(c)
    {}

    void operator()(BaseType *btp)
    {
        btp->print_xml(d_out, d_space, d_constrained);
    }
};

void
Dap4Group::print_xml(ostream &out, string space, bool constrained)
{
    if (constrained && !send_p())
        return;

    bool has_dimensions = dim_num() > 0;
    bool has_attributes = get_attr_table().get_size() > 0;
    bool has_variables = (var_begin() != var_end());

    out << space << "<" << type_name() ;
    if (!name().empty())
	out << " name=\"" << id2xml(name()) << "\"" ;

    if (has_dimensions || has_attributes || has_variables) {
	out << ">\n" ;

	for_each(dim_begin(), dim_end(),
		PrintDimStrm(out, space + "    ", constrained));

        get_attr_table().print_xml(out, space + "    ", constrained);

        for_each(var_begin(), var_end(),
                 PrintFieldStrm(out, space + "    ", constrained));

	out << space << "</" << type_name() << ">\n" ;
    }
    else {
	out << "/>\n" ;
    }
}

class PrintDimXML : public unary_function<Dap4SharedDimension*, void>
{
    xmlTextWriterPtr d_writer;
    bool d_constrained;
public:
    PrintDimXML(xmlTextWriterPtr w, bool c)
            : d_writer(w), d_constrained(c)
    {}

    void operator()(Dap4SharedDimension *sdp)
    {
        sdp->print_xml(d_writer, d_constrained);
    }
};

class PrintFieldXML : public unary_function<BaseType *, void>
{
    xmlTextWriterPtr d_writer;
    bool d_constrained;
public:
    PrintFieldXML(xmlTextWriterPtr w, bool c)
            : d_writer(w), d_constrained(c)
    {}

    void operator()(BaseType *btp)
    {
	cerr << "Printing: " << btp->name() << endl;
#if 0
        btp->print_xml(writer, d_constrained);
#endif
    }
};

void Dap4Group::print_xml(xmlTextWriterPtr writer, bool constrained) {

    if (constrained && !send_p())
        return;

    bool has_dimensions = dim_num() > 0;
    bool has_attributes = get_attr_table().get_size() > 0;
    bool has_variables = (var_begin() != var_end());

    if (xmlTextWriterStartElement(writer, XML_CHAR("Group")) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error writing Dimension");

    if (!name().empty())
	if (xmlTextWriterWriteAttribute(writer, XML_CHAR("name"),
		XML_CHAR(name().c_str())) < 0)
	    throw InternalErr(__FILE__, __LINE__, "Error writing Dimension name");

    if (has_dimensions || has_attributes || has_variables) {
	for_each(dim_begin(), dim_end(),
		PrintDimXML(writer, constrained));
#if 0
        get_attr_table().print_xml(out, space + "    ", constrained);
#endif
        for_each(var_begin(), var_end(),
                 PrintFieldXML(writer, constrained));
    }

    if (xmlTextWriterEndElement(writer) < 0)
        throw InternalErr(__FILE__, __LINE__, "Error closing Group");
}


class DumpSharedDim : public unary_function<Dap4SharedDimension, void> {
private:
    ostream &d_out;
    DumpSharedDim() : d_out(cout) {}
public:

    DumpSharedDim(ostream &out) : d_out(out) {}
    void operator()(Dap4SharedDimension *sd) {
	sd->dump(d_out);
    }
};

void
Dap4Group::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "Dap4Group::dump - ("
    << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    for_each(dim_cbegin(), dim_cend(), DumpSharedDim(strm));
    Structure::dump(strm) ;
    DapIndent::UnIndent() ;
}

}
