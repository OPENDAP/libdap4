/*
 * Dap4Group.h
 *
 *  Created on: Nov 10, 2009
 *      Author: jimg
 */

#ifndef dap4_group_h
#define dap4_group_h

#include <string>
#include <list>

#ifndef _structure_h
#include "Structure.h"
#endif

#ifndef _dap4_shared_dimension_h
#include "Dap4SharedDimension.h"
#endif

using namespace std;

namespace libdap {

class Dap4Group : public Structure {

private:
    typedef list<Dap4SharedDimension*> SharedDims;
    SharedDims d_shared_dims;

    void m_clone(const Dap4Group &rhs);

public:
    typedef SharedDims::iterator SharedDimsIter;
    typedef SharedDims::const_iterator SharedDimsCIter;

    Dap4Group() : Structure("default", dap4_group_c) {}
    Dap4Group(const string &name) : Structure(name, dap4_group_c) {}
    Dap4Group(const string &name, const string &dataset)
	: Structure(name, dataset, dap4_group_c) {}

    Dap4Group(const Dap4Group &rhs);
    virtual ~Dap4Group();

    Dap4Group &operator=(const Dap4Group &rhs);
    virtual BaseType *ptr_duplicate();

    // The standard compliment of accessors & mutators & iterators
    SharedDimsCIter dim_cbegin() const { return d_shared_dims.begin(); }
    SharedDimsCIter dim_cend() const { return d_shared_dims.end(); }

    int dim_num() const { return d_shared_dims.size(); }
    string dim_name(SharedDimsCIter d) const { return (*d)->get_name(); }
    int dim_size(SharedDimsCIter d) const { return (*d)->get_size(); }
    int dim_size(const string &name) const;

    // non-const iterators
    SharedDimsIter dim_begin() { return d_shared_dims.begin(); }
    SharedDimsIter dim_end() { return d_shared_dims.end(); }

    void add_dimension(const string &name, int size);
    void add_dimension(SharedDimsIter d);
    void insert_dimension(SharedDimsIter pos, const string &name, int size);
    void insert_dimension(SharedDimsIter pos, SharedDimsIter d);

    int delete_dimension(SharedDimsIter pos);
    int delete_dimension(const string &name);

    virtual unsigned int buf2val(void **) { throw InternalErr(__FILE__,
	    __LINE__, "buf2val: Undefined for Dap4Groups");
    }
    virtual unsigned int val2buf(void *, bool) { throw InternalErr(__FILE__,
	    __LINE__, "val2buf: Undefined for Dap4Groups");
    }

    // This can be removed if we provide a decent set of read() methods for the
    // constructor types. See NCGrid.cc for an example of 'decent'. jhrg
    virtual bool serialize(ConstraintEvaluator &eval, DDS &dds,
    			   Marshaller &m, bool ce_eval = true);

    virtual void print_xml(ostream &out, string space = "    ",
	    bool constrained = false);
    virtual void print_xml(xmlTextWriterPtr writer, bool constrained = false);

    virtual void dump(ostream &strm) const;
};

}
#endif /* dap4_group_h */
