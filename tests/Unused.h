#ifndef _Unused_h
#define _Unused_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>
class Byte ;
class Float32 ;
class Float64 ;
class Function ;
class Grid ;
class Int32 ;
class List ;
class UInt16 ;
class UInt32 ;
class Url ;
class BaseType ;

using std::string ;

extern Byte *NewByte( const string &n = "" ) ;
extern Float32 *NewFloat32( const string &n = "" ) ;
extern Float64 *NewFloat64( const string &n = "" ) ;
extern Function *NewFunction( const string &n = "" ) ;
extern Grid *NewGrid( const string &n = "" ) ;
extern Int32 *NewInt32( const string &n = "" ) ;
extern List *NewList( const string &n = "", BaseType *v = 0 ) ;
extern UInt16 *NewUInt16( const string &n = "" ) ;
extern UInt32 *NewUInt32( const string &n = "" ) ;
extern Url *NewUrl( const string &n = "" ) ;

#endif // _Unused_h
