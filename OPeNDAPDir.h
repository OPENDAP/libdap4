// OPeNDAPDir.h

#ifndef I_OPeNDAPDir_h
#define I_OPeNDAPDir_h

#include <list>
#include <string>

using std::list ;
using std::string ;

#include "OPeNDAPFile.h"

class OPeNDAPDir {
private:
    string				_dirName ;
    string				_fileExpr ;
    list<OPeNDAPFile>			_fileList ;
    list<OPeNDAPDir>				_dirList ;
    bool				_dirLoaded ;

    void				loadDir() ;
public:
    					OPeNDAPDir( const string &dirName ) ;
					OPeNDAPDir( const string &dirName,
					       const string &fileExpr ) ;
    					OPeNDAPDir( const OPeNDAPDir &copyFrom ) ;
    virtual				~OPeNDAPDir() ;

    typedef list<OPeNDAPDir>::iterator	dirIterator ;
    virtual OPeNDAPDir::dirIterator		beginOfDirList() ;
    virtual OPeNDAPDir::dirIterator		endOfDirList() ;

    typedef list<OPeNDAPFile>::iterator	fileIterator ;
    virtual OPeNDAPDir::fileIterator		beginOfFileList() ;
    virtual OPeNDAPDir::fileIterator		endOfFileList() ;

    virtual string			getDirName() { return _dirName ; }
} ;

#endif // I_OPeNDAPDir_h

