// XDRFileMarshaller.h

#ifndef I_XDRFileMarshaller_h
#define I_XDRFileMarshaller_h 1

#include "Marshaller.h"
#include "XDRUtils.h"

class XDRFileMarshaller : public Marshaller
{
private:
    XDR *			_sink ;
    				XDRFileMarshaller() : _sink( 0 ) {} ;
public:
    				XDRFileMarshaller( FILE *out ) ;
    virtual			~XDRFileMarshaller() ;

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

#endif // I_XDRFileMarshaller_h

