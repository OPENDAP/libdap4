#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include <fstream>
#include <strstream>
#include "DDS.h"

using std::cout ;
using std::cerr ;
using std::endl ;
using std::ostrstream ;
using std::ifstream ;

int
main( int argc, char **argv )
{
    if( argc > 3 )
    {
	cerr << "Usage: " << argv[0]
			  << " [<num of test file(1-7)>]"
			  << " [expr] - FAIL"
	     << endl ;
	return 0 ;
    }

    bool do_all = false ;
    int do_this = 0 ;
    if( argc == 1 )
    {
	do_all = true ;
    } else {
	do_this = atoi( argv[1] ) ;
	if( do_this < 1 || do_this > 7 )
	{
	    cerr << "Usage: " << argv[0]
			      << " [<num of test file(1-7)>]"
			      << " [expr] - FAIL"
		 << endl ;
	    return 0 ;
	}
    }
    string expr ;
    if( argc == 3 )
    {
	expr = argv[2] ;
    }

    cout << endl << __LINE__ << " **** create the DDS" << endl ;
    DDS dds( "TestDDS" ) ;

    for( int i = 1; i < 8; i++ )
    {
	if( i == do_this || do_all == true )
	{
	    char testfile[64] ;
	    sprintf( testfile, "test.%d", i ) ;
	    cout << endl << __LINE__ << " **** parse " << testfile << endl ;
	    try
	    {
		dds.parse( testfile ) ;
		cout << "parsed successfully - PASS" << endl ;
		if( expr != "" )
		{
		    dds.parse_constraint( expr ) ;
		    dds.print_constrained( cerr ) ;
		}
	    }
	    catch( InternalErr &e )
	    {
		cerr << "failed to parse - FAIL" << endl ;
		e.print( cerr ) ;
	    }
	}
    }

    cout << endl << __LINE__ << " **** done" << endl ;
    return 0 ;
}

