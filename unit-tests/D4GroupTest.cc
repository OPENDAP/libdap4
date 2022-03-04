// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003,2013 OPeNDAP, Inc.
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "D4Group.h"
#include "D4Attributes.h"

#include "crc.h"
#include "Byte.h"
#include "Int64.h"
#include "Structure.h"

#include "XMLWriter.h"
#include "debug.h"

#include "testFile.h"
#include "run_tests_cppunit.h"
#include "test_config.h"






using namespace CppUnit;
using namespace std;
using namespace libdap;

class D4GroupTest: public TestFixture {
private:
    XMLWriter *xml;
    D4Group *root, *named;

public:
    D4GroupTest() :
        xml(0), root(0), named(0)
    {
    }

    ~D4GroupTest()
    {
    }

    void setUp()
    {
        root = new D4Group("/");
        named = new D4Group("test");
        xml = new XMLWriter;
    }

    void tearDown()
    {
        delete xml;
        delete root;
        delete named;
    }

    void load_group_with_scalars(D4Group *g)
    {
        g->add_var_nocopy(new Byte("b"));
        g->add_var_nocopy(new Int64("i64"));
    }

    void load_group_with_constructors_and_scalars(D4Group *g)
    {
        Structure *s = new Structure("s");
        s->add_var_nocopy(new Byte("b"));
        s->add_var_nocopy(new Int64("i64"));
        g->add_var_nocopy(s);
    }

    void load_group_with_nested_constructors_and_scalars(D4Group *g)
    {
        Structure *c = new Structure("c");
        c->add_var_nocopy(new Byte("b"));
        c->add_var_nocopy(new Int64("i64"));

        Structure *p = new Structure("p");
        p->add_var_nocopy(c);

        g->add_var_nocopy(p);
    }

    void load_group_with_stuff(D4Group *g)
    {
        g->dims()->add_dim_nocopy(new D4Dimension("lat", 1024));
        g->dims()->add_dim_nocopy(new D4Dimension("lon", 1024));
        g->dims()->add_dim_nocopy(new D4Dimension("time", 20));

        D4EnumDef *color_values = new D4EnumDef("colors", dods_byte_c);
        color_values->add_value("red", 1);
        color_values->add_value("green", 2);
        color_values->add_value("blue", 3);
        g->enum_defs()->add_enum_nocopy(color_values);

        D4Attribute *attr = new D4Attribute("test", StringToD4AttributeType("Int16"));
        attr->add_value("1");
        g->attributes()->add_attribute_nocopy(attr);
    }

    void test_cons()
    {
        D4Group g1 = D4Group("a", "b");
        CPPUNIT_ASSERT(g1.name() == "a" && g1.dataset() == "b");
    }

    void test_assignment()
    {
        D4Group g1 = D4Group("a", "b");
        D4Group g2 = D4Group("c", "d");
        //g2 = g1;
        CPPUNIT_ASSERT(g2.name() == "c" && g2.dataset() == "d");
        g2 = g1;
        CPPUNIT_ASSERT(g2.name() == "a" && g2.dataset() == "b");

    }

    void test_is_linear()
    {
        D4Group g1 = D4Group("a", "b");        
        CPPUNIT_ASSERT(!g1.is_linear());
    }

    void test_errors()
    {
        D4Group g1 = D4Group("a", "b");
        Crc32 cs;
        Constructor *c = &g1;
        CPPUNIT_ASSERT_THROW(g1.find_dim("/"), InternalErr);
        CPPUNIT_ASSERT_THROW(g1.find_enum_def("/"), InternalErr);
        CPPUNIT_ASSERT_THROW(g1.find_var("/"), InternalErr);
        CPPUNIT_ASSERT_THROW(g1.add_var(0), InternalErr);
        CPPUNIT_ASSERT_THROW(c->compute_checksum(cs), InternalErr);
    }

    void test_riter()
    {
        load_group_with_scalars(root);
        int count = 0;
        for (Constructor::Vars_riter r = root->var_rbegin(); r != root->var_rend(); r++) {
            CPPUNIT_ASSERT((count == 0 && (*r)->name() == "i64") || (count == 1 && (*r)->name() == "b"));
            count++;
        }
    }
    
    void test_get_iter()
    {
        load_group_with_scalars(root);
        int count = 0;
        for (Constructor::Vars_iter r = root->get_vars_iter(0); r != root->var_end(); r++) {
            CPPUNIT_ASSERT((count == 0 && (*r)->name() == "b") || (count == 1 && (*r)->name() == "i64"));
            count++;
        }
    }

    void test_get_var_index()
    {
        load_group_with_scalars(root);
        BaseType *bt = root->get_var_index(0);
        CPPUNIT_ASSERT(bt->name() == "b");
        bt = root->get_var_index(1);
        CPPUNIT_ASSERT(bt->name() == "i64");
    }

    void test_delete_var()
    {
        load_group_with_scalars(root);
        Constructor::Vars_iter r = root->get_vars_iter(0);        
        root->del_var(r);
        BaseType *bt = root->get_var_index(0);
        CPPUNIT_ASSERT(bt->name() == "i64");        
    }

    void test_size()
    {
        D4Group g1 = D4Group("a", "b");
        CPPUNIT_ASSERT(g1.request_size(true) == 0);
    }

    // An empty D4Group object prints nothing; the XMLWriter class adds
    // a xml doc preface.
    void test_print_empty()
    {
        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_empty.xml");
        DBG(cerr << "test_print_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_empty()
    {
        named->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_empty.xml");
        DBG(cerr << "test_print_named_empty: doc: " << doc << endl);
        DBG(cerr << "test_print_named_empty: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_with_vars()
    {
        load_group_with_scalars(root);

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_with_scalars.xml");
        DBG(cerr << "test_print_with_vars: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_with_vars()
    {
        load_group_with_scalars(named);

        named->print_dap4(*xml);

        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_with_scalars.xml");
        DBG(cerr << "test_print_named_with_vars: doc: " << doc << endl);
        DBG(cerr << "test_print_named_with_vars: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_with_vars_and_stuff()
    {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_with_scalars_and_stuff.xml");
        DBG(cerr << "test_print_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_named_with_vars_and_stuff()
    {
        load_group_with_scalars(named);
        load_group_with_stuff(named);

        named->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_named_with_scalars_and_stuff.xml");
        DBG(cerr << "test_print_named_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_named_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_find_var()
    {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group(child);

        child->dims()->add_dim_nocopy(new D4Dimension("extra", 17));

        // Used add_group() and not add_group_nocopy()
        delete child;

        BaseType *btp = root->find_var("/b");
        DBG(cerr << "btp: " << btp << ", name:" << btp->name() << endl);
        DBG(cerr << "btp->parent: " << btp->get_parent() << ", name:" << btp->get_parent()->name() << endl);
        CPPUNIT_ASSERT(btp && btp->name() == "b");
        CPPUNIT_ASSERT(btp->get_parent()->name() == "/");

        btp = root->find_var("/child/b");
        DBG(cerr << "btp: " << btp << ", name:" << btp->name() << endl);
        DBG(cerr << "btp->parent: " << btp->get_parent() << ", name:" << btp->get_parent()->name() << endl);
        CPPUNIT_ASSERT(btp && btp->name() == "b");
        CPPUNIT_ASSERT(btp->get_parent()->name() == "child" && btp->get_parent()->get_parent()->name() == "/");
    }

    void test_find_var_2()
    {
        D4Group *child = new D4Group("child");
        D4Group *g_child_1 = new D4Group("g_child_1");
        D4Group *g_child_2 = new D4Group("g_child_2");

        load_group_with_scalars(g_child_1);
        load_group_with_stuff(g_child_2);

        child->add_group_nocopy(g_child_1);
        child->add_group_nocopy(g_child_2);

        root->add_group_nocopy(child);

        g_child_1->add_var_nocopy(new Byte("byte_var_gc_1"));
        g_child_2->add_var_nocopy(new Byte("byte_var_gc_2"));

        auto bv_gc_1 = root->find_var("/child/g_child_1/byte_var_gc_1");
        CPPUNIT_ASSERT(bv_gc_1);

        auto bv_gc_2 = root->find_var("/child/g_child_2/byte_var_gc_2");
        CPPUNIT_ASSERT(bv_gc_2);

        auto its_not_there = root->find_var("/child/g_child_2/byte_var_gc_1");
        CPPUNIT_ASSERT(its_not_there == nullptr);
        // grp1 = new D4Group("Joey");
        // grp1->dims()->add_dim_nocopy(new D4Dimension("lat", 1024));
        // grp1->add_var_nocopy(new Byte("byte_var"));
    }
    void test_print_everything()
    {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group(child);

        child->dims()->add_dim_nocopy(new D4Dimension("extra", 17));

        // Used add_group() and not add_group_nocopy()
        delete child;

        root->print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_with_vars_and_stuff: doc: " << doc << endl);
        DBG(cerr << "test_print_with_vars_and_stuff: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_copy_ctor()
    {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group_nocopy(child); // add it using ...nocopy() this time

        D4Group lhs(*root);

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_copy_ctor: doc: " << doc << endl);
        DBG(cerr << "test_print_copy_ctor: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_print_assignment()
    {
        load_group_with_scalars(root);
        load_group_with_stuff(root);

        D4Group *child = new D4Group("child");
        load_group_with_scalars(child);
        load_group_with_stuff(child);
        root->add_group_nocopy(child);

        D4Group lhs = *root;

        lhs.print_dap4(*xml);
        string doc = xml->get_doc();
        string baseline = read_test_baseline(string(TEST_SRC_DIR) + "/D4-xml/D4Group_everything.xml");
        DBG(cerr << "test_print_assignment: doc: " << doc << endl);
        DBG(cerr << "test_print_assignment: baseline: " << baseline << endl);
        CPPUNIT_ASSERT(doc == baseline);
    }

    void test_fqn_1()
    {
        load_group_with_scalars(root);

        BaseType *btp = root->find_var("b");
        DBG(cerr << "test_fqn_1: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp->FQN() == "/b");
    }

    void test_fqn_2()
    {
        load_group_with_constructors_and_scalars(root);

        BaseType *btp = root->find_var("s.b");
        DBG(cerr << "test_fqn_2: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp->FQN() == "/s.b");

        btp = root->find_var("/s.b");
        DBG(cerr << "test_fqn_2: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp->FQN() == "/s.b");
    }

    void test_fqn_3()
    {
        load_group_with_nested_constructors_and_scalars(root);

        BaseType *btp = root->find_var("p.c.b");
        DBG(cerr << "test_fqn_3: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp->FQN() == "/p.c.b");

        btp = root->find_var("/p.c.b");
        DBG(cerr << "test_fqn_3: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp->FQN() == "/p.c.b");
    }

    void test_fqn_4()
    {
        D4Group *local = new D4Group("child");
        load_group_with_nested_constructors_and_scalars(local);
        root->add_group_nocopy(local);

        BaseType *btp = root->find_var("child/p.c.b");
        DBG(cerr << "test_fqn_4: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp && btp->FQN() == "/child/p.c.b");

        btp = root->find_var("/child/p.c.b");
        DBG(cerr << "test_fqn_4: " << btp->FQN() << endl);
        CPPUNIT_ASSERT(btp && btp->FQN() == "/child/p.c.b");
    }



    CPPUNIT_TEST_SUITE (D4GroupTest);

    CPPUNIT_TEST (test_assignment);
    CPPUNIT_TEST (test_cons);
    CPPUNIT_TEST (test_is_linear);
    CPPUNIT_TEST (test_errors);
    CPPUNIT_TEST (test_size);
    CPPUNIT_TEST (test_riter);
    CPPUNIT_TEST (test_get_iter);
    CPPUNIT_TEST (test_get_var_index);
    CPPUNIT_TEST (test_delete_var);
    
    CPPUNIT_TEST (test_print_empty);

    CPPUNIT_TEST (test_print_named_empty);
    CPPUNIT_TEST (test_print_with_vars);
    CPPUNIT_TEST (test_print_named_with_vars);

    CPPUNIT_TEST (test_print_with_vars_and_stuff);

    CPPUNIT_TEST (test_print_named_with_vars_and_stuff);
    CPPUNIT_TEST (test_print_everything);

    CPPUNIT_TEST (test_find_var);
    CPPUNIT_TEST (test_find_var_2);

    CPPUNIT_TEST (test_print_copy_ctor);
    CPPUNIT_TEST (test_print_assignment);

    CPPUNIT_TEST (test_fqn_1);
    CPPUNIT_TEST (test_fqn_2);
    CPPUNIT_TEST (test_fqn_3);
    CPPUNIT_TEST (test_fqn_4);

    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION (D4GroupTest);

int main(int argc, char *argv[])
{
    return run_tests<D4GroupTest>(argc, argv) ? 0: 1;
}
