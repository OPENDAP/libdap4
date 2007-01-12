// DapIndent.h

#ifndef I_DapIndent_h
#define I_DapIndent_h 1

#include <string>
#include <iostream>

using std::string ;
using std::ostream ;

class DapIndent
{
private:
    static string		_indent ;
public:
    static void			Indent() ;
    static void			UnIndent() ;
    static void			Reset() ;
    static const string &	GetIndent() ;
    static void			SetIndent( const string &indent ) ;
    static ostream &		LMarg( ostream &strm ) ;
} ;

#endif // I_DapIndent_h

