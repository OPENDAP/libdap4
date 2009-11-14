/*
 * Dap4Group.h
 *
 *  Created on: Nov 10, 2009
 *      Author: jimg
 */

#ifndef DAP4GROUP_H_
#define DAP4GROUP_H_

#include <string>
#include <list>
#include <map>

#ifndef _constructor_h
#include "Constructor.h"
#endif

using namespace std;

namespace libdap {

class Dap4Group : public Constructor {
public:
    typedef map<string, int> Dimension;
    typedef Dimension::iterator DimensionIter;
    typedef Dimension::const_iterator DimensionCIter;

private:
    Dimension d_dimensions;

    void m_clone(const Dap4Group &rhs);

public:
    Dap4Group() : Constructor("default", dap4_group_c) {}
    Dap4Group(const string &name) : Constructor(name, dap4_group_c) {}
    Dap4Group(const string &name, const string &dataset)
	: Constructor(name, dataset, dap4_group_c) {}

    Dap4Group(const Dap4Group &rhs);
    virtual ~Dap4Group();

    Dap4Group &operator=(const Dap4Group &rhs);
    virtual BaseType *ptr_duplicate();

    // The standard compliment of accessors & mutators & iterators
    DimensionCIter begin() const { return d_dimensions.begin(); }
    DimensionCIter end() const { return d_dimensions.end(); }
    string name(DimensionIter d) { return d->first; }
    int size(DimensionIter d) const { return d->second; }

    int size(const string &name) const;

    void add_dimension(const string &name, int size);
    int delete_dimension(DimensionIter pos);

    int delete_dimension(const string &name);

    /** Methods declared abstract in BaseType and not defined by Constructor */
    // temporary definitions; many of these shoulf be defined in Constructor
    virtual unsigned int width() { return 0; }

    virtual unsigned int buf2val(void **val) { throw InternalErr(__FILE__,
	    __LINE__, "buf2val unimplemented");
    }
    virtual unsigned int val2buf(void *val, bool reuse = false) {
	throw InternalErr(__FILE__, __LINE__, "val2buf unimplemented");
    }

    virtual bool serialize(ConstraintEvaluator &eval, DDS &dds,
    			   Marshaller &m, bool ce_eval = true) {
	throw InternalErr(__FILE__, __LINE__, "serialize unimplemented");
    }

    virtual bool deserialize(UnMarshaller &um, DDS *dds, bool reuse = false) {
	throw InternalErr(__FILE__, __LINE__, "deserialize unimplemented");
    }

    virtual void print_val(FILE *out, string space = "",
                               bool print_decl_p = true) {
	throw InternalErr(__FILE__,   __LINE__, "print_val unimplemented");
    }

    virtual void print_val(ostream &out, string space = "",
                           bool print_decl_p = true) {
	throw InternalErr(__FILE__,    __LINE__, "print_val unimplemented");
    }

};

}
#endif /* DAP4GROUP_H_ */
