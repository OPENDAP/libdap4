
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Interface for Int32 type. 
//
// jhrg 9/7/94

#ifndef _int32_h
#define _int32_h 1


#ifndef _dods_datatypes_h
#include "dods-datatypes.h"
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

/** @brief Holds a 32-bit signed integer. 

    @see BaseType
    */

class Int32: public BaseType {
    /** This class allows Byte, ..., Float64 access to <tt>_buf</tt> to 
	simplify and speed up the relational operators.

	NB: According to Stroustrup it does not matter where (public, private
	or protected) friend classes are declared. */
    friend class Byte;
    friend class Int16;
    friend class UInt16;
    friend class UInt32;
    friend class Float32;
    friend class Float64;

protected:
    dods_int32 _buf;

public:
  Int32(const string &n = "");

  Int32(const Int32 &copy_from);

  Int32 &operator=(const Int32 &rhs);

  virtual ~Int32();

    virtual BaseType *ptr_duplicate();
    
  virtual unsigned int width();

  virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual unsigned int val2buf(void *val, bool reuse = false);
    virtual unsigned int buf2val(void **val);
#if 0
  virtual void print_val(ostream &os, string space = "",
			 bool print_decl_p = true);
#endif
    virtual void print_val(FILE *out, string space = "",
			   bool print_decl_p = true);

    virtual bool ops(BaseType *b, int op, const string &dataset);
};

/* 
 * $Log: Int32.h,v $
 * Revision 1.39  2004/07/07 21:08:47  jimg
 * Merged with release-3-4-8FCS
 *
 * Revision 1.37.2.3  2004/07/02 20:41:52  jimg
 * Removed (commented) the pragma interface/implementation lines. See
 * the ChangeLog for more details. This fixes a build problem on HP/UX.
 *
 * Revision 1.38  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.37.2.2  2003/09/06 22:37:50  jimg
 * Updated the documentation.
 *
 * Revision 1.37.2.1  2003/06/23 11:49:18  rmorris
 * The // #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.37  2003/04/22 19:40:27  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.36  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.35.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.35  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.34  2003/01/10 19:46:40  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.31.4.4  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.31.4.3  2002/08/08 06:54:57  jimg
 * Changes for thread-safety. In many cases I found ugly places at the
 * tops of files while looking for globals, et c., and I fixed them up
 * (hopefully making them easier to read, ...). Only the files RCReader.cc
 * and usage.cc actually use pthreads synchronization functions. In other
 * cases I removed static objects where they were used for supposed
 * improvements in efficiency which had never actually been verifiied (and
 * which looked dubious).
 *
 * Revision 1.33  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.31.4.2  2002/05/22 16:57:51  jimg
 * I modified the `data type classes' so that they do not need to be
 * subclassed for clients. It might be the case that, for a complex client,
 * subclassing is still the best way to go, but you're not required to do
 * it anymore.
 *
 * Revision 1.32  2001/06/15 23:49:02  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.31.4.1  2001/06/07 20:31:13  jimg
 * Added copy constructor and operator=(). The _buf field is copied.
 *
 * Revision 1.31  2000/09/22 02:17:20  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.30  2000/09/21 16:22:08  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.29  2000/08/02 22:46:49  jimg
 * Merged 3.1.8
 *
 * Revision 1.26.6.2  2000/08/02 21:10:07  jimg
 * Removed the header config.h. If this file uses the dods typedefs for
 * cardinal datatypes, then it gets those definitions from the header
 * dods-datatypes.h.
 *
 * Revision 1.28  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.26.20.1  2000/06/02 18:29:31  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.27  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.26.14.1  2000/01/28 22:14:05  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.26.6.1  1999/09/11 04:51:38  tom
 * corrected and added (minute) documentation
 *
 * Revision 1.26  1999/04/29 02:29:30  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.25  1999/03/24 23:37:15  jimg
 * Added support for the Int16, UInt16 and Float32 types
 *
 * Revision 1.24.6.1  1999/02/02 21:57:00  jimg
 * String to string version
 *
 * Revision 1.24  1998/01/12 14:27:58  tom
 * Second pass at class documentation.
 *
 * Revision 1.23  1997/12/18 15:06:12  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.22  1997/10/09 22:19:21  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.21  1997/08/11 18:19:17  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.20  1996/12/02 23:10:23  jimg
 * Added dataset as a parameter to the ops member function.
 *
 * Revision 1.19  1996/06/04 21:33:36  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.18  1996/05/31 23:29:52  jimg
 * Updated copyright notice.
 *
 * Revision 1.17  1996/05/16 22:50:04  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.16  1996/05/14 15:38:30  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.15  1996/03/05 18:07:21  jimg
 * Added ce_eval to serailize member function.
 * Added the ops member function.
 *
 * Revision 1.14  1995/12/09  01:06:50  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.13  1995/12/06  21:35:18  jimg
 * Changed read() from three to two parameters.
 * Removed store_val() and read_val() (use buf2val() and val2buf()).
 *
 * Revision 1.12  1995/08/26  00:31:38  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.11  1995/08/22  23:48:17  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.10  1995/03/04  14:35:02  jimg
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
 * Revision 1.9  1995/02/10  02:22:42  jimg
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
 * Revision 1.8  1995/01/19  21:59:19  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.7  1995/01/18  18:38:52  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:30  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/11/29  20:10:37  jimg
 * Added functions for data transmission.
 * Added boolean parameter to serialize which, when true, causes the output
 * buffer to be flushed. The default value is false.
 * Added FILE *in and *out parameters to the ctor. The default values are
 * stdin/out.
 * Removed the `type' parameter from the ctor.
 *
 * Revision 1.4  1994/11/22  14:05:59  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/09/23  14:36:09  jimg
 * Fixed errors in comments.
 *
 * Revision 1.2  1994/09/15  21:09:05  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is now represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 */

#endif // _int32_h

