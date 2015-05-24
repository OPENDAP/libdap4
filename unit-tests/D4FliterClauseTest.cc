// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of libdap, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2014 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#if 0
#include <sstream>

#include "Byte.h"
#include "Int16.h"
#include "UInt16.h"
#include "Int32.h"
#include "UInt32.h"
#include "Float32.h"
#include "Float64.h"
#include "Str.h"
#include "Url.h"
#include "Array.h"
#include "Structure.h"
#include "Sequence.h"
#include "Grid.h"

#include "DDS.h"
#include "DMR.h"
#include "XMLWriter.h"
#include "D4BaseTypeFactory.h"
#include "D4ParserSax2.h"

#include "GNURegex.h"
#endif
#include "GetOpt.h"
#include "util.h"
#include "debug.h"

#include "testFile.h"
#include "test_config.h"

static bool debug = false;

#undef DBG
#define DBG(x) do { if (debug) {x;} } while(false)

using namespace CppUnit;
using namespace std;

namespace libdap {

class D4FilterClauseTest: public TestFixture {
	// Build a DMR and build several D4RValuse objects that reference its variables.
	// Then build several D4RValue objects that hold constants

private:
	D4RValue int_const;
	D4RValue float_const;
	D4RValue str_const;

	DMR sequence_1;

public:
	D4FilterClauseTest() {
    }
    ~D4FilterClauseTest() {
    }

    void setUp() {
    }

    void tearDown() {
    }

    bool re_match(Regex &r, const string &s) {
        int match = r.match(s.c_str(), s.length());
        DBG(cerr << "Match: " << match << " should be: " << s.length() << endl);
        return match == static_cast<int> (s.length());
    }

    /**
     * Open the named XML file and parse it, assuming that it contains a DMR.
     * @param name The name of the DMR XML file (or '-' for stdin)
     * @param debug True if the debug mode of the parse should be used
     * @param print Once parsed, should the DMR object be printed?
     * @return true if the parse worked, false otherwise
     */
    DMR *
    test_dap4_parser(const string &name, bool debug, bool print)
    {
        D4TestTypeFactory *factory = new D4TestTypeFactory;
        DMR *dataset = new DMR(factory, path_to_filename(name));

        try {
            D4ParserSax2 parser;
            if (name == "-") {
                parser.intern(cin, dataset, debug);
            }
            else {
                fstream in(name.c_str(), ios_base::in);
                parser.intern(in, dataset, debug);
            }
        }
        catch(...) {
            delete factory;
            delete dataset;
            throw;
        }

        cout << "Parse successful" << endl;

        if (print) {
            XMLWriter xml("    ");
            dataset->print_dap4(xml, false);
            cout << xml.get_doc() << endl;
        }

        return dataset;
    }

    /**
     * Given the name of a DDS and optional DAS file, build a DMR using the
     * hackery known as transform_to_dap4 and the new DMR ctor.
     *
     * @param dds_file
     * @param attr
     * @return A pointer to the new DMR; caller must delete
     */
    DMR *build_dmr(const string &dds_file, const string &attr = "") {
		try {
			string prefix = string(TEST_SRC_DIR) + "/dds-testsuite/";

			BaseTypeFactory factory;
			DDS dds(&factory, dds_file);
			dds.parse(prefix + dds_file);
			DBG(cerr << "DDS: " << endl; dds.print(cerr));

			if (!attr.empty()) {
				DAS das;
				das.parse(prefix + attr);
				dds.transfer_attributes(&das);
			}

			D4BaseTypeFactory d4_factory;
			return new DMR(&d4_factory, dds);
		}
    	catch (Error &e) {
    		CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
    	}

    	return 0;
    }

    void test_template(const string &dds_file, const string &dmr_baseline, const string &attr = "") {
    	DMR *dmr = 0;
		try {
			dmr = build_dmr(dds_file, attr);
			XMLWriter xml;
			dmr->print_dap4(xml);
			DBG(cerr << "DMR: " << endl << xml.get_doc() << endl);

			string prefix = string(TEST_SRC_DIR) + "/dds-testsuite/";
			CPPUNIT_ASSERT(string(xml.get_doc()) == readTestBaseline(prefix + dmr_baseline));
			delete dmr;
		}
    	catch (Error &e) {
    		delete dmr;
    		CPPUNIT_FAIL(string("Caught Error: ") + e.get_error_message());
    	}
    }

    CPPUNIT_TEST_SUITE( D4FilterClauseTest );

    // Build tests using D4Sequences - use FreeForm or hand write them


    CPPUNIT_TEST_SUITE_END();


};

CPPUNIT_TEST_SUITE_REGISTRATION(DMRTest);


} // namepsace libdap

int main(int argc, char*argv[]) {
    CppUnit::TextTestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    GetOpt getopt(argc, argv, "d");
    char option_char;

    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'd':
            debug = 1;  // debug is a static global
            break;
        default:
            break;
        }

    bool wasSuccessful = true;
    string test = "";
    int i = getopt.optind;
    if (i == argc) {
        // run them all
        wasSuccessful = runner.run("");
    }
    else {
        while (i < argc) {
            test = string("libdap::DMRTest::") + argv[i++];
            DBG(cerr << "test: " << test << endl);
            wasSuccessful = wasSuccessful && runner.run(test);
        }
    }

    xmlMemoryDump();

    return wasSuccessful ? 0 : 1;
}

