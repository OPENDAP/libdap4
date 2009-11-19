/*
 * Dap4SharedDimension.h
 *
 *  Created on: Nov 10, 2009
 *      Author: jimg
 */
#include "config.h"

#ifndef _dap4_shared_dimension_h
#define _dap4_shared_dimension_h

#include <string>
#include <libxml/xmlwriter.h>

#ifndef A_DapObj_h
#include "DapObj.h"
#endif

namespace libdap {

class Dap4SharedDimension : public DapObj {
private:
    int d_size;
    string d_name;

public:
    Dap4SharedDimension() {}
    Dap4SharedDimension(int size, const string &name)
	: d_size(size),  d_name(name) {}
    Dap4SharedDimension(const Dap4SharedDimension &rhs)
	: DapObj(), d_size(rhs.d_size), d_name(rhs.d_name) {}

    virtual ~Dap4SharedDimension() {}

    int get_size() {return d_size;}
    void set_size(int size) {d_size = size;}

    string get_name() { return d_name; }
    void set_name(const string &name) { d_name = name; }

    virtual void print_xml(ostream &out, string space = "    ",
            bool constrained = false);
    virtual void print_xml(xmlTextWriterPtr writer, bool constrained = false);

    virtual void dump(ostream &strm) const;
};

}
#endif /* _dap4_shared_dimension_h */
