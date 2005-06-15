
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

// Using the DASVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

#ifndef _das_h
#define _das_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
// #pragma interface
#endif
#endif

#include <stdio.h>

#include <string>
#include <iostream>

#include "Pix.h"

#ifndef _attrtable_h
#include "AttrTable.h"
#endif
#include "DODSResponseObject.h"

using std::cout;
using std::ostream;

/** @brief Hold attribute data for a DODS dataset.

    The Data Attribute Structure is a set of name-value pairs used to
    describe the data in a particular dataset. The name-value pairs are
    called the ``attributes''. The values may be of any of the DODS simple
    data types (Byte, Int16, UInt16, Int32, UInt32, Float32, Float64, String
    and URL), and may be scalar or vector. Note that all values are actually
    stored as String data, making the easy to read/check using a web browser.

    A value may also consist of a set of other name-value pairs.  This
    makes it possible to nest collections of attributes, giving rise
    to a hierarchy of attributes.  DODS uses this structure to provide
    information about variables in a dataset.  For example, consider
    the dataset used in the DDS example earlier.

    In the following example of a DAS, several of the attribute
    collections have names corresponding to the names of variables in
    the DDS example.  The attributes in that collection are said to
    belong to that variable.  For example, the <tt>lat</tt> variable has an
    attribute ``units'' of ``degrees_north''.

    <pre>
    Attributes {
        GLOBAL {
            String title "Reynolds Optimum Interpolation (OI) SST";
        }
        lat {
            String units "degrees_north";
            String long_name "Latitude";
            Float64 actual_range 89.5, -89.5;
        }
        lon {
            String units "degrees_east";
            String long_name "Longitude";
            Float64 actual_range 0.5, 359.5;
        }
        time {
            String units "days since 1-1-1 00:00:00";
            String long_name "Time";
            Float64 actual_range 726468., 729289.;
            String delta_t "0000-00-07 00:00:00";
        }
        sst {
            String long_name "Weekly Means of Sea Surface Temperature";
            Float64 actual_range -1.8, 35.09;
            String units "degC";
            Float64 add_offset 0.;
            Float64 scale_factor 0.0099999998;
            Int32 missing_value 32767;
        }
    }
    </pre>

    Attributes may have arbitrary names, although in most datasets it
    is important to choose these names so a reader will know what they
    describe.  In the above example, the ``GLOBAL'' attribute provides
    information about the entire dataset.

    Data attribute information is an important part of the the data
    provided to a DODS client by a server, and the DAS is how this
    data is packaged for sending (and how it is received). 

    The DAS class is simply a sequence of attribute tables and names.
    It may be thought of as the top level of the attribute hierarchy.

    @see DDS 
    @see AttrTable */
class DAS : public AttrTable, public DODSResponseObject {
private:
protected:
    AttrTable *das_find(string name);

public:
    DAS(AttrTable *dflt=(AttrTable *)NULL, unsigned int sz=0);

    DAS(AttrTable *attr_table, string name);

    virtual ~DAS();

    Pix first_var();
    void next_var(Pix p);
    string get_name(Pix p);
    AttrTable *get_table(Pix p);

    /** Returns a reference to the first attribute table. */
    AttrTable::Attr_iter var_begin() ;

    /** Returns a reference to the end of the attribute table. Does not 
        point to an attribute. */
    AttrTable::Attr_iter var_end() ;

    /** Returns the name of the referenced attribute table. */
    string get_name(Attr_iter &i);

    /** Returns the referenced attribute table. */
    AttrTable *get_table(Attr_iter &i);


    AttrTable *get_table(const string &name);
    AttrTable *get_table(const char *name); // avoid converting char * to Pix

    AttrTable *add_table(const string &name, AttrTable *at);
    AttrTable *add_table(const char *name, AttrTable *at);

    void parse(string fname);
    void parse(int fd);
    void parse(FILE *in=stdin);

    void print(ostream &os = cout, bool dereference = false);

    void print(FILE *out, bool dereference = false);
};

/* 
 * $Log: DAS.h,v $
 * Revision 1.40  2005/01/28 17:25:12  jimg
 * Resolved conflicts from merge with release-3-4-9
 *
 * Revision 1.36.2.4  2005/01/18 23:02:54  jimg
 * FIxed documentation.
 *
 * Revision 1.39  2004/07/07 21:08:47  jimg
 * Merged with release-3-4-8FCS
 *
 * Revision 1.36.2.3  2004/07/02 20:41:51  jimg
 * Removed (commented) the pragma interface/implementation lines. See
 * the ChangeLog for more details. This fixes a build problem on HP/UX.
 *
 * Revision 1.38  2004/06/28 16:59:43  pwest
 * Inherit DDS and DAS from DODSResponseObject
 *
 * Revision 1.37  2003/12/08 18:02:29  edavis
 * Merge release-3-4 into trunk
 *
 * Revision 1.36.2.2  2003/09/06 22:37:50  jimg
 * Updated the documentation.
 *
 * Revision 1.36.2.1  2003/06/23 11:49:18  rmorris
 * The // #pragma interface directive to GCC makes the dynamic typing functionality
 * go completely haywire under OS X on the PowerPC.  We can't use that directive
 * on that platform and it was ifdef'd out for that case.
 *
 * Revision 1.36  2003/04/22 19:40:27  jimg
 * Merged with 3.3.1.
 *
 * Revision 1.34.2.2  2003/04/15 01:19:31  jimg
 * Rearranged some of the methods so that their organization makes more sense.
 *
 * Revision 1.35  2003/02/21 00:14:24  jimg
 * Repaired copyright.
 *
 * Revision 1.34.2.1  2003/02/21 00:10:07  jimg
 * Repaired copyright.
 *
 * Revision 1.34  2003/01/23 00:22:24  jimg
 * Updated the copyright notice; this implementation of the DAP is
 * copyrighted by OPeNDAP, Inc.
 *
 * Revision 1.33  2003/01/10 19:46:40  jimg
 * Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
 * cases files were added on that branch (so they appear on the trunk for
 * the first time).
 *
 * Revision 1.29.4.8  2002/12/17 22:35:02  pwest
 * Added and updated methods using stdio. Deprecated methods using iostream.
 *
 * Revision 1.29.4.7  2002/11/18 18:51:59  jimg
 * Changed the include of Pix.h from #include <Pix.h> to "Pix.h" to fix
 * a problem with the dependencies (see today's check in of Makefile.in).
 *
 * Revision 1.29.4.6  2002/10/28 21:17:44  pwest
 * Converted all return values and method parameters to use non-const iterator.
 * Added operator== and operator!= methods to IteratorAdapter to handle Pix
 * problems.
 *
 * Revision 1.29.4.5  2002/09/12 22:49:57  pwest
 * Corrected signature changes made with Pix to IteratorAdapter changes. Rather
 * than taking a reference to a Pix, taking a Pix value.
 *
 * Revision 1.29.4.4  2002/09/05 22:52:54  pwest
 * Replaced the GNU data structures SLList and DLList with the STL container
 * class vector<>. To maintain use of Pix, changed the Pix.h header file to
 * redefine Pix to be an IteratorAdapter. Usage remains the same and all code
 * outside of the DAP should compile and link with no problems. Added methods
 * to the different classes where Pix is used to include methods to use STL
 * iterators. Replaced the use of Pix within the DAP to use iterators instead.
 * Updated comments for documentation, updated the test suites, and added some
 * unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
 *
 * Revision 1.32  2002/06/18 15:36:24  tom
 * Moved comments and edited to accommodate doxygen documentation-generator.
 *
 * Revision 1.31  2001/08/27 16:38:34  jimg
 * Merged with release-3-2-6
 *
 * Revision 1.29.4.3  2001/07/28 01:10:42  jimg
 * Some of the numeric type classes did not have copy ctors or operator=.
 * I added those where they were needed.
 * In every place where delete (or delete []) was called, I set the pointer
 * just deleted to zero. Thus if for some reason delete is called again
 * before new memory is allocated there won't be a mysterious crash. This is
 * just good form when using delete.
 * I added calls to www2id and id2www where appropriate. The DAP now handles
 * making sure that names are escaped and unescaped as needed. Connect is
 * set to handle CEs that contain names as they are in the dataset (see the
 * comments/Log there). Servers should not handle escaping or unescaping
 * characters on their own.
 *
 * Revision 1.30  2001/01/26 19:48:09  jimg
 * Merged with release-3-2-3.
 *
 * Revision 1.29.4.2  2000/11/30 05:24:46  jimg
 * Significant changes and improvements to the AttrTable and DAS classes. DAS
 * now is a child of AttrTable, which makes attributes behave uniformly at
 * all levels of the DAS object. Alias now work. I've added unit tests for
 * several methods in AttrTable and some of the functions in parser-util.cc.
 * In addition, all of the DAS tests now work.
 *
 * Revision 1.29.4.1  2000/11/22 21:47:42  jimg
 * Changed the implementation of DAS; it now inherits from AttrTable
 *
 * Revision 1.29  2000/09/22 02:17:19  jimg
 * Rearranged source files so that the CVS logs appear at the end rather than
 * the start. Also made the ifdef guard symbols use the same naming scheme and
 * wrapped headers included in other headers in those guard symbols (to cut
 * down on extraneous file processing - See Lakos).
 *
 * Revision 1.28  2000/09/21 16:22:07  jimg
 * Merged changes from Jose Garcia that add exceptions to the software.
 * Many methods that returned error codes now throw exectptions. There are
 * two classes which are thrown by the software, Error and InternalErr.
 * InternalErr is used to report errors within the library or errors using
 * the library. Error is used to reprot all other errors. Since InternalErr
 * is a subclass of Error, programs need only to catch Error.
 *
 * Revision 1.27  2000/08/02 22:46:48  jimg
 * Merged 3.1.8
 *
 * Revision 1.23.6.2  2000/08/01 21:09:35  jimg
 * Destructor is now virtual
 *
 * Revision 1.26  2000/07/09 21:57:09  rmorris
 * Mods's to increase portability, minimuze ifdef's in win32 and account
 * for differences between the Standard C++ Library - most notably, the
 * iostream's.
 *
 * Revision 1.25  2000/06/07 19:33:21  jimg
 * Merged with verson 3.1.6
 *
 * Revision 1.24  2000/06/07 18:06:58  jimg
 * Merged the pc port branch
 *
 * Revision 1.23.20.1  2000/06/02 18:16:48  rmorris
 * Mod's for port to Win32.
 *
 * Revision 1.23.6.1  2000/05/12 18:45:24  jimg
 * Made das_find protected to simplfy future subclassing.
 *
 * Revision 1.23.14.1  2000/02/07 21:11:35  jgarcia
 * modified prototypes and implementations to use exceeption handling
 *
 * Revision 1.23  1999/05/04 19:47:20  jimg
 * Fixed copyright statements. Removed more of the GNU classes.
 *
 * Revision 1.22  1999/04/29 02:29:28  jimg
 * Merge of no-gnu branch
 *
 * Revision 1.21  1999/03/24 23:37:14  jimg
 * Added support for the Int16, UInt16 and Float32 types
 *
 * Revision 1.20  1999/01/21 20:42:01  tom
 * Fixed comment formatting problems for doc++
 *
 * Revision 1.19  1998/11/24 06:49:08  jimg
 * Replaced the DASVHMap object with an SLList of toplevel_entry structs. The
 * DASVHMap software is not being maintained by the FSF and had bugs. There are
 * no changes to the DAS class interface.
 *
 * Revision 1.18.4.1  1999/02/02 21:56:57  jimg
 * String to string version
 *
 * Revision 1.18  1998/07/13 20:20:42  jimg
 * Fixes from the final test of the new build process
 *
 * Revision 1.17  1998/02/05 20:13:51  jimg
 * DODS now compiles with gcc 2.8.x
 *
 * Revision 1.16  1998/01/12 14:27:56  tom
 * Second pass at class documentation.
 *
 * Revision 1.15  1997/08/11 18:19:14  jimg
 * Fixed comment leaders for new CVS version
 *
 * Revision 1.14  1997/06/06 00:43:34  jimg
 * Removed add_table(char *, ...).
 *
 * Revision 1.13  1997/05/13 23:32:16  jimg
 * Added changes to handle the new Alias and lexical scoping rules.
 *
 * Revision 1.12  1996/08/13 18:04:11  jimg
 * Removed the system includes - this is part of my drive to eliminate nested
 * includes from the DODS core software. I'm still waffling on this, though.
 *
 * Revision 1.11  1996/05/31 23:29:35  jimg
 * Updated copyright notice.
 *
 * Revision 1.10  1996/04/05 00:21:27  jimg
 * Compiled with g++ -Wall and fixed various warnings.
 *
 * Revision 1.9  1994/10/17  23:39:50  jimg
 * Removed unnecessary print functions.
 *
 * Revision 1.8  1994/10/13  15:46:58  jimg
 * Added compile-time switched instrumentation.
 * Removed the three definitions of DAS::print().
 * Added DAS::print(ostream &os = cout) -- this is the only function for
 * printing the in-memory DAS.
 *
 * Revision 1.7  1994/10/05  16:44:27  jimg
 * Changed from Map to DLList for representation of the attribute table.
 * Added TYPE to the attribute table.
 *
 * Revision 1.6  1994/09/27  22:46:31  jimg
 * Changed the implementation of the class DAS from one which inherited
 * from DASVHMap to one which contains an instance of DASVHMap.
 * Added mfuncs to set/access the new instance variable.
 *
 * Revision 1.5  1994/09/23  14:38:03  jimg
 * Fixed broken header. Agian.
 *
 * Revision 1.4  1994/09/15  21:08:59  jimg
 * Added many classes to the BaseType hierarchy - the complete set of types
 * described in the DODS API design documet is not represented.
 * The parser can parse DDS files.
 * Fixed many small problems with BaseType.
 * Added CtorType.
 *
 * Revision 1.3  1994/09/09  15:33:40  jimg
 * Changed the base name of this class's parents from `Var' to DAS.
 * Added print() and removed operator<< (see the comments in AttrTable).
 * Added overloaded versions of print() and parse(). They can be called
 * using nothing (which defaults to std{in,out}), with a file descriptor,
 * with a FILE *, or with a String givin a file name.
 *
 * Revision 1.2  1994/08/02  19:17:41  jimg
 * Fixed log comments and rcsid[] variables (syntax errors due to //
 * comments caused compilation failures).
 * das.tab.c and .h are commited now as well.
 *
 * Revision 1.1  1994/08/02  18:39:00  jimg
 * This Class is a container that maps Strings onto AttrTable pointers.
 * It inherits from DASVHMap.
 */

#endif // _das_h
