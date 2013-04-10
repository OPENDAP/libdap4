/*
 * D4Group.h
 *
 *  Created on: Sep 27, 2012
 *      Author: jimg
 */

#ifndef D4GROUP_H_
#define D4GROUP_H_

#include "Constructor.h"
#include "D4Dimensions.h"
#include "D4EnumDef.h"

namespace libdap {

class D4Group :public Constructor {
private:
    // I use the same object to define the dimensions (here) and to
    // include them in the individual variables (in BaseType.h)
    D4Dimensions d_dims;

    // The Group object is a container for both variables and enumeration
    // definitions.
    vector<D4EnumDef*> d_enums;

    // TODO Must define m_duplicate so that d_enums are copied.
    // Must be a deep copy
public:
    D4Group(const string &n);
    D4Group(const string &n, const string &d);

    D4Group(const D4Group &rhs);
    virtual ~D4Group();

    D4Group &operator=(const D4Group &rhs);
    virtual BaseType *ptr_duplicate();

    void print_xml_writer(XMLWriter &xml, bool constrained);

    void add_enumeration_nocopy(D4EnumDef *enum_def);
};

} /* namespace libdap */
#endif /* D4GROUP_H_ */
