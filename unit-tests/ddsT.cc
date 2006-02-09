
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

#include "DDS.h"
//#include "Pix.h"
#include "TestArray.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "TestTypeFactory.h"
#include "ce_functions.h"
#include "util.h"
#include "debug.h"

#include "testFile.cc"

using namespace std;

int test_variable_sleep_interval = 0; // Used in Test* classes for testing
				      // timeouts. 
string cprint = "\
Dataset {\n\
    Int16 var1;\n\
    String var6;\n\
    Int16 var7;\n\
    Structure {\n\
        Structure {\n\
            Int16 var10;\n\
        } var9;\n\
    } var8;\n\
} Test%20Data%20Set;\n\
" ;

string pprint = "\
Dataset {\n\
    Int16 var1;\n\
    String var6;\n\
    Int16 var7;\n\
} Test%20Data%20Set;\n\
" ;

string nprint = "\
Dataset {\n\
} Test%20Data%20Set;\n\
" ;


class ddsT : public CppUnit::TestFixture {

CPPUNIT_TEST_SUITE( ddsT ) ;
CPPUNIT_TEST( ddsT_test ) ;
CPPUNIT_TEST_SUITE_END( ) ;

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
        delete factory; factory = 0;
    }

    void ddsT_test()
    {
	DDS dds( factory, "TestDDS" ) ;

	string dsn = dds.get_dataset_name() ;
	CPPUNIT_ASSERT( dsn == "TestDDS" ) ;

	dds.set_dataset_name( "Test Data Set" ) ;
	dsn = dds.get_dataset_name() ;
	CPPUNIT_ASSERT( dsn == "Test Data Set" ) ;

	string fn = dds.filename() ;
	CPPUNIT_ASSERT( fn == "" ) ;

	dds.filename( "dds_test.data" ) ;
	fn = dds.filename() ;
	CPPUNIT_ASSERT( fn == "dds_test.data" ) ;

	try
	{
	    dds.add_var( (BaseType *)NULL ) ;
	    CPPUNIT_FAIL( "succeeded in adding a null var" ) ;
	}
	catch( InternalErr &e )
	{
	}

	try
	{
	    BaseType *bt = factory->NewInt16( "var1" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewInt16( "var2" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewInt16( "var3" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewInt16( "var4" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewInt16( "var5" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewStr( "var6" ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewArray( "var7", factory->NewInt16( "" ) ) ;
	    dds.add_var( bt ) ;
	    delete bt ;
	    bt = factory->NewStructure( "var8" ) ;
	    Structure *s = (Structure *)bt ;
	    BaseType *bts1 = factory->NewStructure( "var9" ) ;
	    Structure *s1 = (Structure *)bts1 ;
	    BaseType *bts2 = factory->NewInt16( "var10" ) ;
	    s1->add_var( bts2 ) ;
	    delete bts2 ; bts2 = 0 ;
	    s->add_var( bts1 ) ;
	    delete bts1 ; bts1 = 0 ;
	    dds.add_var( bt ) ;
	}
	catch( InternalErr &e )
	{
	    CPPUNIT_FAIL( "failed to add a var" ) ;
	}

	int nv = dds.num_var() ;
	CPPUNIT_ASSERT( nv == 8 ) ;

	vector<string> vs ;
	typedef std::vector<string>::const_iterator vs_citer ;
	typedef std::vector<string>::iterator vs_iter ;
	vs.push_back( "var1" ) ;
	vs.push_back( "var2" ) ;
	vs.push_back( "var3" ) ;
	vs.push_back( "var4" ) ;
	vs.push_back( "var5" ) ;
	vs.push_back( "var6" ) ;
	vs.push_back( "var7" ) ;
	vs.push_back( "var8" ) ;

#if 0
	Pix p = dds.first_var() ;
	vs_citer vsc = vs.begin() ;
	for( ; p && vsc != vs.end(); dds.next_var( p ), vsc++ )
	{
	    DBG(cerr << dds.var(p)->name() << ": " << *vsc << endl);
	    CPPUNIT_ASSERT( dds.var(p)->name() == *vsc ) ;
	}
	CPPUNIT_ASSERT( !p && vsc == vs.end() ) ;
	if( p && vsc == vs.end() )
	{
	    CPPUNIT_FAIL( "Too many vars" ) ;
	}
	else if( !p && vsc != vs.end() )
	{
	    CPPUNIT_FAIL( "Too few vars" ) ;
	}
#endif

	DDS::Vars_iter dvsc = dds.var_begin() ;
	vs_citer vsc = vs.begin() ;
	for( ; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++ )
	{
	    CPPUNIT_ASSERT( (*dvsc)->name() == *vsc ) ;
	}
	CPPUNIT_ASSERT( dvsc == dds.var_end() && vsc == vs.end() ) ;
	if( dvsc != dds.var_end() && vsc == vs.end() )
	{
	    CPPUNIT_FAIL( "Too many vars" ) ;
	}
	else if( dvsc == dds.var_end() && vsc != vs.end() )
	{
	    CPPUNIT_FAIL( "Too few vars" ) ;
	}

	for( vsc = vs.begin(); vsc != vs.end(); vsc++ )
	{
	    if( *vsc == "var2" )
	    {
		vs_iter &vsi = (vs_iter &)vsc ;
		vs.erase( vsi ) ;
		break ;
	    }
	}

#if 1
        dvsc = dds.var_begin() ;
        vsc = vs.begin() ;
        for( ; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++ )
        {
            if( (*dvsc)->name() == "var2" )
            {
                DDS::Vars_iter &dvsi = (DDS::Vars_iter &)dvsc ;
                dds.del_var( dvsi ) ;
            }
            CPPUNIT_ASSERT( (*dvsc)->name() == *vsc ) ;
        }
#else
	p = dds.first_var() ;
	vsc = vs.begin() ;
	for( ; p && vsc != vs.end(); dds.next_var( p ), vsc++ )
	{
	    if( dds.var(p)->name() == "var2" )
	    {
		dds.del_var( "var2" ) ;
	    }
	    CPPUNIT_ASSERT( dds.var(p)->name() == *vsc ) ;
	}
#endif
	nv = dds.num_var() ;
	CPPUNIT_ASSERT( nv == 7 ) ;
	if( nv != 7 )
	{
	    for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	    {
		DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
	    }
	}

	for( vsc = vs.begin(); vsc != vs.end(); vsc++ )
	{
	    if( *vsc == "var3" )
	    {
		vs_iter &vsi = (vs_iter &)vsc ;
		vs.erase( vsi ) ;
		break ;
	    }
	}
	dvsc = dds.var_begin() ;
	vsc = vs.begin() ;
	for( ; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++ )
	{
	    if( (*dvsc)->name() == "var3" )
	    {
		DDS::Vars_iter &dvsi = (DDS::Vars_iter &)dvsc ;
		dds.del_var( dvsi ) ;
	    }
	    CPPUNIT_ASSERT( (*dvsc)->name() == *vsc ) ;
	}

	nv = dds.num_var() ;
	CPPUNIT_ASSERT( nv == 6 ) ;
	if( nv != 6 )
	{
	    for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	    {
		DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
	    }
	}

	for( vsc = vs.begin(); vsc != vs.end(); vsc++ )
	{
	    if( *vsc == "var4" )
	    {
		vs_citer vsc2 = vsc ;
		vsc2++ ;
		vsc2++ ;
		vs_iter &vsi = (vs_iter &)vsc ;
		vs_iter &vsi2 = (vs_iter &)vsc2 ;
		vs.erase( vsi, vsi2 ) ;
		break ;
	    }
	}
	dvsc = dds.var_begin() ;
	vsc = vs.begin() ;
	for( ; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++ )
	{
	    if( (*dvsc)->name() == "var4" )
	    {
		DDS::Vars_iter dvsc2 = dvsc ;
		dvsc2++ ;
		dvsc2++ ;
		DDS::Vars_iter &dvsi = (DDS::Vars_iter &)dvsc ;
		DDS::Vars_iter &dvsi2 = (DDS::Vars_iter &)dvsc2 ;
		dds.del_var( dvsi, dvsi2 ) ;
	    }
	    CPPUNIT_ASSERT( (*dvsc)->name() == *vsc ) ;
	}

	nv = dds.num_var() ;
	CPPUNIT_ASSERT( nv == 4 ) ;
	if( nv != 4 )
	{
	    for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	    {
		DBG2( cerr << "    " << (*dvsc)->name() << endl ) ;
	    }
	}

	BaseType *bt = dds.var( "varnot" ) ;
	CPPUNIT_ASSERT( !bt ) ;

	bt = dds.var( "var6" ) ;
	CPPUNIT_ASSERT( bt ) ;
	if( bt )
	{
	    CPPUNIT_ASSERT( bt->name() == "var6" ) ;
	}

	string find_var = "var6" ;
	bt = dds.var( find_var ) ;
	CPPUNIT_ASSERT( bt ) ;
	if( bt )
	{
	    CPPUNIT_ASSERT( bt->name() == "var6" ) ;
	}

	find_var = "var10" ;
	bt = dds.var( find_var ) ;
	CPPUNIT_ASSERT( bt ) ;
	if( bt )
	{
	    CPPUNIT_ASSERT( bt->name() == "var10" ) ;
	}

	find_var = "var10" ;
	btp_stack btps ;
	bt = dds.var( find_var, &btps ) ;
	CPPUNIT_ASSERT( bt ) ;
	if( bt )
	{
	    CPPUNIT_ASSERT( bt->name() == "var10" ) ;
	    if( bt->name() == "var10" )
	    {
		CPPUNIT_ASSERT( btps.size() == 2 ) ;
		if( btps.size() == 2 )
		{
		    CPPUNIT_ASSERT( btps.top()->name() == "var8" ) ;
		    btps.pop() ;
		    CPPUNIT_ASSERT( btps.top()->name() == "var9" ) ;
		    btps.pop() ;
		}
	    }
	}

	find_var = "var8.var9.var10" ;
	bt = dds.var( find_var, &btps ) ;
	CPPUNIT_ASSERT( bt ) ;
	if( bt )
	{
	    CPPUNIT_ASSERT( bt->name() == "var10" ) ;
	    if( bt->name() == "var10" )
	    {
		CPPUNIT_ASSERT( btps.size() == 2 ) ;
		if( btps.size() == 2 )
		{
		    CPPUNIT_ASSERT( btps.top()->name() == "var9" ) ;
		    btps.pop() ;
		    CPPUNIT_ASSERT( btps.top()->name() == "var8" ) ;
		    btps.pop() ;
		}
	    }
	}

#if 0
	dds.add_function( "test_null", func_null ) ;
	bool_func boolf ;
	bool found_func = dds.find_function( "test_null", &boolf ) ;
	CPPUNIT_ASSERT( found_func == true ) ;
	CPPUNIT_ASSERT( boolf ) ;

	dds.add_function( "test_nth", func_nth ) ;
	btp_func btpf ;
	found_func = dds.find_function( "test_nth", &btpf ) ;
	CPPUNIT_ASSERT( found_func == true ) ;
	CPPUNIT_ASSERT( btpf ) ;
#endif

	dds.add_function( "test_grid_select", func_grid_select ) ;
	proj_func projf ;
	bool found_func = dds.find_function( "test_grid_select", &projf ) ;
	CPPUNIT_ASSERT( found_func == true ) ;
	CPPUNIT_ASSERT( projf ) ;

#if 0
	{
	    ostringstream strm;
	    dds.print( strm ) ;
	    string outstr = strm.str() ;
	    if( cprint != outstr )
	    {
		DBG( cerr << "expected output = " << cprint << endl ) ;
		DBG( cerr << "output received = " << outstr << endl ) ;
	    }
	    CPPUNIT_ASSERT( cprint == outstr ) ;
	}
#endif

	{
            string sof;
            FILE2string(sof, of, dds.print( of ));
            CPPUNIT_ASSERT(sof.find(cprint) != string::npos);
	}

#if 0
	{
	    ostringstream strm;
	    dds.print_constrained( strm ) ;
	    string outstr = strm.str() ;
	    if( nprint != outstr ) ;
	    {
		DBG( cerr << "expected output = " << nprint << endl ) ;
		DBG( cerr << "output received = " << outstr << endl ) ;
	    }
	    CPPUNIT_ASSERT( nprint == outstr ) ;
	}
#endif

	{
            string sof;
            FILE2string(sof, of, dds.print_constrained( of ));
            CPPUNIT_ASSERT(sof.find(nprint) != string::npos);
	}

	dds.mark_all( true ) ;
#if 0
	{
	    ostringstream strm;
	    dds.print_constrained( strm ) ;
	    string outstr = strm.str() ;
	    if( cprint != outstr )
	    {
		DBG( cerr << "expected output = " << cprint << endl ) ;
		DBG( cerr << "output received = " << outstr << endl ) ;
	    }
	    CPPUNIT_ASSERT( cprint == outstr ) ;
	}
#endif
	{
            string sof;
            FILE2string(sof, of, dds.print_constrained( of ));
            CPPUNIT_ASSERT(sof.find(cprint) != string::npos);
	}

	bool mark_ret = dds.mark( "var8", false ) ;
	CPPUNIT_ASSERT( mark_ret == true ) ;
#if 0
	{
	    ostringstream strm;
	    dds.print_constrained( strm ) ;
	    string outstr = strm.str() ;
	    if( pprint != outstr )
	    {
		DBG( cerr << "expected output = " << pprint << endl ) ;
		DBG( cerr << "output received = " << outstr << endl ) ;
	    }
	    CPPUNIT_ASSERT( pprint == outstr ) ;
	}
#endif
	{
            string sof;
            FILE2string(sof, of, dds.print_constrained( of ));
            CPPUNIT_ASSERT(sof.find(pprint) != string::npos);
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ddsT ) ;

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

