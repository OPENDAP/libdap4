// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Specialize DDS for returned data. This currently means adding version
// information about the source of the data. Was it from a version 1, 2 or
// later server?
// 
// jhrg 9/19/97

// $Log: DataDDS.h,v $
// Revision 1.1  1997/09/22 22:19:27  jimg
// Created this subclass of DDS to hold version information in the data DDS
//

#ifndef _DataDDS_h
#define _DataDDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "config_dap.h"

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"
#include "Clause.h"
#include "expr.h"
#include "debug.h"
#include "DDS.h"

class DataDDS : public DDS {
private:
    String _server_version;
    int _server_version_major;
    int _server_version_minor;

    void _version_string_to_numbers();

public:
    DataDDS(const String &n = (char *)0, const String &v = (char *)0);
    virtual ~DataDDS();

    void set_version(const String &v);
    int get_version_major();
    int get_version_minor();
};

#endif // _DataDDS_h
