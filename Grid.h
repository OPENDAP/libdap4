
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Interface to the Grid ctor class. Grids contain a single array (the `main'
// array) of dimension N and N single dimension arrays (map arrays). For any
// dimension n of the main array, the size of the nth map array must match
// the size of the main array's nth dimension. Grids are used to map
// non-integer scales to multidimensional point data.
//
// jhrg 9/15/94

/* $Log: Grid.h,v $
/* Revision 1.23  1997/06/05 23:16:13  jimg
/* Added components() and projection_yields_grid() mfuncs.
/*
 * Revision 1.22  1997/03/08 19:02:03  jimg
 * Changed default param to check_semantics() from  to String()
 * and removed the default from the argument list in the mfunc definition
 *
 * Revision 1.21  1997/02/28 01:29:05  jimg
 * Changed check_semantics() so that it now returns error messages in a String
 * object (passed by reference).
 *
 * Revision 1.20  1996/06/04 21:33:33  jimg
 * Multiple connections are now possible. It is now possible to open several
 * URLs at the same time and read from them in a round-robin fashion. To do
 * this I added data source and sink parameters to the serialize and
 * deserialize mfuncs. Connect was also modified so that it manages the data
 * source `object' (which is just an XDR pointer).
 *
 * Revision 1.19  1996/05/31 23:29:48  jimg
 * Updated copyright notice.
 *
 * Revision 1.18  1996/05/16 22:50:02  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.17  1996/03/05 18:08:59  jimg
 * Added ce_eval to serailize member function.
 *
 * Revision 1.16  1995/12/09  01:06:47  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.15  1995/12/06  21:56:26  jimg
 * Added `constrained' flag to print_decl.
 * Removed third parameter of read.
 * Modified print_decl() to print only those parts of a dataset that are
 * selected when `constrained' is true.
 *
 * Revision 1.14  1995/10/23  23:20:57  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.13  1995/08/26  00:31:34  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.12  1995/08/22  23:48:25  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.11  1995/05/10  13:45:19  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.10  1995/03/04  14:35:01  jimg
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
 * Revision 1.9  1995/02/10  02:23:06  jimg
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
 * Revision 1.8  1995/01/19  21:59:17  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.7  1995/01/18  18:40:08  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.6  1995/01/11  15:54:48  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.5  1994/12/15  21:25:45  dan
 * Added print_val() member function.
 *
 * Revision 1.4  1994/11/22  14:05:57  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:55  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:45:29  jimg
 * Added mfunc check_semantics().
 * Added sanity checking on the variable list (is it empty?).
 */

#ifndef _Grid_h
#define _Grid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include <SLList.h>
#include <Pix.h>

#include "BaseType.h"
#include "config_dap.h"
#ifdef TRACE_NEW
#include "trace_new.h"
#endif

/** The Grid data-type. A Grid groups a set of N Map vectors with a single
    N-dimensional array. This type supports data that are stored in
    rectilinear arrays in a computer but which represent some parameter
    mapped over a grid that is not, in actuality, rectilinear. */

class Grid: public BaseType {
private:
    BaseType *_array_var;
    SLList<BaseTypePtr> _map_vars;

    void _duplicate(const Grid &s);

public:
    Grid(const String &n = (char *)0);
    Grid(const Grid &rhs);
    virtual ~Grid();
    
    const Grid &operator=(const Grid &rhs);
    virtual BaseType *ptr_duplicate() = 0;

    virtual void set_send_p(bool state);
    virtual void set_read_p(bool state);

    virtual BaseType *var(const String &name);
    virtual void add_var(BaseType *bt, Part part);

    BaseType *array_var();

    Pix first_map_var();
    void next_map_var(Pix &p);
    BaseType *map_var(Pix p);

    virtual unsigned int width();

    /** Return the number of components in this Grid object. If the optional
        parameter #constrained# is true then return only those components
	that are part of the current constraint (i.e., that are projected). */
    virtual int components(bool constrained = false);

    /** Return true if the current projection will yield a Grid that will
        pass the check_semantics() mfunc. A `Grid' that, when projected, will
	not pass the check_semantics mfunc must be sent as either a Structure
	of Arrays or a single Array depending on the projection. */
    virtual bool projection_yields_grid();

    virtual bool serialize(const String &dataset, DDS &dds, XDR *sink,
			   bool ce_eval = true);
    virtual bool deserialize(XDR *source, bool reuse = false);

    virtual bool read(const String &dataset, int &error) = 0;

    virtual unsigned int val2buf(void *buf, bool reuse = false);
    virtual unsigned int buf2val(void **val);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, String space = "",
			   bool print_decl_p = true);

    virtual bool check_semantics(String &msg = String(), bool all = false);
};

#endif

