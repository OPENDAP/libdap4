
// Tests for the AttrTable class.

#include "TestCase.h"
#include "TestCaller.h"
#include "TestSuite.h"

#include <strstream.h>
#include <Regex.h>

#include "AttrTable.h"

class AttrTableTest:public TestCase {
private:
    AttrTable *at1;
    AttrTable *cont_a, *cont_b, *cont_c, *cont_ba, *cont_ca, *cont_caa;

public:
    AttrTableTest(string name):TestCase(name) {
    } 

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

    // Tests for methods
    void find_container_test() {
	AttrTable *tmp = at1->find_container("a");
	assert(tmp != 0);
	assert(tmp == cont_a);

	assert(at1->find_container("b.ba") == cont_ba);
	assert(at1->find_container("a.b") == 0);
	assert(at1->find_container("c.ca.caa") == cont_caa);
	assert(at1->find_container("caa") == 0);

	assert(at1->find_container("a.size") == 0);
	assert(at1->find_container("b.ba.name") == 0);
    } 

    void find_test() { 
	AttrTable *tmp;
	Pix p = at1->find("a", &tmp);
	assert(tmp && p && tmp->is_container(p)
	       && tmp->get_name(p) == "a");
	p = at1->find("a.size", &tmp);
	assert(tmp && p && !tmp->is_container(p)
	       && tmp->get_name(p) == "size" && tmp->get_attr(p) == "7");
	p = at1->find("b.type", &tmp);
	assert(tmp && p && !tmp->is_container(p)
	       && tmp->get_name(p) == "type"
	       && tmp->get_attr(p) == "houses");
	p = at1->find("c.ca.caa.color", &tmp);
	assert(tmp && p && !tmp->is_container(p)
	       && tmp->get_name(p) == "color"
	       && tmp->get_attr(p) == "red");
	p = at1->find("d.size", &tmp);
	assert(!tmp);
	p = at1->find("c.size", &tmp);
	assert(tmp == cont_c && !p);
    } 

    void copy_ctor() {
	AttrTable at2 = *at1;
	Pix p = at2.first_attr(); 
	assert(at2.get_name(p) == "a");
	assert(at2.is_container(p));
	AttrTable *tmp = at2.get_attr_table(p);
	Pix q = tmp->first_attr();
	assert(tmp->get_name(q) == "size");
	at2.next_attr(p);
	assert(at2.get_name(p) == "b");
	assert(at2.is_container(p));
	at2.next_attr(p);
	assert(at2.get_name(p) == "c");
	assert(at2.is_container(p));
    }

    void assignment() {
	AttrTable at2;
	at2 = *at1;

	Pix p = at2.first_attr(); 
	assert(at2.get_name(p) == "a");
	assert(at2.is_container(p));
	AttrTable *tmp = at2.get_attr_table(p);
	Pix q = tmp->first_attr();
	assert(tmp->get_name(q) == "size");
	at2.next_attr(p);
	assert(at2.get_name(p) == "b");
	assert(at2.is_container(p));
	at2.next_attr(p);
	assert(at2.get_name(p) == "c");
	assert(at2.is_container(p));
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
String longer%20name \"second test\";
");
	assert(re_match(r, oss.str()));
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
	    assert("Caught Error exception!" && false);
	}
	ostrstream oss;
	top->print(oss, ""); oss << ends;
	Regex r("Data%20Field {
.*String long%20name first;
.*Alias an%20alias long%20name;
}
");
	assert(re_match(r, oss.str()));
	delete top; top = 0;

    }

    static Test *suite() { TestSuite *s = new TestSuite("AttrTableTest");

    s->addTest(new TestCaller < AttrTableTest > ("find_container_test",
						 &AttrTableTest::
						 find_container_test));
    s->addTest(new TestCaller < AttrTableTest >
	       ("find_test", &AttrTableTest::find_test));
    s->addTest(new TestCaller < AttrTableTest >
	       ("copy_ctor", &AttrTableTest::copy_ctor));
    s->addTest(new TestCaller < AttrTableTest >
	       ("assignment", &AttrTableTest::assignment));
    s->addTest(new TestCaller < AttrTableTest >
	       ("names_with_spaces_test", 
		&AttrTableTest::names_with_spaces_test));
    s->addTest(new TestCaller < AttrTableTest >
	       ("containers_with_spaces_test", 
		&AttrTableTest::containers_with_spaces_test));
    
    return s;
    }
};
