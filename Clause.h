
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
 
// (c) COPRIGHT URI/MIT 1995-1999
// Please first read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//	jhrg,jimg	James Gallagher <jgallagher@gso.uri.edu>

// Interface for the CE Clause class.

#ifndef _clause_h
#define _clause_h


#ifndef _expr_h
#include "expr.h"
#endif

#ifndef _rvalue_h
#include "RValue.h"
#endif

/** The selection part of a a DODS constraint expression may contain one or
    more clauses, separated by ampersands (\&). This is modeled in the DDS
    class structure as a singly-linked list of Clause objects. In addition, a
    constraint expression may be a single function call, also represented in
    the DDS using an instance of Clause.

    Each clause object can contain a representation of one of three
    possible forms:

    <ol>

    <li> A relational clause, where an operator tests the relation
    between two operands.  This kind of clause evaluates to a boolean
    value. For example: <tt>a > b</tt>.

    <li> A boolean function, where some function operates on
    arguments in the clause to return a boolean value.  For example,
    consider a scalar A and a list L.  The clause <tt>find(A,L)</tt> might
    return TRUE if A is a member of L (if the <tt>find()</tt> function is
    defined). 

    <li> A clause that returns a pointer to a DODS BaseType value.
    This is a clause that evaluates to some data value (be it scalar
    or vector).  For example, <tt>sig0()</tt> might be included in the
    constraint expression parser to calculate density from pressure,
    temperature, and salinity.  In this case, <tt>sig0(p,t,s)</tt> would be a
    clause that evaluates to a data value.

    </ol>

    This might be a bit confusing; in the first, and by far more common, form
    of constraint expressions (CEs) only the first two types of clauses may
    appear. In the second form of the CE only the last type of clause may
    occur. The Clause class, however, can store them all.

    The Clause object holds the constraint expression <i>after</i> it
    has been parsed.  The parser renders the relational operator into
    an integer, and the functions into pointers.

    @brief Holds a fragment of a constraint expression.
    @see DDS::parse_constraint */
struct Clause {

private:
    /** The relational operator, if any. */
    int _op;
    /** A pointer to a valid boolean function. */
    bool_func _b_func;
    /** A pointer to a valid function that returns a pointer to a
	BaseType. */
    btp_func _bt_func;

    int _argc;			// arg count
    rvalue *_arg1;		// only for operator
    rvalue_list *_args;		// vector arg

public:
    Clause(const int oper, rvalue *a1, rvalue_list *rv);
    Clause(bool_func func, rvalue_list *rv);
    Clause(btp_func func, rvalue_list *rv);
    Clause();

    virtual ~Clause();

  bool OK();

  bool boolean_clause();

  bool value_clause();

  bool value(const string &dataset, DDS &dds);

  bool value(const string &dataset, DDS &dds, BaseType **value);
};
	
// $Log: Clause.h,v $
// Revision 1.14  2004/07/07 21:08:47  jimg
// Merged with release-3-4-8FCS
//
// Revision 1.12.2.2  2004/07/02 20:41:51  jimg
// Removed (commented) the pragma interface/implementation lines. See
// the ChangeLog for more details. This fixes a build problem on HP/UX.
//
// Revision 1.13  2003/12/08 18:02:29  edavis
// Merge release-3-4 into trunk
//
// Revision 1.12.2.1  2003/06/23 11:49:18  rmorris
// The // #pragma interface directive to GCC makes the dynamic typing functionality
// go completely haywire under OS X on the PowerPC.  We can't use that directive
// on that platform and it was ifdef'd out for that case.
//
// Revision 1.12  2003/04/22 19:40:27  jimg
// Merged with 3.3.1.
//
// Revision 1.11  2003/02/21 00:14:24  jimg
// Repaired copyright.
//
// Revision 1.10.2.1  2003/02/21 00:10:07  jimg
// Repaired copyright.
//
// Revision 1.10  2003/01/23 00:22:23  jimg
// Updated the copyright notice; this implementation of the DAP is
// copyrighted by OPeNDAP, Inc.
//
// Revision 1.9  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.8  2000/09/22 02:17:19  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.7  2000/08/02 22:46:48  jimg
// Merged 3.1.8
//
// Revision 1.6.6.1  2000/08/01 21:09:35  jimg
// Destructor is now virtual
//
// Revision 1.6  1999/04/29 02:29:27  jimg
// Merge of no-gnu branch
//
// Revision 1.5  1998/10/21 16:35:22  jimg
// Fixed doc++ comments.
// Changed member names so they start with an underscore - makes the member
// functions easier to read.
//
// Revision 1.4.6.1  1999/02/02 21:56:56  jimg
// String to string version
//
// Revision 1.4  1998/01/12 14:27:56  tom
// Second pass at class documentation.
//
// Revision 1.3  1997/12/18 15:06:10  tom
// First draft of class documentation, entered in doc++ format,
// in the comments
//
// Revision 1.2  1996/11/27 22:40:18  jimg
// Added DDS as third parameter to function in the CE evaluator
//
// Revision 1.1  1996/05/31 22:45:03  jimg
// Added.
//

#endif // _clause_h
