// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Str type.
//
// jhrg 9/7/94

/* 
 * $Log: Str.h,v $
 * Revision 1.28  1999/05/04 19:47:22  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.27  1999/04/29 02:29:31  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.26  1999/03/24 23:37:15  jimg
 * Added support for the Int16, UInt16 and Float32 types
 *
 * Revision 1.25.6.1  1999/02/02 21:57:01  jimg
 * String to string version
 *
 * Revision 1.25  1998/01/12 14:27:59  tom
 * Second pass at class documentation.
 *
 * Revision 1.24  1997/12/18 15:06:13  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.23  1997/10/09 22:19:24  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.22  1997/08/11 18:19:19  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.21  1997/05/13 23:34:22  jimg
 * Changed max_str_len to 32k - a temporary fix for String variables
 *
 * Revision 1.20  1996/12/02 23:10:27  jimg
 * Added dataset as a parameter to the ops member function.
 *
 * Revision 1.19  1996/06/04 21:33:44  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.18  1996/05/31 23:30:03  jimg
 * Updated copyright notice.
 *
 * Revision 1.17  1996/05/16 22:50:07  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.16  1996/03/05 17:38:49  jimg
 * Added ce_eval to serailize member function.
 * Replaced <limits.h> with "dods-limits.h".
 *
 * Revision 1.15  1995/12/09  01:06:58  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.14  1995/12/06  21:35:25  jimg
 * Changed read() from three to two parameters.
 * Removed store_val() and read_val() (use buf2val() and val2buf()).
 *
 * Revision 1.13  1995/08/26  00:31:47  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.12  1995/08/22  23:48:20  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.11  1995/03/04  14:35:05  jimg
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
 * Revision 1.10  1995/02/10  02:22:48  jimg
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
 * Revision 1.9  1995/01/19  21:59:24  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.8  1995/01/18  18:39:09  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.7  1995/01/11  15:54:36  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.6  1994/12/14  19:18:02  jimg
 * Added mfunc len(). Replaced size() with a mfunc that returns the size of
 * a pointer to a string (this simplifies Structure, ...).
 *
 * Revision 1.5  1994/11/29  20:16:33  jimg
 * Added mfunc for data transmission.
 * Uses special xdr function for serialization and xdr_coder.
 * Removed `type' parameter from ctor.
 * Added FILE *in and *out to ctor parameter list.
 *
 * Revision 1.4  1994/11/22  14:06:08  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/11/03  04:43:07  reza
 * Changed the default type to match the dds parser.
 *
 * Revision 1.2  1994/09/23  14:36:14  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:08  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#ifndef _Str_h
#define _Str_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include <string>

#include "dods-limits.h"
#include "BaseType.h"

// max_str_len should be large since we always send strings with length bytes
// as a prefix (so xdr_string will always know how much memory to malloc) but
// if deserialize gets confused and thinks a ctor (in particular) is a string
// xdr_string in turn will max_str_len if it cannot get a length byte. A long
// term solution is to fix DODS, but strings should not routinely be > 32k
// for the time being... jhrg 4/30/97

const unsigned int max_str_len = 32767; // DODS_UINT_MAX-1; 

/** Holds character string data.

    @see BaseType
    @see Url
    */
    
class Str: public BaseType {

protected:
    string _buf;

public:
  /** The Str constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The Str constructor. */
    Str(const string &n = "");

    virtual ~Str() {}

    virtual BaseType *ptr_duplicate() = 0;
    
    virtual unsigned int width();

    // Return the length of the stored string or zero if no string has been
    // stored in the instance's internal buffer.
    unsigned int length();

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

