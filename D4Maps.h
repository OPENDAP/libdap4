/*
 * D4Maps.h
 *
 *  Created on: Sep 26, 2012
 *      Author: jimg
 */

#ifndef D4MAPS_H_
#define D4MAPS_H_

#include <string>
#include <vector>

using namespace std;

namespace libdap {

/**
 * Maps in DAP4 are simply the names of Dimensions. When a dimensioned
 * variable (i.e., an array) also has 'maps' that array is a 'grid' in the
 * sense that the 'maps' define the domain of a sampled function.
 *
 * @todo Is this class needed?
 */
class D4Maps {
private:
    vector<string> d_names;

public:
    D4Maps();
    virtual ~D4Maps();

    typedef vector<string>::iterator D4MapsIter;

    void add_map(const string &map) { d_names.push_back(map); }
    string get_map(int i) { return d_names.at(i); }

    D4MapsIter maps_begin() { return d_names.begin(); }
    D4MapsIter maps_end() { return d_names.end(); }
};

} /* namespace libdap */
#endif /* D4MAPS_H_ */
