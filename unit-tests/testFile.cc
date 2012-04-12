#include "config.h"

#include <fstream>
#include <string>
#include <sstream>

#include "InternalErr.h"

#include "testFile.h"

using namespace std;
using namespace libdap;

string
testFile(const string &fn)
{
    ifstream ifs(fn.c_str());
    if (!ifs)
        throw InternalErr(__FILE__, __LINE__, "Could not open file");

    ostringstream strm;
    char line[1024];
    while (!ifs.eof()) {
        ifs.getline(line, 1024);
        strm << line << endl;
    }
    ifs.close();
    
    return strm.str();
}
