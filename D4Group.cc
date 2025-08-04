// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <cassert>

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include <cstdint>

#include "crc.h"

#include "Array.h"
#include "BaseType.h"
#include "Grid.h"

#include "D4Attributes.h"
#include "D4Dimensions.h"
#include "D4Enum.h"
#include "D4Group.h"
#include "XMLWriter.h"

#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"
#include "DMR.h"

#include "debug.h"
#include "escaping.h"
#include "util.h"

/**
 * Define this symbol iff we decide to include information about the
 * byte order of the response (as sent from the server) so that the
 * client can determine the correct CRC32 hash code. jhrg 1/4/16
 */
#undef INCLUDE_SOURCE_BYTE_ORDER

namespace libdap {

void D4Group::m_duplicate(const D4Group &g) {
    DBG(cerr << "In D4Group::m_duplicate for " << g.name() << endl);

    // dims; deep copy, this is the parent
    if (g.d_dims) {
        d_dims = new D4Dimensions(*(g.d_dims));
        d_dims->set_parent(this);

        // Update all of the D4Dimension weak pointers in the Array objects.
        // This is a hack - we know that Constructor::m_duplicate() has been
        // called at this point and any Array instances have dimension pointers
        // that reference the 'old' dimensions (g.d_dims) and not the 'new'
        // dimensions made above. Scan every array and re-wire the weak pointers.
        // jhrg 8/15/14
        Vars_citer vi = d_vars.begin();
        while (vi != d_vars.end()) {
            if ((*vi)->type() == dods_array_c)
                static_cast<Array *>(*vi)->update_dimension_pointers(g.d_dims, d_dims);
            ++vi;
        }
    }

    // enums; deep copy
    if (g.d_enum_defs)
        d_enum_defs = new D4EnumDefs(*g.d_enum_defs);

    // groups
    groupsCIter i = g.d_groups.begin();
    while (i != g.d_groups.end()) {
        // Only D4Groups are in the d_groups container.
        D4Group *g = static_cast<D4Group *>((*i++)->ptr_duplicate());
        add_group_nocopy(g);
    }

    DBG(cerr << "Exiting D4Group::m_duplicate" << endl);
}

/** The D4Group constructor requires only the name of the variable
    to be created. The name may be omitted, which will create a
    nameless variable. This may be adequate for some applications.

    @note This type is available in DAP4 only.
    See http://docs.opendap.org/index.php/DAP4:_Specification_Volume_1#Groups


    @param n A string containing the name of the variable.
*/
D4Group::D4Group(const string &name) : Constructor(name, dods_group_c, /*is_dap4*/ true), d_dims(0), d_enum_defs(0) {}

/** The D4Group server-side constructor requires the name of the variable
    to be created and the dataset name from which this variable is being
    created. Used on server-side handlers.

    @note This type is available in DAP4 only.
    See http://docs.opendap.org/index.php/DAP4:_Specification_Volume_1#Groups

    @param n A string containing the name of the variable.
    @param d A string containing the name of the dataset.
*/
D4Group::D4Group(const string &name, const string &dataset)
    : Constructor(name, dataset, dods_group_c, /*is_dap4*/ true), d_dims(0), d_enum_defs(0) {}

/** The D4Group copy constructor. */
D4Group::D4Group(const D4Group &rhs) : Constructor(rhs), d_dims(0), d_enum_defs(0) {
    DBG(cerr << "In D4Group::copy_ctor for " << rhs.name() << endl);
    m_duplicate(rhs);
}

D4Group::~D4Group() {
    delete d_dims;
    delete d_enum_defs;

    groupsIter i = d_groups.begin();
    while (i != d_groups.end())
        delete *i++;
}

BaseType *D4Group::ptr_duplicate() { return new D4Group(*this); }

D4Group &D4Group::operator=(const D4Group &rhs) {
    if (this == &rhs)
        return *this;
    Constructor::operator=(rhs);
    m_duplicate(rhs);
    return *this;
}

/**
 * Get the Fully Qualified Name for this Group, including the Group. This
 * uses the name representation described in the DAP4 specification.
 *
 * @return The FQN in a string
 */
string D4Group::FQN() const {
    // The root group is named "/" (always)
    return (name() == "/") ? "/" : static_cast<D4Group *>(get_parent())->FQN() + name() + "/";
}

D4Group *D4Group::find_child_grp(const string &grp_name) {
    auto g = find_if(grp_begin(), grp_end(), [grp_name](const D4Group *g) { return g->name() == grp_name; });
    return (g == grp_end()) ? nullptr : *g;
}

// This is a private method. The grp_path is not supposed to start with the '/'.
D4Group *D4Group::find_grp_internal(const string &grp_path) {

    string::size_type pos = grp_path.find('/');
    if (pos == string::npos) {
        D4Group *d4_grp = this->find_child_grp(grp_path);
        return d4_grp;
    } else {
        string top_grp_path = grp_path.substr(0, pos);
        string rest_grp_path = grp_path.substr(pos + 1);
        D4Group *d4_grp = this->find_child_grp(top_grp_path);
        if (d4_grp != nullptr) {
            return d4_grp->find_grp_internal(rest_grp_path);
        } else
            return nullptr;
    }
}
// Add constraint param? jhrg 11/17/13
BaseType *D4Group::find_first_var_that_uses_dimension(D4Dimension *dim) {
    // for each group, starting with the root group
    //    for each variable in the group that is marked to send and is an array
    //        return the btp if it uses the D4Dimension
    //    if it contains child groups, search those
    //        return the btp if it uses the D4Dimension
    // return null

    // exhaustive breadth-first search for 'dim

    // root group
    for (Vars_iter i = var_begin(), e = var_end(); i != e; ++i) {
        if ((*i)->send_p() && (*i)->type() == dods_array_c) {
            Array *a = static_cast<Array *>(*i);
            for (Array::Dim_iter di = a->dim_begin(), de = a->dim_end(); di != de; ++di) {
                if (a->dimension_D4dim(di) == dim)
                    return a;
            }
        }
    }

    for (groupsIter i = grp_begin(), e = grp_end(); i != e; ++i) {
        BaseType *btp = (*i)->find_first_var_that_uses_dimension(dim);
        if (btp)
            return btp;
    }

    return 0;
}

BaseType *D4Group::find_first_var_that_uses_enumeration(D4EnumDef *enum_def) {
    // for each group, starting with the root group
    //    for each variable in the group that is marked to send and is an array
    //        return the btp if it uses the D4EnumDef
    //    if it contains child groups, search those
    //        return the btp if it uses the D4EnumDef
    // return null

    // exhaustive breadth-first search for 'dim

    // root group
    for (Vars_iter i = var_begin(), e = var_end(); i != e; ++i) {
        if ((*i)->send_p() && (*i)->type() == dods_enum_c) {
            D4Enum *e = static_cast<D4Enum *>(*i);
            if (e->enumeration() == enum_def)
                return e;
        }
    }

    for (groupsIter i = grp_begin(), e = grp_end(); i != e; ++i) {
        BaseType *btp = (*i)->find_first_var_that_uses_enumeration(enum_def);
        if (btp)
            return btp;
    }

    return 0;
}

/**
 * @brief Find the dimension using a path.
 * Using the DAP4 name syntax, lookup a dimension. The dimension must
 * be defined before it is used. The \c path argument may be either an
 * absolute path or a relative path. Note that the name syntax does not
 * provide for paths to contain an 'up one level' symbol.
 * @param path The path to the dimension
 * @return A pointer to the D4Dimension object.
 */
D4Dimension *D4Group::find_dim(const string &path) {
    string lpath = path; // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }

    string::size_type pos = lpath.find('/');
    if (pos == string::npos) {
        // name looks like 'bar'
        return dims()->find_dim(lpath);
    }

    // name looks like foo/bar/baz where foo and bar must be groups
    string grp_name = lpath.substr(0, pos);
    lpath = lpath.substr(pos + 1);

    D4Group *grp = find_child_grp(grp_name);
    return (grp == 0) ? 0 : grp->find_dim(lpath);
}

/**
 * @brief Given a path to an Array that is also a Map, get that Array.
 * @param path The path to the Map. May be a FQN or a path relative to this group.
 * @return A pointer to the Array or nullptr if the Map cannot be found.
 */
Array *D4Group::find_map_source(const string &path) {
    BaseType *map_source = m_find_map_source_helper(path);

    if (map_source && map_source->type() == dods_array_c)
        return static_cast<Array *>(map_source);

    return 0;
}

/**
 * @brief Get the Map Array named by the path
 * @param path The path to the Map. May be a FQN or a path relative to this group.
 * @return A pinter to the variable named by the path
 */
BaseType *D4Group::m_find_map_source_helper(const string &path) {
    string lpath = path; // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }
    string::size_type pos = lpath.find('/');
    if (pos == string::npos) {
        // name looks like 'bar'
        return var(lpath);
    }

    // name looks like foo/bar/baz where foo and bar must be groups
    string grp_name = lpath.substr(0, pos);

    D4Group *grp = find_child_grp(grp_name);
    lpath = lpath.substr(pos + 1);

    // We need to resolve the case that
    // many group layers are involved such as /foo/bar/bar2/bar3/.../baz
    // The following code handles this.
    // KY 2023-05-21
    //
    pos = lpath.find('/');

    if (pos == string::npos)
        return (grp == nullptr) ? nullptr : grp->var(lpath);

    // Recursively check the child groups until we hit the leaf.
    while (pos != string::npos) {

        grp_name = lpath.substr(0, pos);
        grp = grp->find_child_grp(grp_name);
        lpath = lpath.substr(pos + 1);
        pos = lpath.find('/');
    }

    return (grp == nullptr) ? nullptr : grp->var(lpath);
}

D4EnumDef *D4Group::find_enum_def(const string &path) {

    string lpath = path; // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }

    string::size_type pos = lpath.rfind('/');
    if (pos == string::npos) {
        // name looks like 'bar'
        return enum_defs()->find_enum_def(lpath);
    }
    // name looks like foo/bar/baz where foo and bar must be groups

    // Now we need to recurisvely find the group of foo/bar since we only need to
    // find if the enum_defs under this /foo/bar contains the enum path.

    // Note pos cannot be string::npos when code runs here.
    // Obtain the final enum_def name in the lpath.
    string enum_def_name = lpath.substr(pos + 1);

    // Obtain the full group path that contains the final enum_def name.
    string enum_def_path = lpath.substr(0, pos);

    D4Group *grp = find_grp_internal(enum_def_path);
    if (grp == nullptr)
        return nullptr;
    else if (grp->enum_defs() == nullptr)
        return nullptr;
    else
        return grp->enum_defs()->find_enum_def(enum_def_name);
}

/**
 * Find a variable using its Fully Qualified Name (FQN). The leading '/' is optional.
 *
 * @param path The FQN to the variable
 * @return A BaseType* to the variable or null if it was not found
 * @see BaseType::FQN()
 */
BaseType *D4Group::find_var(const string &path) {
    string lpath = path; // get a mutable copy

    // special-case for the root group
    if (lpath[0] == '/') {
        if (name() != "/")
            throw InternalErr(__FILE__, __LINE__, "Lookup of a FQN starting in non-root group.");
        else
            lpath = lpath.substr(1);
    }

    string::size_type pos = lpath.find('/');
    if (pos == string::npos) {
        // New behavior to accommodate cases where the path ends in a group - the
        // CE is being used to request all the variables in a Group. So, first check
        // if this is the name of a Group and if so, return that. Otherwise, look in
        // the Group's Constructor for a matching variable. jhrg 8/3/22
        D4Group *grp = find_child_grp(lpath);
        if (grp != nullptr)
            return grp;
        else
            return var(lpath);
    }

    // name looks like foo/bar/baz where foo and bar must be groups
    string grp_name = lpath.substr(0, pos);
    lpath = lpath.substr(pos + 1);

    D4Group *grp = find_child_grp(grp_name);
    if (grp == nullptr)
        return nullptr;
    else if (lpath.empty())
        return grp;
    else
        return grp->find_var(lpath);
}

/**
 * Compute the size of all of the variables in this group and its children,
 * in kilobytes
 *
 * @param constrained Should the current constraint be taken into account?
 * @return The size in kilobytes
 * @deprecated Use request_size_kb() instead.
 */
long D4Group::request_size(bool constrained) { return (long)request_size_kb(constrained); }

/**
 * @brief Get the estimated size of a response in kilobytes.
 *
 * This method looks at the variables in the DMR and computes
 * the number of kilobytes in the response.
 *
 * @note This version of the method does a poor job with Sequences. A better
 * implementation would look at row-constraint-based limitations and use them
 * for size computations. If a row-constraint is missing, return an error.
 *
 * @param constrained Should the size of the whole DMR be used or should the
 * current constraint be taken into account?
 * @return The size in kilobytes
 */
uint64_t D4Group::request_size_kb(bool constrained) {
    uint64_t size = 0;
    // variables
    for (auto &btp : d_vars) {
        if (constrained) {
            if (btp->send_p())
                size += btp->width_ll(constrained);
        } else {
            size += btp->width_ll(constrained);
        }
    }

    size = size / 1024; // Make into kilobytes

    // All the child groups
    for (auto grp : d_groups)
        size += grp->request_size_kb(constrained);

    return size;
}

void D4Group::set_read_p(bool state) {
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_read_p(state);

    Constructor::set_read_p(state);
}

void D4Group::set_send_p(bool state) {
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->set_send_p(state);

    Constructor::set_send_p(state);
}

void D4Group::intern_data(/*Crc32 &checksum, DMR &dmr, ConstraintEvaluator &eval*/) {
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->intern_data(/*checksum, dmr, eval*/);

    // Specialize how the top-level variables in any Group are sent; include
    // a checksum for them. A subset operation might make an interior set of
    // variables, but the parent structure will still be present and the checksum
    // will be computed for that structure. In other words, DAP4 does not try
    // to sort out which variables are the 'real' top-level variables and instead
    // simply computes the CRC for whatever appears as a variable in the root
    // group.
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        // Only send the stuff in the current subset.
        if ((*i)->send_p()) {
            (*i)->intern_data(/*checksum, dmr, eval*/);
        }
    }
}

/**
 * @brief Serialize a Group
 * @param m The DAP4 Stream Marshaller. This object serializes the data values and
 * writes checksums (using CRC32) for the top level variables in every Group for which
 * one or more variables are sent. The DAP4 Marshaller object can be made so that only
 * the checksums are written.
 * @param dmr Unused
 * @param eval Unused
 * @param filter Unused
 * @exception Error is thrown if the value needs to be read and that operation fails.
 */
void D4Group::serialize(D4StreamMarshaller &m, DMR &dmr, /*ConstraintEvaluator &eval,*/ bool filter) {
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->serialize(m, dmr, filter);

    // Specialize how the top-level variables in any Group are sent; include
    // a checksum for them. A subset operation might make an interior set of
    // variables, but the parent structure will still be present and the checksum
    // will be computed for that structure. In other words, DAP4 does not try
    // to sort out which variables are the 'real' top-level variables and instead
    // simply computes the CRC for whatever appears as a variable in the root
    // group.
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        // Only send the stuff in the current subset.
        if ((*i)->send_p()) {
            if (dmr.use_checksums())
                m.reset_checksum();

            DBG(cerr << "Serializing variable " << (*i)->type_name() << " " << (*i)->name() << endl);
            (*i)->serialize(m, dmr, filter);
            if (dmr.use_checksums()) {
                m.put_checksum();
                DBG(cerr << "Wrote CRC32: " << m.get_checksum() << " for " << (*i)->name() << endl);
            }
        }
    }
}

void D4Group::deserialize(D4StreamUnMarshaller &um, DMR &dmr) {
    groupsIter g = d_groups.begin();
    while (g != d_groups.end()) {
        DBG(cerr << "Deserializing group " << (*g)->name() << endl);
        (*g++)->deserialize(um, dmr);
    }
    // Specialize how the top-level variables in any Group are received; read
    // their checksum and store the value in a magic attribute of the variable
    for (Vars_iter i = d_vars.begin(); i != d_vars.end(); i++) {
        DBG(cerr << "Deserializing variable " << (*i)->type_name() << " " << (*i)->name() << endl);
        (*i)->deserialize(um, dmr);

        if (dmr.use_checksums()) {

            D4Attribute *a = new D4Attribute("DAP4_Checksum_CRC32", attr_str_c);
            // This call to um.get_checksum_str() calls um.get_Checksum which
            // is what reads the checksum bytes from the input stream.
            string crc = um.get_checksum_str();
            a->add_value(crc);
#if INCLUDE_SOURCE_BYTE_ORDER
            if (um.is_source_big_endian())
                a->add_value("source:big-endian");
            else
                a->add_value("source:little-endian");
#endif
            DBG(cerr << "Read CRC32: " << crc << " for " << (*i)->name() << endl);
            (*i)->attributes()->add_attribute_nocopy(a);
        }
    }
}

void D4Group::print_dap4(XMLWriter &xml, bool constrained) {
    if (!name().empty() && name() != "/") {
        // For named groups, if constrained is true only print if this group
        // has variables that are marked for transmission. For the root group
        // this test is not made.
        if (constrained && !send_p())
            return;

        if (xmlTextWriterStartElement(xml.get_writer(), (const xmlChar *)type_name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write " + type_name() + " element");

        if (xmlTextWriterWriteAttribute(xml.get_writer(), (const xmlChar *)"name", (const xmlChar *)name().c_str()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
    }

    // dims
    if (!dims()->empty())
        dims()->print_dap4(xml, constrained);

    // enums
    if (!enum_defs()->empty())
        enum_defs()->print_dap4(xml, constrained);

    // variables
    Constructor::Vars_iter v = var_begin();
    while (v != var_end())
        (*v++)->print_dap4(xml, constrained);

    // attributes
    attributes()->print_dap4(xml);

    // groups
    groupsIter g = d_groups.begin();
    while (g != d_groups.end())
        (*g++)->print_dap4(xml, constrained);

    if (!name().empty() && name() != "/") {
        if (xmlTextWriterEndElement(xml.get_writer()) < 0)
            throw InternalErr(__FILE__, __LINE__, "Could not end " + type_name() + " element");
    }
}

void D4Group::print_decl(FILE *out, string space, bool print_semi, bool constraint_info, bool constrained) {
    ostringstream oss;
    print_decl(oss, space, print_semi, constraint_info, constrained);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void D4Group::print_decl(ostream &out, string space, bool print_semi, bool constraint_info, bool constrained) {
    if (constrained && !send_p())
        return;

    out << space << type_name() << " {\n";
    for (auto var : d_vars) {
        var->print_decl(out, space + "    ", true, constraint_info, constrained);
    }

    for (auto grp : d_groups) {
        grp->print_decl(out, space + "    ", true, constraint_info, constrained);
    }

    out << space << "} " << id2www(name());

    if (constraint_info) { // Used by test drivers only.
        if (send_p())
            out << ": Send True";
        else
            out << ": Send False";
    }

    if (print_semi)
        out << ";\n";
}

void D4Group::print_val(FILE *out, string space, bool print_decl_p) {
    ostringstream oss;
    print_val(oss, space, print_decl_p);
    fwrite(oss.str().data(), sizeof(char), oss.str().length(), out);
}

void D4Group::print_val(ostream &out, string space, bool print_decl_p) {
    if (print_decl_p) {
        print_decl(out, space, false);
        out << " = ";
    }

    out << "{ ";
    bool padding_needed = false; // Add padding - which is complex with the two parts. jhrg 8/5/22
    for (Vars_citer i = d_vars.begin(), e = d_vars.end(); i != e; i++, (void)(i != e && out << ", ")) {
        (*i)->print_val(out, "", false);
        padding_needed = true;
    }

    if (padding_needed)
        out << " ";

    padding_needed = false;
    for (auto grp : d_groups) {
        grp->print_val(out, "", false);
        padding_needed = true;
    }

    if (padding_needed)
        out << " }";
    else
        out << "}";

    if (print_decl_p)
        out << ";\n";
}

/**
 * @brief Transform the D4Group's variables to DAP2 variables
 *
 * For all the variables in a D4Group, build a vector of DAP2 variables
 * that can be directly added to DDS object. Extract the DAP4 variables'
 * attributes and transfer them to the AttrTable object passed as the first
 * argument.
 *
 * For all variables in the D4Group that are members of child groups (i.e.,
 * not the root group), the name of the group (or names of the groups) will
 * be prepended to the name of the variable. Group names are separated using
 * a '/' character.
 *
 * The Group attributes are transferred to the parent_attr_table.
 *
 * @todo Fix the comment.
 *
 * @param parent_attr_table The AttrTable pointer parent_attr_table is used by Groups, which disappear
 * from the DAP2 representation. Their children are returned in to the BaseType vector
 * their attributes are added to parent_attr_table;
 * @param show_shared_dims is false by default;
 * @return A pointer to a vector of BaseType pointers. In this case, the
 * vector will contain DAP2 versions of the D4Group's variables.
 * (ex: UInt64) the will return a NULL pointer and so this must be tested!
 */
vector<BaseType *> *D4Group::transform_to_dap2(AttrTable *parent_attr_table, bool show_shared_dims) {
    DBG(cerr << __func__ << "() - BEGIN (" << name() << ")" << endl);

    auto *results = new vector<BaseType *>(); // LEAK

    // Get the D4Group's attributes
    auto *group_attrs = new AttrTable();
    attributes()->transform_attrs_to_dap2(group_attrs);
    group_attrs->set_name(name());

    // If this is the root group then copy all of its attributes into the parent_attr_table.
    // The group_attrs AttrTable* above will be replaced by the parent_attr_table.
    const bool is_root = (name() == "/");

    if (is_root) {
        for (auto i = group_attrs->attr_begin(), e = group_attrs->attr_end(); i != e; ++i) {
            if ((*i)->type == Attr_container) {
                // copy the source container so that the DAS passed in can be
                // deleted after calling this method.
                auto *at = new AttrTable(*(*i)->attributes);
                parent_attr_table->append_container(at, at->get_name());
            } else {
                parent_attr_table->append_attr((*i)->name, AttrType_to_String((*i)->type), (*i)->attr,
                                               (*i)->is_utf8_str);
            }
        }
        delete group_attrs;
        group_attrs = parent_attr_table;
    }

    // Now we process the child variables of this group
    map<string, Array *, std::less<>> potential_shared_dims; // Holds Grid maps, see below. jhrg 10/3/19
    vector<BaseType *> dropped_vars;
    for (auto var : d_vars) {
        // for (D4Group::Vars_citer i = var_begin(), e = var_end(); i != e; ++i) {

        DBG(cerr << __func__ << "() - Processing member variable '" << (*i)->name()
                 << "' root: " << (is_root ? "true" : "false") << endl);

        const vector<BaseType *> *new_vars = var->transform_to_dap2(group_attrs, show_shared_dims);
        if (new_vars) { // Might be unmappable
            // It's mappable, so game on...
            for (auto &new_var : *new_vars) {
                string new_name = (is_root ? "" : FQN()) + new_var->name();
                new_var->set_name(new_name);
                new_var->set_parent(nullptr);
                results->push_back(new_var);
                // collect all map dimensions from grids if show_shared_dims is false
                if (!show_shared_dims && new_var->type() == dods_grid_c) {
                    // Add only one copy of each Map with the same name
                    auto *g = static_cast<Grid *>(new_var);
                    for (auto m = g->map_begin(); m != g->map_end(); ++m) {
                        if (!potential_shared_dims[(*m)->name()])
                            potential_shared_dims[(*m)->name()] = dynamic_cast<Array *>(*m);
                    }
                }
                DBG(cerr << __func__ << "() - Added member variable '" << (*i)->name() << "' "
                         << "to results vector. root: " << (is_root ? "true" : "false") << endl);
            }
            delete new_vars;
        } else {
            DBG(cerr << __func__ << "() - Dropping member variable " << (*i)->name()
                     << " root: " << (is_root ? "true" : "false") << endl);
            // Got back a NULL, so we are dropping this var.
            dropped_vars.push_back(var);
        }
    }

    // If we're not showing shared dimensions, then remove the dimensions
    if (!show_shared_dims) {
        for (auto it = results->end(); it != results->begin();) {
            --it;
            if (potential_shared_dims.find((*it)->name()) != potential_shared_dims.end()) {
                delete *it;
                it = results->erase(it);
            }
        }
    }

    // Process dropped DAP4 vars
    DBG(cerr << __func__ << "() - Processing " << dropped_vars.size() << " Dropped Variable(s)" << endl);
    AttrTable *dv_attr_table = make_dropped_vars_attr_table(&dropped_vars);
    if (dv_attr_table) {
        group_attrs->append_container(dv_attr_table, dv_attr_table->get_name());
    }

    // Get all the child groups.
    for (auto group : d_groups) {
        // for (auto gi = grp_begin(), ge = grp_end(); gi != ge; ++gi) {
        const vector<BaseType *> *d2_vars = group->transform_to_dap2(group_attrs, false);
        if (d2_vars) {
            for (auto &d2_var : *d2_vars) {
                results->push_back(d2_var);
            }
        }
        delete d2_vars;
    }

    if (!is_root) {
        group_attrs->set_name(name());
        parent_attr_table->append_container(group_attrs, group_attrs->get_name());
    }

    return results;
}

/**
 * When send_p() is true a description of the instance is added to the inventory and true is returned.
 * @param inventory is a value-result parameter
 * @return True when send_p() is true, false otherwise
 */
bool D4Group::is_dap4_projected(std::vector<std::string> &inventory) {
    bool has_projected_dap4 = false;
    if (send_p()) {
        // Groups are a dap4 thing, so if the Group is projected...
        has_projected_dap4 = true;
        inventory.emplace_back(type_name() + " " + FQN());

        // Even tho this Group is a projected dap4 variable we still need to
        // generate an inventory of it's dap4 attributes and projected dap4 child variables
        // and groups.

        // Inventory the Group's dap4 attributes
        has_projected_dap4 |= attributes()->has_dap4_types(FQN(), inventory);

        // Process the child variables.
        for (const auto var : variables()) {
            has_projected_dap4 |= var->is_dap4_projected(inventory);
        }
        // Process the child Groups.
        for (const auto grp : groups()) {
            has_projected_dap4 |= grp->is_dap4_projected(inventory);
        }
    }
    return has_projected_dap4;
}

} /* namespace libdap */
