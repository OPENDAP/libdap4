#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include <fstream>
#include <strstream>
#include "DDS.h"
#include "Pix.h"
#include "TestArray.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "ce_functions.h"
#include "util.h"

using std::cout ;
using std::cerr ;
using std::endl ;
using std::ostrstream ;
using std::ifstream ;

string cprint = "\
Dataset {\n\
    Int16 var1;\n\
    String var6;\n\
    Int16 arrayInt;\n\
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
    Int16 arrayInt;\n\
} Test%20Data%20Set;\n\
" ;

string nprint = "\
Dataset {\n\
} Test%20Data%20Set;\n\
" ;

void testFile( char *fn, const string &expstr ) ;

int
main( int argc, char **argv )
{
    DDS dds( "TestDDS" ) ;

    cout << endl << __LINE__ << " **** get the name" << endl ;
    string dsn = dds.get_dataset_name() ;
    if( dsn == "TestDDS" )
    {
	cout << "Name = " << dsn << " - PASS" << endl ;
    } else {
	cerr << "Name = " << dsn << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** set the name" << endl ;
    dds.set_dataset_name( "Test Data Set" ) ;
    dsn = dds.get_dataset_name() ;
    if( dsn == "Test Data Set" )
    {
	cout << "Name = " << dsn << " - PASS" << endl ;
    } else {
	cerr << "Name = " << dsn << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get filename" << endl ;
    string fn = dds.filename() ;
    if( fn == "" )
    {
	cout << "Name = " << fn << " - PASS" << endl ;
    } else {
	cerr << "Name = " << fn << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** set filename" << endl ;
    dds.filename( "dds_test.data" ) ;
    fn = dds.filename() ;
    if( fn == "dds_test.data" )
    {
	cout << "Name = " << fn << " - PASS" << endl ;
    } else {
	cerr << "Name = " << fn << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** try to add a null var" << endl ;
    try
    {
	dds.add_var( (BaseType *)NULL ) ;
	cerr << "succeeded in adding a null var - FAIL" << endl ;
    }
    catch( InternalErr &e )
    {
	cout << "failed to add a null var - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** add four vars" << endl ;
    try
    {
	BaseType *bt = NewInt16( "var1" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var1 - PASS" << endl ;
	delete bt ;
	bt = NewInt16( "var2" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var2 - PASS" << endl ;
	delete bt ;
	bt = NewInt16( "var3" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var3 - PASS" << endl ;
	delete bt ;
	bt = NewInt16( "var4" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var4 - PASS" << endl ;
	delete bt ;
	bt = NewInt16( "var5" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var5 - PASS" << endl ;
	delete bt ;
	bt = NewStr( "var6" ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var6 - PASS" << endl ;
	delete bt ;
	bt = NewArray( "var7", NewInt16( "arrayInt" ) ) ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var7 - PASS" << endl ;
	delete bt ;
	bt = NewStructure( "var8" ) ;
	Structure *s = (Structure *)bt ;
	BaseType *bts1 = NewStructure( "var9" ) ;
	Structure *s1 = (Structure *)bts1 ;
	BaseType *bts2 = NewInt16( "var10" ) ;
	s1->add_var( bts2 ) ;
	delete bts2 ; bts2 = 0 ;
	s->add_var( bts1 ) ;
	delete bts1 ; bts1 = 0 ;
	dds.add_var( bt ) ;
	cout << "succeeded in adding a var8 - PASS" << endl ;
    }
    catch( InternalErr &e )
    {
	cerr << "failed to add a var - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** number of variables" << endl ;
    int nv = dds.num_var() ;
    if( nv == 8 )
    {
	cout << "Number of vars = " << nv << " - PASS" << endl ;
    } else {
	cerr << "Number of vars = " << nv << " - FAIL" << endl ;
    }

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

    cout << endl << __LINE__ << " **** iterate through vars with Pix" << endl ;
    Pix p = dds.first_var() ;
    vs_citer vsc = vs.begin() ;
    for( ; p && vsc != vs.end(); dds.next_var( p ), vsc++ )
    {
	if( dds.var(p)->name() == *vsc )
	{
	    cout << "Var = " << dds.var(p)->name() << " - PASS" << endl ;
	} else {
	    cerr << "Var = " << dds.var(p)->name() << " - FAIL" << endl ;
	}
    }
    if( p && vsc == vs.end() )
    {
	cerr << "Too many vars - FAIL" << endl ;
    }
    else if( !p && vsc != vs.end() )
    {
	cerr << "Too few vars - FAIL" << endl ;
    }
    else
    {
	cout << "Enough vars - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate through vars with iter" << endl ;
    DDS::Vars_iter dvsc = dds.var_begin() ;
    vsc = vs.begin() ;
    for( ; dvsc != dds.var_end() && vsc != vs.end(); dvsc++, vsc++ )
    {
	if( (*dvsc)->name() == *vsc )
	{
	    cout << "Var = " << (*dvsc)->name() << " - PASS" << endl ;
	} else {
	    cerr << "Var = " << (*dvsc)->name() << " - FAIL" << endl ;
	}
    }
    if( dvsc != dds.var_end() && vsc == vs.end() )
    {
	cerr << "Too many vars - FAIL" << endl ;
    }
    else if( dvsc == dds.var_end() && vsc != vs.end() )
    {
	cerr << "Too few vars - FAIL" << endl ;
    }
    else
    {
	cout << "Enough vars - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate using Pix, del var2" << endl ;
    for( vsc = vs.begin(); vsc != vs.end(); vsc++ )
    {
	if( *vsc == "var2" )
	{
	    vs_iter &vsi = (vs_iter &)vsc ;
	    vs.erase( vsi ) ;
	    break ;
	}
    }
    p = dds.first_var() ;
    vsc = vs.begin() ;
    for( ; p && vsc != vs.end(); dds.next_var( p ), vsc++ )
    {
	if( dds.var(p)->name() == "var2" )
	{
	    dds.del_var( "var2" ) ;
	}
	if( dds.var(p)->name() == *vsc )
	{
	    cout << "Var = " << dds.var(p)->name() << " - PASS" << endl ;
	} else {
	    cerr << "Var = " << dds.var(p)->name() << " - FAIL" << endl ;
	}
    }

    cout << endl << __LINE__ << " **** number of variables" << endl ;
    nv = dds.num_var() ;
    if( nv == 7 )
    {
	cout << "Number of vars = " << nv << " - PASS" << endl ;
    } else {
	cerr << "Number of vars = " << nv << " - FAIL" << endl ;
	for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	{
	    cerr << "    " << (*dvsc)->name() << endl ;
	}
    }

    cout << endl << __LINE__ << " **** iterate using iter, del var3" << endl ;
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
	if( (*dvsc)->name() == *vsc )
	{
	    cout << "Var = " << (*dvsc)->name() << " - PASS" << endl ;
	} else {
	    cerr << "Var = " << (*dvsc)->name() << " - FAIL" << endl ;
	}
    }

    cout << endl << __LINE__ << " **** number of variables" << endl ;
    nv = dds.num_var() ;
    if( nv == 6 )
    {
	cout << "Number of vars = " << nv << " - PASS" << endl ;
    } else {
	cerr << "Number of vars = " << nv << " - FAIL" << endl ;
	for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	{
	    cerr << "    " << (*dvsc)->name() << endl ;
	}
    }

    cout << endl << __LINE__ << " **** iterate using iter, del 4-5" << endl ;
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
	    //dvsc = dvsc2 ;
	}
	if( (*dvsc)->name() == *vsc )
	{
	    cout << "Var = " << (*dvsc)->name() << " - PASS" << endl ;
	} else {
	    cerr << "Var = " << (*dvsc)->name() << " - FAIL" << endl ;
	}
    }

    cout << endl << __LINE__ << " **** number of variables" << endl ;
    nv = dds.num_var() ;
    if( nv == 4 )
    {
	cout << "Number of vars = " << nv << " - PASS" << endl ;
    } else {
	cerr << "Number of vars = " << nv << " - FAIL" << endl ;
	for( dvsc = dds.var_begin(); dvsc != dds.var_end(); dvsc++ )
	{
	    cerr << "    " << (*dvsc)->name() << endl ;
	}
    }

    cout << endl << __LINE__ << " **** find varnot using string" << endl ;
    BaseType *bt = dds.var( "varnot" ) ;
    if( bt )
    {
	cerr << "Found var " << bt->name() << " - FAIL" << endl ;
    } else {
	cout << "Didn't find varnot - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** find var6 using char *" << endl ;
    bt = dds.var( "var6" ) ;
    if( bt )
    {
	if( bt->name() == "var6" )
	{
	    cout << "Found var " << bt->name() << " - PASS" << endl ;
	} else {
	    cerr << "Found wrong var " << bt->name() << " - FAIL" << endl ;
	}
    } else {
	cerr << "Didn't find var6 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find var6 using string" << endl ;
    string find_var = "var6" ;
    bt = dds.var( find_var ) ;
    if( bt )
    {
	if( bt->name() == "var6" )
	{
	    cout << "Found var " << bt->name() << " - PASS" << endl ;
	} else {
	    cerr << "Found wrong var " << bt->name() << " - FAIL" << endl ;
	}
    } else {
	cerr << "Didn't find var6 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find var10 using string" << endl ;
    find_var = "var10" ;
    bt = dds.var( find_var ) ;
    if( bt )
    {
	if( bt->name() == "var10" )
	{
	    cout << "Found var " << bt->name() << " - PASS" << endl ;
	} else {
	    cerr << "Found wrong var " << bt->name() << " - FAIL" << endl ;
	}
    } else {
	cerr << "Didn't find var10 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find var10 using string" << endl ;
    find_var = "var10" ;
    btp_stack btps ;
    bt = dds.var( find_var, &btps ) ;
    if( bt )
    {
	if( bt->name() == "var10" )
	{
	    cout << "Found var " << bt->name() << " - PASS" << endl ;
	    if( btps.size() == 2 )
	    {
		if( btps.top()->name() == "var8" )
		{
		    cout << "first on stack = " << btps.top()->name()
			 << " - PASS" << endl ;
		} else {
		    cerr << "first on stack = " << btps.top()->name()
			 << " - FAIL" << endl ;
		}
		btps.pop() ;
		if( btps.top()->name() == "var9" )
		{
		    cout << "first on stack = " << btps.top()->name()
			 << " - PASS" << endl ;
		} else {
		    cerr << "first on stack = " << btps.top()->name()
			 << " - FAIL" << endl ;
		}
		btps.pop() ;
	    } else {
		cerr << "nothing on the search stack - FAIL" << endl ;
	    }
	} else {
	    cerr << "Found wrong var " << bt->name() << " - FAIL" << endl ;
	}
    } else {
	cerr << "Didn't find var10 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find var8.var9.var10" << endl ;
    find_var = "var8.var9.var10" ;
    bt = dds.var( find_var, &btps ) ;
    if( bt )
    {
	if( bt->name() == "var10" )
	{
	    cout << "Found var " << bt->name() << " - PASS" << endl ;
	    if( btps.size() == 2 )
	    {
		if( btps.top()->name() == "var9" )
		{
		    cout << "first on stack = " << btps.top()->name()
			 << " - PASS" << endl ;
		} else {
		    cerr << "first on stack = " << btps.top()->name()
			 << " - FAIL" << endl ;
		}
		btps.pop() ;
		if( btps.top()->name() == "var8" )
		{
		    cout << "first on stack = " << btps.top()->name()
			 << " - PASS" << endl ;
		} else {
		    cerr << "first on stack = " << btps.top()->name()
			 << " - FAIL" << endl ;
		}
		btps.pop() ;
	    } else {
		cerr << "nothing on the search stack - FAIL" << endl ;
	    }
	} else {
	    cerr << "Found wrong var " << bt->name() << " - FAIL" << endl ;
	}
    } else {
	cerr << "Didn't find var10 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add bool function" << endl ;
    dds.add_function( "test_null", func_null ) ;
    bool_func boolf ;
    bool found_func = dds.find_function( "test_null", &boolf ) ;
    if( found_func == true )
    {
	if( boolf )
	{
	    cout << "found function test_null - PASS" << endl ;
	} else {
	    cerr << "says found test_null, but null - FAIL" << endl ;
	}
    } else {
	cerr << "did not find test_null - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add btp function" << endl ;
    dds.add_function( "test_nth", func_nth ) ;
    btp_func btpf ;
    found_func = dds.find_function( "test_nth", &btpf ) ;
    if( found_func == true )
    {
	if( btpf )
	{
	    cout << "found function test_nth - PASS" << endl ;
	} else {
	    cerr << "says found test_nth, but null - FAIL" << endl ;
	}
    } else {
	cerr << "did not find test_nth - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add proj function" << endl ;
    dds.add_function( "test_grid_select", func_grid_select ) ;
    proj_func projf ;
    found_func = dds.find_function( "test_grid_select", &projf ) ;
    if( found_func == true )
    {
	if( projf )
	{
	    cout << "found function test_grid_select - PASS" << endl ;
	} else {
	    cerr << "says found test_grid_select, but null - FAIL" << endl ;
	}
    } else {
	cerr << "did not find test_grid_select - FAIL" << endl ;
    }

    {
	cout << endl << __LINE__ << " **** print using ostream" << endl ;
	char *tmpstr = new char[2048] ;
	memset( tmpstr, '\0', 2048 ) ;
	ostrstream strm( tmpstr, 2048 ) ;
	dds.print( strm ) ;
	string outstr = strm.str() ;
	if( cprint == outstr )
	{
	    cout << "expected output - PASS" << endl ;
	} else {
	    cerr << "unexpected output - FAIL" << endl ;
	    cerr << "expected output = " << cprint << endl ;
	    cerr << "output received = " << outstr << endl ;
	}
	delete [] tmpstr ;
    }

    {
	cout << endl << __LINE__ << " **** print using FILE" << endl ;
	FILE *of = fopen( "testout", "w" ) ;
	dds.print( of ) ;
	fclose( of ) ;
	testFile( "testout", cprint ) ;
    }

    {
	cout << endl << __LINE__ << " **** printc using ostream" << endl ;
	char *tmpstr = new char[2048] ;
	memset( tmpstr, '\0', 2048 ) ;
	ostrstream strm( tmpstr, 2048 ) ;
	dds.print_constrained( strm ) ;
	string outstr = strm.str() ;
	if( nprint == outstr )
	{
	    cout << "expected output - PASS" << endl ;
	} else {
	    cerr << "unexpected output - FAIL" << endl ;
	    cerr << "expected output = " << nprint << endl ;
	    cerr << "output received = " << outstr << endl ;
	}
	delete [] tmpstr ;
    }

    {
	cout << endl << __LINE__ << " **** printc using FILE" << endl ;
	FILE *of = fopen( "testout", "w" ) ;
	dds.print_constrained( of ) ;
	fclose( of ) ;
	testFile( "testout", nprint ) ;
    }

    cout << endl << __LINE__ << " **** mark all as send_p" << endl ;
    dds.mark_all( true ) ;

    {
	cout << endl << __LINE__ << " **** printc using ostream" << endl ;
	char *tmpstr = new char[2048] ;
	memset( tmpstr, '\0', 2048 ) ;
	ostrstream strm( tmpstr, 2048 ) ;
	dds.print_constrained( strm ) ;
	string outstr = strm.str() ;
	if( cprint == outstr )
	{
	    cout << "expected output - PASS" << endl ;
	} else {
	    cerr << "unexpected output - FAIL" << endl ;
	    cerr << "expected output = " << cprint << endl ;
	    cerr << "output received = " << outstr << endl ;
	}
	delete [] tmpstr ;
    }

    {
	cout << endl << __LINE__ << " **** printc using FILE" << endl ;
	FILE *of = fopen( "testout", "w" ) ;
	dds.print_constrained( of ) ;
	fclose( of ) ;
	testFile( "testout", cprint ) ;
    }

    cout << endl << __LINE__ << " **** unmark var8" << endl ;
    bool mark_ret = dds.mark( "var8", false ) ;
    if( mark_ret == true )
    {
	cout << "successfully unmarked var8 - PASS" << endl ;
    } else {
	cerr << "failed to unmark var8 - FAIL" << endl ;
    }

    {
	cout << endl << __LINE__ << " **** printc using ostream" << endl ;
	char *tmpstr = new char[2048] ;
	memset( tmpstr, '\0', 2048 ) ;
	ostrstream strm( tmpstr, 2048 ) ;
	dds.print_constrained( strm ) ;
	string outstr = strm.str() ;
	if( pprint == outstr )
	{
	    cout << "expected output - PASS" << endl ;
	} else {
	    cerr << "unexpected output - FAIL" << endl ;
	    cerr << "expected output = " << pprint << endl ;
	    cerr << "output received = " << outstr << endl ;
	}
	delete [] tmpstr ;
    }

    {
	cout << endl << __LINE__ << " **** printc using FILE" << endl ;
	FILE *of = fopen( "testout", "w" ) ;
	dds.print_constrained( of ) ;
	fclose( of ) ;
	testFile( "testout", pprint ) ;
    }

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

void
testFile( char *fn, const string &expstr )
{
    ifstream ifs( fn ) ;
    char *tmpstr = new char[2048] ;
    memset( tmpstr, '\0', 2048 ) ;
    ostrstream strm( tmpstr, 2048 ) ;
    char line[80];
    while( !ifs.eof( ) )
    {
	ifs.getline( line, 80 ) ;
	if( !ifs.eof() )
	    strm << line << endl ;
    }
    ifs.close();
    string outstr = strm.str() ;
    if( expstr == outstr )
    {
	cout << "expected output - PASS" << endl ;
    } else {
	cerr << "unexpected output - FAIL" << endl ;
	cerr << "expected output = " << expstr << endl ;
	cerr << "output received = " << outstr << endl ;
    }
    delete [] tmpstr ;
}

