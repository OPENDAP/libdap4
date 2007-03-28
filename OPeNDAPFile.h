// OPeNDAPFile.h

#ifndef I_OPeNDAPFile_h
#define I_OPeNDAPFile_h

#include <string>

using std::string ;

#include "OPeNDAPFile.h"

class OPeNDAPFile
{
private:
    string    _dirName ;
    string    _fileName ;
    string    _baseName ;
    string    _extension ;

    void    breakApart(const string &fullPath) ;
    void    breakExtension() ;
public:
    OPeNDAPFile(const string &fullPath) ;
    OPeNDAPFile(const string &dirName,
                const string &fileName) ;
    OPeNDAPFile(const OPeNDAPFile &copyFrom) ;
    virtual    ~OPeNDAPFile() ;

    virtual string   getDirName() ;
    virtual string   getFileName() ;
    virtual string   getBaseName() ;
    virtual string   getExtension() ;
    virtual string   getFullPath() ;
} ;

#endif // I_OPeNDAPDir_h

