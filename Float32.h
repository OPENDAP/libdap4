// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Float32 type.
//
// 3/22/9 jhrg9

#ifndef _float32_h
#define _float32_h 1

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
#endif // 0

#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

/** Holds a 32-bit floating point value.

    @see BaseType
    */

class Float32: public BaseType {
    /** This class allows Byte, ..., Float64 acesss to _buf to simplify and
	speed up the relational operators.

	NB: According to Stroustrup it does not matter where (public, private
	or protected) friend classes are declared. */
    friend class Byte;
    friend class Int16;
    friend class UInt16;
    friend class Int32;
    friend class UInt32;
    friend class Float64;

protected:
    dods_float32 _buf;

public:
  /** The Float32 constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The Float32 constructor. */
    Float32(const string &n = "");

    Float32(const Float32 &copy_from);

    Float32 &operator=(const Float32 &rhs);

    virtual ~Float32() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const string &dataset) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, string space = "", 
			   bool print_decl_p = true);

    virtual bool ops(BaseType *b, int op, const string &dataset);
};

// $Log: Float32.h,v $
// Revision 1.14  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.13.4.1  2001/07/28 01:10:42  jimg
// Some of the numeric type classes did not have copy ctors or operator=.
// I added those where they were needed.
// In every place where delete (or delete []) was called, I set the pointer
// just deleted to zero. Thus if for some reason delete is called again
// before new memory is allocated there won't be a mysterious crash. This is
// just good form when using delete.
// I added calls to www2id and id2www where appropriate. The DAP now handles
// making sure that names are escaped and unescaped as needed. Connect is
// set to handle CEs that contain names as they are in the dataset (see the
// comments/Log there). Servers should not handle escaping or unescaping
// characters on their own.
//
// Revision 1.13  2000/09/22 02:17:20  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.12  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.11  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.8.6.2  2000/08/02 21:10:07  jimg
// Removed the header config_dap.h. If this file uses the dods typedefs for
// cardinal datatypes, then it gets those definitions from the header
// dods-datatypes.h.
//
// Revision 1.10  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.9  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.8.20.1  2000/06/02 18:21:27  rmorris
// Mod's for port to Win32.
//
// Revision 1.8.14.1  2000/01/28 22:14:04  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.8.6.1  1999/09/11 04:51:38  tom
// corrected and added (minute) documentation
//
// Revision 1.8  1999/04/29 02:29:29  jimg
// Merge of no-gnu branch
//
// Revision 1.7  1999/03/24 23:40:05  jimg
// Added
//
// Revision 1.5.6.1  1999/02/02 21:56:58  jimg
// String to string version
//
// Revision 1.5  1998/01/12 14:27:57  tom
// Second pass at class documentation.
//
// Revision 1.4  1997/12/18 15:06:10  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.3  1997/09/22 23:03:28  jimg
// Added DDS * to deserialize parameters.
//
// Revision 1.2  1996/12/02 23:10:13  jimg
// Added dataset as a parameter to the ops member function.
//
// Revision 1.1  1996/08/26 20:17:54  jimg
// Added.
//

#endif // _float32_h

