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
#include <vector>
#include "LongIterAdapter.h"
#include "Pix.h"

using std::cerr ;
using std::endl ;
using std::vector ;


class iterT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( iterT ) ;
CPPUNIT_TEST( iterT_test ) ;
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

    long getValue( Pix &p )
    {
	IteratorAdapter *ia = p.getIterator() ;
	CPPUNIT_ASSERT( ia != NULL ) ;
	if( ia == NULL )
	{
	    return -1 ;
	}
	LongIterAdapter *iaT = (LongIterAdapter *)ia ;
	long retval = iaT->entry() ;
	return retval ;
    }

    Pix first_var( vector<long> &vec )
    {
	LongIterAdapter *i = new LongIterAdapter( vec ) ;
	i->first() ;
	return i ;
    }

    void next_var( Pix p )
    {
	p.next() ;
    }

    Pix findValue( long val, vector<long> &vec )
    {
	for( Pix p = first_var( vec ); p; next_var( p ) )
	{
	    long e = getValue( p ) ;
	    if( e == val )
	    {
		return p ;
	    }
	}
	return (IteratorAdapter *)0 ;
    }

    void iterT_test()
    {
	vector<long> vl ;
	vector<long> vlt ;
	typedef std::vector<long>::const_iterator citer ;

	for( long l = 0; l < 10; l++ )
	{
	    vl.push_back( l ) ;
	    vlt.push_back( l ) ;
	}
	vlt.pop_back() ;

	Pix p = first_var( vl ) ;

	citer ci = vlt.begin() ;
	for( ; p && ci != vlt.end(); next_var(p), ci++ )
	{
	    long lv = getValue( p ) ;
	    CPPUNIT_ASSERT( lv != -1 ) ;
	    if( lv != -1 )
	    {
		CPPUNIT_ASSERT( lv == *ci ) ;
		CPPUNIT_ASSERT( p != (Pix)NULL ) ;
	    }
	}
	CPPUNIT_ASSERT( p && ci == vlt.end() ) ;
	if( !p && ci != vlt.end() )
	{
	    CPPUNIT_FAIL( "Too few values" ) ;
	}
	else if( !p && ci == vlt.end() )
	{
	    CPPUNIT_FAIL( "Enough values" ) ;
	}
	CPPUNIT_ASSERT( !(p == (Pix)NULL) ) ;

	vlt.push_back( 9 ) ;
	vlt.push_back( 10 ) ;
	p = first_var(vl) ;
	ci = vlt.begin() ;
	for( ; p && ci != vlt.end(); next_var(p), ci++ )
	{
	    long lv = getValue( p ) ;
	    CPPUNIT_ASSERT( lv != -1 ) ;
	    if( lv != -1 )
	    {
		CPPUNIT_ASSERT( lv == *ci ) ;
		CPPUNIT_ASSERT( p != (Pix)NULL ) ;
	    }
	}
	CPPUNIT_ASSERT( !p && ci != vlt.end() ) ;
	if( p && ci == vlt.end() )
	{
	    CPPUNIT_FAIL( "Too many values" ) ;
	}
	else if( !p && ci == vlt.end() )
	{
	    CPPUNIT_FAIL( "Enough values" ) ;
	}
	CPPUNIT_ASSERT( p == (Pix)NULL ) ;

	vlt.pop_back( ) ;
	p = first_var(vl) ;
	ci = vlt.begin() ;
	for( ; p && ci != vlt.end(); next_var(p), ci++ )
	{
	    long lv = getValue( p ) ;
	    CPPUNIT_ASSERT( lv != -1 ) ;
	    if( lv != -1 )
	    {
		CPPUNIT_ASSERT( lv == *ci ) ;
		CPPUNIT_ASSERT( p != (Pix)NULL ) ;
	    }
	}
	CPPUNIT_ASSERT( !p && ci == vlt.end() ) ;
	if( p && ci == vlt.end() )
	{
	    CPPUNIT_FAIL( "Too many values" ) ;
	}
	else if( !p && ci != vlt.end() )
	{
	    CPPUNIT_FAIL( "Too few values" ) ;
	}
	CPPUNIT_ASSERT( p == (Pix)NULL ) ;

	Pix r ;
	Pix s ;
	CPPUNIT_ASSERT( !(r == s) ) ;
	CPPUNIT_ASSERT( r != s ) ;
	
	r = findValue( 5, vl ) ;
	CPPUNIT_ASSERT( r ) ;
	if( r )
	{
	    long retval = getValue( r ) ;
	    CPPUNIT_ASSERT( retval == 5 ) ;
	}
	CPPUNIT_ASSERT( !(r == s) ) ;
	CPPUNIT_ASSERT( r != s ) ;
	
	s = findValue( 6, vl ) ;
	CPPUNIT_ASSERT( s ) ;
	if( s )
	{
	    long retval = getValue( s ) ;
	    CPPUNIT_ASSERT( retval == 6 ) ;
	}
	CPPUNIT_ASSERT( !(r == s) ) ;
	CPPUNIT_ASSERT( r != s ) ;
	
	s = findValue( 5, vl ) ;
	CPPUNIT_ASSERT( s ) ;
	if( s )
	{
	    long retval = getValue( s ) ;
	    CPPUNIT_ASSERT( retval == 5 ) ;
	}
	CPPUNIT_ASSERT( r == s ) ;
	CPPUNIT_ASSERT( !(r != s) ) ;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( iterT ) ;

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

