#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include <vector>
#include "LongIterAdapter.h"
#include "Pix.h"

using std::cout ;
using std::cerr ;
using std::endl ;
using std::vector ;

long
getValue( Pix &p )
{
    IteratorAdapter *ia = p.getIterator() ;
    if( ia == NULL )
    {
	cerr << "IteratorAdapter is NULL - FAIL" << endl ;
	return -1 ;
    }
    LongIterAdapter *iaT = (LongIterAdapter *)ia ;
    long retval = iaT->entry() ;
    return retval ;
}

Pix
first_var( vector<long> &vec )
{
    LongIterAdapter *i = new LongIterAdapter( vec ) ;
    i->first() ;
    return i ;
}

void
next_var( Pix p )
{
    p.next() ;
}

Pix
findValue( long val, vector<long> &vec )
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

int
main( int argc, char **argv )
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

    cout << endl << __LINE__ << " **** iterate, too many" << endl ;
    citer ci = vlt.begin() ;
    for( ; p && ci != vlt.end(); next_var(p), ci++ )
    {
	long lv = getValue( p ) ;
	if( lv != -1 )
	{
	    if( lv == *ci )
	    {
		cout << "Value = " << lv << " - PASS" << endl ;
	    } else {
		cerr << "Value = " << lv << " - FAIL" << endl ;
	    }

	    if( p != (Pix)NULL )
	    {
		cout << "Not at the end - PASS" << endl ;
	    } else {
		cerr << "Reached the end - FAIL" << endl ;
	    }

	}
    }
    if( p && ci == vlt.end() )
    {
	cout << "Too many values - PASS" << endl ;
    }
    else if( !p && ci != vlt.end() )
    {
	cerr << "Too few values - FAIL" << endl ;
    }
    else
    {
	cerr << "Enough values - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** after iteration, at the end?" << endl ;
    if( p == (Pix)NULL )
    {
	cerr << "Reached the end - FAIL" << endl ;
    } else {
	cout << "Not at the end - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate, too few" << endl ;
    vlt.push_back( 9 ) ;
    vlt.push_back( 10 ) ;
    p = first_var(vl) ;
    ci = vlt.begin() ;
    for( ; p && ci != vlt.end(); next_var(p), ci++ )
    {
	long lv = getValue( p ) ;
	if( lv != -1 )
	{
	    if( lv == *ci )
	    {
		cout << "Value = " << lv << " - PASS" << endl ;
	    } else {
		cerr << "Value = " << lv << " - FAIL" << endl ;
	    }

	    if( p != (Pix)NULL )
	    {
		cout << "Not at the end - PASS" << endl ;
	    } else {
		cerr << "Reached the end - FAIL" << endl ;
	    }

	}
    }
    if( p && ci == vlt.end() )
    {
	cerr << "Too many values - FAIL" << endl ;
    }
    else if( !p && ci != vlt.end() )
    {
	cout << "Too few values - PASS" << endl ;
    }
    else
    {
	cerr << "Enough values - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** after iteration, at the end?" << endl ;
    if( p == (Pix)NULL )
    {
	cout << "Reached the end - PASS" << endl ;
    } else {
	cerr << "Not at the end - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate, enough" << endl ;
    vlt.pop_back( ) ;
    p = first_var(vl) ;
    ci = vlt.begin() ;
    for( ; p && ci != vlt.end(); next_var(p), ci++ )
    {
	long lv = getValue( p ) ;
	if( lv != -1 )
	{
	    if( lv == *ci )
	    {
		cout << "Value = " << lv << " - PASS" << endl ;
	    } else {
		cerr << "Value = " << lv << " - FAIL" << endl ;
	    }

	    if( p != (Pix)NULL )
	    {
		cout << "Not at the end - PASS" << endl ;
	    } else {
		cerr << "Reached the end - FAIL" << endl ;
	    }

	}
    }
    if( p && ci == vlt.end() )
    {
	cerr << "Too many values - FAIL" << endl ;
    }
    else if( !p && ci != vlt.end() )
    {
	cerr << "Too few values - FAIL" << endl ;
    }
    else
    {
	cout << "Enough values - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** after iteration, at the end?" << endl ;
    if( p == (Pix)NULL )
    {
	cout << "Reached the end - PASS" << endl ;
    } else {
	cerr << "Not at the end - FAIL" << endl ;
    }

    Pix r ;
    Pix s ;
    cout << endl << __LINE__ << " **** testing pix == pix, not set" << endl ;
    if( r == s )
    {
	cerr << "Pix values are equal - FAIL" << endl ;
    } else {
	cout << "Pix values are not equal - PASS" << endl ;
    }
    
    cout << endl << __LINE__ << " **** testing pix != pix, not set" << endl ;
    if( r != s )
    {
	cout << "Pix values are not equal - PASS" << endl ;
    } else {
	cerr << "Pix values are equal - FAIL" << endl ;
    }
    
    cout << endl << __LINE__ << " **** find the value 5" << endl ;
    r = findValue( 5, vl ) ;
    if( r )
    {
	long retval = getValue( r ) ;
	if( retval == 5 )
	{
	    cout << "Found value " << retval << " - PASS" << endl ;
	} else {
	    cerr << "Found value " << retval << " - FAIL" << endl ;
	}
    } else {
	cerr << "didn't find the value 5 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** testing pix == pix, one set" << endl ;
    if( r == s )
    {
	cerr << "Pix values are equal - FAIL" << endl ;
    } else {
	cout << "Pix values are not equal - PASS" << endl ;
    }
    
    cout << endl << __LINE__ << " **** testing pix != pix, one set" << endl ;
    if( r != s )
    {
	cout << "Pix values are not equal - PASS" << endl ;
    } else {
	cerr << "Pix values are equal - FAIL" << endl ;
    }
    
    cout << endl << __LINE__ << " **** find the value 6" << endl ;
    s = findValue( 6, vl ) ;
    if( s )
    {
	long retval = getValue( s ) ;
	if( retval == 6 )
	{
	    cout << "Found value " << retval << " - PASS" << endl ;
	} else {
	    cerr << "Found value " << retval << " - FAIL" << endl ;
	}
    } else {
	cerr << "didn't find the value 6 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** testing pix == pix, both set" << endl ;
    if( r == s )
    {
	cerr << "Pix values are equal - FAIL" << endl ;
    } else {
	cout << "Pix values are not equal - PASS" << endl ;
    }
    
    cout << endl << __LINE__ << " **** testing pix != pix, one set" << endl ;
    if( r != s )
    {
	cout << "Pix values are not equal - PASS" << endl ;
    } else {
	cerr << "Pix values are equal - FAIL" << endl ;
    }
    
    cout << endl << __LINE__ << " **** find the value 5" << endl ;
    s = findValue( 5, vl ) ;
    if( s )
    {
	long retval = getValue( s ) ;
	if( retval == 5 )
	{
	    cout << "Found value " << retval << " - PASS" << endl ;
	} else {
	    cerr << "Found value " << retval << " - FAIL" << endl ;
	}
    } else {
	cerr << "didn't find the value 5 - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** testing pix == pix, should be" << endl ;
    if( r == s )
    {
	cout << "Pix values are equal - PASS" << endl ;
    } else {
	cerr << "Pix values are not equal - FAIL" << endl ;
    }
    
    cout << endl << __LINE__ << " **** testing pix != pix, should be" << endl ;
    if( r != s )
    {
	cerr << "Pix values are not equal - FAIL" << endl ;
    } else {
	cout << "Pix values are equal - PASS" << endl ;
    }
    
    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

