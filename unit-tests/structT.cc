#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <sstream>
#include "TestStructure.h"
#include "TestArray.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "TestTypeFactory.h"
#include "util.h"
//#include "Pix.h"

using std::cerr;
using std::endl;
using std::ostringstream;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
// timeouts.
string ExpectedPrint1(
		"Structure {\n\
    Int16 name_int16;\n\
    String name_str;\n\
    Int16 array_int[dim1 = 4][dim2 = 3][dim3 = 2];\n\
} my_structure = { 32000, \"Silly test string: 1\", {{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}}} };\n");

string ExpectedPrint2(
		"Structure {\n\
    Int16 name_int16;\n\
    Int16 array_int[dim1 = 4][dim2 = 3][dim3 = 2];\n\
} my_structure = { 32000, {{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}},{{32000, 32000},{32000, 32000},{32000, 32000}}} };\n");

class structT: public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( structT );
	CPPUNIT_TEST( structT_test );
	CPPUNIT_TEST_SUITE_END( )
	;

private:
	/* TEST PRIVATE DATA */
	TestTypeFactory *factory;

public:
	void setUp()
	{
		factory = new TestTypeFactory;
	}

	void tearDown()
	{
		delete factory;
		factory = 0;
	}

	void structT_test()
	{
		TestStructure s("my_structure");

		BaseType *bt = factory->NewInt16("name_int16");
		s.add_var(bt);
		delete bt;
		bt = 0;

		bt = factory->NewStr("name_str");
		s.add_var(bt);
		delete bt;
		bt = 0;

		Array *abt = factory->NewArray("name_array", factory->NewInt16("array_int"));
		abt->append_dim(4, "dim1");
		abt->append_dim(3, "dim2");
		abt->append_dim(2, "dim3");
		s.add_var(abt);
		delete abt;
		abt = 0;

		bt = 0;
		bt = s.var("name_str", true);
		CPPUNIT_ASSERT( bt && bt->name() == "name_str" );

		bt = 0;
		bt = s.var("name_str", false);
		CPPUNIT_ASSERT( bt && bt->name() == "name_str" );

		vector<string> varnames;
		varnames.push_back("name_int16");
		varnames.push_back("name_str");
		varnames.push_back("array_int");
		typedef vector<string>::const_iterator niter;

		Structure::Vars_iter viter = s.var_begin();
		niter n = varnames.begin();
		for (; viter != s.var_end() && n != varnames.end(); viter++, n++) {
			CPPUNIT_ASSERT( (*viter)->name() == *n );
		}
		CPPUNIT_ASSERT( viter == s.var_end() && n == varnames.end() );
		if (viter != s.var_end() && n == varnames.end()) {
			CPPUNIT_FAIL( "Too many variables" );
		}
		else if( viter == s.var_end() && n != varnames.end() )
		{
			CPPUNIT_FAIL( "Too few varialbes" );
		}

		int num_elems = s.element_count();
		CPPUNIT_ASSERT( num_elems == 3 );

		unsigned int w = s.width();
		unsigned int wsb = sizeof(string) + sizeof(dods_int16) + 24 * sizeof(dods_int16);
		cerr << "s.width(): " << s.width() << endl;
		CPPUNIT_ASSERT( w == wsb );

		bool is_read = s.read();
		CPPUNIT_ASSERT( is_read == true );

		ostringstream sstrm1;
		s.print_val(sstrm1);
		CPPUNIT_ASSERT( sstrm1.str() == ExpectedPrint1 );

		s.del_var("name_str");

		bt = 0;
		bt = s.var("name_str", false);
		CPPUNIT_ASSERT( bt == 0 );

		w = s.width();
		wsb = +sizeof(dods_int16) + 24 * sizeof(dods_int16);
		CPPUNIT_ASSERT( w == wsb );

		ostringstream sstrm2;
		s.print_val(sstrm2);
		CPPUNIT_ASSERT( sstrm2.str() == ExpectedPrint2 );
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( structT );

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main(int, char **)
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	runner.setOutputter(CppUnit::CompilerOutputter::defaultOutputter(&runner.result(), std::cerr));
	bool wasSuccessful = runner.run("", false);
	return wasSuccessful ? 0 : 1;
}

