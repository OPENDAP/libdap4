
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
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

#ifndef _constructor_h
#define _constructor_h 1

#include <vector>

#include "BaseType.h"

class Crc32;

namespace libdap {

class DMR;
class XMLWriter;
class D4StreamUnMarshaller;

/** Common methods for all constructor types. */
class Constructor : public BaseType {
private:
    void m_duplicate(const Constructor &s);

protected:
    /** @brief Child variables owned by this constructor instance. */
    std::vector<BaseType *> d_vars;

    /**
     * @brief Finds a descendant variable by leaf-name match.
     * @param name Name fragment to match.
     * @param s Optional stack used to record the match path.
     * @return Matching variable, or null when none is found.
     */
    BaseType *m_leaf_match(const string &name, btp_stack *s = nullptr);
    /**
     * @brief Finds a descendant variable by exact fully-qualified match.
     * @param name Fully-qualified name to match.
     * @param s Optional stack used to record the match path.
     * @return Matching variable, or null when none is found.
     */
    BaseType *m_exact_match(const string &name, btp_stack *s = nullptr);

    /**
     * @brief Constructs a constructor type with name and explicit type.
     * @param name Variable name.
     * @param type Concrete constructor type.
     * @param is_dap4 True when this variable is part of a DAP4 model.
     */
    Constructor(const string &name, const Type &type, bool is_dap4 = false) : BaseType(name, type, is_dap4) {}
    /**
     * @brief Constructs a constructor type with dataset declaration metadata.
     * @param name Variable name.
     * @param dataset Declaration context.
     * @param type Concrete constructor type.
     * @param is_dap4 True when this variable is part of a DAP4 model.
     */
    Constructor(const string &name, const string &dataset, const Type &type, bool is_dap4 = false)
        : BaseType(name, dataset, type, is_dap4) {}

    /**
     * @brief Copy-constructs a constructor and duplicates child variables.
     * @param copy_from Source constructor.
     */
    Constructor(const Constructor &copy_from) : BaseType(copy_from) { m_duplicate(copy_from); }

public:
    /** @brief Iterator type for read-only traversal of child variables. */
    typedef std::vector<BaseType *>::const_iterator Vars_citer;
    /** @brief Iterator type for mutable traversal of child variables. */
    typedef std::vector<BaseType *>::iterator Vars_iter;
    /** @brief Reverse iterator type for mutable traversal of child variables. */
    typedef std::vector<BaseType *>::reverse_iterator Vars_riter;

    Constructor() = delete; // Why? jhrg 4/25/22

    ~Constructor() override {
        for (auto var : d_vars)
            delete var;
    }

    /**
     * @brief Assigns this constructor from another.
     * @param rhs Source constructor.
     * @return This instance after assignment.
     */
    Constructor &operator=(const Constructor &rhs) {
        if (this == &rhs)
            return *this;
        BaseType::operator=(rhs);
        m_duplicate(rhs);
        return *this;
    }

    void transform_to_dap4(D4Group *root, Constructor *dest) override;

    std::string FQN() const override;

    int element_count(bool leaves = false) override;

    void set_send_p(bool state) override;
    void set_read_p(bool state) override;

    unsigned int width(bool constrained = false) const override;
    int64_t width_ll(bool constrained = false) const override;

    /// btp_stack no longer needed; use back pointers (BaseType::get_parent())
    BaseType *var(const string &name, bool exact_match = true, btp_stack *s = nullptr) override;

    /// btp_stack no longer needed; use back pointers (BaseType::get_parent())
    /// @deprecated
    BaseType *var(const string &n, btp_stack &s) override;

    Vars_iter var_begin();
    Vars_iter var_end();
    Vars_riter var_rbegin();
    Vars_riter var_rend();
    Vars_iter get_vars_iter(int i);
    BaseType *get_var_index(int i);
    void set_var_index(BaseType *bt, int i);

    /**
     * Get a const reference to the vector of BaseType pointers.
     * @note Use this in range-based for loops to iterate over the variables.
     * @return A const reference to the vector of BaseType pointers.
     */
    const vector<BaseType *> &variables() const { return d_vars; }

    bool is_dap4_projected(std::vector<std::string> &inventory) override;

    void add_var(BaseType *bt, Part part = nil) override;
    void add_var_nocopy(BaseType *bt, Part part = nil) override;

    virtual void del_var(const string &name);
    virtual void del_var(Vars_iter i);

    bool read() override;

    // DAP2
    void intern_data(ConstraintEvaluator &eval, DDS &dds) override;
    bool serialize(ConstraintEvaluator &eval, DDS &dds, Marshaller &m, bool ce_eval = true) override;
    bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false) override;

    // DAP4
    void compute_checksum(Crc32 &checksum) override;
    /** @brief Reads any required DAP4-side data into this constructor. */
    void intern_data() override;
    void serialize(D4StreamMarshaller &m, DMR &dmr, bool filter = false) override;
    void deserialize(D4StreamUnMarshaller &um, DMR &dmr) override;

    // Do not store values in memory as for C; users work with the C++ objects for this class
    unsigned int val2buf(void *, bool) override {
        throw InternalErr(__FILE__, __LINE__, "Never use this method; see the programmer's guide documentation.");
    }
    unsigned int buf2val(void **) override {
        throw InternalErr(__FILE__, __LINE__, "Never use this method; see the programmer's guide documentation.");
    }

    virtual bool is_linear();

    void set_in_selection(bool state) override;

    /**
     * @brief Prints the declaration using C++ streams.
     * @param out Output stream.
     * @param space Indentation prefix.
     * @param print_semi True to print a trailing semicolon.
     * @param constraint_info True to include projection details.
     * @param constrained True to print constrained declarations.
     * @param is_root_grp True when printing in root-group context.
     * @param array_member True when printing as an array member declaration.
     */
    void print_decl(ostream &out, string space = "    ", bool print_semi = true, bool constraint_info = false,
                    bool constrained = false, bool is_root_grp = true, bool array_member = false) override;

    void print_xml(ostream &out, string space = "    ", bool constrained = false) override;

    void print_dap4(XMLWriter &xml, bool constrained = false) override;

    /**
     * @brief Prints XML representation using an XML writer.
     * @param xml Output XML writer.
     * @param constrained True to emit constrained form.
     */
    void print_xml_writer(XMLWriter &xml, bool constrained = false) override;

    /**
     * @brief Prints the declaration using C stdio.
     * @param out Output file stream.
     * @param space Indentation prefix.
     * @param print_semi True to print a trailing semicolon.
     * @param constraint_info True to include projection details.
     * @param constrained True to print constrained declarations.
     * @param is_root_grp True when printing in root-group context.
     * @param array_member True when printing as an array member declaration.
     */
    void print_decl(FILE *out, string space = "    ", bool print_semi = true, bool constraint_info = false,
                    bool constrained = false, bool is_root_grp = true, bool array_member = false) override;

    void print_xml(FILE *out, string space = "    ", bool constrained = false) override;

    /**
     * @brief Prints values using C stdio.
     * @param out Output file stream.
     * @param space Indentation prefix.
     * @param print_decl_p True to include declaration text.
     * @param is_root_grp True when printing in root-group context.
     */
    void print_val(FILE *out, string space = "", bool print_decl_p = true, bool is_root_grp = true) override;
    /**
     * @brief Prints values using C++ streams.
     * @param out Output stream.
     * @param space Indentation prefix.
     * @param print_decl_p True to include declaration text.
     * @param is_root_grp True when printing in root-group context.
     */
    void print_val(ostream &out, string space = "", bool print_decl_p = true, bool is_root_grp = true) override;

    bool check_semantics(string &msg, bool all = false) override;

    void transfer_attributes(AttrTable *at) override;
    /**
     * @brief Builds an attribute table that stores dropped-variable metadata.
     * @param dropped_vars Variables omitted from a transformed response.
     * @return Newly allocated attribute table describing dropped variables.
     */
    static AttrTable *make_dropped_vars_attr_table(vector<BaseType *> *dropped_vars);

    void dump(ostream &strm) const override;
};

} // namespace libdap

#endif // _constructor_h
