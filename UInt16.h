
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1996,1997,1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Int16 type. 
//
// jhrg 9/7/94

// $Log: UInt16.h,v $
// Revision 1.6  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.3.6.2  2000/08/02 21:10:08  jimg
// Removed the header config_dap.h. If this file uses the dods typedefs for
// cardinal datatypes, then it gets those definitions from the header
// dods-datatypes.h.
//
// Revision 1.5  2000/07/09 21:57:10  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.4  2000/06/07 18:06:59  jimg
// Merged the pc port branch
//
// Revision 1.3.6.1  1999/09/11 04:51:38  tom
// corrected and added (minute) documentation
//
// Revision 1.3.20.1  2000/06/02 18:29:31  rmorris
// Mod's for port to Win32.
//
// Revision 1.3  1999/05/04 19:47:23  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.2  1999/04/29 02:29:34  jimg
// Merge of no-gnu branch
//
// Revision 1.1  1999/03/24 23:40:06  jimg
// Added
//
// Revision 1.4  1997/12/18 15:06:12  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.3  1997/09/22 22:59:59  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.2  1996/12/02 23:10:20  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.1  1996/08/26 20:17:52  jimg
// Added.
//

#ifndef _UInt16_h
#define _UInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#if 0

#ifdef WIN32
#include <rpc.h>
#include <winsock.h>
#include <xdr.h>
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#include "dods-datatypes.h"
#include "BaseType.h"

class UInt16: public BaseType {
    /** This class allows Byte, ..., Float64 acesss to _buf to simplify and
	speed up the relational operators.

	NB: According to Stroustrup it does not matter where (public, private
	or protected) friend classes are declared. */
    friend class Byte;
    friend class Int16;
    friend class Int32;
    friend class UInt32;
    friend class Float32;
    friend class Float64;

protected:
    dods_uint16 _buf;

public:
    UInt16(const string &n = "");
    virtual ~UInt16() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, string space = "",
			   bool print_decl_p = true);

    virtual bool ops(BaseType *b, int op, const string &dataset);
};

#endif

