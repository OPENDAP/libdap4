
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
 
// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// This is the interface definition file for the abstract class
// Vector. Vector is the parent class for List and Array.

#ifndef _vector_h
#define _vector_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#ifdef WIN32
#include <rpc.h>
#include <winsock.h>
#else
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>
#endif

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _dds_h
#include "DDS.h"
#endif

  /** Holds a one-dimensional array of DODS data types.  This class
      takes two forms, depending on whether the elements of the vector
      are themselves simple or compound objects. This class contains
      common functionality for the List and Array classes, and should
      rarely be used directly.

      When each element of the class is a simple data type, the Vector
      is implemented as a simple array of C types, rather than as an
      array of BaseType data types.  A single private ``template''
      BaseType instance (<tt>_var</tt>) is used to hold information in common
      to all the members of the array.  The template is also used as a
      container to pass values back and forth to an application
      program, as in <tt>var()</tt>.

      If the elements of the vector are themselves compound data
      types, the array is stored as a vector of BaseType pointers (see
      the DODS class <b>BaseTypePtrVec</b>). The template is still used to
      hold information in common to all the members of the array, but
      is not used to pass information to and from the application
      program. 

      @brief Holds a one-dimensional collection of DODS data types.  
      @see BaseType 
      @see List
      @see Array
      */

class Vector: public BaseType {
private:
    int _length;		// number of elements in the vector
    BaseType *_var;		// base type of the Vector

    // _buf was a pointer to void; delete[] complained. 6/4/2001 jhrg
    char *_buf;			// array which holds cardinal data

    vector<BaseType *> _vec;	// array for other data

protected:
    // This function copies the private members of Vector.
    void _duplicate(const Vector &v);

public:
    Vector(const string &n = "", BaseType *v = 0, const Type &t = dods_null_c);
    Vector(const Vector &rhs);

    virtual ~Vector();

    Vector &operator=(const Vector &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

    virtual int element_count(bool leaves);

    virtual void set_send_p(bool state); 

    virtual void set_read_p(bool state);

    virtual unsigned int width();

    virtual int length();

    virtual void set_length(int l);

    virtual bool serialize(const string &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, DDS *dds, bool reuse = false);

    virtual unsigned int val2buf(void *val, bool reuse = false);

    virtual unsigned int buf2val(void **val);

    void set_vec(unsigned int i, BaseType *val);

    void vec_resize(int l);

    virtual BaseType *var(const string &name = "", bool exact = true);

    virtual BaseType *var(const string &name, btp_stack &s);

    virtual BaseType *var(unsigned int i);

    virtual void add_var(BaseType *v, Part p = nil);

    virtual bool check_semantics(string &msg, bool all = false);
};

/* 
 * $Log: Vector.h,v $
 * Revision 1.40  2003/12/08 18:02:30  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.38.2.2  2003/09/06 22:56:43  jimg
 * Updated the documentation.
 *
 * Revision 1.38.2.1  2003/06/23 11:49:18  rmorris
 * The #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.39  2003/05/23 03:24:57  jimg
 * Changes that add support for the DDX response. I've based this on Nathan
 * Potter's work in the Java DAP software. At this point the code can
 * produce a DDX from a DDS and it can merge attributes from a DAS into a
 * DDS to produce a DDX fully loaded with attributes. Attribute aliases
 * are not supported yet. I've also removed all traces of strstream in
 * favor of stringstream. This code should no longer generate warnings
 * about the use of deprecated headers.
 *
 * Revision 1.38  2003/04/22 19:40:28  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.37  2003/02/21 00:14:25  jimg
 * Repaired copyright.
 *
 * Revision 1.36.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.36  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.35  2003/01/10 19:46:40  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.31.4.5  2002/12/27 19:34:42  jimg
 * Modified the var() methods so that www2id() is called before looking
 * up identifier names. See bug 563.
 *
 * Revision 1.31.4.4  2002/12/17 22:35:03  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.34  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.31.4.3  2002/05/22 16:57:51  jimg
 * I modified the `data type classes' so that they do not need to be
 * subclassed for clients. It might be the case that, for a complex client,
 * subclassing is still the best way to go, but you're not required to do
 * it anymore.
 *
 * Revision 1.33  2001/08/24 17:46:22  jimg
 * Resolved conflicts from the merge of release 3.2.6
 *
 * Revision 1.31.4.2  2001/07/11 05:29:53  jimg
 * Clarified the documentation for var().
 *
 * Revision 1.32  2001/06/15 23:49:03  jimg
 * Merged with release-3-2-4.
 *
 * Revision 1.31.4.1  2001/06/05 06:49:19  jimg
 * Added the Constructor class which is to Structures, Sequences and Grids
 * what Vector is to Arrays and Lists. This should be used in future
 * refactorings (I thought it was going to be used for the back pointers).
 * Introduced back pointers so children can refer to their parents in
 * hierarchies of variables.
 * Added to Sequence methods to tell if a child sequence is done
 * deserializing its data.
 * Fixed the operator=() and copy ctors; removed redundency from
 * _duplicate().
 * Changed the way serialize and deserialize work for sequences. Now SOI and
 * EOS markers are written for every `level' of a nested Sequence. This
 * should fixed nested Sequences. There is still considerable work to do
 * for these to work in all cases.
 *
 * Revision 1.31  2000/09/22 02:17:22  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.30  2000/09/21 16:22:09  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.29  2000/07/09 21:57:10  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.28  2000/06/16 18:15:00  jimg
 * Merged with 3.1.7
 *
 * Revision 1.26.6.1  2000/06/07 23:08:31  jimg
 * Added code to explicitly delete BaseType *s in _vec.
 * Also tried recoding using DLList, but that didn't fix the problem I was
 * after---fixed in the client code but decided to leave this is with #if 0
 * just in case.
 *
 * Revision 1.27  2000/06/07 18:06:59  jimg
 * Merged the pc port branch
 *
 * Revision 1.26.20.1  2000/06/02 18:29:32  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.26.14.1  2000/01/28 22:14:07  jgarcia
 * Added exception handling and modify add_var to get a copy of the object
 *
 * Revision 1.26  1999/05/04 19:47:23  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.25  1999/04/29 02:29:34  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.24  1999/01/21 20:42:01  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.23  1998/11/10 00:56:44  jimg
 * Fixed up the doc++ comments.
 *
 * Revision 1.22  1998/09/17 17:01:12  jimg
 * Fixed errant documentation.
 *
 * Revision 1.21.6.1  1999/02/02 21:57:04  jimg
 * String to string version
 *
 * Revision 1.21  1998/03/17 17:51:28  jimg
 * Added an implementation of element_count().
 *
 * Revision 1.20  1998/02/05 20:13:59  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.19  1998/02/04 14:55:33  tom
 * Another draft of documentation.
 *
 * Revision 1.18  1998/01/12 14:28:00  tom
 * Second pass at class documentation.
 *
 * Revision 1.17  1997/12/18 15:06:14  tom
 * First draft of class documentation, entered in doc++ format,
 * in the comments
 *
 * Revision 1.16  1997/12/18 14:57:05  tom
 * merged conflicts
 *
 * Revision 1.15  1997/12/16 00:42:53  jimg
 * The return type of set_vec() is now bool (was void).
 *
 * Revision 1.14  1997/10/09 22:19:26  jimg
 * Resolved conflicts in merge of 2.14c to trunk.
 *
 * Revision 1.13  1997/08/11 18:19:31  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.12  1997/03/08 19:02:13  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.11  1997/02/28 01:29:13  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.10  1996/08/13 18:40:46  jimg
 * Changes return type of length() member function from unsigned to int. A
 * return value of -1 indicates that the vector has no length.
 *
 * Revision 1.9  1996/06/04 21:33:51  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.8  1996/05/31 23:30:44  jimg
 * Updated copyright notice.
 *
 * Revision 1.7  1996/05/16 22:50:27  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.6  1996/05/14 15:38:48  jimg
 * These changes have already been checked in once before. However, I
 * corrupted the source repository and restored it from a 5/9/96 backup
 * tape. The previous version's log entry should cover the changes.
 *
 * Revision 1.5  1996/04/05 00:22:10  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.4  1996/03/05 01:03:04  jimg
 * Added ce_eval parameter to serialize() member function.
 * Added vec_resize() member function to class.
 *
 * Revision 1.3  1995/12/09  01:07:34  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.2  1995/12/06  19:49:35  jimg
 * Changed the var() and print_decl() mfuncs. var() now takes an index and
 * returns a pointer to the BaseType object with the correct
 * value. print_decl() takes a new flag - constrained - which causes only
 * those dimensions selected by the current constraint expression to be printed
 *
 * Revision 1.1  1995/11/22  22:30:20  jimg
 * Created.
 */

#endif /* _vector_h */
