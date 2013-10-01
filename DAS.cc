
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Methods for the class DAS - a class used to parse the dataset attribute
// structure.
//
// jhrg 7/25/94

#include "config.h"

#include <cstdio>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

#include <iostream>
#include <string>

#include "DAS.h"
#include "AttrTable.h"
#include "Error.h"
#include "InternalErr.h"
#include "parser.h"
#include "escaping.h"
#include "debug.h"

using std::cerr;
using std::endl;

// Glue routines declared in das.lex
extern void das_switch_to_buffer(void *new_buffer);
extern void das_delete_buffer(void * buffer);
extern void *das_buffer(FILE *fp);

//extern void dasrestart(FILE *yyin);
//extern int dasparse(void *arg); // defined in das.tab.c
extern int dasparse(libdap::parser_arg *arg); // defined in das.tab.c

namespace libdap {

/** Create an empty DAS
 */
DAS::DAS() : DapObj(), d_container( 0 )
{}

#if 0
DAS::DAS(AttrTable *attr, string name)
{
    append_container(attr, www2id(name));
}
#endif

// FIXME: Need to create copy constructor and op=.

/** @brief This deletes the pointers to AttrTables allocated during the parse
 * (and at other times). jhrg 7/29/94
 */
DAS::~DAS()
{}

/** @brief Returns the name of the current attribute container when multiple
 * files used to build this DAS
 */
string
DAS::container_name()
{
    return _container_name ;
}

/** @brief Sets the name of the current attribute container when multiple
 * files used to build this DAS.
 *
 * @param cn container name
 */
void DAS::container_name(const string &cn)
{
    // We want to find a top level attribute table with the given name. So
    // set d_container to null first so that we aren't searching some
    // previous container
    if (cn != _container_name) {
        d_container = 0;
        if (!cn.empty()) {
            d_container = get_table(cn);
            if (!d_container) {
                d_container = add_table(cn, new AttrTable);
            }
        }
        _container_name = cn;
    }
}

/** @brief Returns the current attribute container when multiple files
 * used to build this DAS.
 *
 * @return current attribute table for current container
 */
AttrTable *
DAS::container()
{
    return d_container ;
}

/** @brief Returns the number of attributes in the current attribute table
 *
 * If the there is a container set, then return the number of variable
 * attribute tables for the current container. If not set then return the
 * number of current attribute tables in the outermost attribute table.
 */
unsigned int DAS::get_size() const
{
    if (d_container) {
        return d_container->get_size();
    }
    return d_attrs.get_size();
}

/** @brief erase all attributes in this DAS
 */
void DAS::erase()
{
    if (d_container) {
        d_container->erase();
    }
    else {
        d_attrs.erase();
    }
}

/** @brief Returns a reference to the attribute table for the first variable.
 */
AttrTable::Attr_iter DAS::var_begin()
{
    if (d_container) {
        return d_container->attr_begin();
    }
    return d_attrs.attr_begin();
}

/** Returns a reference to the end of the attribute table. Does not
 *  point to an attribute table.
 */
AttrTable::Attr_iter DAS::var_end()
{
    if (d_container) {
        return d_container->attr_end();
    }
    return d_attrs.attr_end();
}

/** @brief Returns the name of the referenced variable attribute table.
 */
string DAS::get_name(AttrTable::Attr_iter &i)
{
    if (d_container) {
        return d_container->get_name(i);
    }
    return d_attrs.get_name(i);
}

/** @brief Returns the referenced variable attribute table.
 */
AttrTable *
DAS::get_table(AttrTable::Attr_iter &i)
{
    if (d_container) {
        return d_container->get_attr_table(i);
    }
    return d_attrs.get_attr_table(i);
}

/** @brief Returns the variable attribute table with the given name.
 */
AttrTable *
DAS::get_table(const string &name)
{
    if (d_container) {
        return d_container->get_attr_table(name);
    }
    return d_attrs.get_attr_table(name);
}

//@}

/** @brief Adds an attribute table to the DAS.
    @name add_table()
*/
//@{

/** @brief Adds a variable attribute table to the DAS or the current
 * dataset container attribute table.
 */
AttrTable *
DAS::add_table( const string &name, AttrTable *at )
{
    if (d_container) {
        at->set_is_global_attribute(false);
        return d_container->append_container(at, name);
    }
    return d_attrs.append_container( at, name ) ;
}

//@}

/** @brief Reads a DAS in from an external source.

    @name parse()
*/
//@{


/** @brief Reads a DAS from the named file.

    Read attributes from a file. Returns false if unable to open
    the file, otherwise returns the result of the mfunc parse. */
void
DAS::parse(string fname)
{
    FILE *in = fopen(fname.c_str(), "r");

    if (!in) {
        throw Error(cannot_read_file, "Could not open: " + fname);
    }

    parse(in);

    int res = fclose(in);
    if (res) {
        DBG(cerr << "DAS::parse - Failed to close file " << (void *)in << endl ;) ;
    }
}

/** @brief Read attributes from a file descriptor.

    If the file descriptor cannot be fdopen'd, return false, otherwise
    return the status of the mfunc parse.

    \note Added call to dup() within fdopen so that once the FILE * is
    closed the decriptor fd will not also be closed (instead the
    duplicate descriptor will be closed). Thus further information can
    be read from the descriptor fd.
*/
void
DAS::parse(int fd)
{
#ifdef WIN32
    FILE *in = fdopen(_dup(fd), "r");
#else
    FILE *in = fdopen(dup(fd), "r");
#endif

    if (!in) {
        throw InternalErr(__FILE__, __LINE__, "Could not access file.");
    }

    parse(in);

    int res = fclose(in);
    if (res) {
        DBG(cerr << "DAS::parse(fd) - Failed to close " << (void *)in << endl ;) ;
    }
}



/** @brief Reads a DAS from an open file descriptor.

    Read attributes from in (which defaults to stdin). If
    dasrestart() fails, return false, otherwise return the status
    of dasparse().
*/
void
DAS::parse(FILE *in)
{
    if (!in) {
        throw InternalErr(__FILE__, __LINE__, "Null input stream.");
    }

    void *buffer = das_buffer(in);
    das_switch_to_buffer(buffer);

    parser_arg arg(this);

    //bool status = dasparse((void *) & arg) == 0;
    bool status = dasparse(&arg) == 0;

    das_delete_buffer(buffer);

    //  STATUS is the result of the parser function; if a recoverable error
    //  was found it will be true but arg.status() will be false.
    if (!status || !arg.status()) {// Check parse result
        if (arg.error())
            throw *arg.error();
    }
}

//@}

/** Creates an ASCII representation of a DAS on the given output
    stream.

    When an identifier contains a character that contains
    characters that cannot be present in a URL (e.g., a space)
    AttrTable::print replaces those characters with WWW
    escape codes. 7/13/2001 jhrg

    @param out output FILE on which to print the DAS
    @param dereference If true, follow aliases. Default is false.
*/

void
DAS::print(FILE *out, bool dereference)
{
    fprintf(out, "Attributes {\n") ;

    d_attrs.print(out, "    ", dereference);

    fprintf(out, "}\n") ;
}

/** Creates an ASCII representation of a DAS on the given output
    stream.

    When an identifier contains a character that contains
    characters that cannot be present in a URL (e.g., a space)
    AttrTable::print replaces those characters with WWW
    escape codes. 7/13/2001 jhrg

    @param out output ostream on which to print the DAS
    @param dereference If true, follow aliases. Default is false.
*/

void
DAS::print(ostream &out, bool dereference)
{
    out << "Attributes {\n" ;

    d_attrs.print(out, "    ", dereference);

    out << "}\n" ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance and then calls parent dump
 *
 * @param strm C++ i/o stream to dump the information to
 * @return void
 */
void
DAS::dump(ostream &strm) const
{
    strm << DapIndent::LMarg << "DAS::dump - ("
         << (void *)this << ")" << endl ;
    DapIndent::Indent() ;
    if( d_container )
    {
	strm << DapIndent::LMarg << "current container: " << _container_name
	     << endl ;
    }
    else
    {
	strm << DapIndent::LMarg << "current container: NONE" << endl ;
    }
    d_attrs.dump(strm) ;
    DapIndent::UnIndent() ;
}

} // namespace libdap

