#ifdef __GNUG__
#pragma implementation
#endif

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include "TestSequence.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "util.h"
#include "Pix.h"

using std::cerr ;
using std::endl ;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 
class sequenceT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( sequenceT ) ;
CPPUNIT_TEST( sequenceT_test ) ;
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

    void sequenceT_test()
    {
	TestSequence s( "Supporters" ) ;

	BaseType *nm = NewStr( "Name" ) ;
	s.add_var( nm ) ;

	BaseType *age = NewInt16( "Age" ) ;
	s.add_var( age ) ;

	TestSequence *friends = (TestSequence *)NewSequence( "Friends" ) ;
	friends->add_var( nm ) ;
	delete nm ; nm = 0 ;
	friends->add_var( age ) ;
	delete age ; age = 0 ;
	s.add_var( friends ) ;
	delete friends ; friends = 0 ;

	BaseType *bt = s.var( "Age" ) ;
	CPPUNIT_ASSERT( bt->name( ) == "Age" ) ;

	bt = s.var( "Age", false ) ;
	CPPUNIT_ASSERT( bt->name( ) == "Age" ) ;

	btp_stack btps ;
	bt = s.var( "Friends.Age", btps ) ;
	CPPUNIT_ASSERT( bt->name( ) == "Age" ) ;
	CPPUNIT_ASSERT( btps.size( ) == 2 ) ;

	vector<string> names ;
	names.push_back( "Friends" ) ;
	names.push_back( "Supporters" ) ;
	typedef vector<string>::const_iterator names_iter ;
	names_iter n = names.begin( ) ;
	while( !btps.empty( ) && n != names.end( ) )
	{
	    BaseType *curr = btps.top( ) ;
	    CPPUNIT_ASSERT( curr->name( ) == (*n) ) ;
	    btps.pop( ) ;
	    n++ ;
	}

	int num_elems = s.element_count( false ) ;
	CPPUNIT_ASSERT( num_elems == 3 ) ;

	num_elems = s.element_count( true ) ;
	CPPUNIT_ASSERT( num_elems == 4 ) ;

	unsigned int w = s.width( ) ;
	CPPUNIT_ASSERT( w == 12 ) ;

	vector<string> elems ;
	elems.push_back( "Name" ) ;
	elems.push_back( "Age" ) ;
	elems.push_back( "Friends" ) ;
	typedef vector<string>::const_iterator elems_iter ;
	Pix p = s.first_var() ;
	elems_iter e = elems.begin() ;
	for( ; p && e != elems.end(); s.next_var( p ), e++ )
	{
	    // Why cast??? See structT.cc 11/16/04 jhrg
	    CPPUNIT_ASSERT( dynamic_cast<Constructor&>(s).var( p )->name() == (*e) ) ;
	}
	CPPUNIT_ASSERT( !p && e == elems.end() ) ;
	if( p && e == elems.end() )
	{
	    CPPUNIT_FAIL( "Too many elements" ) ;
	}
	else if( !p && e != elems.end() )
	{
	    CPPUNIT_FAIL( "Too few elements" ) ;
	}

	Sequence::Vars_iter v = s.var_begin() ;
	e = elems.begin() ;
	for( ; v != s.var_end() && e != elems.end(); v++, e++ )
	{
	    CPPUNIT_ASSERT( (*v)->name() == (*e) ) ;
	}
	CPPUNIT_ASSERT( v == s.var_end() && e == elems.end() ) ;
	if( v != s.var_end() && e == elems.end() )
	{
	    CPPUNIT_FAIL( "Too many elements" ) ;
	}
	else if( v == s.var_end() && e != elems.end() )
	{
	    CPPUNIT_FAIL( "Too few elements" ) ;
	}

	int num_rows = s.number_of_rows( ) ;
	CPPUNIT_ASSERT( num_rows == 0 ) ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( sequenceT ) ;

/* NOTHING NEEDS TO BE CHANGED BELOW HERE */

int main( int argc, char **argv )
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

