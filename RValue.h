
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1998-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

#ifndef _rvalue_h
#define _rvalue_h

/** Holds the rvalues for the parser, Clause objects and evaluator.

    @see Clause.h
    @see DDS.h
    @see expr.y */

class rvalue {
private:
    BaseType *value;
    btp_func func;		// pointer to a function returning BaseType *
    SLList<rvalue *> *args;		// arguments to the function

public:
  rvalue(BaseType *bt);
  rvalue(btp_func f, SLList<rvalue *> *a);
  rvalue();

  virtual ~rvalue();

  string value_name();

  BaseType *bvalue(const string &dataset, DDS &dds);
};

// This type def must come after the class definition above. It is used in
// the Clause and DDS classes.
typedef SLList<rvalue *> rvalue_list;

BaseType **build_btp_args(rvalue_list *args, DDS &dds);

// $Log: RValue.h,v $
// Revision 1.7  2002/06/18 15:36:24  tom
// Moved comments and edited to accommodate doxygen documentation-generator.
//
// Revision 1.6  2000/09/22 02:17:21  jimg
// Rearranged source files so that the CVS logs appear at the end rather than
// the start. Also made the ifdef guard symbols use the same naming scheme and
// wrapped headers included in other headers in those guard symbols (to cut
// down on extraneous file processing - See Lakos).
//
// Revision 1.5  2000/08/02 22:46:49  jimg
// Merged 3.1.8
//
// Revision 1.4.6.1  2000/08/01 21:09:36  jimg
// Destructor is now virtual
//
// Revision 1.4  1999/05/04 19:47:22  jimg
// Fixed copyright statements. Removed more of the GNU classes.
//
// Revision 1.3  1999/04/29 02:29:31  jimg
// Merge of no-gnu branch
//
// Revision 1.2  1999/01/21 20:42:01  tom
// Fixed comment formatting problems for doc++
//
// Revision 1.1  1998/10/21 16:14:17  jimg
// Added. Based on code that used to be in expr.h/cc

#endif // _rvalue_h
