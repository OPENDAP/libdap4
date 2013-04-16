/*
    This header is used when the compile-time definitions won't work.
*/

#ifndef __DODS_DATATYPES__
#define __DODS_DATATYPES__

#include <inttypes.h>

namespace libdap
{

typedef int32_t dods_int32;

typedef uint32_t dods_uint32;

typedef int16_t dods_int16;

typedef uint16_t dods_uint16;

typedef uint8_t dods_byte;

typedef double dods_float64;

typedef float dods_float32;		

// Added for DAP4
typedef int8_t dods_int8;

typedef int64_t dods_int64;

typedef uint64_t dods_uint64;

typedef int64_t dods_enum;

} // namespace libdap

#endif /* __DODS_DATATYPES__ */
