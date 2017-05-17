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

#include "config.h"

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstring>
#include <string>
#include <sstream>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "GNURegex.h"
#include "AttrTable.h"
#include "debug.h"

#include "testFile.h"
#include "GetOpt.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) (x); } while(false);

using namespace CppUnit;
using namespace std;
using namespace libdap;

static string build_fqn(AttrTable *at, string fqn)
{
    // The strange behavior at the top level is because the top level of an
    // AttrTable (i.e. the DAS) is anonymous. Another bad design... jhrg 2/8/06
    if (!at || !at->get_parent() || at->get_name().empty())
        return fqn;
    else
        return build_fqn(at->get_parent(), at->get_name() + string(".") + fqn);
}

namespace libdap {

class AttrTableTest: public TestFixture {
private:
    AttrTable *at1;
    AttrTable *cont_a, *cont_b, *cont_c, *cont_ba, *cont_ca, *cont_caa;

public:
    AttrTableTest()
    {
    }
    ~AttrTableTest()
    {
    }

    void setUp()
    {
        at1 = new AttrTable;
        cont_a = at1->append_container("a");
        cont_a->append_attr("size", "Int32", "7");
        cont_a->append_attr("type", "String", "cars");

        cont_b = at1->append_container("b");
        cont_b->append_attr("number", "Int32", "1");
        cont_b->append_attr("type", "String", "houses");
        cont_ba = cont_b->append_container("ba");
        cont_ba->append_attr("name", "String", "fred");

        cont_c = at1->append_container("c");
        cont_ca = cont_c->append_container("ca");
        cont_caa = cont_ca->append_container("caa");
        cont_caa->append_attr("color", "String", "red");

        // This AttrTable looks like:
        //      Attributes {
        //          a {
        //              Int32 size 7;
        //              String type cars;
        //          }
        //          b {
        //              Int32 number 1;
        //              String type houses;
        //              ba {
        //                  String name fred;
        //              }
        //          }
        //          c {
        //              ca {
        //                  caa {
        //                      String color red;
        //                  }
        //              }
        //          }
        //      }
    }

    void tearDown()
    {
        delete at1;
        at1 = 0;
    }

    bool re_match(Regex &r, const char *s)
    {
        return r.match(s, strlen(s)) == (int) strlen(s);
    }

    CPPUNIT_TEST_SUITE (AttrTableTest);
#if 1

    CPPUNIT_TEST (clone_test);
    CPPUNIT_TEST (find_container_test);
    CPPUNIT_TEST (get_parent_test);
    CPPUNIT_TEST (recurrsive_find_test);
    CPPUNIT_TEST (find_test);
    CPPUNIT_TEST (copy_ctor);
    CPPUNIT_TEST (assignment);
    CPPUNIT_TEST (erase_test);
    CPPUNIT_TEST (names_with_spaces_test);
#endif
    CPPUNIT_TEST (containers_with_spaces_test);
#if 1
    CPPUNIT_TEST (get_attr_iter_test);
    CPPUNIT_TEST (del_attr_table_test);
    CPPUNIT_TEST (append_attr_vector_test);
#endif
#if 0
    CPPUNIT_TEST(print_xml_test);
#endif

    CPPUNIT_TEST_SUITE_END();

    // Tests for methods

    // This is to test for leaks in the clone() method.
    void clone_test()
    {
        AttrTable *att = new AttrTable;
        att->append_container(new AttrTable(*cont_a), "copy_of_a");
        delete att;
    }

    void recurrsive_find_test()
    {
        AttrTable::Attr_iter location;
        AttrTable *a = at1->recurrsive_find("color", &location);
        CPPUNIT_ASSERT(a && a == cont_caa && a->get_name(location) == "color");

        a = cont_caa->recurrsive_find("color", &location);
        CPPUNIT_ASSERT(a && a == cont_caa && a->get_name(location) == "color");

        a = at1->recurrsive_find("ba", &location);
        CPPUNIT_ASSERT(a && a == cont_b && a->get_name(location) == "ba");
    }

    void get_parent_test()
    {
        CPPUNIT_ASSERT(cont_caa->get_parent() == cont_ca);
        CPPUNIT_ASSERT(cont_ca->get_parent() == cont_c);
        CPPUNIT_ASSERT(cont_c->get_parent() == at1);
        CPPUNIT_ASSERT(at1->get_parent() == 0);
        CPPUNIT_ASSERT(build_fqn(cont_caa, "color") == "c.ca.caa.color");
    }

    void find_container_test()
    {
        AttrTable *tmp = at1->find_container("a");
        CPPUNIT_ASSERT(tmp != 0);
        CPPUNIT_ASSERT(tmp == cont_a);

        CPPUNIT_ASSERT(at1->find_container("b.ba") == cont_ba);
        CPPUNIT_ASSERT(at1->find_container("a.b") == 0);
        CPPUNIT_ASSERT(at1->find_container("c.ca.caa") == cont_caa);
        CPPUNIT_ASSERT(at1->find_container("caa") == 0);

        CPPUNIT_ASSERT(at1->find_container("a.size") == 0);
        CPPUNIT_ASSERT(at1->find_container("b.ba.name") == 0);
    }

    void find_test()
    {
        AttrTable *tmp;
        AttrTable::Attr_iter iter;
        at1->find("a", &tmp, &iter);
        CPPUNIT_ASSERT(tmp && iter != tmp->attr_end() && tmp->is_container(iter) && tmp->get_name(iter) == "a");
        at1->find("a.size", &tmp, &iter);
        CPPUNIT_ASSERT(
            tmp && iter != tmp->attr_end() && !tmp->is_container(iter) && tmp->get_name(iter) == "size"
                && tmp->get_attr(iter) == "7");
        at1->find("b.type", &tmp, &iter);
        CPPUNIT_ASSERT(
            tmp && iter != tmp->attr_end() && !tmp->is_container(iter) && tmp->get_name(iter) == "type"
                && tmp->get_attr(iter) == "houses");
        at1->find("c.ca.caa.color", &tmp, &iter);
        CPPUNIT_ASSERT(
            tmp && iter != tmp->attr_end() && !tmp->is_container(iter) && tmp->get_name(iter) == "color"
                && tmp->get_attr(iter) == "red");
        at1->find("d.size", &tmp, &iter);
        CPPUNIT_ASSERT(!tmp);
        at1->find("c.size", &tmp, &iter);
        CPPUNIT_ASSERT(tmp == cont_c && iter == tmp->attr_end());
    }

    void copy_ctor()
    {
        AttrTable at2 = *at1;
        AttrTable::Attr_iter piter = at2.attr_begin();
        CPPUNIT_ASSERT(at2.get_name(piter) == "a");
        CPPUNIT_ASSERT(at2.is_container(piter));
        AttrTable *tmp = at2.get_attr_table(piter);
        AttrTable::Attr_iter qiter = tmp->attr_begin();
        CPPUNIT_ASSERT(tmp->get_name(qiter) == "size");
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "b");
        CPPUNIT_ASSERT(at2.is_container(piter));
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "c");
        CPPUNIT_ASSERT(at2.is_container(piter));
    }

    void assignment()
    {
        AttrTable at2;
        at2 = *at1;

        AttrTable::Attr_iter piter = at2.attr_begin();
        CPPUNIT_ASSERT(at2.get_name(piter) == "a");
        CPPUNIT_ASSERT(at2.is_container(piter));
        AttrTable *tmp = at2.get_attr_table(piter);
        AttrTable::Attr_iter qiter = tmp->attr_begin();
        CPPUNIT_ASSERT(tmp->get_name(qiter) == "size");
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "b");
        CPPUNIT_ASSERT(at2.is_container(piter));
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "c");
        CPPUNIT_ASSERT(at2.is_container(piter));
    }

    void erase_test()
    {
        // Copy at1 to at2 and verify that at2 is full of stuff...
        AttrTable at2 = *at1;
        AttrTable::Attr_iter piter = at2.attr_begin();
        CPPUNIT_ASSERT(at2.get_name(piter) == "a");
        CPPUNIT_ASSERT(at2.is_container(piter));
        AttrTable *tmp = at2.get_attr_table(piter);
        AttrTable::Attr_iter qiter = tmp->attr_begin();
        CPPUNIT_ASSERT(tmp->get_name(qiter) == "size");
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "b");
        CPPUNIT_ASSERT(at2.is_container(piter));
        piter++;
        CPPUNIT_ASSERT(at2.get_name(piter) == "c");
        CPPUNIT_ASSERT(at2.is_container(piter));

        at2.erase();
        CPPUNIT_ASSERT(at2.attr_map.size() == 0);
        CPPUNIT_ASSERT(at2.d_name == "");
    }

    void names_with_spaces_test()
    {
        // Create an AttrTable where some names have spaces. The spaces
        // should be replaced by %20 escapes.
        // Replacing the spaces with %20 was the bad, old, behavior. Now
        // the spaces can stay. If someone is writing a DAS using the {}
        // notation, they can use '%20' for the spaces. In the printed
        // DAS using the {} notation, spaces will be represented by %20.
        AttrTable *t = new AttrTable;
        t->append_attr("long name", "String", "first");
        t->append_attr("longer name", "String", "\"second test\"");
        //string sof;
        ostringstream oss;
        t->print(oss, "");
        //FILE2string(sof, of, t->print(of, ""));
        string attrs = "String long%20name \"first\";\n\
String longer%20name \"second test\";";
        //CPPUNIT_ASSERT(sof.find(attrs) != string::npos);
        CPPUNIT_ASSERT(oss.str().find(attrs) != string::npos);
        delete t;
        t = 0;
    }

    void containers_with_spaces_test()
    {
        AttrTable *top = new AttrTable;
        try {
            AttrTable *cont = top->append_container("Data Field");
            cont->append_attr("long name", "String", "first");
            cont->add_value_alias(top, "an alias", "long name");
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT("Caught Error exception!" && false);
        }
        try {
            ostringstream oss;
            top->print(oss);
            Regex r(
                ".*Data%20Field \\{\n\
.*String long%20name \"first\";\n\
.*Alias an%20alias long%20name;\n\
.*\\}\n");
            DBG(cout << ">" << oss.str() << "<" << endl);
            CPPUNIT_ASSERT(re_match(r, oss.str().c_str()));
            delete top;
            top = 0;
        }
        catch (Error &e) {
            cerr << e.get_error_message() << endl;
            CPPUNIT_ASSERT("Caught Error exception!" && false);
        }
    }

    void get_attr_iter_test()
    {
        int n = at1->get_size();
        CPPUNIT_ASSERT(n == 3);

        AttrTable::Attr_iter i = at1->get_attr_iter(0);
        CPPUNIT_ASSERT(at1->get_name(i) == "a");
        i = at1->get_attr_iter(2);
        CPPUNIT_ASSERT(at1->get_name(i) == "c");

        i = at1->get_attr_iter(1);
        CPPUNIT_ASSERT(at1->is_container(i));
        AttrTable *t1 = at1->get_attr_table(i);
        AttrTable::Attr_iter k = t1->get_attr_iter(1);
        CPPUNIT_ASSERT(t1->get_name(k) == "type");
        CPPUNIT_ASSERT(t1->get_attr(k, 0) == "houses");
    }

    void del_attr_table_test()
    {
        AttrTable *b = at1->find_container("b");
        AttrTable::Attr_iter i = b->attr_begin();
        CPPUNIT_ASSERT(b->get_name(i) == "number");
        i += 2;
        CPPUNIT_ASSERT(b->get_name(i) == "ba");

        b->del_attr_table(i);

        i = b->attr_begin();
        CPPUNIT_ASSERT(b->get_name(i) == "number");
        i += 2;
        CPPUNIT_ASSERT(i == b->attr_end());

        // try a second table. at2 contains a scalar attribute followed by a
        // container named 'a'.
        AttrTable *at2;
        try {
            at2 = new AttrTable;
            at2->set_name("at2");
            at2->append_attr("color", "String", "red");
            AttrTable *cont_at2 = at2->append_container("cont_at2");
            cont_at2->append_attr("size", "Int32", "7");
            cont_at2->append_attr("type", "String", "cars");
            i = at2->attr_begin();
            CPPUNIT_ASSERT(at2->get_name(i) == "color");
            i++;
            CPPUNIT_ASSERT(at2->get_name(i) == "cont_at2");

            at2->del_attr_table(i);

            i = at2->attr_begin();
            CPPUNIT_ASSERT(at2->get_name(i) == "color");
            i++;
            CPPUNIT_ASSERT(i == at2->attr_end());

            delete at2;
            at2 = 0;
        }
        catch (Error &e) {
            cerr << "Error: " << e.get_error_message() << endl;
            delete at2;
            at2 = 0;
            throw;
        }
        catch (...) {
            cerr << "caught an exception!" << endl;
            delete at2;
            at2 = 0;
            throw;
        }
    }

    void append_attr_vector_test()
    {
        // ("size", "Int32", "7")
        vector<string> vs;
        vs.push_back("8");
        vs.push_back("9");
        cont_a->append_attr("size", "Int32", &vs);

        CPPUNIT_ASSERT(cont_a->get_attr("size", 0) == "7");
        CPPUNIT_ASSERT(cont_a->get_attr("size", 1) == "8");
        CPPUNIT_ASSERT(cont_a->get_attr("size", 2) == "9");
        CPPUNIT_ASSERT(cont_a->get_attr_num("size") == 3);
    }

    void print_xml_test()
    {
        at1->print_xml(stdout);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (AttrTableTest);

} // namespace libdap

int main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, "dh");
    int option_char;

    while ((option_char = getopt()) != -1)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;

        case 'h': {     // help - show test names
            cerr << "Usage: AttrTableTest has the following tests:" << endl;
            const std::vector<Test*> &tests = libdap::AttrTableTest::suite()->getTests();
            unsigned int prefix_len = libdap::AttrTableTest::suite()->getName().append("::").length();
            for (std::vector<Test*>::const_iterator i = tests.begin(), e = tests.end(); i != e; ++i) {
                cerr << (*i)->getName().replace(0, prefix_len, "") << endl;
            }
            break;
        }

        default:
            break;
        }

    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        for (; i < argc; ++i) {
            if (debug) cerr << "Running " << argv[i] << endl;
            test = libdap::AttrTableTest::suite()->getName().append("::").append(argv[i]);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    return wasSuccessful ? 0 : 1;
}
