
// (c) COPYRIGHT URI/MIT 1996, 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// The Usage server. Arguments: two arguments; the dataset name and the
// pathname and `api prefix' of the data server. Returns a HTML document that
// describes what information this dataset contains, special characteristics
// of the server users might want to know and any special information that
// the dataset providers want to make available. 
// jhrg 12/9/96

// $Log: usage.cc,v $
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
//

#include "config_dap.h"

static char rcsid[] not_used = {"$Id: usage.cc,v 1.13 2000/06/07 18:07:01 jimg Exp $"};

#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include <string>
#if defined(__GNUG__) || defined(WIN32)
#include <strstream>
#else
#include <sstream>
#endif
#include <Regex.h>

#include "cgi_util.h"
#include "debug.h"

#ifdef WIN32
using namespace std;
#endif

static void
usage(char *argv[])
{
    cerr << argv[0] << " <filename> <CGI directory>" << endl
	 << "      Takes two required arguments; the dataset filename" << endl
	 << "      and the directory and api prefix for the filter" << endl
	 << "      program" << endl; 
}

/** Look for the override file by taking the dataset name and appending
    `.ovr' to it. If such a file exists, then read it in and store the
    contents in #doc#. Note that the file contents are not checked to see if
    they are valid HTML (which they must be). 

    @return True if the `override file' is present, false otherwise. in the
    later case #doc#'s contents are undefined.
*/

bool
found_override(string name, string &doc)
{
    ifstream ifs((name + ".ovr").c_str());
    if (!ifs)
	return false;

    char tmp[256];
    doc = "";
    while (!ifs.eof()) {
	ifs.getline(tmp, 255);
	strcat(tmp, "\n");
	doc += tmp;
    }

    return true;
}

/** Read the input stream #in# and discard the MIME header. The MIME header
    is separated from the body of the document by a single blank line. If no
    MIME header is found, then the input stream is `emptied' and will contain
    nothing.

    @memo Read and discard the MIME header of the stream #in#.
    @return True if a MIME header is found, false otherwise.
*/

bool
remove_mime_header(FILE *in)
{
    char tmp[256];
    while (!feof(in)) {
	fgets(tmp, 255, in);
	if (tmp[0] == '\n')
	    return true;
    }

    return false;
}    

/** Look in the CGI directory (given by #cgi#) for a per-cgi HTML* file. Also
    look for a dataset-specific HTML* document. Catenate the documents and
    return them in a single String variable.

    The #cgi# path must include the `API' prefix at the end of the path. For
    example, for the NetCDF server whose prefix is `nc' and resides in the
    DODS_ROOT/etc directory of my computer, #cgi# is
    `/home/dcz/jimg/src/DODS/etc/nc'. This function then looks for the file
    named #cgi#.html.

    Similarly, to locate the dataset-specific HTML* file it catenates `.html'
    to #name#, where #name# is the name of the dataset. If the filename part
    of #name# is of the form [A-Za-z]+[0-9]*.* then this function also looks
    for a file whose name is [A-Za-z].html For example, if #name# is
    .../data/fnoc1.nc this function first looks for .../data/fnoc1.nc.html.
    However, if that does not exist it will look for .../data/fnoc.html. This
    allows one `per-dataset' file to be used for a collection of files with
    the same root name.

    NB: An HTML* file contains HTML without the <html>, <head> or <body> tags
    (my own notation).

    @memo Look for the user supplied CGI- and dataset-specific HTML* documents.
    @return A String which contains these two documents catenated. Documents
    that don't exist are treated as `empty'.
*/

string
get_user_supplied_docs(string name, string cgi)
{
    char tmp[256];
    ostrstream oss;
    ifstream ifs((cgi + ".html").c_str());

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
	
	oss << "<hr>";
    }

    ifs.open((name + ".html").c_str());

    // If name.html cannot be opened, look for basename.html
    if (!ifs) {
	unsigned int slash = name.find_last_of('/');
	string pathname = name.substr(0, slash);
	string filename = name.substr(slash+1);
	// filename = filename.at(RXalpha); // XXX can't do this with string
	string new_name = pathname + "/" + filename; // XXX + ".html"
	ifs.open(new_name.c_str());
    }

    if (ifs) {
	while (!ifs.eof()) {
	    ifs.getline(tmp, 255);
	    oss << tmp << "\n";
	}
	ifs.close();
    }

    oss << ends;
    string html = oss.str();
    oss.rdbuf()->freeze(0);

    return html;
}

// This code could use a real `kill-file' some day - about the same time that
// the rest of the server gets a `rc' file... For the present just see if a
// small collection of regexs match the name.

static bool
name_in_kill_file(const string &name)
{
    static Regex dim(".*_dim_[0-9]*", 1); // HDF `dimension' attributes.

    return dim.match(name.c_str(), name.length()) != -1;
}

static bool
name_is_global(string &name)
{
    static Regex global("\\(.*global.*\\)\\|\\(.*dods.*\\)", 1);
    downcase(name);
    return global.match(name.c_str(), name.length()) != -1;
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

    for (Pix p = das.first_var(); p; das.next_var(p)) {
	string name = das.get_name(p);

	// I used `name_in_dds' originally, but changed to `name_is_global'
	// because aliases between groups of attributes can result in
	// attribute group names which are not in the DDS and are *not*
	// global attributes. jhrg. 5/22/97
	if (!name_in_kill_file(name) && name_is_global(name)) {
	    AttrTable *attr = das.get_table(p);

	    if (attr) {
		for (Pix a = attr->first_attr(); a; attr->next_attr(a)) {
		    int num_attr = attr->get_attr_num(a);

		    found = true;
		    ga << "\n<tr><td align=right valign=top><b>" 
		       << attr->get_name(a) << "</b>:</td>\n";
		    ga << "<td align=left>";
		    for (int i = 0; i < num_attr; ++i)
			ga << attr->get_attr(a, i) << "<br>";
		    ga << "</td></tr>\n";
		}
	    }
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
	  for (Pix p = a->first_dim(); p; a->next_dim(p))
	      type << "[" << a->dimension_name(p) << " = 0.." 
		   << a->dimension_size(p, false)-1 << "]";
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
	    
    if (attr)			// Not all variables have attributes!
	for (Pix a = attr->first_attr(); a; attr->next_attr(a)) {
	    int num = attr->get_attr_num(a);
	    
	    vs << attr->get_name(a) << ": ";
	    for (int i = 0; i < num; ++i, (void)(i<num && vs << ", "))
		vs << attr->get_attr(a, i);
	    vs << "<br>\n";
	}

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
	for (Pix p = sp->first_var(); p; sp->next_var(p)) {
	    vs << "<tr>";
	    write_variable(sp->var(p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_sequence_c: {
	vs << "<table>\n";
	Sequence *sp = dynamic_cast<Sequence *>(btp);
	for (Pix p = sp->first_var(); p; sp->next_var(p)) {
	    vs << "<tr>";
	    write_variable(sp->var(p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      case dods_grid_c: {
	vs << "<table>\n";
	Grid *gp = dynamic_cast<Grid *>(btp);
	write_variable(gp->array_var(), das, vs);
	for (Pix p = gp->first_map_var(); p; gp->next_map_var(p)) {
	    vs << "<tr>";
	    write_variable(gp->map_var(p), das, vs);
	    vs << "</tr>";
	}
	vs << "</table>\n";
	break;
      }

      default:
	assert("Unknown type" && false);
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

    for (Pix p = dds.first_var(); p; dds.next_var(p)) {
	vs << "<tr>";
	write_variable(dds.var(p), das, vs);
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
    cout << "HTTP/1.0 200 OK" << endl;
    cout << "XDODS-Server: " << DVR << endl;
    cout << "Content-type: text/html" << endl; 
    cout << "Content-Description: dods_description" << endl;
    cout << endl;			// MIME header ends with a blank line
}

#ifdef WIN32
void
#else
int
#endif 
main(int argc, char *argv[])
{
    if (argc != 3) {
	usage(argv);
	exit(1);
    }

    string name = argv[1];
    string doc;

    if (found_override(name, doc)) {
	html_header();
	cout << doc;
	exit(0);
    }

    // The user is not overriding the DAS/DDS generated information, so read
    // the DAS, DDS and user supplied documents. 

    string cgi = argv[2];

    DAS das;
    string command = cgi + "_das '" + name + "'";
    DBG(cerr << "DAS Command: " << command << endl);

#ifndef WIN32
	//  Under win32, this has been temporarily removed to get it to compile.
	//  This code is not relevant for the client side for win32 native port.  Once
	//  one or more of the Dods servers have been ported, this will need patched
	//  similiar to the popen() fixes for ML loaddods.
    FILE *in = popen(command.c_str(), "r");
    if (in && remove_mime_header(in)) {
	das.parse(in);
	pclose(in);
    }
#endif

    DDS dds;
    command = cgi + "_dds '" + name + "'";
    DBG(cerr << "DDS Command: " << command << endl);

#ifndef WIN32
	//  Under win32, this has been temporarily removed to get it to compile.
	//  This code is not relevant for the client side for win32 native port.  Once
	//  one or more of the Dods servers have been ported, this will need patched
	//  similiar to the popen() fixes for ML loaddods.
    in = popen((cgi + "_dds '" + name + "'").c_str(), "r");
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
	cout << "<html><head><title>Dataset Information</title></head>" 
	     << endl 
	     << "<html>" << endl 
	     << global_attrs << endl 
	     << "<hr>" << endl;
    }

    cout << variable_sum << endl;

    cout << "<hr>" << endl;

    cout << user_html << endl;

    cout << "</html>" << endl;

    exit(0);

//  Needed for VC++
#ifdef WIN32
	return;
#endif
}


