// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface for Url type.
//
// jhrg 9/7/94

#ifndef _url_h
#define _url_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#ifndef _dods_limits_h
#include "dods-limits.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _str_h
#include "Str.h"
#endif

const unsigned int max_url_len = 255;

/** Holds an internet address (URL).

    @see BaseType
    @see Str */
class Url: public Str {

protected:
    string _buf;

    /** This enables instances of Str to access _buf in this class. */
    friend class Str;

public:
  /** The URL constructor requires only the name of the variable
      to be created.  The name may be omitted, which will create a
      nameless variable.  This may be adequate for some applications. 
      
      @param n A string containing the name of the variable to be
      created. 

      @memo The URL constructor.
      */
    Url(const string &n = "");
    virtual ~Url() {}

    virtual BaseType *ptr_duplicate() = 0;
};

/* 
 * $Log: Url.h,v $
 * Revision 1.19  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.18  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.17  1999/04/29 02:29:34  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.16  1999/03/24 23:35:11  jimg
 * Added support for the new Int16, UInt16 and Float32 types.
 *
 * Revision 1.15.6.1  1999/02/02 21:57:04  jimg
 * String to string version
 *
 * Revision 1.15  1998/01/12 14:28:00  tom
 * Second pass at class documentation.
 *
 * Revision 1.14  1997/12/18 15:06:14  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.13  1997/08/11 18:19:30  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.12  1996/05/31 23:30:41  jimg
 * Updated copyright notice.
 *
 * Revision 1.11  1996/03/05 01:10:34  jimg
 * Fixed Comments.
 *
 * Revision 1.10  1995/03/16  17:30:46  jimg
 * This class is now a subclass of Str.
 *
 * Revision 1.9  1995/03/04  14:35:09  jimg
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
 * Revision 1.8  1995/02/10  02:22:51  jimg
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
 * Revision 1.7  1995/01/19  21:59:48  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.6  1995/01/18  18:40:34  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.5  1995/01/11  15:54:38  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.4  1994/11/29  20:16:36  jimg
 * Added mfunc for data transmission.
 * Uses special xdr function for serialization and xdr_coder.
 * Removed `type' parameter from ctor.
 * Added FILE *in and *out to ctor parameter list.
 *
 * Revision 1.3  1994/11/22  14:06:18  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.2  1994/09/23  14:36:16  jimg
 * Fixed errors in comments.
 *
 * Revision 1.1  1994/09/15  21:09:10  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#endif // _url_h

