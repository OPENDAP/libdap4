// XDRFileUnMarshaller.cc

#include "XDRFileUnMarshaller.h"

#include "Vector.h"
#include "util.h"

XDRFileUnMarshaller::XDRFileUnMarshaller( FILE *out )
    : _source( 0 )
{
    _source = new_xdrstdio( out, XDR_DECODE ) ;
}

XDRFileUnMarshaller::~XDRFileUnMarshaller( )
{
    delete_xdrstdio( _source ) ;
}

void
XDRFileUnMarshaller::get_byte( dods_byte &val )
{
    if( !xdr_char( _source, (char *)&val ) )
        throw Error("Network I/O Error. Could not read byte data. This may be due to a\nbug in DODS or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_int16( dods_int16 &val )
{
    if( !XDR_INT16( _source, &val ) )
        throw Error("Network I/O Error. Could not read int 16 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_int32( dods_int32 &val )
{
    if( !XDR_INT32( _source, &val ) )
        throw Error("Network I/O Error. Could not read int 32 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_float32( dods_float32 &val )
{
    if( !xdr_float( _source, &val ) )
        throw Error("Network I/O Error. Could not read float 32 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_float64( dods_float64 &val )
{
    if( !xdr_double( _source, &val ) )
        throw Error("Network I/O Error. Could not read float 64 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_uint16( dods_uint16 &val )
{
    if( !XDR_UINT16( _source, &val ) )
        throw Error("Network I/O Error. Could not read uint 16 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_uint32( dods_uint32 &val )
{
    if( !XDR_UINT32( _source, &val ) )
        throw Error("Network I/O Error. Could not read uint 32 data. This may be due to a\nbug in libdap or a problem with the network connection.");
}

void
XDRFileUnMarshaller::get_str( string &val )
{
    char *in_tmp = NULL ;

    if( !xdr_string( _source, &in_tmp, max_str_len ) )
        throw Error("Network I/O Error. Could not read string data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    val = in_tmp ;

    free( in_tmp ) ;
}

void
XDRFileUnMarshaller::get_url( string &val )
{
    get_str( val ) ;
}

void
XDRFileUnMarshaller::get_opaque( char *val, unsigned int len )
{
    xdr_opaque( _source, val, len ) ;
}

void
XDRFileUnMarshaller::get_int( int &val )
{
    if( !xdr_int( _source, &val ) )
	throw Error("Network I/O Error(1). This may be due to a bug in libdap or a\nproblem with the network connection.");
}

void
XDRFileUnMarshaller::get_vector( char **val, unsigned int &num, Vector & )
{
    if( !xdr_bytes( _source, val, &num, DODS_MAX_ARRAY) )
	throw Error("Network I/O error. Could not read packed array data.\nThis may be due to a bug in libdap or a problem with\nthe network connection.");
}

void
XDRFileUnMarshaller::get_vector( char **val, unsigned int &num, int width, Vector &vec )
{
    BaseType *var = vec.var() ;

    if( !xdr_array( _source, val, &num, DODS_MAX_ARRAY, width,
		    XDRUtils::xdr_coder( var->type() ) ) )
    {
	throw Error("Network I/O error. Could not read packed array data.\nThis may be due to a bug in libdap or a problem with\nthe network connection.");
    }
}

void
XDRFileUnMarshaller::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "XDRFileUnMarshaller::dump - ("
         << (void *)this << ")" << endl ;
}

