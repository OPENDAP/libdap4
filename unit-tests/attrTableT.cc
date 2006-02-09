
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>

#include <iostream>
#include <vector>
#include "AttrTable.h"
//#include "Pix.h"

using std::cerr ;
using std::endl ;
using std::vector ;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 

class attrTableT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( attrTableT ) ;
CPPUNIT_TEST( attrTableT_test ) ;
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

    void attrTableT_test()
    {
	AttrTable at ;

	unsigned int at_size = at.get_size() ;
	CPPUNIT_ASSERT( at_size == 0 ) ;

	string at_name = at.get_name() ;
	CPPUNIT_ASSERT( at_name == "" ) ;

	at.set_name( "My Attributes" ) ;
	at_name = at.get_name() ;
	CPPUNIT_ASSERT( at_name == "My Attributes" ) ;

	AttrTable *container = at.find_container( "dummy_container" ) ;
	CPPUNIT_ASSERT( !container ) ;

	AttrTable *dummy_at = 0 ;
#if 0
	Pix p = at.find( "dummy_attr", &dummy_at ) ;
	CPPUNIT_ASSERT( !p ) ;
#endif

	AttrTable::Attr_iter iter ;
	at.find( "dummy_attr", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter == at.attr_end() ) ;

	string attr_name = "attr1" ;
	string attr_type = "string" ;
	string attr_value = "attr1Value1" ;
	at.append_attr( attr_name, attr_type, attr_value ) ;

	attr_name = "attr2" ;
	attr_type = "string" ;
	attr_value = "attr2Value1" ;
	at.append_attr( attr_name, attr_type, attr_value ) ;

	attr_name = "attr3" ;
	attr_type = "string" ;
	attr_value = "attr3Value1" ;
	at.append_attr( attr_name, attr_type, attr_value ) ;

	at.append_attr( "attr4", "string", "attr4Value1" ) ;

	at_size = at.get_size() ;
	CPPUNIT_ASSERT( at_size == 4 ) ;

	//at.print( stdout ) ;
#if 0
	p = at.find( "attr2", &dummy_at ) ;
	CPPUNIT_ASSERT( p ) ;

	p = at.find( "dummy_attr", &dummy_at ) ;
	CPPUNIT_ASSERT( !p ) ;
#endif
	iter = at.attr_end() ;
	at.find( "attr3", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter != at.attr_end() ) ;

	iter = at.attr_end() ;
	at.find( "dummy_attr", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter == at.attr_end() ) ;

	attr_type = at.get_type( "attr3" ) ;
	CPPUNIT_ASSERT( attr_type == "String" ) ;

	AttrType attr_type_enum = at.get_attr_type( "attr3" ) ;
	CPPUNIT_ASSERT( attr_type_enum == Attr_string ) ;

	unsigned int num_attrs = at.get_attr_num( "attr3" ) ;
	CPPUNIT_ASSERT( num_attrs == 1 ) ;

	attr_value = at.get_attr( "attr3" ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value1" ) ;

	at.append_attr( "attr3", "string", "attr3Value2" ) ;
	at.append_attr( "attr3", "string", "attr3Value3" ) ;
	at.append_attr( "attr3", "string", "attr3Value4" ) ;

	attr_value = at.get_attr( "attr3" ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value1" ) ;

	vector<string> sb ;
	sb.push_back( "attr3Value1" ) ;
	sb.push_back( "attr3Value2" ) ;
	sb.push_back( "attr3Value3" ) ;
	sb.push_back( "attr3Value4" ) ;

	typedef vector<string>::const_iterator str_citer ;
	typedef vector<string>::iterator str_iter ;

	vector<string> *values = at.get_attr_vector( "attr3" ) ;
	CPPUNIT_ASSERT( values ) ;
	if( values )
	{
	    str_citer vi = values->begin() ;
	    str_citer sbi = sb.begin() ;
	    for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	    {
		CPPUNIT_ASSERT( (*vi) == (*sbi) ) ;
	    }
	    CPPUNIT_ASSERT( vi == values->end() && sbi == sb.end() ) ;
	    if( vi == values->end() && sbi != sb.end() )
	    {
		CPPUNIT_FAIL( "not enough values" ) ;
	    }
	    else if( vi != values->end() && sbi == sb.end() )
	    {
		CPPUNIT_FAIL( "too many values" ) ;
	    }
	}

	vector<string> attrs ;
	attrs.push_back( "attr1" ) ;
	attrs.push_back( "attr2" ) ;
	attrs.push_back( "attr3" ) ;
	attrs.push_back( "attr4" ) ;

#if 0
	Pix q = at.first_attr() ;
        str_citer ai = attrs.begin() ;
	for(; q && ai != attrs.end(); at.next_attr( q ), ai++ )
	{
	    CPPUNIT_ASSERT( at.attr(q)->name == (*ai) ) ;
	}
	CPPUNIT_ASSERT( !q && ai == attrs.end() ) ;
	if( q && ai == attrs.end() )
	{
	    CPPUNIT_FAIL( "too many attributes"  ) ;
	}
	else if( !q && ai != attrs.end() )
	{
	    CPPUNIT_FAIL( "not enough attributes"  ) ;
	}
#endif

        str_citer ai = attrs.begin() ;
        AttrTable::Attr_iter i = at.attr_begin() ;
	// ai = attrs.begin() ;
	for( ; i != at.attr_end() && ai != attrs.end(); i++, ai++ )
	{
	    CPPUNIT_ASSERT( (*i)->name == (*ai) ) ;
	}
	CPPUNIT_ASSERT( i == at.attr_end() && ai == attrs.end() ) ;
	if( i != at.attr_end() && ai == attrs.end() )
	{
	    CPPUNIT_FAIL( "too many attributes"  ) ;
	}
	else if( i == at.attr_end() && ai != attrs.end() )
	{
	    CPPUNIT_FAIL( "not enough attributes"  ) ;
	}

#if 0
	p = at.find( "attr3", &dummy_at ) ;
	CPPUNIT_ASSERT( p ) ;
#endif

	iter = at.attr_end() ;
	at.find( "attr3", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter != at.attr_end() ) ;

#if 0
	attr_name =  at.get_name( p ) ;
	CPPUNIT_ASSERT( attr_name == "attr3" ) ;
#endif

	attr_name = at.get_name( iter ) ;
	CPPUNIT_ASSERT( attr_name == "attr3" ) ;

#if 0
	bool isit = at.is_container( p ) ;
	CPPUNIT_ASSERT( isit == false ) ;
#endif

	bool isit = at.is_container( iter ) ;
	CPPUNIT_ASSERT( isit == false ) ;

#if 0
	dummy_at = at.get_attr_table( p ) ;
	CPPUNIT_ASSERT( !dummy_at ) ;
#endif

	dummy_at = at.get_attr_table( iter ) ;
	CPPUNIT_ASSERT( !dummy_at ) ;

#if 0
	attr_type = at.get_type( p ) ;
	CPPUNIT_ASSERT( attr_type == "String" ) ;
#endif

	attr_type = at.get_type( iter ) ;
	CPPUNIT_ASSERT( attr_type == "String" ) ;

#if 0
	attr_type_enum = at.get_attr_type( p ) ;
	CPPUNIT_ASSERT( attr_type_enum == Attr_string ) ;
#endif

	attr_type_enum = at.get_attr_type( iter ) ;
	CPPUNIT_ASSERT( attr_type_enum == Attr_string ) ;

#if 0
	attr_value = at.get_attr( p ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value1" ) ;
#endif
	attr_value = at.get_attr( iter ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value1" ) ;

#if 0
	attr_value = at.get_attr( p, 1 ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value2" ) ;
#endif

	attr_value = at.get_attr( iter, 1 ) ;
	CPPUNIT_ASSERT( attr_value == "attr3Value2" ) ;

#if 0
	values = at.get_attr_vector( p ) ;
	CPPUNIT_ASSERT( values ) ;
	if( values )
	{
	    str_citer vi = values->begin() ;
	    str_citer sbi = sb.begin() ;
	    for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	    {
		CPPUNIT_ASSERT( (*vi) == (*sbi) ) ;
	    }
	    CPPUNIT_ASSERT( vi == values->end() && sbi == sb.end() ) ;
	    if( vi == values->end() && sbi != sb.end() )
	    {
		CPPUNIT_FAIL( "not enough values"  ) ;
	    }
	    else if( vi != values->end() && sbi == sb.end() )
	    {
		CPPUNIT_FAIL( "too many values"  ) ;
	    }
	}
#endif

	values = at.get_attr_vector( iter ) ;
	CPPUNIT_ASSERT( values ) ;
	if( values )
	{
	    str_citer vi = values->begin() ;
	    str_citer sbi = sb.begin() ;
	    for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	    {
		CPPUNIT_ASSERT( (*vi) == (*sbi) ) ;
	    }
	    CPPUNIT_ASSERT( vi == values->end() && sbi == sb.end() ) ;
	    if( vi == values->end() && sbi != sb.end() )
	    {
		CPPUNIT_FAIL( "not enough values"  ) ;
	    }
	    else if( vi != values->end() && sbi == sb.end() )
	    {
		CPPUNIT_FAIL( "too many values"  ) ;
	    }
	}


	{
	    str_iter sbi = sb.begin() ;
	    sbi++ ;
	    sb.erase( sbi ) ;
	}

	at.del_attr( "attr3", 1 ) ;
	values = at.get_attr_vector( iter ) ;
	CPPUNIT_ASSERT( values ) ;
	if( values )
	{
	    str_citer vi = values->begin() ;
	    str_citer sbi = sb.begin() ;
	    for( ; vi != values->end() && sbi != sb.end(); vi++, sbi++ )
	    {
		CPPUNIT_ASSERT( (*vi) == (*sbi) ) ;
	    }
	    CPPUNIT_ASSERT( vi == values->end() && sbi == sb.end() ) ;
	    if( vi == values->end() && sbi != sb.end() )
	    {
		CPPUNIT_FAIL( "not enough values"  ) ;
	    }
	    else if( vi != values->end() && sbi == sb.end() )
	    {
		CPPUNIT_FAIL( "too many values"  ) ;
	    }
	}

	at.del_attr( "attr3" ) ;
#if 0
	p = at.find( "attr3", &dummy_at ) ;
	CPPUNIT_ASSERT( !p ) ;
#endif
	container = 0 ;
	try
	{
	    container = at.append_container( "attr2" ) ;
	    CPPUNIT_FAIL( "added container named attr2 successfully - already exists" ) ;
	}
	catch( Error &e )
	{
	}
	CPPUNIT_ASSERT( !container ) ;

	try
	{
	    container = at.append_container( "attr5" ) ;
	}
	catch( Error &e )
	{
	    CPPUNIT_FAIL( "failed to add new container attr5" ) ;
	}
	CPPUNIT_ASSERT( container ) ;
	if( container )
	{
	    CPPUNIT_ASSERT( container->get_name() == "attr5" ) ;
	}

	container = at.find_container( "attr5" ) ;
	CPPUNIT_ASSERT( container ) ;
	if( container )
	{
	    string container_name = container->get_name() ;
	    CPPUNIT_ASSERT( container_name == "attr5" ) ;
	}

#if 0
	p = at.find( "attr5", &dummy_at ) ;
	CPPUNIT_ASSERT( p ) ;
#endif

	iter = at.attr_end() ;
	at.find( "attr5", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter != at.attr_end() ) ;
#if 0
	attr_name =  at.get_name( p ) ;
	CPPUNIT_ASSERT( attr_name == "attr5" ) ;
#endif
	attr_name =  at.get_name( iter ) ;
	CPPUNIT_ASSERT( attr_name == "attr5" ) ;
#if 0
	isit = at.is_container( p ) ;
	CPPUNIT_ASSERT( isit == true ) ;
#endif
	isit = at.is_container( iter ) ;
	CPPUNIT_ASSERT( isit == true ) ;
#if 0
	container = at.get_attr_table( p ) ;
	CPPUNIT_ASSERT( container ) ;
#endif
	container = at.get_attr_table( iter ) ;
	CPPUNIT_ASSERT( container ) ;
#if 0
	attr_type = at.get_type( p ) ;
	CPPUNIT_ASSERT( attr_type == "Container" ) ;
#endif
	attr_type = at.get_type( iter ) ;
	CPPUNIT_ASSERT( attr_type == "Container" ) ;
#if 0
	attr_type_enum = at.get_attr_type( p ) ;
	CPPUNIT_ASSERT( attr_type_enum == Attr_container ) ;
#endif
	attr_type_enum = at.get_attr_type( iter ) ;
	CPPUNIT_ASSERT( attr_type_enum == Attr_container ) ;

	/* FIX: does append attr return anything? */
	container->append_attr( "attr5-1", "string", "attr5.1Value1" ) ;
	container->append_attr( "attr5-2", "string", "attr5.2Value1" ) ;
	container->append_attr( "attr5-3", "string", "attr5.3Value1" ) ;
	container->append_attr( "attr5-4", "string", "attr5.4Value1" ) ;
#if 0
	p = at.find( "attr5.attr5-2", &dummy_at ) ;
	CPPUNIT_ASSERT( p ) ;
	CPPUNIT_ASSERT( container == dummy_at ) ;
#endif
	iter = at.attr_end() ;
	at.find( "attr5.attr5-3", &dummy_at, &iter ) ;
	CPPUNIT_ASSERT( iter != at.attr_end() ) ;
	CPPUNIT_ASSERT( container == dummy_at ) ;

	//at.print( stdout ) ;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( attrTableT ) ;

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

