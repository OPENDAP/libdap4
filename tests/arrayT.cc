#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include "TestArray.h"
#include "TestInt16.h"
#include "util.h"
#include "Pix.h"

using std::cout ;
using std::cerr ;
using std::endl ;

int
main( int argc, char **argv )
{
    cout << endl << __LINE__ << " **** construct an array" << endl ;
    TestArray ar( "My Array" ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    int l = ar.length() ;
    if( l == -1 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the width" << endl ;
    try
    {
	int w = ar.width() ;
	cerr << "width = " << w << " - FAIL" << endl ;
    }
    catch( InternalErr &e )
    {
	cout << "unable to get the width, var not set - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** set var" << endl ;
    BaseType *bt = NewInt16() ;
    ar.add_var( bt ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == -1 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the width" << endl ;
    try
    {
	int w = ar.width() ;
	if( w == ( l * (int)bt->width() ) )
	{
	    cout << "width = " << w << " - PASS" << endl ;
	} else {
	    cerr << "width = " << w << " - FAIL" << endl ;
	}
    }
    catch( InternalErr &e )
    {
	cerr << "unable to get the width - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add first dimension of 4" << endl ;
    ar.append_dim( 4, "dim1" ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 4 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the width" << endl ;
    try
    {
	int w = ar.width() ;
	if( w == ( l * (int)bt->width() ) )
	{
	    cout << "width = " << w << " - PASS" << endl ;
	} else {
	    cerr << "width = " << w << " - FAIL" << endl ;
	}
    }
    catch( InternalErr &e )
    {
	cerr << "unable to get the width - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add next dimension of 3" << endl ;
    ar.append_dim( 3, "dim2" ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 12 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the width" << endl ;
    try
    {
	int w = ar.width() ;
	if( w == ( l * (int)bt->width() ) )
	{
	    cout << "width = " << w << " - PASS" << endl ;
	} else {
	    cerr << "width = " << w << " - FAIL" << endl ;
	}
    }
    catch( InternalErr &e )
    {
	cerr << "unable to get the width - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add next dimension of 2" << endl ;
    ar.append_dim( 2, "dim3" ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 24 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** get the width" << endl ;
    try
    {
	int w = ar.width() ;
	if( w == ( l * (int)bt->width() ) )
	{
	    cout << "width = " << w << " - PASS" << endl ;
	} else {
	    cerr << "width = " << w << " - FAIL" << endl ;
	}
    }
    catch( InternalErr &e )
    {
	cerr << "unable to get the width - FAIL" << endl ;
    }

    vector<string> dims ;
    typedef vector<string>::const_iterator citer ;
    dims.push_back( "dim1" ) ;
    dims.push_back( "dim2" ) ;
    dims.push_back( "dim3" ) ;

    vector<int> dimsize ;
    typedef vector<int>::const_iterator dsiter ;
    dimsize.push_back( 4 ) ;
    dimsize.push_back( 3 ) ;
    dimsize.push_back( 2 ) ;

    cout << endl << __LINE__ << " **** iterate dims using Pix" << endl ;
    citer i = dims.begin() ;
    dsiter d = dimsize.begin() ;
    Pix p ;
    p = ar.first_dim() ;
    for( ; p && i != dims.end(); ar.next_dim( p ), i++, d++ )
    {
	if( ar.dimension_name( p ) == (*i) )
	{
	    cout << "comparing " << ar.dimension_name(p) << " to " << (*i) 
		 << " - PASS" << endl ;
	    if( ar.dimension_size( p ) == (*d) )
	    {
		cout << "comparing size " << ar.dimension_size(p) 
		     << " to expected " << (*d) << " - PASS" << endl ;
	    } else {
		cerr << "comparing size " << ar.dimension_size(p) 
		     << " to expected " << (*d) << " - FAIL" << endl ;
	    }
	    if( ar.dimension_start( p ) == 0 )
	    {
		cout << "comparing start " << ar.dimension_start(p)
		     << " to expected 0 - PASS" << endl ;
	    } else {
		cerr << "comparing start " << ar.dimension_start(p)
		     << " to expected 0 - FAIL" << endl ;
	    }
	    if( ar.dimension_stop( p ) == (*d)-1 )
	    {
		cout << "comparing stop " << ar.dimension_stop(p)
		     << " to expected " << (*d)-1 << " - PASS" << endl ;
	    } else {
		cerr << "comparing stop " << ar.dimension_stop(p)
		     << " to expected " << (*d)-1 << " - FAIL" << endl ;
	    }
	    if( ar.dimension_stride( p ) == 1 )
	    {
		cout << "comparing stride " << ar.dimension_stride(p)
		     << " to expected 1 - PASS" << endl ;
	    } else {
		cerr << "comparing stride " << ar.dimension_stride(p)
		     << " to expected 1 - FAIL" << endl ;
	    }
	} else {
	    cerr << "comparing " << ar.dimension_name(p) << " to " << (*i) 
		 << " - FAIL" << endl ;
	}
    }
    if( p && i == dims.end() )
    {
	cerr << "too many dimensions - FAIL" << endl ;
    }
    else if( !p && i != dims.end() )
    {
	cerr << "not enough dimensions - FAIL" << endl ;
    }
    else {
	cout << "enough dimensions found - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** iterate dims using iter" << endl ;
    Array::Dim_iter diter = ar.dim_begin() ;
    i = dims.begin() ;
    d = dimsize.begin() ;
    for( ; diter != ar.dim_end() && i != dims.end(); diter++, i++, d++ )
    {
	if( ar.dimension_name( diter ) == (*i) )
	{
	    cout << "comparing " << ar.dimension_name(diter) << " to " << (*i) 
		 << " - PASS" << endl ;
	    if( ar.dimension_size( diter ) == (*d) )
	    {
		cout << "comparing size " << ar.dimension_size(diter) 
		     << " to expected " << (*d) << " - PASS" << endl ;
	    } else {
		cerr << "comparing size " << ar.dimension_size(diter) 
		     << " to expected " << (*d) << " - FAIL" << endl ;
	    }
	    if( ar.dimension_start( diter ) == 0 )
	    {
		cout << "comparing start " << ar.dimension_start(diter)
		     << " to expected 0 - PASS" << endl ;
	    } else {
		cerr << "comparing start " << ar.dimension_start(diter)
		     << " to expected 0 - FAIL" << endl ;
	    }
	    if( ar.dimension_stop( diter ) == (*d)-1 )
	    {
		cout << "comparing stop " << ar.dimension_stop(diter)
		     << " to expected " << (*d)-1 << " - PASS" << endl ;
	    } else {
		cerr << "comparing stop " << ar.dimension_stop(diter)
		     << " to expected " << (*d)-1 << " - FAIL" << endl ;
	    }
	    if( ar.dimension_stride( diter ) == 1 )
	    {
		cout << "comparing stride " << ar.dimension_stride(diter)
		     << " to expected 1 - PASS" << endl ;
	    } else {
		cerr << "comparing stride " << ar.dimension_stride(diter)
		     << " to expected 1 - FAIL" << endl ;
	    }
	} else {
	    cerr << "comparing " << ar.dimension_name(diter) << " to " << (*i) 
		 << " - FAIL" << endl ;
	}
    }
    if( diter != ar.dim_end() && i == dims.end() )
    {
	cerr << "too many dimensions - FAIL" << endl ;
    }
    else if( diter == ar.dim_end() && i != dims.end() )
    {
	cerr << "not enough dimensions - FAIL" << endl ;
    }
    else {
	cout << "enough dimensions found - PASS" << endl ;
    }

    cout << endl << __LINE__ << " **** get number of dimensions" << endl ;
    unsigned int numdims = ar.dimensions( ) ;
    if( numdims == 3 )
    {
	cout << "number of dimensions = " << numdims << " - PASS" << endl ;
    } else {
	cerr << "number of dimensions = " << numdims << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add constraint for first dim using Pix"
	 << endl ;
    p = ar.first_dim() ;
    ar.add_constraint( p, 0, 2, 3 ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 12 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** add constraint for second dim using iter"
		 << endl ;
    diter = ar.dim_begin() ;
    diter++ ;
    ar.add_constraint( diter, 0, 2, 2 ) ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 8 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** reset constraints" << endl ;
    ar.reset_constraint() ;

    cout << endl << __LINE__ << " **** get the length" << endl ;
    l = ar.length() ;
    if( l == 24 )
    {
	cout << "length = " << l << " - PASS" << endl ;
    } else {
	cerr << "length = " << l << " - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** call read" << endl ;
    bool is_read = ar.read( "dataset" ) ;
    if( is_read == true )
    {
	cout << "read completed successfully - PASS" << endl ;
    } else {
	cerr << "read completed successfully - FAIL" << endl ;
    }

    cout << endl << __LINE__ << " **** print the array" << endl ;
    ar.print_val( cout ) ;

    cout << endl << __LINE__ << " **** delete BaseType" << endl ;
    delete bt ;

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

