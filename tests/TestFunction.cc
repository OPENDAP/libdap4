
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2013 OPeNDAP, Inc.
// Authors: Nathan Potter <npotter@opendap.org>
//         James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <BaseType.h>
#include <TestFloat64.h>
#include <TestStr.h>
#include <TestArray.h>

#include <Error.h>
#include <DDS.h>

#include <debug.h>
#include <util.h>

#include "TestFunction.h"

namespace libdap {

/**
 * @brief scale a scalar or array variable
 * This does not work for DAP2 Grids; only Array and scalar variables.
 */
void
function_scale(int argc, BaseType * argv[], DDS &, BaseType **btpp)
{
    string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"scale\" version=\"1.0\" href=\"http://docs.opendap.org/index.php/Server_Side_Processing_Functions\">\n" +
    "</function>";

    if (argc == 0) {
        Str *response = new TestStr("info");
        response->set_value(info);
        *btpp = response;
        return;
    }

    // Check for 2 arguments
    DBG(cerr << "argc = " << argc << endl);
    if (argc != 2)
        throw Error(malformed_expr,"Wrong number of arguments to scale().");

    double m = extract_double_value(argv[1]);

    DBG(cerr << "m: " << m << << endl);

    // Read the data, scale and return the result.
    BaseType *dest = 0;
    double *data;
    if (argv[0]->is_vector_type()) {
        TestArray &source = static_cast<TestArray&>(*argv[0]);
        source.read();

        data = extract_double_array(&source);
        int length = source.length();
        for (int i = 0; i < length; ++i)
            data[i] = data[i] * m;

        Array *result = new TestArray(source);

        result->add_var_nocopy(new TestFloat64(source.name()));
        result->set_value(data, length);

        delete[] data; // set_value copies.

        dest = result;
    }
    else if (argv[0]->is_simple_type() && !(argv[0]->type() == dods_str_c || argv[0]->type() == dods_url_c)) {
    	argv[0]->read();
        double data = extract_double_value(argv[0]);

        data *= m;

        Float64 *fdest = new TestFloat64(argv[0]->name());

        fdest->set_value(data);
        dest = fdest;
    }
    else {
        throw Error(malformed_expr,"The scale() function works only for Arrays and scalars.");
    }

    *btpp = dest;
    return;
}

} // namesspace libdap
