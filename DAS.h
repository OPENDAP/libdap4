
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Using the DASVHMap class, build a parser for the DAS and add functions
// that provide access to the variables, their attributes and values.
//
// jhrg 7/25/94

/* 
 * $Log: DAS.h,v $
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

#ifndef _DAS_h
#define _DAS_h 1

#ifdef __GNUG__
#pragma interface
#endif

#if 0
#include "DASVHMap.h"
#endif

#include "SLList.h"
#include "AttrTable.h"

/** The Data Attribute Structure is a set of name-value pairs used to
    describe the data in a particular dataset. The name-value pairs are
    called the ``attributes''. The values may be of any of the DODS simple
    data types (Byte, Int16, UInt16, Int32, UInt32, Float32, Float64, String
    and URL), and may be scalar or vector. Note that all values are actually
    stored as String data, making the easy to read/check using a web browser.
    \label{api:das}

    A value may also consist of a set of other name-value pairs.  This
    makes it possible to nest collections of attributes, giving rise
    to a hierarchy of attributes.  DODS uses this structure to provide
    information about variables in a dataset.  For example, consider
    the dataset used in the DDS example earlier.

    In the following example of a DAS, several of the attribute
    collections have names corresponding to the names of variables in
    the DDS example.  The attributes in that collection are said to
    belong to that variable.  For example, the #lat# variable has an
    attribute ``units'' of ``degrees\_north''.

    \begin{verbatim}
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
    \end{verbatim}

    Attributes may have arbitrary names, although in most datasets it
    is important to choose these names so a reader will know what they
    describe.  In the above example, the ``GLOBAL'' attribute provides
    information about the entire dataset.

    Data attribute information is an important part of the the data
    provided to a DODS client by a server, and the DAS is how this
    data is packaged for sending (and how it is received). 

    The DAS class is simply a sequence of attribute tables and names.
    It may be thought of as the top level of the attribute hierarchy.

    @memo Holds a DODS Data Attribute Structure.
    @see DDS 
    @see AttrTable */
class DAS {
private:
    struct toplevel_entry {
	string name;
	AttrTable *attr_table;
    };

    SLList<toplevel_entry> entries;

    AttrTable *das_find(string name);

public:
    /** Create a DAS from a single attribute table.  

	\note{In an older version of this class, #dflt# and #sz#
	initialized a hash table. That is no longer used and these
	params should no longer matter. Note that this constructor is
	effectively the empty constructor. 11/23/98 jhrg}

	@param dflt A pointer to a valid attribute table.
	@param sz The number of entries in the table. */
    DAS(AttrTable *dflt=(AttrTable *)NULL, unsigned int sz=0);

    /** Create a DAS object with one attribute table. Use #append_attr()#
	to add additional attributes.

	@see append_attr()
	@param attr_table The initial AttrTable. */
    DAS(AttrTable *attr_table, string name);

    ~DAS();

    /** Returns a pointer to the first attribute table. */
    Pix first_var();
    /** Increments an attribute table pointer to indicate the next table
	in the series. */
    void next_var(Pix &p);
    /** Returns the name of the indicated attribute table. */
    string get_name(Pix p);
    /** Returns the indicated attribute table. */
    AttrTable *get_table(Pix p);

    /** Returns the attribute table with the given name. 
	@name get\_table()
    */

    //@{
    /** Returns the attribute table with the given name string. */
    AttrTable *get_table(const string &name);
    /** Returns the attribute table with the given name. */
    AttrTable *get_table(const char *name); // avoid converting char * to Pix
    //@}

    /** Adds an attribute table to the DAS.
	@name add\_table()
    */
    //@{
    /** Adds an attribute table to the DAS. */
    AttrTable *add_table(const string &name, AttrTable *at);
    /** Adds an attribute table to the DAS. */
    AttrTable *add_table(const char *name, AttrTable *at);
    //@}

    /** Reads a DAS in from an external source. 

	@name parse()
    */
    //@{
    /** Reads a DAS from the named file. */
    bool parse(string fname);
    /** Reads a DAS from the given file descriptor. */
    bool parse(int fd);
    /** Reads a DAS from an open file descriptor. */
    bool parse(FILE *in=stdin);
    //@}

    /** Creates an ASCII representation of a DAS on the given output
	stream. */
    bool print(ostream &os = cout);
};

#endif
