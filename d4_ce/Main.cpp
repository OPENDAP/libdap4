
#include <iostream>

#include "GetOpt.h"

#include "D4CEDriver.h"

using namespace std;

const string options = "p:vdDh?";
const string usage = "d4_ce [dDvh?] -p <expression>";

int main(int argc, char * argv[])
{
    GetOpt getopt(argc, argv, options.c_str());
    libdap::D4CEDriver parser;

    std::string expression;
    bool verbose = false;

    int option_char;
    while ((option_char = getopt()) != EOF)
        switch (option_char) {
        case 'p':
        	expression = getopt.optarg;
        	break;
        case 'v':
        	verbose = true;
        	break;
        case 'd':
            parser.set_trace_scanning(true);
            break;
        case 'D':
            parser.set_trace_parsing(true);
            break;
        case 'h':
        case '?':
        default:
        	cerr << usage << endl;
        	exit(1);
        }

    if (expression.empty()) {
    	cerr << usage << endl;
    	exit(1);
    }

    if (verbose) {
    	cerr << "Parsing: '" << expression << "'" << endl;
    }

    bool result = parser.parse(expression);

    if (!result)
    	cerr << "Error parsing: '" << expression << "'" << endl;

    if (verbose && result)
    	cerr << "Parse successful" << endl;
}

