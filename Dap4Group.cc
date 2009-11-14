/*
 * Dap4Group.cc
 *
 *  Created on: Nov 12, 2009
 *      Author: jimg
 */

#include "config.h"

#include "Dap4Group.h"

using namespace std;

namespace libdap {

void
Dap4Group::m_clone(const Dap4Group &g)
{
    d_dimensions = g.d_dimensions;
}

Dap4Group::Dap4Group(const Dap4Group &rhs) : Constructor(rhs) {
    m_clone(rhs);
}

Dap4Group::~Dap4Group() {
    // Not much yet
}

Dap4Group &Dap4Group::operator=(const Dap4Group &rhs) {
    if (this == &rhs)
        return *this;

    dynamic_cast<Dap4Group &>(*this) = rhs;

    return *this;
}

BaseType *Dap4Group::ptr_duplicate() {
    return new Dap4Group(*this);
}

// replace with something const.
int Dap4Group::size(const string &name) const {
    DimensionCIter pos = d_dimensions.find(name);
    if (pos != d_dimensions.end()) {
	return pos->second;
    }
    else
	throw Error("Unknown dimension.");
}

void Dap4Group::add_dimension(const string &name, int size) {
    DimensionIter pos = d_dimensions.find(name);
    if (pos == d_dimensions.end())
	d_dimensions.insert(std::make_pair(name, size));
    else
	throw Error("Attempt to add conflicting dimension name.");
}

int Dap4Group::delete_dimension(DimensionIter pos) {
    if (pos != d_dimensions.end()) {
	int size = pos->second;
	d_dimensions.erase(pos);
	return size;
    }
    else
	throw Error("Attempt to delete a non-existent dimension.");
}

int Dap4Group::delete_dimension(const string &name) {
    DimensionIter pos = d_dimensions.find(name);
    return delete_dimension(pos);
}

}
