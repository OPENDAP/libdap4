#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include "TestStructure.h"
#include "TestArray.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "util.h"
#include "Pix.h"

int
main( int argc, char **argv )
{
    cout << endl << __LINE__ << " **** construct a structure array" << endl ;
    TestStructure s( "my_structure" ) ;

    cout << endl << __LINE__ << " **** add int16" << endl ;
    BaseType *bt = NewInt16( "name_int16" ) ;
    s.add_var( bt ) ;
    delete bt ; bt = 0 ;

    cout << endl << __LINE__ << " **** add str" << endl ;
    bt = NewStr( "name_str" ) ;
    s.add_var( bt ) ;
    delete bt ; bt = 0 ;

    cout << endl << __LINE__ << " **** add array" << endl ;
    Array *abt = NewArray( "name_array", NewInt16( "array_int" ) ) ;
    abt->append_dim( 4, "dim1" ) ;
    abt->append_dim( 3, "dim2" ) ;
    abt->append_dim( 2, "dim3" ) ;
    s.add_var( abt ) ;
    delete abt ; abt = 0 ;

    cout << endl << __LINE__ << " **** find the string exactly" << endl ;
    bt = 0 ;
    bt = s.var( "name_str", true ) ;
    if( bt && bt->name() == "name_str" )
    {
	cout << "found var with name " << bt->name() << " - PASS" << endl ;
    } else {
	cerr << "found var with name " << bt->name() << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find the string not exactly" << endl ;
    bt = 0 ;
    bt = s.var( "name_str", false ) ;
    if( bt && bt->name() == "name_str" )
    {
	cout << "found var with name " << bt->name() << " - PASS" << endl ;
    } else {
	cerr << "found var with name " << bt->name() << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate using Pix" << endl ;
    vector<string> varnames ;
    varnames.push_back( "name_int16" ) ;
    varnames.push_back( "name_str" ) ;
    varnames.push_back( "name_array" ) ;
    typedef vector<string>::const_iterator niter ;

    Pix p = s.first_var() ;
    niter n = varnames.begin() ;
    for( ; p && n != varnames.end(); s.next_var( p ), n++ )
    {
	if( s.var( p )->name() == *n )
	{
	    cout << "comparing " << s.var(p)->name() << " to " << (*n)
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << s.var(p)->name() << " to " << (*n)
		 << " - FAIL" << endl ;
	}
    }
    if( p && n == varnames.end() )
    {
	cerr << "too many variables - FAIL" << endl ;
    }
    else if( !p && n != varnames.end() )
    {
	cerr << "too few varialbes - FAIL" << endl ;
    }
    else
    {
	cout << "right number of variables - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate using iter" << endl ;
    Structure::Vars_citer viter = s.var_begin() ;
    n = varnames.begin() ;
    for( ; viter != s.var_end() && n != varnames.end(); viter++, n++ )
    {
	if( (*viter)->name() == *n )
	{
	    cout << "comparing " << (*viter)->name() << " to " << (*n)
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << (*viter)->name() << " to " << (*n)
		 << " - FAIL" << endl ;
	}
    }
    if( viter != s.var_end() && n == varnames.end() )
    {
	cerr << "too many variables - FAIL" << endl ;
    }
    else if( viter == s.var_end() && n != varnames.end() )
    {
	cerr << "too few varialbes - FAIL" << endl ;
    }
    else
    {
	cout << "right number of variables - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** get num elements" << endl ;
    int num_elems = s.element_count( ) ;
    if( num_elems == 3 )
    {
	cout << "number of elements = " << num_elems << " - PASS" << endl ;
    } else {
	cerr << "number of elements = " << num_elems << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get width" << endl ;
    unsigned int w = s.width() ;
    unsigned int wsb = sizeof(string)
		       + sizeof(dods_int16)
		       + 24*sizeof(dods_int16) ;
    if( w == wsb )
    {
	cout << "width of structure = " << w << " - PASS" << endl ;
    } else {
	cerr << "width of structure = " << w << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** call read" << endl ;
    bool is_read = s.read( "dataset" ) ;
    if( is_read == true )
    {
	cout << "read completed successfully - PASS" << endl ;
    } else {
	cerr << "read completed successfully - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** print the structure" << endl ;
    s.print_val( cout ) ;

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

