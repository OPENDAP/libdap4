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

// Provide access to the DDS. This class is used to parse DDS text files, to
// produce a printed representation of the in-memory variable table, and to
// update the table on a per-variable basis.
//
// jhrg 9/8/94

#ifndef _dds_h
#define _dds_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
// #pragma interface
#endif
#endif

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "Pix.h"

#ifndef _basetype_h
#include "BaseType.h"
#endif

#ifndef _constructor_h
#include "Constructor.h"
#endif

#ifndef _das_h
#include "DAS.h"
#endif

#ifndef _clause_h
#include "Clause.h"
#endif

#ifndef _expr_h
#include "expr.h"
#endif

#ifndef _rvalue_h
#include "RValue.h"
#endif

using std::cout;

int get_sinks(FILE *out, bool compress, FILE **comp_sink, XDR **xdr_sink);
void clean_sinks(int childpid, bool compress, XDR *xdr_sink, FILE *comp_sink);

/** The DODS Data Descriptor Object (DDS) is a data structure used by
    the DODS software to describe datasets and subsets of those
    datasets.  The DDS may be thought of as the declarations for the
    data structures that will hold data requested by some DODS client.
    Part of the job of a DODS server is to build a suitable DDS for a
    specific dataset and to send it to the client.  Depending on the
    data access API in use, this may involve reading part of the
    dataset and inferring the DDS.  Other APIs may require the server
    simply to read some ancillary data file with the DDS in it.

    On the server side, in addition to the data declarations, the DDS
    holds the clauses of any constraint expression that may have
    accompanied the data request from the DODS client.  The DDS object
    includes methods for modifying the DDS according to the given
    constraint expression.  It also has methods for directly modifying
    a DDS, and for transmitting it from a server to a client.

    For the client, the DDS object includes methods for reading the
    persistent form of the object sent from a server. This includes parsing
    the ASCII representation of the object and, possibly, reading data
    received from a server into a data object.

    Note that the class DDS is used to instantiate both DDS and DataDDS
    objects. A DDS that is empty (contains no actual data) is used by servers
    to send structural information to the client. The same DDS can becomes a
    DataDDS when data values are bound to the variables it defines.

    For a complete description of the DDS layout and protocol, please
    refer to <i>The DODS User Guide</i>. 
    
    The DDS has an ASCII representation, which is what is transmitted
    from a DODS server to a client.  Here is the DDS representation of
    an entire dataset containing a time series of worldwide grids of
    sea surface temperatures:

    <pre>
    Dataset {
        Float64 lat[lat = 180];
        Float64 lon[lon = 360];
        Float64 time[time = 404];
        Grid {
         ARRAY:
            Int32 sst[time = 404][lat = 180][lon = 360];
         MAPS:
            Float64 time[time = 404];
            Float64 lat[lat = 180];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    If the data request to this dataset includes a constraint
    expression, the corresponding DDS might be different.  For
    example, if the request was only for northern hemisphere data
    at a specific time, the above DDS might be modified to appear like
    this:

    <pre>
    Dataset {
        Grid {
         ARRAY:
            Int32 sst[time = 1][lat = 90][lon = 360];
         MAPS:
            Float64 time[time = 1];
            Float64 lat[lat = 90];
            Float64 lon[lon = 360];
        } sst;
    } weekly;
    </pre>

    Since the constraint has narrowed the area of interest, the range
    of latitude values has been halved, and there is only one time
    value in the returned array.  Note that the simple arrays (<tt>lat</tt>,
    <tt>lon</tt>, and <tt>time</tt>) described in the dataset are also
    part of the <tt>sst</tt> Grid object.  They can be requested by
    themselves or as part of that larger object.

    See the <i>The DODS User Guide</i>, or the documentation of the
    BaseType class for descriptions of the DODS data types.


    @see BaseType
    @see DAS */

class DDS : public DODSResponseObject {
private:
    // This struct is used to hold all the known `user defined' functions
    // (including those that are `built-in'). 
    struct function {
	string name;
	bool_func b_func;
	btp_func bt_func;
	proj_func p_func;

	function(const string &n, const bool_func f)
	    : name(n), b_func(f), bt_func(0), p_func(0) {}
	function(const string &n, const btp_func f)
	    : name(n), b_func(0), bt_func(f), p_func(0) {}
	function(const string &n, const proj_func f)
	    : name(n), b_func(0), bt_func(0), p_func(f) {}
	function(): name(""), b_func(0), bt_func(0), p_func(0) {}
    };

    string name;		// The dataset name

    string _filename;		// File name (or other OS identifier) for
				// dataset or part of dataset.
    AttrTable d_attr;           // Global attributes.

    vector<BaseType *> vars;	// Variables at the top level 
    
    vector<Clause *> expr;	// List of CE Clauses

    vector<BaseType *> constants;// List of temporary objects

    vector<function> functions; // Known external functions

    bool is_global_attr(string name);
    void add_global_attribute(AttrTable::entry *entry);

    int d_timeout;		// alarm time in seconds. If greater than
				// zero, raise the alarm signal if more than
				// d_timeout seconds are spent reading data.

protected:
    void duplicate(const DDS &dds);
    BaseType *leaf_match(const string &name, btp_stack *s = 0);
    BaseType *exact_match(const string &name, btp_stack *s = 0);

    void transfer_attr(DAS *das, const AttrTable::entry *ep, BaseType *btp,
                       const string &suffix = "");
    void transfer_attr_table(DAS *das, AttrTable *at, BaseType *btp,
                             const string &suffix = "");
    void transfer_attr_table(DAS *das, AttrTable *at, Constructor *c,
                             const string &suffix = "");
    
public:
    typedef std::vector<BaseType *>::const_iterator Vars_citer ;
    typedef std::vector<BaseType *>::iterator Vars_iter ;
    typedef std::vector<BaseType *>::reverse_iterator Vars_riter ;
    
    typedef std::vector<Clause *>::const_iterator Clause_citer ;
    typedef std::vector<Clause *>::iterator Clause_iter ;
    typedef std::vector<BaseType *>::const_iterator Constants_citer ;
    typedef std::vector<BaseType *>::iterator Constants_iter ;
    typedef std::vector<function>::const_iterator Functions_citer ;
    typedef std::vector<function>::iterator Functions_iter ;

    DDS(const string &n = "");

    DDS(const DDS &dds);

    virtual ~DDS();

    DDS & operator=(const DDS &rhs); 

    virtual void transfer_attributes(DAS *das);

    string get_dataset_name();
    void set_dataset_name(const string &n);

    virtual AttrTable &get_attr_table();

    string filename();
    void filename(const string &fn);

    void add_var(BaseType *bt);

    /// Removes a variable from the DDS.
    void del_var(const string &n);

    BaseType *var(const string &n, btp_stack &s);
    BaseType *var(const string &n, btp_stack *s = 0);
    BaseType *var(const char *n, btp_stack *s = 0);

    BaseType *var(Pix p);
 
    Pix first_var();
    void next_var(Pix p);
    int num_var();

    /// Return an iteraor
    Vars_iter var_begin();
    /// Return a reverse iterator 
    Vars_riter var_rbegin();
    /// Return an iterator
    Vars_iter var_end();
    /// Return a reverse iterator
    Vars_riter var_rend();
    /// Get an iterator
    Vars_iter get_vars_iter(int i);
    /// Get a variablue
    BaseType *get_var_index(int i);
    /// Removes a variable from the DDS.
    void del_var(Vars_iter i);
    /// Removes a range of variables from the DDS.
    void del_var(Vars_iter i1, Vars_iter i2);

    void timeout_on();
    void timeout_off();
    void set_timeout(int t);
    int get_timeout();

    void add_function(const string &name, bool_func f);
    void add_function(const string &name, btp_func f);
    void add_function(const string &name, proj_func f);

    bool find_function(const string &name, bool_func *f) const;
    bool find_function(const string &name, btp_func *f) const;
    bool find_function(const string &name, proj_func *f) const;

    Pix first_clause();
    void next_clause(Pix p);
    Clause *clause(Pix p);
    bool clause_value(Pix p, const string &dataset);

    void append_clause(int op, rvalue *arg1, rvalue_list *arg2);
    void append_clause(bool_func func, rvalue_list *args);
    void append_clause(btp_func func, rvalue_list *args);

    bool functional_expression();
    bool boolean_expression();
    bool eval_selection(const string &dataset);
    BaseType *eval_function(const string &dataset);
    /** Returns the first clause in a parsed constraint expression. */
    Clause_iter clause_begin();

    /** Returns a reference to the end of the list of clauses in a parsed 
	constraint expression. It does not reference the last clause */
    Clause_iter clause_end();

    /** Returns the value of the indicated clause of a constraint
	expression. */
    bool clause_value(Clause_iter &i, const string &dataset);

    void parse_constraint(const string &constraint);

    // Both of the following are deprecated methods.
    void parse_constraint(const string &constraint, ostream &os,
			  bool server = true);

    void parse_constraint(const string &constraint, FILE *out,
			  bool server = true);

    void append_constant(BaseType *btp);

    void parse(string fname);
    void parse(int fd);
    void parse(FILE *in=stdin);

    void print(ostream &os = cout);
    void print(FILE *out);
    void print_constrained(ostream &os = cout);
    void print_constrained(FILE *out);

    void print_xml(FILE *out, bool constrained, const string &blob);

    bool send(const string &dataset, const string &constraint, FILE *out, 
	      bool compressed = true, const string &cgi_ver = "",
	      time_t lmt = 0);

    void mark_all(bool state);
    bool mark(const string &name, bool state);
    bool check_semantics(bool all = false);

    void tag_nested_sequences();
};

// $Log: DDS.h,v $
// Revision 1.60  2005/01/28 17:25:12  jimg
// Resolved conflicts from merge with release-3-4-9
//
// Revision 1.50.2.5  2005/01/18 23:03:07  jimg
// FIxed documentation.
//
// Revision 1.50.2.4  2004/12/23 20:53:10  dan
// Added method tag_nested_sequences() that traverses Sequence
// members to set the leaf_node for the innermost Sequence.
//
// Revision 1.59  2004/11/16 18:37:05  jimg
// Added reverese iterators.
//
// Revision 1.58  2004/08/03 23:11:38  jimg
// I changed the three static functions that are helpers for
// transfer_attributes() to methods. This makes them easier to test, although I
// never wrote any unit tests for them...
//
// Revision 1.57  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.50.2.3  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.56  2004/06/28 16:59:43  pwest
// Inherit DDS and DAS from DODSResponseObject
//
// Revision 1.55  2004/03/01 22:32:10  jimg
// Bring the trunk up to date with the code in my working directory...
//
// Revision 1.54  2003/12/10 21:11:57  jimg
// Merge with 3.4. Some of the files contains erros (some tests fail). See
// the ChangeLog for information about fixes.
//
// Revision 1.53  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.52  2003/09/25 22:37:34  jimg
// Misc changes.
//
// Revision 1.50.2.2  2003/07/25 06:04:28  jimg
// Refactored the code so that DDS:send() is now incorporated into
// DODSFilter::send_data(). The old DDS::send() is still there but is
// depracated.
// Added 'smart timeouts' to all the variable classes. This means that
// the new server timeouts are active only for the data read and CE
// evaluation. This went inthe BaseType::serialize() methods because it
// needed to time both the read() calls and the dds::eval() calls.
//
// Revision 1.50.2.1  2003/06/23 11:49:18  rmorris
// The // #pragma interface directive to GCC makes the dynamic typing functionality
// go completely haywire under OS X on the PowerPC.  We can't use that directive
// on that platform and it was ifdef'd out for that case.
//
// Revision 1.51  2003/05/23 03:24:57  jimg
// Changes that add support for the DDX response. I've based this on Nathan
// Potter's work in the Java DAP software. At this point the code can
// produce a DDX from a DDS and it can merge attributes from a DAS into a
// DDS to produce a DDX fully loaded with attributes. Attribute aliases
// are not supported yet. I've also removed all traces of strstream in
// favor of stringstream. This code should no longer generate warnings
// about the use of deprecated headers.
//
// Revision 1.50  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.48.2.2  2003/04/15 01:17:12  jimg
// Added a method to get the iterator for a variable (or map) given its
// index. To get the iterator for the ith variable/map, call
// get_vars_iter(i).
//
// Revision 1.49  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.48.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.48  2003/01/23 00:22:24  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.47  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.41.4.11  2002/12/17 22:35:02  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.41.4.10  2002/11/18 18:51:59  jimg
// Changed the include of Pix.h from #include <Pix.h> to "Pix.h" to fix
// a problem with the dependencies (see today's check in of Makefile.in).
//
// Revision 1.41.4.9  2002/10/28 21:17:44  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.41.4.8  2002/10/02 17:50:36  pwest
// Added two new del_vars methods. The first takes an iterator and deltes the
// variable referenced by that iterator. The iterator now points to the element
// after the deleted element. The second method takes two iterators and will
// delete the variables starting from the first iterator and up to, not
// including the second iterator.
//
// Revision 1.41.4.7  2002/09/22 14:15:43  rmorris
// Changed the use of vector to std::vector.  The 'using' directive for VC++
// no longer cut it in this case.
//
// Revision 1.41.4.6  2002/09/12 22:49:57  pwest
// Corrected signature changes made with Pix to IteratorAdapter changes. Rather
// than taking a reference to a Pix, taking a Pix value.
//
// Revision 1.41.4.5  2002/09/05 22:52:54  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.46  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.45  2002/06/03 22:21:15  jimg
// Merged with release-3-2-9
//
// Revision 1.41.4.4  2002/03/01 21:03:08  jimg
// Significant changes to the var(...) methods. These now take a btp_stack
// pointer and are used by DDS::mark(...). The exact_match methods have also
// been updated so that leaf variables which contain dots in their names
// will be found. Note that constructor variables with dots in their names
// will break the lookup routines unless the ctor is the last field in the
// constraint expression. These changes were made to fix bug 330.
//
// Revision 1.44  2001/09/28 17:50:07  jimg
// Merged with 3.2.7.
//
// Revision 1.41.4.3  2001/09/25 20:33:26  jimg
// Removed debug.h
//
// Revision 1.43  2001/08/24 17:46:22  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.41.4.2  2001/07/28 01:10:42  jimg
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
// Revision 1.42  2001/06/15 23:49:01  jimg
// Merged with release-3-2-4.
//
// Revision 1.41.4.1  2001/04/23 22:34:46  jimg
// Added support for the Last-Modified MIME header in server responses.`
//
// Revision 1.41  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.40  2000/09/21 16:22:07  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.39  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.34.2.4  2000/08/02 20:56:36  jimg
// Removed the symbol DVR from the send method's declaration. That removes the
// only dependence in this file on config_dap.h. Clients of this class can now
// included this header without also including config_dap.h.
//
// Revision 1.34.2.3  2000/07/19 19:01:56  jimg
// I made the dotr virtual! This was causing problems when request_data
// returned a DataDDS * but the object was assigned to a DDS * and later
// deleted. Since DDS:~DDS() was not virtual, the DataDDS dtor was not being
// run. 
//
// Revision 1.38  2000/07/09 21:57:09  rmorris
// Mods's to increase portability, minimuze ifdef's in win32 and account
// for differences between the Standard C++ Library - most notably, the
// iostream's.
//
// Revision 1.37  2000/06/16 18:14:59  jimg
// Merged with 3.1.7
//
// Revision 1.34.2.2  2000/06/14 17:00:53  jimg
// Changed vars from a SLList of BaseType *s to a DLList.
//
// Revision 1.36  2000/06/07 19:33:21  jimg
// Merged with verson 3.1.6
//
// Revision 1.35  2000/06/07 18:06:58  jimg
// Merged the pc port branch
//
// Revision 1.34.14.1  2000/06/02 18:16:48  rmorris
// Mod's for port to Win32.
//
// Revision 1.34.2.1  2000/05/12 18:44:38  jimg
// Made the duplicate and *_match functions protected to simplfy future
// subclassing.
//
// Revision 1.34.8.1  2000/02/07 21:11:35  jgarcia
// modified prototypes and implementations to use exceeption handling
//
// Revision 1.34  1999/07/22 18:47:44  tom
// fixed doc typo
//
// Revision 1.33  1999/07/22 17:11:50  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.32.8.1  1999/06/08 17:36:15  dan
// Replaced template declaration of add_function with 3 separate definitions
// for the possible instances of this method, bool*, BaseType*, void*.
//
// Revision 1.32  1999/05/05 00:51:02  jimg
// Added cgi_ver parameter to send() member function.
//
// Revision 1.31  1999/04/29 02:29:28  jimg
// Merge of no-gnu branch
//
// Revision 1.30  1999/01/21 20:42:01  tom
// Fixed comment formatting problems for doc++
//
// Revision 1.29  1998/12/22 04:42:19  jimg
// Corrected some of the doc comments; fixed spelling and added error return
// values.
//
// Revision 1.28  1998/11/10 01:05:38  jimg
// This class now holds a list of Clause pointers, not objects.
//
// Revision 1.27  1998/10/21 16:40:13  jimg
// Added a proj_func member (for support of Projection Functions).
// Fixed up the doc++ comments.
//
// Revision 1.26  1998/09/17 17:20:44  jimg
// Added leaf_match and exact_match.
// Added two new versions of the var member function.
// Fixed documentation.
//
// Revision 1.25.4.1  1999/02/02 21:56:57  jimg
// String to string version
//
// Revision 1.25  1998/07/13 20:20:43  jimg
// Fixes from the final test of the new build process
//
// Revision 1.24  1998/03/20 00:12:41  jimg
// Added documentation for the new parse_constraint() member function.
//
// Revision 1.23  1998/02/04 14:55:31  tom
// Another draft of documentation.
//
// Revision 1.22  1998/01/12 14:27:56  tom
// Second pass at class documentation.
//
// Revision 1.21  1997/08/11 18:19:14  jimg
// Fixed comment leaders for new CVS version
//
// Revision 1.20  1997/03/05 08:35:03  jimg
// Added bool parameter `compressed' (defaults to true) to the send member
// function. See DDS.cc.
//
// Revision 1.19  1997/03/03 08:17:17  reza
// Changed default error object's output stream to cout. This will send it
// to the client side (versus the local server's log file, cerr).
//
// Revision 1.18  1996/12/03 00:14:58  jimg
// Added ostream and bool params to parse_constraint(). The bool parameter is
// used to tell the member function that it is running in the server of the
// client. The ostream is the sink for error objects (server side) or messages
// (client side).
//
// Revision 1.17  1996/12/02 23:14:54  jimg
// Added `filename' field and access functions. This field is for recording
// the filename associated with the dataset from which the DDS is generated.
// It does not actually have to be a filename; rather it is intended to be
// used by BaseType's read() member function when that code must access some
// OS controlled resource to get data for a particular variable. For most
// systems it will be a file, while for some systems it may be a RDB or
// blank.
//
// Revision 1.16  1996/06/04 21:33:20  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.15  1996/05/31 23:29:38  jimg
// Updated copyright notice.
//
// Revision 1.14  1996/05/29 22:08:37  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.13  1996/05/22 18:05:09  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.12  1996/04/05 00:21:28  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.11  1996/04/04 18:41:07  jimg
// Merged changes from version 1.1.1.
//
// Revision 1.10  1996/03/05 18:32:26  jimg
// Added the clause and function subclasses. Clause is used to hold a single
// clause of the current CE. Clause has ctors, a dtor (which is currently
// broken) and member function used to get the boolean value of the clause.
// Function is used to hold a single pointer to either a function returning a
// boolean or a BaseType *. The DDS class contains a list of clauses and a list
// of functions.
//
// Revision 1.9  1996/02/01 17:43:10  jimg
// Added support for lists as operands in constraint expressions.
//
// Revision 1.8  1995/12/09  01:06:39  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.7  1995/12/06  21:05:08  jimg
// Added print_constrained(): prints only those parts of the DDS that satisfy
// the constraint expression (projection + array selection).
// Added eval_constraint(): given the text of a constraint expression, evaluate
// it in the environment of the current DDS.
// Added mark*(): add the named variables to the current projection.
// Added send(): combines many functions like reading and serializing variables
// with constraint evaluation.
//
// Revision 1.6.2.2  1996/03/01 00:06:11  jimg
// Removed bad attempt at multiple connect implementation.
//
// Revision 1.6.2.1  1996/02/23 21:37:26  jimg
// Updated for new configure.in.
// Fixed problems on Solaris 2.4.
//
// Revision 1.6  1995/02/10  02:30:49  jimg
// Misc comment edits.
//
// Revision 1.5  1994/11/03  04:58:03  reza
// Added two overloading for function parse to make it consistent with DAS
// class. 
//
// Revision 1.4  1994/10/18  00:20:47  jimg
// Added copy ctor, dtor, duplicate, operator=.
// Added var() for const char * (to avoid confusion between char * and
// Pix (which is void *)).
// Switched to errmsg library.
// Added formatting to print().
//
// Revision 1.3  1994/09/23  14:42:23  jimg
// Added mfunc check_semantics().
// Replaced print mfunc stub with real code.
// Fixed some errors in comments.
//
// Revision 1.2  1994/09/15  21:09:00  jimg
// Added many classes to the BaseType hierarchy - the complete set of types
// described in the DODS API design documet is now represented.
// The parser can parse DDS files.
// Fixed many small problems with BaseType.
// Added CtorType.
//
// Revision 1.1  1994/09/08  21:09:42  jimg
// First version of the Dataset descriptor class. 

#endif // _dds_h
