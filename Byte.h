// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Byte type. 
//
// jhrg 9/7/94

/* 
 * $Log: Byte.h,v $
 * Revision 1.21  1997/10/09 22:19:15  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.20  1997/08/11 18:19:12  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.19  1996/12/02 23:10:07  jimg
 * Added dataset as a parameter to the ops member function.
 *
 * Revision 1.18  1996/06/04 21:33:14  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.17  1996/05/31 23:29:28  jimg
 * Updated copyright notice.
 *
 * Revision 1.16  1996/05/16 22:49:58  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.15  1996/05/14 15:38:19  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.14  1996/03/05 18:42:06  jimg
 * Added ce_eval to serailize member function.
 * Added ops member function and byte_ops interface function.
 *
 * Revision 1.13  1995/12/09  01:06:36  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.12  1995/12/06  21:35:15  jimg
 * Changed read() from three to two parameters.
 * Removed store_val() and read_val() (use buf2val() and val2buf()).
 *
 * Revision 1.11  1995/08/26  00:31:28  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.10  1995/08/22  23:48:13  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.9  1995/03/04  14:34:57  jimg
 * Major modifications to the transmission and representation of values:
 * Added card() virtual function which is true for classes that
 * contain cardinal types (byte, int float, string).
 * Changed the representation of Str from the C rep to a C++
 * class represenation.
 * Chnaged read_val and store_val so that they take and return
 * types that are stored by the object (e.g., inthe case of Str
 * an URL, read_val returns a C++ String object).
 * Modified Array representations so that arrays of card()
 * objects are just that - no more storing strings, ... as
 * C would store them.
 * Arrays of non cardinal types are arrays of the DODS objects (e.g.,
 * an array of a structure is represented as an array of Structure
 * objects).
 *
 * Revision 1.8  1995/02/10  02:22:40  jimg
 * Added DBMALLOC includes and switch to code which uses malloc/free.
 * Private and protected symbols now start with `_'.
 * Added new accessors for name and type fields of BaseType; the old ones
 * will be removed in a future release.
 * Added the store_val() mfunc. It stores the given value in the object's
 * internal buffer.
 * Made both List and Str handle their values via pointers to memory.
 * Fixed read_val().
 * Made serialize/deserialize handle all malloc/free calls (even in those
 * cases where xdr initiates the allocation).
 * Fixed print_val().
 *
 * Revision 1.7  1995/01/19  21:59:12  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.6  1995/01/18  18:38:26  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.5  1995/01/11  15:54:27  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.4  1994/11/29  20:06:33  jimg
 * Added mfuncs for data transmission.
 * Made the xdr_coder function pointer xdr_bytes() while (de)serialize() uses
 * xdr_char().
 * Removed `type' from ctor parameter list.
 * Added FILE *in and *out to parameter list (they default to stdin/out).
 *
 * Revision 1.3  1994/11/22  14:05:32  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.2  1994/09/23  14:36:07  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:08:57  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Byte_h
#define _Byte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include "BaseType.h"

class Byte: public BaseType {
protected:
    dods_byte _buf;

public:
    Byte(const String &n = (char *)0);
    virtual ~Byte() {}

    virtual BaseType *ptr_duplicate() = 0;

    virtual unsigned int width();

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool ops(BaseType &b, int op, const String &dataset);
};

typedef Byte * BytePtr;

#endif

