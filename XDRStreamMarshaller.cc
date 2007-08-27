// XDRStreamMarshaller.cc

#include "XDRStreamMarshaller.h"

#include "Vector.h"
#include "util.h"

char *XDRStreamMarshaller::_buf = 0 ;

XDRStreamMarshaller::XDRStreamMarshaller( ostream &out )
    : _sink( 0 ),
      _out( out )
{
    if( !_buf )
	_buf = (char *)malloc( DODS_MAX_ARRAY ) ;
    _sink = new XDR ;
    xdrmem_create( _sink, _buf, DODS_MAX_ARRAY, XDR_ENCODE ) ;
}

XDRStreamMarshaller::~XDRStreamMarshaller( )
{
    if( _sink )
	delete_xdrstdio( _sink ) ;
}

void
XDRStreamMarshaller::put_byte( dods_byte val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !xdr_char( _sink, (char *)&val ) )
        throw Error("Network I/O Error. Could not send byte data.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_int16( dods_int16 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !XDR_INT16( _sink, &val ) )
        throw Error("Network I/O Error. Could not send int 16 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_int32( dods_int32 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !XDR_INT32( _sink, &val ) )
        throw Error("Network I/O Error. Culd not read int 32 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_float32( dods_float32 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !xdr_float( _sink, &val ) )
        throw Error("Network I/O Error. Could not send float 32 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_float64( dods_float64 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !xdr_double( _sink, &val ) )
        throw Error("Network I/O Error. Could not send float 64 data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_uint16( dods_uint16 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !XDR_UINT16( _sink, &val ) )
        throw Error("Network I/O Error. Could not send uint 16 data. This may be due to a\nbug in libdap or a problem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_uint32( dods_uint32 val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !XDR_UINT32( _sink, &val ) )
        throw Error("Network I/O Error. Could not send uint 32 data. This may be due to a\nbug in libdap or a problem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_str( const string &val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    const char *out_tmp = val.c_str() ;
    if( !xdr_string( _sink, (char **)&out_tmp, max_str_len) )
        throw Error("Network I/O Error. Could not send string data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");

    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_url( const string &val )
{
    put_str( val ) ;
}

void
XDRStreamMarshaller::put_opaque( char *val, unsigned int len )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !xdr_opaque( _sink, val, len ) )
        throw Error("Network I/O Error. Could not send opaque data.\nThis may be due to a bug in libdap, on the server or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_int( int val )
{
    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    if( !xdr_int( _sink, &val) )
	throw Error("Network I/O Error(1). This may be due to a bug in libdap or a\nproblem with the network connection.");
    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_vector( char *val, int num, Vector & )
{
    if (!val)
	throw InternalErr(__FILE__, __LINE__,
			  "Buffer pointer is not set.");

    // write the number of members of the array being written and then set the position to 0
    put_int( num ) ;

    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    if( !xdr_bytes( _sink, (char **)&val,
		    (unsigned int *) &num,
		    DODS_MAX_ARRAY) )
    {
	throw Error("Network I/O Error(2). This may be due to a bug in libdap or a\nproblem with the network connection.");
    }

    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::put_vector( char *val, int num, int width, Vector &vec )
{
    if (!val)
	throw InternalErr(__FILE__, __LINE__,
			  "Buffer pointer is not set.");
    // write the number of array members being written, then set the position back to 0
    put_int( num ) ;

    if( !xdr_setpos( _sink, 0 ) )
        throw Error("Network I/O Error. Could not send byte data - unable to set stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");

    BaseType *var = vec.var() ;
    if( !xdr_array( _sink, (char **)&val,
		    (unsigned int *) & num,
		    DODS_MAX_ARRAY, width,
		    XDRUtils::xdr_coder( var->type() ) ) )
    {
	throw Error("Network I/O Error(2). This may be due to a bug in libdap or a\nproblem with the network connection.");
    }

    unsigned int bytes_written = xdr_getpos( _sink ) ;
    if( !bytes_written )
        throw Error("Network I/O Error. Could not send byte data - unable to get stream position.\nThis may be due to a bug in DODS, on the server or a\nproblem with the network connection.");
    _out.write( _buf, bytes_written ) ;
}

void
XDRStreamMarshaller::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "XDRStreamMarshaller::dump - ("
         << (void *)this << ")" << endl ;
}

