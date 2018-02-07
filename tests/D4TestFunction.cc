
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

#include "DMR.h"
#include "D4RValue.h"

#include <debug.h>
#include <util.h>

#include "D4TestFunction.h"

namespace libdap {

/**
 * @brief scale a scalar or array variable
 * This does not work for DAP2 Grids; only Array and scalar variables.
 */
void
function_scale_dap2(int argc, BaseType * argv[], DDS &, BaseType **btpp)
{
    string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"scale\" version=\"1.1\" href=\"http://docs.opendap.org/index.php/Server_Side_Processing_Functions\">\n" +
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

/**
 * @brief DAP4 scale a scalar or array variable
 * This does not work for DAP2 Grids; only Array and scalar variables.
 */
BaseType *
function_scale_dap4(D4RValueList *args, DMR &dmr)
{
    string info =
    string("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n") +
    "<function name=\"scale\" version=\"1.1\" href=\"http://docs.opendap.org/index.php/Server_Side_Processing_Functions\">\n" +
    "</function>";

    // DAP4 function porting information: in place of 'argc' use 'args.size()'
    if (args->size() == 0) {
        Str *response = new TestStr("info");
        response->set_value(info);
        // DAP4 function porting: return a BaseType* instead of using the value-result parameter
        return response;
    }

    // Check for 2 arguments
    DBG(cerr << "args.size() = " << args.size() << endl);
    if (args->size() != 2)
        throw Error(malformed_expr, "Wrong number of arguments to scale().");

    // DAP4 function porting information: in place of 'argv[n]' use args->get_rvalue(n)->value(dmr)
    // where 'n' is between 0 and args.size()-1. The line below is the DAP4 equivalent of 'argv[1].'
    double m = extract_double_value(args->get_rvalue(1)->value(dmr));

    DBG(cerr << "m: " << m << << endl);

    // Read the data, scale and return the result.
    //BaseType *dest = 0;
    double *data;
    BaseType *arg0 = args->get_rvalue(0)->value(dmr); // DAP4 function porting: ... 'argv[0]'
    if (arg0->is_vector_type()) {
        Array *source = dynamic_cast<Array*>(arg0);
        if (!source)
            throw Error(malformed_expr, string("Expected an Array as an argument, but got '"
                + arg0->type_name()) + "' instead.");

        if (!source->read_p())
            source->read();

        data = extract_double_array(source);
        int length = source->length();
        for (int i = 0; i < length; ++i)
            data[i] = data[i] * m;

        Array *result = new Array(*source);

        result->add_var_nocopy(new Float64(source->name()));
        result->set_value(data, length);

        delete[] data; // set_value copies.

        // DAP4 function porting: return a BaseType* instead of using the value-result parameter
        return result;
    }
    else if (arg0->is_simple_type() && !(arg0->type() == dods_str_c || arg0->type() == dods_url_c)) {
    	arg0->read();
        double data = extract_double_value(arg0);

        data *= m;

        Float64 *fdest = new TestFloat64(arg0->name());

        fdest->set_value(data);
        return fdest;
    }
    else {
        throw Error(malformed_expr,"The scale() function works only for numerical arrays and scalars.");
    }

    return 0;
}

} // namesspace libdap
