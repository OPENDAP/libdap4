
// -*- C++ -*-

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <strstream.h>
#include <string>

#include "Regex.h"

#include "AttrTable.h"

using namespace CppUnit;

class AttrTableTest:public TestFixture {
private:
    AttrTable *at1;
    AttrTable *cont_a, *cont_b, *cont_c, *cont_ba, *cont_ca, *cont_caa;

public:
    AttrTableTest() {} 
    ~AttrTableTest() {} 

    void setUp() { 
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

    void tearDown() { 
	delete at1;
	at1 = 0;
    }

    bool re_match(Regex &r, const char *s) {
	return r.match(s, strlen(s)) == (int)strlen(s);
    }

    CPPUNIT_TEST_SUITE( AttrTableTest );

    CPPUNIT_TEST(find_container_test);
    CPPUNIT_TEST(find_test);
    CPPUNIT_TEST(copy_ctor);
    CPPUNIT_TEST(assignment);
    CPPUNIT_TEST(names_with_spaces_test);
    CPPUNIT_TEST(containers_with_spaces_test);

    CPPUNIT_TEST_SUITE_END();

    // Tests for methods
    void find_container_test() {
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

    void find_test() { 
	AttrTable *tmp;
	AttrTable::Attr_iter iter ;
	at1->find("a", &tmp, iter);
	CPPUNIT_ASSERT(tmp && iter != tmp->attr_end() && tmp->is_container(iter)
	       && tmp->get_name(iter) == "a");
	at1->find("a.size", &tmp, iter);
	CPPUNIT_ASSERT(tmp && iter != tmp->attr_end() && !tmp->is_container(iter)
	       && tmp->get_name(iter) == "size" && tmp->get_attr(iter) == "7");
	at1->find("b.type", &tmp, iter);
	CPPUNIT_ASSERT(tmp && iter != tmp->attr_end() && !tmp->is_container(iter)
	       && tmp->get_name(iter) == "type"
	       && tmp->get_attr(iter) == "houses");
	at1->find("c.ca.caa.color", &tmp, iter);
	CPPUNIT_ASSERT(tmp && iter != tmp->attr_end() && !tmp->is_container(iter)
	       && tmp->get_name(iter) == "color"
	       && tmp->get_attr(iter) == "red");
	at1->find("d.size", &tmp, iter);
	CPPUNIT_ASSERT(!tmp);
	at1->find("c.size", &tmp, iter);
	CPPUNIT_ASSERT(tmp == cont_c && iter == tmp->attr_end());
    } 

    void copy_ctor() {
	AttrTable at2 = *at1;
	AttrTable::Attr_iter piter = at2.attr_begin(); 
	CPPUNIT_ASSERT(at2.get_name(piter) == "a");
	CPPUNIT_ASSERT(at2.is_container(piter));
	AttrTable *tmp = at2.get_attr_table(piter);
	AttrTable::Attr_iter qiter = tmp->attr_begin();
	CPPUNIT_ASSERT(tmp->get_name(qiter) == "size");
	piter++ ;
	CPPUNIT_ASSERT(at2.get_name(piter) == "b");
	CPPUNIT_ASSERT(at2.is_container(piter));
	piter++ ;
	CPPUNIT_ASSERT(at2.get_name(piter) == "c");
	CPPUNIT_ASSERT(at2.is_container(piter));
    }

    void assignment() {
	AttrTable at2;
	at2 = *at1;

	AttrTable::Attr_iter piter = at2.attr_begin(); 
	CPPUNIT_ASSERT(at2.get_name(piter) == "a");
	CPPUNIT_ASSERT(at2.is_container(piter));
	AttrTable *tmp = at2.get_attr_table(piter);
	AttrTable::Attr_iter qiter = tmp->attr_begin();
	CPPUNIT_ASSERT(tmp->get_name(qiter) == "size");
	piter++ ;
	CPPUNIT_ASSERT(at2.get_name(piter) == "b");
	CPPUNIT_ASSERT(at2.is_container(piter));
	piter++ ;
	CPPUNIT_ASSERT(at2.get_name(piter) == "c");
	CPPUNIT_ASSERT(at2.is_container(piter));
    }

    void names_with_spaces_test() {
	// Create an AttrTable where some names have spaces. The spaces
	// should be replaced by %20 escapes.
	AttrTable *t = new AttrTable;
	t->append_attr("long name", "String", "first");
	t->append_attr("longer name", "String", "\"second test\"");
	ostrstream oss;
	t->print(oss, ""); oss << ends;
	Regex r("String long%20name first;
String longer%20name \"second test\";");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
	delete t; t = 0;
    }

    void containers_with_spaces_test() {
	AttrTable *top = new AttrTable;
	try {
	    AttrTable *cont = top->append_container("Data Field");
	    cont->append_attr("long name", "String", "first");
	    cont->add_value_alias(top, "an alias", "long name");
	}
	catch (Error &e) {
	    e.display_message();
	    CPPUNIT_ASSERT("Caught Error exception!" && false);
	}
	ostrstream oss;
	top->print(oss, ""); oss << ends;
	Regex r("Data%20Field {
.*String long%20name first;
.*Alias an%20alias long%20name;
}
");
	CPPUNIT_ASSERT(re_match(r, oss.str()));
	delete top; top = 0;

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(AttrTableTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    runner.run();

    return 0;
}

