// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Specialize DDS for returned data. This currently means adding version
// information about the source of the data. Was it from a version 1, 2 or
// later server?
// 
// jhrg 9/19/97

// $Log: DataDDS.h,v $
// Revision 1.4  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.3.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.3  1998/02/05 20:13:52  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.2  1998/01/12 14:27:57  tom
// Second pass at class documentation.
//
// Revision 1.1  1997/09/22 22:19:27  jimg
// Created this subclass of DDS to hold version information in the data DDS
//

#ifndef _DataDDS_h
#define _DataDDS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "config_dap.h"

#include <iostream>
#include <string>
#include <Pix.h>
#include <SLList.h>

#include "BaseType.h"
#include "Clause.h"
#include "expr.h"
#include "debug.h"
#include "DDS.h"

/** This class adds some useful state information to the DDS
    structure.  It is for use on the client side of the DODS
    connection. 
    
    @memo Holds a DODS DDS.
    @see Connect
    */

class DataDDS : public DDS {
private:
    string _server_version;
    int _server_version_major;
    int _server_version_minor;

    void _version_string_to_numbers();

    // The last level read from a sequence. This is used to read nested
    // sequences. 
    int _sequence_level;
public:
  /** The DataDDS constructor needs a name and a version string.  This
      is generally received from the server.
      */
    DataDDS(const string &n = "", const string &v = "");
    virtual ~DataDDS();

  /** Sets the version string.  This typically looks something like:
      #DODS/2.15#, where ``2'' is the major version number, and ``15''
      the minor number.
      */
    void set_version(const string &v);
  /** Returns the major version number. */
    int get_version_major();
  /** Returns the minor version number. */
    int get_version_minor();

  /** Return the last level of a sequence object that was read. Note
      that #Sequence::deserialize()# is the main user of this
      information and it really only matters in cases where the
      Sequence object contains other Sequence objects. In that case,
      this information provides state for #Sequence::deserialize()# so
      that it can return to the level at which it last read.

      @name sequence\_level()
      @memo Returns the level of the last sequence read.  */
    int sequence_level();
    
  /** Set the value for #sequence_level()#. Use this function to store
      state information about the current sequence. This is used
      mostly when reading nested sequences so that
      #Sequence::deserialize()# can return to the correct level when
      resuming a deserialization from a subsequent call.

      @name set\_sequence\_level(int level)
      @memo Sets the level of the sequence being read.  */
    void set_sequence_level(int level);
};

#endif // _DataDDS_h
