// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include "AttrTable.h"
#include "BaseType.h"
#include "BaseTypeFactory.h"
#include "ConstraintEvaluator.h"
#include "DDS.h"
#include "D4Attributes.h"
#include "D4Group.h"
#include "D4StreamMarshaller.h"
#include "D4StreamUnMarshaller.h"
#include "DMR.h"
#include "Error.h"
#include "InternalErr.h"
#include "Marshaller.h"
#include "Structure.h"
#include "UnMarshaller.h"
#include "XMLWriter.h"
#include "run_tests_cppunit.h"

using namespace CppUnit;
using namespace libdap;
using namespace std;

namespace {

class DummyBase : public BaseType {
public:
    DummyBase(const string &n, const Type &t, bool is_dap4 = false) : BaseType(n, t, is_dap4) {}
    DummyBase(const string &n, const string &d, const Type &t, bool is_dap4 = false) : BaseType(n, d, t, is_dap4) {}
    DummyBase(const DummyBase &rhs) : BaseType(rhs) {}
    DummyBase &operator=(const DummyBase &rhs) = default;

    BaseType *ptr_duplicate() override { return new DummyBase(*this); }

    unsigned int buf2val(void **val) override {
        if (!val)
            throw InternalErr(__FILE__, __LINE__, "NULL pointer");
        if (!*val)
            *val = new int(7);
        return sizeof(int);
    }

    unsigned int val2buf(void *val, bool) override {
        if (!val)
            throw InternalErr(__FILE__, __LINE__, "NULL pointer");
        return sizeof(int);
    }

    void compute_checksum(Crc32 &checksum) override {
        (void)checksum;
    }

    void print_val(ostream &out, string space = "", bool print_decl_p = true, bool is_root_grp = true) override {
        if (print_decl_p) {
            print_decl(out, space, true, false, false, is_root_grp);
            out << " = 7;\n";
        } else {
            out << "7";
        }
    }

    void set_parent_public(BaseType *p) { BaseType::set_parent(p); }
};

class ReadableBase : public DummyBase {
public:
    int read_count = 0;

    ReadableBase(const string &n, const Type &t, bool is_dap4 = false) : DummyBase(n, t, is_dap4) {}

    bool read() override {
        ++read_count;
        set_read_p(true);
        return true;
    }
};

class FakeMarshaller : public Marshaller {
public:
    void put_byte(dods_byte) override {}
    void put_int16(dods_int16) override {}
    void put_int32(dods_int32) override {}
    void put_float32(dods_float32) override {}
    void put_float64(dods_float64) override {}
    void put_uint16(dods_uint16) override {}
    void put_uint32(dods_uint32) override {}
    void put_str(const string &) override {}
    void put_url(const string &) override {}
    void put_opaque(char *, unsigned int) override {}
    void put_int(int) override {}
    void put_vector(char *, int, Vector &) override {}
    void put_vector(char *, int, int, Vector &) override {}
    void dump(ostream &strm) const override { strm << "FakeMarshaller"; }
};

class FakeUnMarshaller : public UnMarshaller {
public:
    void get_byte(dods_byte &) override {}
    void get_int16(dods_int16 &) override {}
    void get_int32(dods_int32 &) override {}
    void get_float32(dods_float32 &) override {}
    void get_float64(dods_float64 &) override {}
    void get_uint16(dods_uint16 &) override {}
    void get_uint32(dods_uint32 &) override {}
    void get_str(string &) override {}
    void get_url(string &) override {}
    void get_opaque(char *, unsigned int) override {}
    void get_int(int &) override {}
    void get_vector(char **, unsigned int &, Vector &) override {}
    void get_vector(char **, unsigned int &, int, Vector &) override {}
    void dump(ostream &strm) const override { strm << "FakeUnMarshaller"; }
};

static D4Attributes *make_d4_attrs_with_int8(const string &name) {
    auto *attrs = new D4Attributes();
    auto *attr = new D4Attribute(name, attr_int8_c);
    attr->add_value("1");
    attrs->add_attribute_nocopy(attr);
    return attrs;
}

} // namespace

class BaseTypeTest : public TestFixture {
    CPPUNIT_TEST_SUITE(BaseTypeTest);
    CPPUNIT_TEST(test_to_string_and_dump);
    CPPUNIT_TEST(test_copy_and_assignment_deep_attributes);
    CPPUNIT_TEST(test_transform_to_dap4);
    CPPUNIT_TEST(test_transform_to_dap2_empty_attr_table);
    CPPUNIT_TEST(test_transform_to_dap2_preserve_attr_table);
    CPPUNIT_TEST(test_fqn_and_print_decl_dap4);
    CPPUNIT_TEST(test_print_decl_constrained_and_constraint_info);
    CPPUNIT_TEST(test_print_xml_writer_dap2_and_dap4);
    CPPUNIT_TEST(test_transfer_attributes_adds);
    CPPUNIT_TEST(test_transfer_attributes_missing_container);
    CPPUNIT_TEST(test_set_parent_validation_and_get_ancestor);
    CPPUNIT_TEST(test_read_default_and_intern_data);
    CPPUNIT_TEST(test_intern_data_dap4_throws);
    CPPUNIT_TEST(test_serialize_deserialize_throws);
    CPPUNIT_TEST(test_ops_and_width_throw);
    CPPUNIT_TEST(test_is_dap4_projected);
    CPPUNIT_TEST_SUITE_END();

public:
    void test_to_string_and_dump() {
        DummyBase b("a", "dataset", dods_int32_c, false);
        string s = b.toString();
        CPPUNIT_ASSERT(s.find("a") != string::npos);
        CPPUNIT_ASSERT(s.find("Int32") != string::npos);
        CPPUNIT_ASSERT(s.find("dataset") != string::npos);

        ostringstream oss;
        b.dump(oss);
        string d = oss.str();
        CPPUNIT_ASSERT(d.find("BaseType::dump") != string::npos);
        CPPUNIT_ASSERT(d.find("name: a") != string::npos);
    }

    void test_copy_and_assignment_deep_attributes() {
        DummyBase a("a", dods_int32_c, true);
        a.get_attr_table().append_attr("units", "String", "m");
        a.set_attributes_nocopy(make_d4_attrs_with_int8("d4"));

        DummyBase copy(a);
        CPPUNIT_ASSERT(copy.get_attr_table().get_size() == a.get_attr_table().get_size());
        CPPUNIT_ASSERT(copy.attributes() != a.attributes());
        CPPUNIT_ASSERT(copy.attributes()->find("d4") != nullptr);

        DummyBase assigned("b", dods_int32_c, true);
        assigned = a;
        CPPUNIT_ASSERT(assigned.get_attr_table().get_size() == a.get_attr_table().get_size());
        CPPUNIT_ASSERT(assigned.attributes() != a.attributes());
        CPPUNIT_ASSERT(assigned.attributes()->find("d4") != nullptr);
    }

    void test_transform_to_dap4() {
        DummyBase src("var", dods_int32_c, false);
        src.get_attr_table().append_attr("units", "String", "m");

        D4Group root("/");
        src.transform_to_dap4(&root, &root);

        BaseType *dest = root.var("var", true);
        CPPUNIT_ASSERT(dest != nullptr);
        CPPUNIT_ASSERT(dest->is_dap4());
        CPPUNIT_ASSERT(!dest->attributes()->empty());
    }

    void test_transform_to_dap2_empty_attr_table() {
        DummyBase src("v", dods_int32_c, true);
        src.set_attributes_nocopy(make_d4_attrs_with_int8("d4"));

        vector<BaseType *> *vars = src.transform_to_dap2(nullptr, false);
        CPPUNIT_ASSERT(vars != nullptr);
        CPPUNIT_ASSERT(vars->size() == 1);
        BaseType *dest = (*vars)[0];

        CPPUNIT_ASSERT(!dest->is_dap4());
        CPPUNIT_ASSERT(dest->get_attr_table().get_size() > 0);
        CPPUNIT_ASSERT(dest->get_attr_table().get_name() == "v");

        delete dest;
        delete vars;
    }

    void test_transform_to_dap2_preserve_attr_table() {
        DummyBase src("v", dods_int32_c, true);
        src.get_attr_table().append_attr("units", "String", "m");

        vector<BaseType *> *vars = src.transform_to_dap2(nullptr, false);
        CPPUNIT_ASSERT(vars != nullptr);
        BaseType *dest = (*vars)[0];

        CPPUNIT_ASSERT(!dest->is_dap4());
        CPPUNIT_ASSERT(dest->get_attr_table().get_size() == 1);

        delete dest;
        delete vars;
    }

    void test_fqn_and_print_decl_dap4() {
        DummyBase top("top", dods_int32_c, false);
        CPPUNIT_ASSERT_EQUAL(string("top"), top.FQN());

        Structure parent("parent");
        auto *child = new DummyBase("child", dods_int32_c, false);
        parent.add_var_nocopy(child);
        CPPUNIT_ASSERT_EQUAL(string("parent.child"), child->FQN());

        D4Group root("/");
        auto *grp = new D4Group("grp");
        root.add_group_nocopy(grp);

        auto *var = new DummyBase("var", dods_int32_c, true);
        grp->add_var_nocopy(var);

        CPPUNIT_ASSERT_EQUAL(string("/grp/var"), var->FQN());

        ostringstream oss;
        var->print_decl(oss, "", true, false, false, false);
        CPPUNIT_ASSERT(oss.str().find("/grp/var") != string::npos);

        ostringstream oss2;
        var->print_decl(oss2, "", true, false, false, false, true);
        CPPUNIT_ASSERT(oss2.str().find("/grp/") != string::npos);
    }

    void test_print_decl_constrained_and_constraint_info() {
        DummyBase b("x", dods_int32_c, false);
        b.set_send_p(false);

        ostringstream oss;
        b.print_decl(oss, "", true, false, true);
        CPPUNIT_ASSERT(oss.str().empty());

        b.set_send_p(true);
        ostringstream oss2;
        b.print_decl(oss2, "", true, true, false);
        CPPUNIT_ASSERT(oss2.str().find("Send True") != string::npos);

        FILE *tmp = tmpfile();
        CPPUNIT_ASSERT(tmp != nullptr);
        b.print_decl(tmp, "", true, false, false, true);
        fclose(tmp);
    }

    void test_print_xml_writer_dap2_and_dap4() {
        DummyBase dap2("a", dods_int32_c, false);
        dap2.get_attr_table().append_attr("units", "String", "m");
        XMLWriter xml2;
        dap2.print_xml_writer(xml2, false);
        string doc2 = xml2.get_doc();
        CPPUNIT_ASSERT(doc2.find("units") != string::npos);

        DummyBase dap4("b", dods_int32_c, true);
        dap4.set_attributes_nocopy(make_d4_attrs_with_int8("d4"));
        XMLWriter xml4;
        dap4.print_xml_writer(xml4, false);
        string doc4 = xml4.get_doc();
        CPPUNIT_ASSERT(doc4.find("name=\"b\"") != string::npos);

        DummyBase constrained("c", dods_int32_c, false);
        constrained.set_send_p(false);
        XMLWriter xmlc;
        constrained.print_xml_writer(xmlc, true);
        string docc = xmlc.get_doc();
        CPPUNIT_ASSERT(docc.find("name=\"c\"") == string::npos);

        ostringstream oss;
        dap2.print_xml(oss, "", false);
        CPPUNIT_ASSERT(oss.str().find("units") != string::npos);

        FILE *tmp = tmpfile();
        CPPUNIT_ASSERT(tmp != nullptr);
        dap2.print_xml(tmp, "", false);
        fclose(tmp);
    }

    void test_transfer_attributes_adds() {
        DummyBase b("var", dods_int32_c, false);
        AttrTable root;
        AttrTable *container = root.append_container("var");
        container->append_attr("units", "String", "m");
        AttrTable *sub = container->append_container("sub");
        sub->append_attr("color", "String", "red");

        b.transfer_attributes(&root);
        AttrTable &bt_attrs = b.get_attr_table();
        CPPUNIT_ASSERT(bt_attrs.get_size() >= 2);
        CPPUNIT_ASSERT(bt_attrs.get_attr("units") == "m");
        CPPUNIT_ASSERT(bt_attrs.get_attr_table("sub") != nullptr);
    }

    void test_transfer_attributes_missing_container() {
        DummyBase b("var", dods_int32_c, false);
        AttrTable root;
        root.append_container("other");

        b.transfer_attributes(&root);
        CPPUNIT_ASSERT(b.get_attr_table().get_size() == 0);
    }

    void test_set_parent_validation_and_get_ancestor() {
        DummyBase child("child", dods_int32_c, false);
        DummyBase bad_parent("bad", dods_int32_c, false);
        CPPUNIT_ASSERT_THROW(child.set_parent_public(&bad_parent), InternalErr);

        D4Group root("/");
        auto *grp = new D4Group("grp");
        root.add_group_nocopy(grp);

        auto *var = new DummyBase("var", dods_int32_c, false);
        grp->add_var_nocopy(var);

        BaseType *ancestor = var->get_ancestor();
        CPPUNIT_ASSERT(ancestor == &root);
    }

    void test_read_default_and_intern_data() {
        DummyBase b("v", dods_int32_c, false);
        CPPUNIT_ASSERT_THROW(b.read(), InternalErr);

        b.set_read_p(true);
        CPPUNIT_ASSERT(b.read());

        DummyBase synth("s", dods_int32_c, false);
        synth.set_synthesized_p(true);
        synth.set_read_p(true);
        CPPUNIT_ASSERT(!synth.read_p());

        ReadableBase r("r", dods_int32_c, false);
        ConstraintEvaluator eval;
        BaseTypeFactory factory;
        DDS dds(&factory);
        r.intern_data(eval, dds);
        CPPUNIT_ASSERT(r.read_p());
        CPPUNIT_ASSERT(r.read_count == 1);

        ReadableBase r2("r2", dods_int32_c, false);
        r2.intern_data();
        CPPUNIT_ASSERT(r2.read_p());
        CPPUNIT_ASSERT(r2.read_count == 1);
    }

    void test_intern_data_dap4_throws() {
        DummyBase b("v", dods_int32_c, true);
        ConstraintEvaluator eval;
        BaseTypeFactory factory;
        DDS dds(&factory);
        CPPUNIT_ASSERT_THROW(b.intern_data(eval, dds), Error);
    }

    void test_serialize_deserialize_throws() {
        DummyBase b("v", dods_int32_c, false);
        ConstraintEvaluator eval;
        BaseTypeFactory factory;
        DDS dds(&factory);
        FakeMarshaller fm;
        FakeUnMarshaller fu;

        CPPUNIT_ASSERT_THROW(b.serialize(eval, dds, fm, false), InternalErr);
        CPPUNIT_ASSERT_THROW(b.deserialize(fu, &dds, false), InternalErr);

        stringstream ss(ios::in | ios::out | ios::binary);
        D4StreamMarshaller d4m(ss, true, false);
        D4StreamUnMarshaller d4u(ss, false);
        DMR dmr;

        CPPUNIT_ASSERT_THROW(b.serialize(d4m, dmr, false), InternalErr);
        CPPUNIT_ASSERT_THROW(b.deserialize(d4u, dmr), InternalErr);
    }

    void test_ops_and_width_throw() {
        DummyBase b("v", dods_int32_c, false);
        DummyBase other("o", dods_int32_c, false);
        CPPUNIT_ASSERT_THROW(b.ops(&other, 0), InternalErr);
        CPPUNIT_ASSERT_THROW(b.d4_ops(&other, 0), InternalErr);
        CPPUNIT_ASSERT_THROW(b.width(false), InternalErr);
        CPPUNIT_ASSERT_THROW(b.width_ll(false), InternalErr);
    }

    void test_is_dap4_projected() {
        DummyBase b("v", dods_int32_c, true);
        vector<string> inventory;
        b.set_send_p(false);
        CPPUNIT_ASSERT(!b.is_dap4_projected(inventory));
        CPPUNIT_ASSERT(inventory.empty());

        b.set_send_p(true);
        b.set_attributes_nocopy(make_d4_attrs_with_int8("d4"));
        CPPUNIT_ASSERT(b.is_dap4_projected(inventory));
        CPPUNIT_ASSERT(!inventory.empty());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BaseTypeTest);

int main(int argc, char *argv[]) { return run_tests<BaseTypeTest>(argc, argv) ? 0 : 1; }
