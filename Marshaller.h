// Marshaller.h

#ifndef A_Marshaller_h
#define A_Marshaller_h 1

#include <string>
#include <vector>

using std::string ;
using std::vector ;

#include "DapObj.h"

#include "dods-datatypes.h"

class Vector ;

class Marshaller : public DapObj
{
public:
    virtual void		put_byte( dods_byte val ) = 0 ;

    virtual void		put_int16( dods_int16 val ) = 0 ;
    virtual void		put_int32( dods_int32 val ) = 0 ;

    virtual void		put_float32( dods_float32 val ) = 0 ;
    virtual void		put_float64( dods_float64 val ) = 0 ;

    virtual void		put_uint16( dods_uint16 val ) = 0 ;
    virtual void		put_uint32( dods_uint32 val ) = 0 ;

    virtual void		put_str( const string &val ) = 0 ;
    virtual void		put_url( const string &val ) = 0 ;

    virtual void		put_opaque( char *val, unsigned int len ) = 0 ;
    virtual void		put_int( int val ) = 0 ;

    virtual void		put_vector( char *val, int num,
                                            Vector &vec ) = 0 ;
    virtual void		put_vector( char *val, int num, int width,
                                            Vector &vec ) = 0 ;

    virtual void		dump(ostream &strm) const = 0 ;
} ;

#endif // A_Marshaller_h

