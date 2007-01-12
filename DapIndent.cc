// DapIndent.cc

#include "DapIndent.h"

string DapIndent::_indent ;

void
DapIndent::Indent()
{
    _indent += "    " ;
}

void
DapIndent::UnIndent()
{
    if( _indent.length() == 0 )
	return ;
    if( _indent.length() == 4 )
	_indent = "" ;
    else
	_indent = _indent.substr( 0, _indent.length() - 4 ) ;
}

void
DapIndent::Reset()
{
    _indent = "" ;
}

const string &
DapIndent::GetIndent()
{
    return _indent ;
}

void
DapIndent::SetIndent( const string &indent )
{
    _indent = indent ;
}

ostream &
DapIndent::LMarg( ostream &strm )
{
    strm << _indent ;
    return strm ;
}

