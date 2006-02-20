// OPeNDAPDir.cc

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef WIN32
#include <config.h>  //  for S_ISDIR macro
#endif
#include <stdio.h>
#include <iostream>

using std::cout ;
using std::endl ;

#include "OPeNDAPDir.h"
#include "GNURegex.h"

OPeNDAPDir::OPeNDAPDir( const string &dirName )
    : _dirName( dirName ),
      _fileExpr( "" ),
      _dirLoaded( false )
{
}

OPeNDAPDir::OPeNDAPDir( const string &dirName, const string &fileExpr )
    : _dirName( dirName ),
      _fileExpr( fileExpr ),
      _dirLoaded( false )
{
}

OPeNDAPDir::OPeNDAPDir( const OPeNDAPDir &copyFrom )
    : _dirName( copyFrom._dirName ),
      _fileExpr( copyFrom._fileExpr ),
      _dirLoaded( false )
{
}

OPeNDAPDir::~OPeNDAPDir()
{
}

OPeNDAPDir::dirIterator
OPeNDAPDir::beginOfDirList()
{
    if( _dirLoaded == false )
    {
	loadDir() ;
	_dirLoaded = true ;
    }
    return _dirList.begin() ;
}

OPeNDAPDir::dirIterator
OPeNDAPDir::endOfDirList()
{
    if( _dirLoaded == false )
    {
	loadDir() ;
	_dirLoaded = true ;
    }
    return _dirList.end() ;
}

OPeNDAPDir::fileIterator
OPeNDAPDir::beginOfFileList()
{
    if( _dirLoaded == false )
    {
	loadDir() ;
	_dirLoaded = true ;
    }
    return _fileList.begin() ;
}

OPeNDAPDir::fileIterator
OPeNDAPDir::endOfFileList()
{
    if( _dirLoaded == false )
    {
	loadDir() ;
	_dirLoaded = true ;
    }
    return _fileList.end() ;
}

void
OPeNDAPDir::loadDir()
{
    DIR * dip;
    struct dirent *dit;

    // open a directory stream
    // make sure the directory is valid and readable
    if( ( dip = opendir( _dirName.c_str() ) ) == NULL )
    {
	string err_str = "ERROR: failed to open directory '" + _dirName + "'" ;
	throw err_str ;
    }
    else
    {
	// read in the files in this directory
	// add each filename to the list of filenames
	while ( ( dit = readdir( dip ) ) != NULL )
	{
	    struct stat buf;
	    string dirEntry = dit->d_name ;
	    if( dirEntry != "." && dirEntry != ".." )
	    {
		string fullPath = _dirName + "/" + dirEntry ;
		stat( fullPath.c_str(), &buf ) ;

		// look at the mode and determine if this is a filename
		// or a directory name
		if ( S_ISDIR( buf.st_mode ) )
		{
		    _dirList.push_back( OPeNDAPDir( fullPath ) ) ;
		}
		else
		{
		    if( _fileExpr != "" )
		    {
			Regex reg_expr( _fileExpr.c_str() ) ;
			if( reg_expr.match( dirEntry.c_str(),
			                    dirEntry.length() ) != -1 )
			{
			    _fileList.push_back( OPeNDAPFile( _dirName, dirEntry ) );
			}
		    }
		    else
		    {
			_fileList.push_back( OPeNDAPFile( _dirName, dirEntry ) ) ;
		    }
		}
	    }
	}
    }

    // close the directory
    closedir( dip ) ;
}

