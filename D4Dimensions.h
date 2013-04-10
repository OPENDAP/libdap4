/*
 * D4Dimensions.h
 *
 *  Created on: Sep 26, 2012
 *      Author: jimg
 */

#ifndef D4DIMENSIONS_H_
#define D4DIMENSIONS_H_

#include <string>
#include <vector>

using namespace std;

namespace libdap {

/**
 * This class holds information about dimensions. This can be used to store
 * actual dimension information in an instance of BaseType and it can be
 * used to store the definition of a dimension in an instance of Group.
 *
 *
 * @todo What about storing constraint information too? Maybe we do need
 * two classes - one for defs and one for 'refs'
 */
class D4Dimensions {
    struct dimension {
        string name;
        unsigned long size;

        dimension(const string &n, const unsigned long s) :
            name(n), size(s) {}
    };

    vector<dimension> d_dims;

public:
    D4Dimensions();
    virtual ~D4Dimensions();

    typedef vector<dimension>::iterator D4DimensionsIter;

    void add_dim(const string &name, const unsigned long size) {
        d_dims.push_back(dimension(name, size));
    }
    void add_dim(const string &name) {
        d_dims.push_back(dimension(name, 0));
    }
    void add_dim(const unsigned long size) {
        d_dims.push_back(dimension("", size));
    }

    string get_dim_name(int i) { return d_dims.at(i).name; }
    unsigned long get_dim_size(int i) { return d_dims.at(i).size; }

    D4DimensionsIter maps_begin() { return d_dims.begin(); }
    D4DimensionsIter maps_end() { return d_dims.end(); }
    string get_dim_name(D4DimensionsIter i) { return (*i).name; }
    unsigned long get_dim_size(D4DimensionsIter i) { return (*i).size; }
};

} /* namespace libdap */
#endif /* D4DIMENSIONS_H_ */
