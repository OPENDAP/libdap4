// OPeNDAPDir.h

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#ifndef I_OPeNDAPDir_h
#define I_OPeNDAPDir_h

#include <list>
#include <string>

using std::list ;
using std::string ;

#include "OPeNDAPFile.h"

class OPeNDAPDir
{
private:
    string    _dirName ;
    string    _fileExpr ;
    list<OPeNDAPFile>   _fileList ;
    list<OPeNDAPDir>   _dirList ;
    bool    _dirLoaded ;

    void    loadDir() ;
public:
    OPeNDAPDir(const string &dirName) ;
    OPeNDAPDir(const string &dirName,
               const string &fileExpr) ;
    OPeNDAPDir(const OPeNDAPDir &copyFrom) ;
    virtual    ~OPeNDAPDir() ;

    typedef list<OPeNDAPDir>::iterator dirIterator ;
    virtual OPeNDAPDir::dirIterator beginOfDirList() ;
    virtual OPeNDAPDir::dirIterator endOfDirList() ;

    typedef list<OPeNDAPFile>::iterator fileIterator ;
    virtual OPeNDAPDir::fileIterator beginOfFileList() ;
    virtual OPeNDAPDir::fileIterator endOfFileList() ;

    virtual string   getDirName()
    {
        return _dirName ;
    }
} ;

#endif // I_OPeNDAPDir_h

