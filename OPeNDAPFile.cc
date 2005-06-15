// OPeNDAPFile.cc

#include "OPeNDAPFile.h"

OPeNDAPFile::OPeNDAPFile( const string &fullPath )
    : _dirName( "" ),
      _fileName( "" ),
      _baseName( "" ),
      _extension( "" )
{
    breakApart( fullPath ) ;
}

OPeNDAPFile::OPeNDAPFile( const string &dirName, const string &fileName )
    : _dirName( dirName ),
      _fileName( fileName ),
      _baseName( "" ),
      _extension( "" )
{
    breakExtension() ;
}

OPeNDAPFile::OPeNDAPFile( const OPeNDAPFile &copyFrom )
    : _dirName( copyFrom._dirName ),
      _fileName( copyFrom._fileName ),
      _baseName( copyFrom._baseName ),
      _extension( copyFrom._extension )
{
}

OPeNDAPFile::~OPeNDAPFile()
{
}

string
OPeNDAPFile::getDirName()
{
    return _dirName ;
}

string
OPeNDAPFile::getFileName()
{
    return _fileName ;
}

string
OPeNDAPFile::getBaseName()
{
    return _baseName ;
}

string
OPeNDAPFile::getExtension()
{
    return _extension ;
}

string
OPeNDAPFile::getFullPath()
{
    return _dirName + "/" + _fileName ;
}

void
OPeNDAPFile::breakApart( const string &fullPath )
{
    unsigned int pos = fullPath.rfind( "/" ) ;
    if( pos != string::npos )
    {
	_dirName = fullPath.substr( 0, pos ) ;
	_fileName = fullPath.substr( pos+1, fullPath.length() - pos ) ;
    }
    else
    {
	_dirName = "./" ;
	_fileName = fullPath ;
    }

    breakExtension() ;
}

void
OPeNDAPFile::breakExtension()
{
    unsigned int pos = _fileName.rfind( "." ) ;
    if( pos != string::npos )
    {
	_baseName = _fileName.substr( 0, pos ) ;
	_extension = _fileName.substr( pos+1, _fileName.length() - pos ) ;
    }
    else
    {
	_baseName = _fileName ;
    }
}

