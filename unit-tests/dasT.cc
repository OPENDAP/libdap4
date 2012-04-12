
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <fstream>
#include <sstream>

// #define DODS_DEBUG

#include "DAS.h"

#include "testFile.h"

using namespace std;
using namespace libdap;

string dprint = "\
Attributes {\n\
    c1 {\n\
        v1 {\n\
            String v1a1 \"v1a1val\";\n\
            String v1a2 \"v1a2val\";\n\
            String v1a3 \"v1a3val\";\n\
            String v1a4 \"v1a4val\";\n\
            v1v1 {\n\
                String v1v1a1 \"v1v1a1val\";\n\
                String v1v1a2 \"v1v1a2val\";\n\
            }\n\
        }\n\
    }\n\
    c2 {\n\
        v2 {\n\
            String v2a1 \"v2a1val\";\n\
            String v2a2 \"v2a2val\";\n\
        }\n\
        v3 {\n\
            String v3a1 \"v3a1val\";\n\
            String v3a2 \"v3a2val\";\n\
            v3v1 {\n\
                String v3v1a1 \"v3v1a1val\";\n\
            }\n\
        }\n\
    }\n\
}\n\
" ;

class dasT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( dasT ) ;
CPPUNIT_TEST( dasT_test ) ;
CPPUNIT_TEST_SUITE_END( ) ;

private:
    /* TEST PRIVATE DATA */
    
public:
    void setUp()
    {
    }

    void tearDown() 
    {
    }

    void dasT_test()
    {
	DAS das ;
	CPPUNIT_ASSERT( das.get_size() == 0 ) ;
	CPPUNIT_ASSERT( das.container_name().empty() ) ;
	CPPUNIT_ASSERT( das.container() == 0 ) ;

	// set container to c1 and make sure set correctly
	das.container_name( "c1" ) ;
	CPPUNIT_ASSERT( das.container_name() == "c1" ) ;
	CPPUNIT_ASSERT( das.container() ) ;
	CPPUNIT_ASSERT( das.container()->get_name() == "c1" ) ;
	CPPUNIT_ASSERT( das.get_size() == 0 ) ;

	// set back to upermost attribute table and test
	das.container_name( "" ) ;
	CPPUNIT_ASSERT( das.container_name().empty() ) ;
	CPPUNIT_ASSERT( !das.container() ) ;
	CPPUNIT_ASSERT( das.get_size() == 1 ) ;
	
	// change to c2
	das.container_name( "c2" ) ;
	CPPUNIT_ASSERT( das.container_name() == "c2" ) ;
	CPPUNIT_ASSERT( das.container() ) ;
	CPPUNIT_ASSERT( das.container()->get_name() == "c2" ) ;
	CPPUNIT_ASSERT( das.get_size() == 0 ) ;

	// set back to upermost attribute table and test
	das.container_name( "" ) ;
	CPPUNIT_ASSERT( das.container_name().empty() ) ;
	CPPUNIT_ASSERT( !das.container() ) ;
	CPPUNIT_ASSERT( das.get_size() == 2 ) ;
	
	// change back to c1, make sure not another c1 added
	das.container_name( "c1" ) ;
	CPPUNIT_ASSERT( das.container_name() == "c1" ) ;
	CPPUNIT_ASSERT( das.container() ) ;
	CPPUNIT_ASSERT( das.container()->get_name() == "c1" ) ;
	CPPUNIT_ASSERT( das.get_size() == 0 ) ;

	// set back to upermost attribute table and test
	das.container_name( "" ) ;
	CPPUNIT_ASSERT( das.container_name().empty() ) ;
	CPPUNIT_ASSERT( !das.container() ) ;
	CPPUNIT_ASSERT( das.get_size() == 2 ) ;
	
	// add stuff to das, should go to c1
	das.container_name( "c1" ) ;
	AttrTable *v1 = new AttrTable ;
	das.add_table( "v1", v1 ) ;
	v1->append_attr( "v1a1", "String", "v1a1val" ) ;
	v1->append_attr( "v1a2", "String", "v1a2val" ) ;
	v1->append_attr( "v1a3", "String", "v1a3val" ) ;
	v1->append_attr( "v1a4", "String", "v1a4val" ) ;
	AttrTable *v1v1 = v1->append_container( "v1v1" ) ;
	v1v1->append_attr( "v1v1a1", "String", "v1v1a1val" ) ;
	v1v1->append_attr( "v1v1a2", "String", "v1v1a2val" ) ;

	// check container and das size
	CPPUNIT_ASSERT( das.get_size() == 1 ) ;
	das.container_name( "" ) ;
	CPPUNIT_ASSERT( das.get_size() == 2 ) ;

	// add stuff to das, should go to c2
	das.container_name( "c2" ) ;
	AttrTable *v2 = new AttrTable ;
	das.add_table( "v2", v2 ) ;
	v2->append_attr( "v2a1", "String", "v2a1val" ) ;
	v2->append_attr( "v2a2", "String", "v2a2val" ) ;
	AttrTable *v3 = new AttrTable ;
	das.add_table( "v3", v3 ) ;
	v3->append_attr( "v3a1", "String", "v3a1val" ) ;
	v3->append_attr( "v3a2", "String", "v3a2val" ) ;
	AttrTable *v3v1 = v3->append_container( "v3v1" ) ;
	v3v1->append_attr( "v3v1a1", "String", "v3v1a1val" ) ;

	// check container and das size
	CPPUNIT_ASSERT( das.get_size() == 2 ) ;
	das.container_name( "" ) ;
	CPPUNIT_ASSERT( das.get_size() == 2 ) ;

	// print to stream and compare results
	ostringstream strm ;
	das.print( strm ) ;
	cout << strm.str() << endl ;
	cout << dprint << endl ;
	CPPUNIT_ASSERT( strm.str() == dprint ) ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( dasT ) ;

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main( int, char ** )
{
    CppUnit::TextUi::TestRunner runner ;
    CppUnit::TestFactoryRegistry &registry =
	CppUnit::TestFactoryRegistry::getRegistry() ;
    runner.addTest( registry.makeTest() ) ;
    runner.setOutputter( CppUnit::CompilerOutputter::defaultOutputter( 
                                                        &runner.result(),
                                                        std::cerr ) );
    bool wasSuccessful = runner.run( "", false ) ;
    return wasSuccessful ? 0 : 1;
}

