
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
 
// (c) COPYRIGHT URI/MIT 1996, 1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// The Usage server. Arguments: three arguments; filter options, the dataset
// name and the pathname and `api prefix' of the data server. Returns a HTML
// document that describes what information this dataset contains, special
// characteristics of the server users might want to know and any special
// information that the dataset providers want to make available. jhrg
// 12/9/96

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: usage.cc,v 1.22 2003/02/21 00:14:25 jimg Exp $"};

#include <stdio.h>

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif

#include "Regex.h"

#include "cgi_util.h"
#include "util.h"
#include "DAS.h"

#include "debug.h"

using std::cerr;
using std::endl;
using std::ends;
using std::ifstream;
using std::ostrstream;

static void
usage(char *argv[])
{
    cerr << argv[0] << "<options> <filename> <CGI directory>" << endl
         << "Takes three required arguments; command options to be" << endl
	 << "passed to the filter programsm the dataset filename and" << endl
	 << "the directory and api prefix for the filter program." << endl; 
}

// This code could use a real `kill-file' some day - about the same time that
// the rest of the server gets a `rc' file... For the present just see if a
// small collection of regexs match the name.

// The pthread code here is used to ensure that the static objects dim and
// global (in name_in_kill_file() and name_is_global()) are initialized only
// once. If the pthread package is not present when libdap++ is built, this
// code is *not* MT-Safe.

static Regex *dim_ptr;
#if HAVE_PTHREAD_H
static pthread_once_t dim_once_control = PTHREAD_ONCE_INIT;
#endif

static void
init_dim_regex()
{
    // MT-Safe if called via pthread_once or similar
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dim' attributes.
    dim_ptr = &dim;
}

static bool
name_in_kill_file(const string &name)
{
#if HAVE_PTHREAD_H
    pthread_once(&dim_once_control, init_dim_regex);
#else
    if (!dim_ptr)
	init_dim_regex();
#endif

    return dim_ptr->match(name.c_str(), name.length()) != -1;
}

static Regex *global_ptr;
#if HAVE_PTHREAD_H
static pthread_once_t global_once_control = PTHREAD_ONCE_INIT;
#endif

static void
init_global_regex()
{
    // MT-Safe if called via pthread_once or similar
    static Regex global("\\(.*global.*\\)\\|\\(.*dods.*\\)", 1);
    global_ptr = &global;
}

static bool
name_is_global(string &name)
{
#if HAVE_PTHREAD_H
    pthread_once(&global_once_control, init_global_regex);
#else
    if (!global_ptr)
	init_global_regex();
#endif

    downcase(name);
    return global_ptr->match(name.c_str(), name.length()) != -1;
}

// write_global_attributes and write_attributes are almost the same except
// that the global attributes use fancier formatting. The formatting could be
// passed in as params, but that would make the code much harder to
// understand. So, I'm keeping this as two separate functions even though
// there's some duplication... 3/27/2002 jhrg
static void
write_global_attributes(ostrstream &oss, AttrTable *attr, 
			const string prefix = "")
{
    if (attr) {
	for (AttrTable::Attr_iter a = attr->attr_begin(); a != attr->attr_end(); a++)
	{
	    if (attr->is_container(a))
		write_global_attributes(oss, attr->get_attr_table(a), 
				 (prefix == "") ? attr->get_name(a) 
				 : prefix + string(".") + attr->get_name(a));
	    else {
		oss << "\n<tr><td align=right valign=top><b>"; 
		if (prefix != "")
		    oss << prefix << "." << attr->get_name(a);
		else
		    oss << attr->get_name(a);
		oss << "</b>:</td>\n";

		int num_attr = attr->get_attr_num(a) - 1;
		oss << "<td align=left>";
		for (int i = 0; i < num_attr; ++i)
		    oss << attr->get_attr(a, i) << ", ";
		oss << attr->get_attr(a, num_attr) << "<br></td></tr>\n";
	    }
	}
    }
}

static void
write_attributes(ostrstream &oss, AttrTable *attr, const string prefix = "")
{
    if (attr) {
	for (AttrTable::Attr_iter a = attr->attr_begin(); a != attr->attr_end(); a++)
	{
	    if (attr->is_container(a))
		write_attributes(oss, attr->get_attr_table(a), 
				 (prefix == "") ? attr->get_name(a) 
				 : prefix + string(".") + attr->get_name(a));
	    else {
		if (prefix != "")
		    oss << prefix << "." << attr->get_name(a);
		else
		    oss << attr->get_name(a);
		oss << ": ";

		int num_attr = attr->get_attr_num(a) - 1 ;
		for (int i = 0; i < num_attr; ++i)
		    oss << attr->get_attr(a, i) << ", ";
		oss << attr->get_attr(a, num_attr) << "<br>\n";
	    }
	}
    }
}

/** Given the DAS and DDS, build the HTML* document which contains all the
    global attributes for this dataset. A global attribute is defined here as
    any attribute not bound to variable in the dataset. Thus the attributes
    of `NC_GLOBAL', `global', etc. will be called global attributes if there
    are no variables `NC_GLOBAL', ... in the dataset. If there are variable
    with those names the attributes will NOT be considered `global
    attributes'.

    @memo Build the global attribute HTML* document.
    @return A string object containing the global attributes in human
    readable form (as an HTML* document).
*/
string
build_global_attributes(DAS &das, DDS &)
{
    bool found = false;
    ostrstream ga;

    ga << "<h3>Dataset Information</h3>\n<center>\n<table>\n";

    for (AttrTable::Attr_iter p = das.var_begin(); p != das.var_end(); p++)
    {
	string name = das.get_name(p);

	// I used `name_in_dds' originally, but changed to `name_is_global'
	// because aliases between groups of attributes can result in
	// attribute group names which are not in the DDS and are *not*
	// global attributes. jhrg. 5/22/97
	if (!name_in_kill_file(name) && name_is_global(name)) {
	    AttrTable *attr = das.get_table(p);
	    found = attr->first_attr() != (Pix)0; // we have global attrs
	    write_global_attributes(ga, attr, "");
	}
    }

    ga << "</table>\n</center><p>\n" << ends;

    if (found) {
	string global_attrs = ga.str();
	ga.rdbuf()->freeze(0);

	return global_attrs;
    }

    return "";
}

static string
fancy_typename(BaseType *v)
{
    string fancy;
    switch (v->type()) {
      case dods_byte_c:
	return "Byte";
      case dods_int16_c:
	return "16 bit Integer";
      case dods_uint16_c:
	return "16 bit Unsigned integer";
      case dods_int32_c:
	return "32 bit Integer";
      case dods_uint32_c:
	return "32 bit Unsigned integer";
      case dods_float32_c:
	return "32 bit Real";
      case dods_float64_c:
	return "64 bit Real";
      case dods_str_c:
	return "String";
      case dods_url_c:
	return "URL";
      case dods_array_c: {
	  ostrstream type;
	  Array *a = (Array *)v;
	  type << "Array of " << fancy_typename(a->var()) <<"s ";
	  for (Array::Dim_iter p = a->dim_begin(); p != a->dim_end(); p++)
	  {
	      type << "[" << a->dimension_name(p) << " = 0.." 
		   << a->dimension_size(p, false)-1 << "]";
	  }
	  type << ends;
	  string fancy = type.str();
	  type.rdbuf()->freeze(0);
	  return fancy;
      }
      case dods_list_c: {
	  ostrstream type;
	  List *l = (List *)v;
	  type << "List of " << fancy_typename(l->var()) <<"s " << ends;
	  string fancy = type.str();
	  type.rdbuf()->freeze(0);
	  return fancy;
      }
      case dods_structure_c:
	return "Structure";
      case dods_sequence_c:
	return "Sequence";
      case dods_grid_c:
	return "Grid";
      default:
	return "Unknown";
    }
}

// This function does not write #ends# to #vs#. 4/29/99 jhrg
static void
write_variable(BaseType *btp, DAS &das, ostrstream &vs)
{
    vs << "<td align=right valign=top><b>" << btp->name() 
	<< "</b>:</td>\n"
	<< "<td align=left valign=top>" << fancy_typename(btp)
	    << "<br>";

    AttrTable *attr = das.get_table(btp->name());
	    
    write_attributes(vs, attr, "");

    switch (btp->type()) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
      case dods_float32_c:
      case dods_float64_c:
      case dods_str_c:
      case dods_url_c:
      case dods_array_c:
      case dods_list_c:
	vs << "</td>\n";
	break;

      case dods_structure_c: {
	vs << "<table>\n";
	Structure *sp = dynamic_cast<Structure *>(btp);
	for (Structure::Vars_iter p = sp->var_begin(); p != sp->var_end(); p++)
	{
	    vs << "<tr>";
	    write_variable((*p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_sequence_c: {
	vs << "<table>\n";
	Sequence *sp = dynamic_cast<Sequence *>(btp);
	for (Sequence::Vars_iter p = sp->var_begin(); p != sp->var_end(); p++)
	{
	    vs << "<tr>";
	    write_variable((*p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_grid_c: {
	vs << "<table>\n";
	Grid *gp = dynamic_cast<Grid *>(btp);
	write_variable(gp->array_var(), das, vs);
	for (Grid::Map_iter p = gp->map_begin(); p != gp->map_end(); p++)
	{
	    vs << "<tr>";
	    write_variable((*p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      default:
	throw InternalErr(__FILE__, __LINE__, "Unknown type");
    }
}

/** Given the DAS and the DDS build an HTML table which describes each one of
    the variables by listing its name, datatype and all of its attriutes.

    @memo Build the variable summaries.
    @return A string object containing the variable summary information in
    human readable form (as an HTML* document).
*/

string
build_variable_summaries(DAS &das, DDS &dds)
{
    ostrstream vs;
    vs << "<h3>Variables in this Dataset</h3>\n<center>\n<table>\n";
    //    vs << "<tr><th>Variable</th><th>Information</th></tr>\n";

    for (DDS::Vars_iter p = dds.var_begin(); p != dds.var_end(); p++)
    {
	vs << "<tr>";
	write_variable((*p), das, vs);
	vs << "</tr>";
    }

    vs << "</table>\n</center><p>\n" << ends;

    string html = vs.str();
    vs.rdbuf()->freeze(0);

    return html;
}

static void
html_header()
{
    fprintf( stdout, "HTTP/1.0 200 OK\n" ) ;
    fprintf( stdout, "XDODS-Server: %s\n", DVR ) ;
    fprintf( stdout, "Content-type: text/html\n" ) ;
    fprintf( stdout, "Content-Description: dods_description\n" ) ;
    fprintf( stdout, "\n" ) ;	// MIME header ends with a blank line
}

#ifdef WIN32
void
#else
int
#endif 
main(int argc, char *argv[])
{
    if (argc != 4) {
	usage(argv);
	exit(1);
    }

    string options = argv[1];

    string name = argv[2];
    string doc;

    if (found_override(name, doc)) {
	html_header();
	fprintf( stdout, "%s", doc.c_str() ) ;
	exit(0);
    }

    // The site is not overriding the DAS/DDS generated information, so read
    // the DAS, DDS and user supplied documents. 

    string cgi = argv[3];

    DAS das;
    string command = "./" + cgi + "_das " + options + " '" + name + "'";
    DBG(cerr << "DAS Command: " << command << endl);

    try {
#ifndef WIN32
	//  Under win32, this has been temporarily removed to get it to
	//  compile. This code is not relevant for the client side for win32
	//  native port. Once one or more of the Dods servers have been
	//  ported, this will need patched similiar to the popen() fixes for
	//  ML loaddods.
	FILE *in = popen(command.c_str(), "r");
	if (in && remove_mime_header(in)) {
	    das.parse(in);
	    pclose(in);
	}
#endif

	DDS dds;
	command = "./" + cgi + "_dds '" + name + "'";
	DBG(cerr << "DDS Command: " << command << endl);

#ifndef WIN32
	//  Under win32, this has been temporarily removed to get it to
	//  compile. This code is not relevant for the client side for win32
	//  native port. Once one or more of the Dods servers have been
	//  ported, this will need patched similiar to the popen() fixes for
	//  ML loaddods.
	in = popen(command.c_str(), "r");
	if (in && remove_mime_header(in)) {
	    dds.parse(in);
	    pclose(in);
	}
#endif

	// Build the HTML* documents.

	string user_html = get_user_supplied_docs(name, cgi);

	string global_attrs = build_global_attributes(das, dds);

	string variable_sum = build_variable_summaries(das, dds);

	// Write out the HTML document.

	html_header();

	if (global_attrs.length()) {
	    fprintf( stdout, "%s\n%s\n%s\n%s\n",
			"<html><head><title>Dataset Information</title></head>",
		        "<html>",
			global_attrs.c_str(),
		        "<hr>" ) ;
	}

	fprintf( stdout, "%s\n", variable_sum.c_str() ) ;

	fprintf( stdout, "<hr>\n" ) ;

	fprintf( stdout, "%s\n", user_html.c_str() ) ;

	fprintf( stdout, "</html>\n" ) ;
    }
    catch (Error &e) {
	string error_msg = e.get_error_message();
	fprintf( stdout, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"\n" ) ;
	fprintf( stdout, "\"http://www.w3.org/TR/REC-html40/loose.dtd\">\n") ;
	fprintf( stdout, "<html><head><title>DODS Error</title>\n" ) ;
	fprintf( stdout, "</head>\n" ) ;
	fprintf( stdout, "<body>\n" ) ;
	fprintf( stdout, "<h3>Error building the DODS dataset usage repsonse</h3>:\n" ) ;
	fprintf( stdout, "%s", error_msg.c_str() ) ;
	fprintf( stdout, "<hr>\n" ) ;

#ifndef WIN32
	return 1;
#endif
    }

    //  Needed for VC++
#ifdef WIN32
    return;
#else
    return 0;
#endif
}

// $Log: usage.cc,v $
// Revision 1.22  2003/02/21 00:14:25  jimg
// Repaired copyright.
//
// Revision 1.21  2003/01/23 00:22:25  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.20  2003/01/10 19:46:41  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.16.2.16  2002/12/17 22:35:03  pwest
// Added and updated methods using stdio. Deprecated methods using iostream.
//
// Revision 1.16.2.15  2002/11/18 03:18:35  rmorris
// Minor porting change - VC++ required an explicit cast.
//
// Revision 1.16.2.14  2002/11/06 21:53:06  jimg
// I changed the includes of Regex.h from <Regex.h> to "Regex.h". This means
// make depend will include the header in the list of dependencies.
//
// Revision 1.16.2.13  2002/10/28 21:17:45  pwest
// Converted all return values and method parameters to use non-const iterator.
// Added operator== and operator!= methods to IteratorAdapter to handle Pix
// problems.
//
// Revision 1.16.2.12  2002/09/05 22:52:55  pwest
// Replaced the GNU data structures SLList and DLList with the STL container
// class vector<>. To maintain use of Pix, changed the Pix.h header file to
// redefine Pix to be an IteratorAdapter. Usage remains the same and all code
// outside of the DAP should compile and link with no problems. Added methods
// to the different classes where Pix is used to include methods to use STL
// iterators. Replaced the use of Pix within the DAP to use iterators instead.
// Updated comments for documentation, updated the test suites, and added some
// unit tests. Updated the Makefile to remove GNU/SLList and GNU/DLList.
//
// Revision 1.16.2.11  2002/08/06 21:24:03  jimg
// Made MT-Safe. Uses/requires pthreads; if configure cannot find pthreads,
// then this code is *not* MT-Safe.
//
// Revision 1.16.2.10  2002/06/18 22:49:19  jimg
// Added include of util.h and DAS.h. This was necessary because I removed the
// an include of Connect.h in cgi_util.h (a bad idea anyway).
//
// Revision 1.16.2.9  2002/06/06 01:03:37  jimg
// I modified main() so that it accepts (and expects) a third argument which
// holds command line options which are to be passed to the filter programs.
// This enables the dispatch code to pass along various options/arguments to the
// filters. Most important of these is the -r <cache_dir> option because the
// .info service for HDF files was often broken without it (see bug 453).
//
// Revision 1.19  2002/06/03 22:21:16  jimg
// Merged with release-3-2-9
//
// Revision 1.16.2.8  2002/05/09 22:00:22  jimg
// Fixed a bug in usage. If the CWD is not on the PATH (that is if `.' is not
// onthe PATH) then usage was not finding the das and dds handlers. I changed
// the code so that it now looks for ./<api>_das, ... where the `./' is new.
// This fixes the bug.
//
// Revision 1.16.2.7  2002/03/27 17:37:49  jimg
// Fixed printing of nested attributes. I replaced code that wrote out the
// attributes so that nested attributes are printed using the dot notation
// (before tehy were ignored). Look at write_attributes and
// write_global_attributes. Bug 167.
//
// Revision 1.16.2.6  2002/02/04 19:05:20  jimg
// Moved code duplicated in www_int into cgi_util.cc/h
//
// Revision 1.16.2.5  2002/01/23 03:17:32  jimg
// Fixed bug 122. The group ancillary files (01base.hdf, 02base.hdf, ...) ->
// base.html) was broken. I added a new function to cgi_util.cc and used
// that to fix this bug.
//
// Revision 1.16.2.4  2001/10/30 06:55:45  rmorris
// Win32 porting changes.  Brings core win32 port up-to-date.
//
// Revision 1.18  2001/10/14 01:28:38  jimg
// Merged with release-3-2-8.
//
// Revision 1.16.2.3  2001/10/09 01:18:16  jimg
// Fixed bug 300. Usage now returns 1 when it encounters an error.
//
// Revision 1.16.2.2  2001/10/02 16:57:18  jimg
// Fixed a documentation bug.
//
// Revision 1.17  2001/08/24 17:46:23  jimg
// Resolved conflicts from the merge of release 3.2.6
//
// Revision 1.16.2.1  2001/08/17 23:59:42  jimg
// Removed WIN32 compile guards from using statements.
//
// Revision 1.16  2000/10/18 17:24:59  jimg
// Fixed an error in get_user_supplied_docs() which caused the data file to be
// read instead of a <data file>.html file.
//
// Revision 1.15  2000/09/22 02:17:23  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.14  2000/07/09 22:05:37  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.13  2000/06/07 18:07:01  jimg
// Merged the pc port branch
//
// Revision 1.12.20.1  2000/06/02 18:39:04  rmorris
// Mod's for port to win32.
//
// Revision 1.12  1999/05/04 19:47:24  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.11  1999/04/29 02:29:37  jimg
// Merge of no-gnu branch
//
// Revision 1.10  1999/04/22 22:30:52  jimg
// Uses dynamic_cast
//
// Revision 1.9  1999/04/09 17:17:30  jimg
// Added support for the new datatypes.
// Removed old code.
// Changed header generated to include XDODS-Server.
//
// Revision 1.8  1999/03/24 23:27:49  jimg
// Added support for the new Int16, UInt16 and Float32 types.
//
// Revision 1.7  1998/12/16 19:10:53  jimg
// Added support for XDODS-Server MIME header. This fixes a problem where our
// use of Server clashed with Java.
//
// Revision 1.6  1998/10/21 16:56:24  jimg
// Removed name_in_dds using #if 0 ... #endif
//
// Revision 1.5.6.2  1999/02/05 09:32:37  jimg
// Fixed __unused__ so that it not longer clashes with Red Hat 5.2 inlined
// math code. 
//
// Revision 1.5.6.1  1999/02/02 21:57:08  jimg
// String to string version
//
// Revision 1.5  1998/02/09 20:11:43  jimg
// Added/fixed doc++ comments.
//
// Revision 1.4  1998/02/05 20:14:07  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.3  1997/05/22 22:32:04  jimg
// Changed the way global attributes are ferreted out. Previously I had
// assumed that any attribute group that was not also the name of a variable
// was a group of `global' attributes. However, the new aliases change this.
// In response I've changed the way usage looks for global attributes; it
// looks for attribute groups which contain the strings `global' and/or
// `dods'. Case is not important.
//
// Revision 1.2  1996/12/18 18:41:33  jimg
// Added massive fixes for the processing of attributes to sort out the `global
// attributes'. Also added changes to the overall layout of the resulting
// document so that the hierarchy of the data is represented. Added some new
// utility functions.
//
// Revision 1.1  1996/12/11 19:55:18  jimg
// Created.
