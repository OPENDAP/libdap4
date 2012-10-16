/*
 * D4EnumDef.h
 *
 *  Created on: Oct 9, 2012
 *      Author: jimg
 */

#ifndef D4ENUMDEF_H_
#define D4ENUMDEF_H_

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "BaseType.h"

using namespace std;

namespace libdap {

class D4EnumDef {
    string d_name; /// The name of the enum definition
    Type d_type; /// Enums are always integer types

    struct enum_val {
        string d_item;
        // cast to unsigned based on d_type.
        long long d_val;

        enum_val(const string &item, unsigned long val) :
                d_item(item), d_val(val)
        {
        }
    };

    vector<enum_val> d_values;
    typedef vector<enum_val>::iterator enumValIter;

public:
    D4EnumDef() {}
    D4EnumDef(const string &name, Type type) : d_name(name), d_type(type) {}

    virtual ~D4EnumDef() {}

    string get_name() const { return d_name; }
    void set_name(const string &name) { d_name = name; }

    Type get_type() const { return d_type; }
    void set_type(Type t) { d_type = t; }

    void add_value(const string &item, unsigned long val) {
        d_values.push_back(enum_val(item, val));
    }

    enumValIter begin_vals() { return d_values.begin(); }
    enumValIter end_vals() { return d_values.end(); }
    long long get_value(enumValIter i) { return i->d_val; }
    string get_item(enumValIter i ) { return i->d_item; }

    static bool is_item(const enum_val e, const string item) {
        return e.d_item == item;
    }

    // Written to use for_each() as an exercise... Cannot use reference
    // types in is_item(...)
    long long get_value(const string &item) {
        // lookup name and return d_val;
        enumValIter i = find_if(d_values.begin(), d_values.end(),
                bind2nd(ptr_fun(libdap::D4EnumDef::is_item), item));
        return i->d_val;
    }

private:
#if 0
    string type_name();
#endif
    static void print_enum_const(XMLWriter *xml, const enum_val ev);

public:
    void print_xml_writer(XMLWriter &xml);
};

} /* namespace libdap */
#endif /* D4ENUMDEF_H_ */
