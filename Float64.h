// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Float64 type.
//
// jhrg 9/7/94

/* 
 * $Log: Float64.h,v $
 * Revision 1.21  1997/12/18 15:06:11  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.20  1997/10/09 22:19:20  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.19  1997/08/11 18:19:15  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.18  1996/12/02 23:10:17  jimg
 * Added dataset as a parameter to the ops member function.
 *
 * Revision 1.17  1996/06/04 21:33:26  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.16  1996/05/31 23:29:41  jimg
 * Updated copyright notice.
 *
 * Revision 1.15  1996/05/16 22:49:59  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.14  1996/03/05 18:21:43  jimg
 * Added ce_eval to serailize member function.
 * Added ops member function and float_op function.
 *
 * Revision 1.13  1995/12/09  01:06:42  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.12  1995/12/06  21:35:22  jimg
 * Changed read() from three to two parameters.
 * Removed store_val() and read_val() (use buf2val() and val2buf()).
 *
 * Revision 1.11  1995/08/26  00:31:30  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.10  1995/08/22  23:48:14  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.9  1995/03/04  14:34:58  jimg
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
 * Revision 1.8  1995/02/10  02:22:46  jimg
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
 * Revision 1.7  1995/01/19  21:59:14  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.6  1995/01/18  18:38:39  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.5  1995/01/11  15:54:32  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.4  1994/11/29  20:10:40  jimg
 * Added functions for data transmission.
 * Added boolean parameter to serialize which, when true, causes the output
 * buffer to be flushed. The default value is false.
 * Added FILE *in and *out parameters to the ctor. The default values are
 * stdin/out.
 * Removed the `type' parameter from the ctor.
 *
 * Revision 1.3  1994/11/22  14:05:50  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.2  1994/09/23  14:36:11  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:01  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Float64_h
#define _Float64_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include "BaseType.h"

/** Holds a 64-bit (double precision) floating point value.

    @memo Holds a 64-bit floating point number.
    @see BaseType
    */

class Float64: public BaseType {
protected:
    double _buf;

public:
    Float64(const String &n = (char *)0);
    virtual ~Float64() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool ops(BaseType &b, int op, const String &dataset);
};

typedef Float64 * Float64Ptr;

#endif

