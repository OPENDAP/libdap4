// XDRFileUnMarshaller.h

#ifndef I_XDRFileUnMarshaller_h
#define I_XDRFileUnMarshaller_h 1

#include "UnMarshaller.h"
#include "XDRUtils.h"

class XDRFileUnMarshaller : public UnMarshaller
{
private:
    XDR *			_source ;
    				XDRFileUnMarshaller() : _source( 0 ) {} ;
public:
    				XDRFileUnMarshaller( FILE *out ) ;
    virtual			~XDRFileUnMarshaller() ;

    virtual void		get_byte( dods_byte &val ) ;

    virtual void		get_int16( dods_int16 &val ) ;
    virtual void		get_int32( dods_int32 &val ) ;

    virtual void		get_float32( dods_float32 &val ) ;
    virtual void		get_float64( dods_float64 &val ) ;

    virtual void		get_uint16( dods_uint16 &val ) ;
    virtual void		get_uint32( dods_uint32 &val ) ;

    virtual void		get_str( string &val ) ;
    virtual void		get_url( string &val ) ;

    virtual void		get_opaque( char *val, unsigned int len ) ;
    virtual void		get_int( int &val ) ;

    virtual void		get_vector( char **val, unsigned int &num,
					    Vector &vec ) ;
    virtual void		get_vector( char **val, unsigned int &num,
					    int width, Vector &vec ) ;

    virtual void		dump(ostream &strm) const ;
} ;

#endif // I_XDRFileUnMarshaller_h

