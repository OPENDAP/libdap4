
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Class for array variables. The dimensions of the array are stored in the
// list SHAPE. 
//
// jhrg 9/6/94

/* $Log: Array.h,v $
/* Revision 1.28  1996/05/31 23:29:19  jimg
/* Updated copyright notice.
/*
 * Revision 1.27  1996/05/16 22:49:55  jimg
 * Dan's changes for version 2.0. Added a parameter to read that returns
 * an error code so that EOF can be distinguished from an actual error when
 * reading sequences. This *may* be replaced by an error member function
 * in the future.
 *
 * Revision 1.26  1996/05/06 21:14:06  jimg
 * Added dimension_start, _stop and _stride member functions to this class.
 * Changed the first argument of add_constraint from Pix &p to Pix p (the member
 * function does not change the Pix).
 *
 * Revision 1.25  1996/03/05 18:46:39  jimg
 * Replaced <limits.h> with "dods-limits.h".
 *
 * Revision 1.24  1996/02/02 00:30:58  jimg
 * Merge changes for DODS-1.1.0 into DODS-2.x
 *
 * Revision 1.23  1995/12/09  01:06:31  jimg
 * Added changes so that relational operators will work properly for all the
 * datatypes (including Sequences). The relational ops are evaluated in
 * DDS::eval_constraint() after being parsed by DDS::parse_constraint().
 *
 * Revision 1.22  1995/12/06  21:37:55  jimg
 * Added members to record information about array index constraints.
 * Added mfuns to access/set those members.
 * Changed read from three to two arguments.
 *
 * Revision 1.21  1995/11/22  22:31:04  jimg
 * Modified so that the Vector class is now the parent class.
 *
 * Revision 1.20  1995/10/23  23:20:46  jimg
 * Added _send_p and _read_p fields (and their accessors) along with the
 * virtual mfuncs set_send_p() and set_read_p().
 *
 * Revision 1.19  1995/08/26  00:31:23  jimg
 * Removed code enclosed in #ifdef NEVER #endif.
 *
 * Revision 1.18  1995/08/22  23:48:12  jimg
 * Removed card() member function.
 * Removed old, deprecated member functions.
 * Changed the names of read_val and store_val to buf2val and val2buf.
 *
 * Revision 1.17.2.1  1995/09/14 20:59:52  jimg
 * Fixed declaration of, and calls to, _duplicate() by changing the formal
 * param from a pointer to a reference.
 *
 * Revision 1.17  1995/05/10  13:45:03  jimg
 * Changed the name of the configuration header file from `config.h' to
 * `config_dap.h' so that other libraries could have header files which were
 * installed in the DODS include directory without overwriting this one. Each
 * config header should follow the convention config_<name>.h.
 *
 * Revision 1.16  1995/04/28  19:53:45  reza
 * First try at adding constraints capability.
 * Enforce a new size calculated from constraint expression.
 *
 * Revision 1.15  1995/03/16  17:23:00  jimg
 * Added include of config_dap.h before all other includes.
 * Fixed deletes of buffers in read_val().
 * Added initialization of _buf in ctor.
 *
 * Revision 1.14  1995/03/04  14:34:53  jimg
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
 * Revision 1.13  1995/02/10  02:22:53  jimg
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
 * Revision 1.12  1995/01/19  21:59:08  jimg
 * Added read_val from dummy_read.cc to the sample set of sub-class
 * implementations.
 * Changed the declaration of readVal in BaseType so that it names the
 * mfunc read_val (to be consistant with the other mfunc names).
 * Removed the unnecessary duplicate declaration of the abstract virtual
 * mfuncs read and (now) read_val from the classes Byte, ... Grid. The
 * declaration in BaseType is sufficient along with the decl and definition
 * in the *.cc,h files which contain the subclasses for Byte, ..., Grid.
 *
 * Revision 1.11  1995/01/18  18:37:54  dan
 * Declared member function 'readVal', defined in dummy_read.cc
 *
 * Revision 1.10  1995/01/11  15:54:40  jimg
 * Added modifications necessary for BaseType's static XDR pointers. This
 * was mostly a name change from xdrin/out to _xdrin/out.
 * Removed the two FILE pointers from ctors, since those are now set with
 * functions which are friends of BaseType.
 *
 * Revision 1.9  1994/12/14  20:36:58  dan
 * Added dimensions() member function definition.
 * Removed alloc_buf() and free_buf() definitions.
 *
 * Revision 1.8  1994/12/14  17:55:23  dan
 * Removed dim() member function.
 *
 * Revision 1.7  1994/12/12  20:33:34  jimg
 * Fixed struct dimensions.
 *
 * Revision 1.6  1994/12/12  19:40:30  dan
 * Modified Array class definition removing inheritance from class CtorType
 * and to inherit directly from class BaseType.  Removed member function
 * dimensions().
 *
 * Revision 1.5  1994/12/09  21:36:34  jimg
 * Added support for named array dimensions.
 *
 * Revision 1.4  1994/11/22  14:05:22  jimg
 * Added code for data transmission to parts of the type hierarchy. Not
 * complete yet.
 * Fixed erros in type hierarchy headers (typos, incorrect comments, ...).
 *
 * Revision 1.3  1994/10/17  23:34:43  jimg
 * Added code to print_decl so that variable declarations are pretty
 * printed.
 * Added private mfunc duplicate().
 * Added ptr_duplicate().
 * Added Copy ctor, dtor and operator=.
 *
 * Revision 1.2  1994/09/23  14:31:37  jimg
 * Added check_semantics mfunc.
 * Added sanity checking for access to shape list (is it empty?).
 * Added cvs log listing to Array.cc.
 */

#ifndef _Array_h
#define _Array_h 1

#ifdef __GNUG__
#pragma interface
#endif

#ifdef NEVER
#include <limits.h>
#endif
#include <SLList.h>

#include "dods-limits.h"
#include "Vector.h"

const int DODS_MAX_ARRAY = DODS_UINT_MAX;

class Array: public Vector {
private:
    struct dimension {		// each dimension has a size and a name
	int size;
	String name;
	int start, stop, stride;// a constraint determines these values
	int c_size;		// size of dimension once constrained
	bool selected;		// true if this dimension is selected
    };

    SLList<dimension> _shape;	// list of dimensions (i.e., the shape)

    unsigned int print_array(ostream &os, unsigned int index,
			     unsigned int dims, unsigned int shape[]);

protected:
    void _duplicate(const Array &a);

public:
    Array(const String &n = (char *)0, BaseType *v = 0);
    Array(const Array &rhs);
    virtual ~Array();

    const Array &operator=(const Array &rhs);
    virtual BaseType *ptr_duplicate() = 0; 

    virtual bool read(const String &dataset, int &error) = 0;

    void update_length(int size);

    // used to create the dimentsions of an array 
    void append_dim(int size, String name = "");

    // once a dimension has be created, set its constraint. This sets the
    // selected flag and c_size .
    void add_constraint(Pix p, int start, int stride, int stop);
    void reset_constraint();
    void clear_constraint();
    
    Pix first_dim();
    void next_dim(Pix &p);

    int dimension_size(Pix p, bool constrained = false);
    int dimension_start(Pix p, bool constrained = false);
    int dimension_stop(Pix p, bool constrained = false);
    int dimension_stride(Pix p, bool constrained = false);
    String dimension_name(Pix p);

    unsigned int dimensions(bool constrained = false);

    virtual void print_decl(ostream &os, String space = "    ",
			    bool print_semi = true,
			    bool constraint_info = false,
			    bool constrained = false);

    virtual void print_val(ostream &os, String space = "", 
			   bool print_decl_p = true);

    virtual bool check_semantics(bool all = false);
};

#endif


