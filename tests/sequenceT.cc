#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include "TestSequence.h"
#include "TestInt16.h"
#include "TestStr.h"
#include "util.h"
#include "Pix.h"

int
main( int argc, char **argv )
{
    cout << endl << __LINE__ << " **** construct a sequence" << endl ;
    TestSequence s( "Supporters" ) ;

    cout << endl << __LINE__ << " **** add string name" << endl ;
    BaseType *nm = NewStr( "Name" ) ;
    s.add_var( nm ) ;

    cout << endl << __LINE__ << " **** add int16 age" << endl ;
    BaseType *age = NewInt16( "Age" ) ;
    s.add_var( age ) ;

    cout << endl << __LINE__ << " **** add sequence friends" << endl ;
    TestSequence *friends = (TestSequence *)NewSequence( "Friends" ) ;
    friends->add_var( nm ) ;
    delete nm ; nm = 0 ;
    friends->add_var( age ) ;
    delete age ; age = 0 ;
    s.add_var( friends ) ;
    delete friends ; friends = 0 ;

    cout << endl << __LINE__ << " **** find age exactly" << endl ;
    BaseType *bt = s.var( "Age" ) ;
    if( bt->name( ) == "Age" )
    {
	cout << "Found var " << bt->name( ) << " - PASS" << endl ;
    } else {
	cerr << "Found var " << bt->name( ) << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find age not exactly" << endl ;
    bt = s.var( "Age", false ) ;
    if( bt->name( ) == "Age" )
    {
	cout << "Found var " << bt->name( ) << " - PASS" << endl ;
    } else {
	cerr << "Found var " << bt->name( ) << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** find age in Friends sequence" << endl ;
    btp_stack btps ;
    bt = s.var( "Friends.Age", btps ) ;
    if( bt->name( ) == "Age" )
    {
	cout << "Found var " << bt->name( ) << " - PASS" << endl ;
    } else {
	cerr << "Found var " << bt->name( ) << " - FAIL" << endl ;
    }
    if( btps.size( ) == 2 )
    {
	cout << "BaseType stack has " << btps.size( ) << " elements - PASS"
	     << endl ;
    } else {
	cerr << "BaseType stack has " << btps.size( ) << " elements - FAIL"
	     << endl ;
    }
    vector<string> names ;
    names.push_back( "Friends" ) ;
    names.push_back( "Supporters" ) ;
    typedef vector<string>::const_iterator names_iter ;
    names_iter n = names.begin( ) ;
    while( !btps.empty( ) && n != names.end( ) )
    {
	BaseType *curr = btps.top( ) ;
	//btps.top( )->BaseType::name( );
	if( curr->name( ) == (*n) )
	{
	    cout << "comparing basetype " << curr->name( ) << " to "
		 << (*n) << " - PASS" << endl ;
	} else {
	    cerr << "comparing basetype " << curr->name( ) << " to "
		 << (*n) << " - FAIL" << endl ;
	}
	btps.pop( ) ;
	n++ ;
    }

    cout << endl << __LINE__ << " **** number of elements no leaves" << endl ;
    int num_elems = s.element_count( false ) ;
    if( num_elems == 3 )
    {
	cout << "number of elements = " << num_elems << " - PASS" << endl ;
    } else {
	cerr << "number of elements = " << num_elems << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** number of elements w/ leaves" << endl ;
    num_elems = s.element_count( true ) ;
    if( num_elems == 4 )
    {
	cout << "number of elements = " << num_elems << " - PASS" << endl ;
    } else {
	cerr << "number of elements = " << num_elems << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** width" << endl ;
    unsigned int w = s.width( ) ;
    if( w == 12 )
    {
	cout << "width = " << w << " - PASS" << endl ;
    } else {
	cerr << "width = " << w << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate using Pix" << endl ;
    vector<string> elems ;
    elems.push_back( "Name" ) ;
    elems.push_back( "Age" ) ;
    elems.push_back( "Friends" ) ;
    typedef vector<string>::const_iterator elems_iter ;
    Pix p = s.first_var() ;
    elems_iter e = elems.begin() ;
    for( ; p && e != elems.end(); s.next_var( p ), e++ )
    {
	if( s.var( p )->name() == (*e) )
	{
	    cout << "comparing " << s.var( p )->name() << " to " << (*e)
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << s.var( p )->name() << " to " << (*e)
		 << " - FAIL" << endl ;
	}
    }
    if( p && e == elems.end() )
    {
	cerr << "too many elements" << endl ;
    }
    else if( !p && e != elems.end() )
    {
	cerr << "too few elements" << endl ;
    }
    else
    {
	cout << "enough elements found" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate using iter" << endl ;
    Sequence::Vars_citer v = s.var_begin() ;
    e = elems.begin() ;
    for( ; v != s.var_end() && e != elems.end(); v++, e++ )
    {
	if( (*v)->name() == (*e) )
	{
	    cout << "comparing " << (*v)->name() << " to " << (*e)
		 << " - PASS" << endl ;
	} else {
	    cerr << "comparing " << (*v)->name() << " to " << (*e)
		 << " - FAIL" << endl ;
	}
    }
    if( v != s.var_end() && e == elems.end() )
    {
	cerr << "too many elements" << endl ;
    }
    else if( v == s.var_end() && e != elems.end() )
    {
	cerr << "too few elements" << endl ;
    }
    else
    {
	cout << "enough elements found" << endl ;
    }

    cout << endl << __LINE__ << " **** number of rows" << endl ;
    int num_rows = s.number_of_rows( ) ;
    if( num_rows == 0 )
    {
	cout << "number of rows = " << num_rows << " - PASS" << endl ;
    } else {
	cerr << "number of rows = " << num_rows << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

