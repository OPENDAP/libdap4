
// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Implementation for the class Structure
//
// jhrg 9/14/94

#ifdef _GNUG_
#pragma implementation
#endif

#include "config_dap.h"

#include <assert.h>

#include "debug.h"
#include "Error.h"
#include "InternalErr.h"
#include "Sequence.h"
#include "DDS.h"
#include "DataDDS.h"
#include "util.h"
#include "InternalErr.h"

#ifdef TRACE_NEW
#include "trace_new.h"
#endif

#ifdef WIN32
using std::endl;
#endif

// Jose Garcia 1/28/2000
// Note: all asserts of nature
// for (Pix p = _vars.first(); p; _vars.next(p)) {
//      assert(_vars(p));
// had been commented out, later when we get sure
// we do not need then we can remove them all.

static unsigned char end_of_sequence = 0xA5; // binary pattern 1010 0101
static unsigned char start_of_instance = 0x5A; // binary pattern 0101 1010

// Private member functions

void
Sequence::_duplicate(const Sequence &s)
{
    BaseType::_duplicate(s);

    Sequence &cs = (Sequence &)s; // cast away const
    
    cs._seq_read_error = s._seq_read_error;
    cs._seq_write_error = s._seq_write_error;
    cs._level = s._level;

    for (Pix p = cs.first_var(); p; cs.next_var(p))
	add_var(cs.var(p)->ptr_duplicate());
}

// Protected member functions

void
Sequence::write_end_of_sequence(XDR *sink)
{
    xdr_opaque(sink, (char *)&end_of_sequence, 1);
}

void
Sequence::write_start_of_instance(XDR *sink)
{
    xdr_opaque(sink, (char *)&start_of_instance, 1);
}

unsigned char
Sequence::read_marker(XDR *source)
{
    unsigned char marker;
    xdr_opaque(source, (char *)&marker, 1);

    return marker;
}

bool
Sequence::is_start_of_instance(unsigned char marker)
{
    return (marker == start_of_instance);
}

bool
Sequence::is_end_of_sequence(unsigned char marker)
{
    return (marker == end_of_sequence);
}

// Public member functions

Sequence::Sequence(const string &n) 
    : BaseType(n, dods_sequence_c), _level(0), _seq_read_error(false),
      _seq_write_error(false), d_row_number(-1), d_starting_row_number(-1),
      d_row_stride(1), d_ending_row_number(-1)
{
}

Sequence::Sequence(const Sequence &rhs)
{
    _duplicate(rhs);
}

Sequence::~Sequence()
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	delete _vars(p);
}

const Sequence &
Sequence::operator=(const Sequence &rhs)
{
    if (this == &rhs)
	return *this;

    _duplicate(rhs);

    return *this;
}

int
Sequence::element_count(bool leaves)
{
    if (!leaves)
	return _vars.length();
    else {
	int i = 0;
	for (Pix p = first_var(); p; next_var(p))
	    i += var(p)->element_count(leaves);
	return i;
    }
}

void
Sequence::set_send_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_send_p(state);

    BaseType::set_send_p(state);
}

void
Sequence::set_read_p(bool state)
{
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->set_read_p(state);

    BaseType::set_read_p(state);
}

// NB: Part p defaults to nil for this class

void 
Sequence::add_var(BaseType *bt, Part)
{
   if(!bt)
       throw InternalErr(__FILE__, __LINE__, 
			 "Cannot add variable: NULL pointer");
   // Jose Garcia
   // We append a copy of bt so the owner
   // of bt is free to deallocate as he wishes.
    _vars.append(bt->ptr_duplicate());

    if (bt->type() == dods_sequence_c) {
	Sequence *s = dynamic_cast<Sequence *>(bt);
	if (s)
	    s->set_level(level() + 1);
    }
}

BaseType *
Sequence::var(const string &name, btp_stack &s)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
      //assert(_vars(p));
	
	if (_vars(p)->name() == name) {
	    s.push((BaseType *)this);
	    return _vars(p);
	}

        if (_vars(p)->is_constructor_type()) {
	    BaseType *btp = _vars(p)->var(name, s);
	    if (btp) {
		s.push((BaseType *)this);
		return btp;
	    }
	}
    }

    return 0;
}

BaseType *
Sequence::var(const string &name, bool exact)
{
    if (exact)
	return exact_match(name);
    else
	return leaf_match(name);
}

BaseType *
Sequence::leaf_match(const string &name)
{
    for (Pix p = _vars.first(); p; _vars.next(p)) {
      //assert(_vars(p));
	
	if (_vars(p)->name() == name)
	    return _vars(p);
        if (_vars(p)->is_constructor_type()) {
	    BaseType *btp = _vars(p)->var(name, false);
	    if (btp)
		return btp;
	}
    }

    return 0;
}

BaseType *
Sequence::exact_match(const string &name)
{
    string::size_type dot_pos = name.find("."); // zero-based index of `.'
    if (dot_pos != string::npos) {
	string aggregate = name.substr(0, dot_pos);
	string field = name.substr(dot_pos + 1);

	BaseType *agg_ptr = var(aggregate);
	if (agg_ptr)
	    return agg_ptr->var(field);	// recurse
	else
	    return 0;		// qualified names must be *fully* qualified
    }
    else {
	for (Pix p = _vars.first(); p; _vars.next(p)) {
	  //assert(_vars(p));
	    if (_vars(p)->name() == name)
		return _vars(p);
	}
    }

    return 0;
}

Pix
Sequence::first_var()
{
    if (_vars.empty())
	return 0;
    else
	return _vars.first();
}

void
Sequence::next_var(Pix &p)
{
    if (!_vars.empty() && p)
	_vars.next(p);
}

BaseType *
Sequence::var(Pix p)
{
    if (!_vars.empty() && p)
	return _vars(p);
    else 
	return NULL;
}

unsigned int
Sequence::width()
{
    unsigned int sz = 0;

    for( Pix p = first_var(); p; next_var(p))
	sz += var(p)->width();

    return sz;
}

// This version returns -1. Each API-specific subclass should define a more
// reasonable version. jhrg 5/24/96

int
Sequence::length()
{
    return -1;
}

void
Sequence::set_level(int lvl)
{
    _level = lvl;
}

int
Sequence::level()
{
    return _level;
}

// Advance the sequence to row number ROW. Note that we can only advance, it
// is not possible to backup (yet, that could be implemented).
bool
Sequence::get_row(int row, const string &dataset, DDS &dds, bool ce_eval)
{
  if (row < d_row_number)
    throw InternalErr("Trying to back up inside a sequence!");

  if (row == d_row_number)
    return true;

  // Start out assuming EOF is false.
  int eof = 0;
  while(!eof && d_row_number < row) {
    // Assume that read() is implemented so that, when reading data for a
    // nested sequence, only the outer most level is *actually* read.
    // This is a consequence of our current (12/7/99) implementation of
    // the JGOFS server (which is the only server to actually use nested
    // sequences). 12/7/99 jhrg
    if (!read_p()) {
      if (level() != 0)	// Read only the outermost level.
	return true;
      // The read() function returns a boolean value, with TRUE indicating
      // that read() should be called again because there's more data to
      // read, and FALSE indicating there's no more data to read. Note that
      // this behavior is necessary to properly handle variables that contain
      // Sequences. 
      // Jose Garcia
      // If an error exists while reading, the implementers of
      // the surrogate library SHOULD throw an Error object 
      // which will propagate beyond this point to to the original 
      // caller. 
      eof = read(dataset) == false;
      // Jose Garcia
      // Now false is returned only to indicate EOF, for errors an
      // Error object should be throw as an exception!!! I know I said
      // the same this 3 times in this method, it is just that this is
      // critical.
    }

    // If the ce selection evals to false, read the next row. If true, break
    // out of the loop. Otherwise only advance the row number if ce_eval is
    // false (we're not supposed to evaluate the selection) or both ce_eval
    // and the selection are true.
    if (!ce_eval || ce_eval && dds.eval_selection(dataset))
      d_row_number++;

    set_read_p(false);	// ...so that the next instance will be read
  }

  // Once we finish te above loop, set read_p to true so that the caller
  // knows that data *has* been read. This is how the read() methods of the
  // elements of the sequence know to not look for data.
  set_read_p(true);

  // Return true if we have valid data, false if we've read to the EOF.
  return eof == 0;
}

// Private
inline bool
Sequence::is_end_of_rows(int i)
{
  return ((d_ending_row_number != -1) ? (d_ending_row_number >= i):true);
}

bool
Sequence::serialize(const string &dataset, DDS &dds, XDR *sink, bool ce_eval)
{
    int error = 0;
    int i = (d_starting_row_number != -1) ? d_starting_row_number : 0;
    
    bool status = get_row(i, dataset, dds, ce_eval);
    while (status && is_end_of_rows(i)) {
      i += d_row_stride;

      if (level() == 0) {
	DBG(cerr << "Writing Start of Instance marker" << endl);
	write_start_of_instance(sink);
      }

      for (Pix p = first_var(); p; next_var(p)) {
	if (var(p)->send_p() 
	    && !var(p)->serialize(dataset, dds, sink, false)) {
	  error = 1;
	  status = false;	// Exit outer while-loop.
	  break;		// Exit inner for-loop
	}
      }

      set_read_p(false);	// ...so this will read the next instance
      status = get_row(i, dataset, dds, ce_eval);
    }

    if (!error && level() == 0) {
      DBG(cerr << "Writing End of Sequence marker" << endl);
      write_end_of_sequence(sink);
    }

    return !error;		// Return true if no error.
}

bool
Sequence::deserialize(XDR *source, DDS *dds, bool reuse)
{
    bool stat = true;
    DataDDS *dd = (DataDDS *)dds; // Use a dynamic cast in the future.

    DBG2(cerr << "Reading from server version: " << dd->get_version_major() \
	 << "." << dd->get_version_minor() << endl);

    // Check for old servers.
    if (dd->get_version_major() < 2 
	|| dd->get_version_major() == 2 && dd->get_version_minor() < 15) {
	return old_deserialize(source, dd, reuse);
    }

    if (level() == 0) {
	unsigned char marker = read_marker(source);

	if (is_start_of_instance(marker))
	    ;			// Read more sequence elements
	else if (is_end_of_sequence(marker))
	    return false;	// No more sequence elements
	else {
	    _seq_read_error = true; // Error, don't read more elements
	    return false;
	}
    }

    for (Pix p = first_var(); p; next_var(p)) {
	stat = var(p)->deserialize(source, dds, reuse);
	if (!stat) 
	    return stat;
    }

    d_row_number++;

    return stat;
}

// Return the current row number.

int
Sequence::get_row_number()
{
  return d_row_number;
}

int
Sequence::get_starting_row_number()
{
  return d_starting_row_number;
}

int
Sequence::get_row_stride()
{
  return d_row_stride;
}

int
Sequence::get_ending_row_number()
{
  return d_ending_row_number;
}

// stride defaults to 1.
void
Sequence::set_row_number_constraint(int start, int stop, int stride)
{
  if (stop < start)
    throw Error(malformed_expr, "Starting row number must precede the ending row number.");

  d_starting_row_number = start;
  d_row_stride = stride;
  d_ending_row_number = stop;
}

// private mfunc. Use this to read from older servers.

bool
Sequence::old_deserialize(XDR *source, DDS *dds, bool reuse)
{
    bool stat = true;

    DBG2(cerr << "Entering old_deserialize()" << endl);

    for (Pix p = first_var(); p; next_var(p)) {
	stat = var(p)->deserialize(source, dds, reuse);
	if (!stat) 
	    return false;
    }

    d_row_number++;

    return stat;
}

bool
Sequence::seq_read_error()
{
    return _seq_read_error;
}

unsigned int
Sequence::val2buf(void *, bool)
{
    return sizeof(Sequence);
}

unsigned int
Sequence::buf2val(void **)
{
    return sizeof(Sequence);
}

void
Sequence::print_decl(ostream &os, string space, bool print_semi,
		     bool constraint_info, bool constrained)
{
    if (constrained && !send_p())
	return;

    os << space << type_name() << " {" << endl;
    for (Pix p = _vars.first(); p; _vars.next(p))
	_vars(p)->print_decl(os, space + "    ", true, constraint_info,
			     constrained);
    os << space << "} " << name();

    if (constraint_info) {
	if (send_p())
	    cout << ": Send True";
	else
	    cout << ": Send False";
    }

    if (print_semi)
	os << ";" << endl;
}

void 
Sequence::print_val(ostream &os, string space, bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os, space, false);
	os << " = ";
    }

    os << "{ ";
    for (Pix p = _vars.first(); p; _vars.next(p), (void)(p && os << ", "))
	_vars(p)->print_val(os, "", false);

    os << " }";

    if (print_decl_p)
	os << ";" << endl;
}

// print_all_vals is from Todd Karakasian. 
// We need to integrate this into print_val somehow, maybe by adding an XDR *
// to Sequence? This can wait since print_val is mostly used for debugging...

void
Sequence::print_all_vals(ostream& os, XDR *src, DDS *dds, string space,
			 bool print_decl_p)
{
    if (print_decl_p) {
	print_decl(os);
	os << " = ";
    }
    os << "{ ";
    deserialize(src, dds);
    print_val(os, space, false);
    while (deserialize(src, dds)) {
	os << ", ";
	print_val(os, space, false);
    }
    if (print_decl_p)
        os << "};" << endl;
}

bool
Sequence::check_semantics(string &msg, bool all)
{
    if (!BaseType::check_semantics(msg))
	return false;

    if (!unique_names(_vars, name(), type_name(), msg))
	return false;

    if (all) 
	for (Pix p = _vars.first(); p; _vars.next(p))
	    if (!_vars(p)->check_semantics(msg, true))
		return false;

    return true;
}

// $Log: Sequence.cc,v $
// Revision 1.58  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.57  2000/09/21 16:22:08  jimg
// Merged changes from Jose Garcia that add exceptions to the software.
// Many methods that returned error codes now throw exectptions. There are
// two classes which are thrown by the software, Error and InternalErr.
// InternalErr is used to report errors within the library or errors using
// the library. Error is used to reprot all other errors. Since InternalErr
// is a subclass of Error, programs need only to catch Error.
//
// Revision 1.56  2000/09/11 16:31:48  jimg
// Added methods to make it simpler to access Sequences by row number. The new
// methods are: get_row(), get_row_number(), get_starting_row_number(),
// get_ending_row_number(), get_row_stride(), set_row_number_constraint(). The
// starting and ending row numbers refer to constraints placed on the sequence,
// to get the number of rows in the current sequence, use the length() method.
//
// Revision 1.55  2000/08/16 00:38:22  jimg
// Added the getRowNumber method.
//
// Revision 1.54  2000/07/09 22:05:36  rmorris
// Changes to increase portability, minimize ifdef's for win32 and account
// for differences in the iostreams implementations.
//
// Revision 1.53  2000/06/16 18:15:00  jimg
// Merged with 3.1.7
//
// Revision 1.49.6.3  2000/06/07 23:05:30  jimg
// The first_*() methods return 0 if there are no variables
//
// Revision 1.51.2.2  2000/02/17 05:03:13  jimg
// Added file and line number information to calls to InternalErr.
// Resolved compile-time problems with read due to a change in its
// parameter list given that errors are now reported using exceptions.
//
// Revision 1.51.2.1  2000/01/28 22:14:05  jgarcia
// Added exception handling and modify add_var to get a copy of the object
//
// Revision 1.52  2000/01/27 06:29:57  jimg
// Resolved conflicts from merge with release-3-1-4
//
// Revision 1.49.6.2  2000/01/26 23:57:15  jimg
// Fixed the return type of string::find.
//
// Revision 1.51  1999/12/31 00:55:11  jimg
// Fixed up the progress indicator
//
// Revision 1.50  1999/09/03 22:07:44  jimg
// Merged changes from release-3-1-1
//
// Revision 1.49.6.1  1999/08/28 06:43:04  jimg
// Fixed the implementation/interface pragmas and misc comments
//
// Revision 1.49  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.48  1999/04/22 22:28:26  jimg
// Uses dynamic_cast
//
// Revision 1.47  1998/09/17 17:18:39  jimg
// Changes for the new variable lookup scheme. Fields of ctor types no longer
// need to be fully qualified. my.thing.f1 can now be named `f1' in a CE. Note
// that if there are two `f1's in a dataset, the first will be silently used;
// There's no warning about the situation. The new code in the var member
// function passes a stack of BaseType pointers so that the projection
// information (send_p field) can be set properly.
//
// Revision 1.46.6.1  1999/02/02 21:57:01  jimg
// String to string version
//
// Revision 1.46  1998/04/03 17:41:42  jimg
// Patch from Jake Hamby to print_all_vals(). deserialize needed to be called
// before the first call to print_val.
//
// Revision 1.45  1998/03/26 00:20:04  jimg
// Removed DODS_DEBUG define.
//
// Revision 1.44  1998/03/17 17:39:24  jimg
// Added an implementation of element_count().
//
// Revision 1.43  1998/02/19 19:41:27  jimg
// Changed name of ...end_of_sequence to ...start_of_sequence since that is
// now how it is used. I hope this will reduce confusion.
// Changed the name of read_end_marker to read_marker (since they are not
// always end markers anymore).
//
// Revision 1.42  1998/02/18 01:00:48  jimg
// Reverted to the old transfer scheme for Sequences.
// Added EOI/EOS markers.
// The code supports (via version numbers in the header of the return
// documents) reading from the old-style servers (which do not have EOI/EOS
// markers.
//
// Revision 1.41  1998/02/05 20:13:55  jimg
// DODS now compiles with gcc 2.8.x
//
// Revision 1.40  1997/12/31 20:55:15  jimg
// Changed name of read_level() to level() to reduce confusion in child
// classes. Also changed return type from unsigned int to int. This allows
// callers to store flags, etc. in the _level field - not the best thing, but
// a realistic expectation
//
// Revision 1.39  1997/12/16 01:36:18  jimg
// Merged changes from release-2.14d.
//
// Revision 1.38  1997/12/16 00:41:22  jimg
// Added debugging code to help finder problems reading sequences from older
// servers.
//
// Revision 1.37  1997/09/22 22:48:17  jimg
// Added DDS * to deserialize parameters.
// Added End of instance and end of sequence marker constants.
// Change serialize() and deserialize() mfuncs so that the new markers are
// used and embedded sequences are sent without needlessly replicating the
// enclosing sequence's instance.
//
// Revision 1.36  1997/07/15 21:53:25  jimg
// Changed length member function to return -1 instead of 0. Thus the default,
// when a subclass does not define its own version of this member function, is
// to return -1. This value will be easy to differentiate from 0, which could
// mean that the sequence has zero elements.
//
// Revision 1.35  1997/03/08 19:02:05  jimg
// Changed default param to check_semantics() from  to String()
// and removed the default from the argument list in the mfunc definition
//
// Revision 1.34  1997/02/28 01:27:57  jimg
// Changed check_semantics() so that it now returns error messages in a String
// object (passed by reference).
//
// Revision 1.33  1997/02/10 02:32:42  jimg
// Added assert statements for pointers
//
// Revision 1.32  1996/10/08 17:07:59  jimg
// Fixed deserialize so that when called on a null Sequence it will return
// true.
//
// Revision 1.31  1996/09/16 18:09:16  jimg
// Fixed var(const String name) so that it would correctly descend names of the
// form <base>.<name> where <name> may itself contain `dots'.
//
// Revision 1.30  1996/08/26 21:12:59  jimg
// Changes for version 2.07
//
// Revision 1.29  1996/08/13 18:36:12  jimg
// Added void casts to values computed in print_val() for loops.
//
// Revision 1.28  1996/06/04 21:33:37  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.27  1996/05/31 23:29:58  jimg
// Updated copyright notice.
//
// Revision 1.26  1996/05/29 22:08:46  jimg
// Made changes necessary to support CEs that return the value of a function
// instead of the value of a variable. This was done so that it would be
// possible to translate Sequences into Arrays without first reading the
// entire sequence over the network.
//
// Revision 1.25  1996/05/22 18:05:15  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.24  1996/05/16 22:44:52  jimg
// Dan's changes for 2.0.
//
// Revision 1.23  1996/05/14 15:38:35  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.22  1996/04/05 00:21:38  jimg
// Compiled with g++ -Wall and fixed various warnings.
//
// Revision 1.21  1996/04/04 18:10:46  jimg
// Merged changes from version 1.1.1.
// Fixed a bug in serialize() which caused the sequence serialization to end
// when the current CE first evaluated to false
//
// Revision 1.20  1996/03/05 17:44:21  jimg
// Added ce_eval to serailize member function.
//
// Revision 1.19  1996/02/02 00:31:12  jimg
// Merge changes for DODS-1.1.0 into DODS-2.x
//
// Revision 1.18  1995/12/09  01:06:54  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.17  1995/12/06  21:56:29  jimg
// Added `constrained' flag to print_decl.
// Removed third parameter of read.
// Modified print_decl() to print only those parts of a dataset that are
// selected when `constrained' is true.
//
// Revision 1.16  1995/10/23  23:21:01  jimg
// Added _send_p and _read_p fields (and their accessors) along with the
// virtual mfuncs set_send_p() and set_read_p().
//
// Revision 1.15  1995/08/26  00:31:43  jimg
// Removed code enclosed in #ifdef NEVER #endif.
//
// Revision 1.14  1995/08/23  00:11:06  jimg
// Changed old, deprecated member functions to new ones.
// Switched from String representation of type to enum.
//
// Revision 1.13.2.1  1995/09/14 20:58:15  jimg
// Moved some loop index variables out of the loop statement.
//
// Revision 1.13  1995/07/09  21:29:03  jimg
// Added copyright notice.
//
// Revision 1.12  1995/05/10  15:34:03  jimg
// Failed to change `config.h' to `config_dap.h' in these files.
//
// Revision 1.11  1995/05/10  13:45:27  jimg
// Changed the name of the configuration header file from `config.h' to
// `config_dap.h' so that other libraries could have header files which were
// installed in the DODS include directory without overwriting this one. Each
// config header should follow the convention config_<name>.h.
//
// Revision 1.10  1995/03/16  17:29:11  jimg
// Added include config_dap.h to top of include list.
// Added TRACE_NEW switched dbnew includes.
// Fixed bug in read_val() where **val was passed incorrectly to
// subordinate read_val() calls.
//
// Revision 1.9  1995/03/04  14:34:49  jimg
// Major modifications to the transmission and representation of values:
// Added card() virtual function which is true for classes that
// contain cardinal types (byte, int float, string).
// Changed the representation of Str from the C rep to a C++
// class represenation.
// Chnaged read_val and store_val so that they take and return
// types that are stored by the object (e.g., inthe case of Str
// an URL, read_val returns a C++ String object).
// Modified Array representations so that arrays of card()
// objects are just that - no more storing strings, ... as
// C would store them.
// Arrays of non cardinal types are arrays of the DODS objects (e.g.,
// an array of a structure is represented as an array of Structure
// objects).
//
// Revision 1.8  1995/02/10  02:23:02  jimg
// Added DBMALLOC includes and switch to code which uses malloc/free.
// Private and protected symbols now start with `_'.
// Added new accessors for name and type fields of BaseType; the old ones
// will be removed in a future release.
// Added the store_val() mfunc. It stores the given value in the object's
// internal buffer.
// Made both List and Str handle their values via pointers to memory.
// Fixed read_val().
// Made serialize/deserialize handle all malloc/free calls (even in those
// cases where xdr initiates the allocation).
// Fixed print_val().
//
// Revision 1.7  1995/01/19  20:05:26  jimg
// ptr_duplicate() mfunc is now abstract virtual.
// Array, ... Grid duplicate mfuncs were modified to take pointers, not
// referenves.
//
// Revision 1.6  1995/01/11  15:54:53  jimg
// Added modifications necessary for BaseType's static XDR pointers. This
// was mostly a name change from xdrin/out to _xdrin/out.
// Removed the two FILE pointers from ctors, since those are now set with
// functions which are friends of BaseType.
//
// Revision 1.5  1994/12/15  21:21:13  dan
// Modified Sequence class to directly inherit from class BaseType
// Modified constructors to reflect new inheritance.
//
// Revision 1.4  1994/12/08  15:55:58  dan
// Added data transmission member functions serialize() and deserialize()
// Modified size() member function to return cumulative size of all members.
// Fixed the duplicate() member function to correctly access the data members
// of the sequence being duplicated.
//
// Revision 1.3  1994/10/17  23:34:49  jimg
// Added code to print_decl so that variable declarations are pretty
// printed.
// Added private mfunc duplicate().
// Added ptr_duplicate().
// Added Copy ctor, dtor and operator=.
//
// Revision 1.2  1994/09/23  14:48:31  jimg
// Fixed some errors in comments.

