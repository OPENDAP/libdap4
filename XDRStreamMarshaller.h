// XDRStreamMarshaller.h

#ifndef I_XDRStreamMarshaller_h
#define I_XDRStreamMarshaller_h 1

#include <iostream>

using std::ostream ;
using std::cout ;

#include "Marshaller.h"
#include "XDRUtils.h"

class XDRStreamMarshaller : public Marshaller
{
private:
    static char *		_buf ;
    XDR *			_sink ;
    ostream &			_out ;
    				XDRStreamMarshaller()
				    : _sink( 0 ), _out( cout ) {} ;
public:
    				XDRStreamMarshaller( ostream &out ) ;
    virtual			~XDRStreamMarshaller() ;

    virtual void		put_byte( dods_byte val ) ;

    virtual void		put_int16( dods_int16 val ) ;
    virtual void		put_int32( dods_int32 val ) ;

    virtual void		put_float32( dods_float32 val ) ;
    virtual void		put_float64( dods_float64 val ) ;

    virtual void		put_uint16( dods_uint16 val ) ;
    virtual void		put_uint32( dods_uint32 val ) ;

    virtual void		put_str( const string &val ) ;
    virtual void		put_url( const string &val ) ;

    virtual void		put_opaque( char *val, unsigned int len ) ;
    virtual void		put_int( int val ) ;

    virtual void		put_vector( char *val, int num,
					    Vector &vec ) ;
    virtual void		put_vector( char *val, int num, int width,
                                            Vector &vec ) ;

    virtual void		dump(ostream &strm) const ;
} ;

#endif // I_XDRStreamMarshaller_h

