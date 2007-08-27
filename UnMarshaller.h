// UnMarshaller.h

#ifndef A_UnMarshaller_h
#define A_UnMarshaller_h 1

#include <string>
#include <vector>

using std::string ;
using std::vector ;

#include "DapObj.h"

#include "dods-datatypes.h"

class Vector ;

class UnMarshaller : public DapObj
{
public:
    virtual void		get_byte( dods_byte &val ) = 0 ;

    virtual void		get_int16( dods_int16 &val ) = 0 ;
    virtual void		get_int32( dods_int32 &val ) = 0 ;

    virtual void		get_float32( dods_float32 &val ) = 0 ;
    virtual void		get_float64( dods_float64 &val ) = 0 ;

    virtual void		get_uint16( dods_uint16 &val ) = 0 ;
    virtual void		get_uint32( dods_uint32 &val ) = 0 ;

    virtual void		get_str( string &val ) = 0 ;
    virtual void		get_url( string &val ) = 0 ;

    virtual void		get_opaque( char *val, unsigned int len ) = 0 ;
    virtual void		get_int( int &val ) = 0 ;

    virtual void		get_vector( char **val, unsigned int &num,
                                            Vector &vec ) = 0 ;
    virtual void		get_vector( char **val, unsigned int &num,
					    int width, Vector &vec ) = 0 ;

    virtual void		dump(ostream &strm) const = 0 ;
} ;

#endif // A_UnMarshaller_h

