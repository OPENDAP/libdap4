#include "config.h"

#include <fstream>
#include <string>
#include <sstream>

#include "InternalErr.h"

#include "testFile.h"

using namespace std;
using namespace libdap;

string
readTestBaseline(const string &fn)
{
    int length;
    char * buffer;

    ifstream is;
    is.open (fn.c_str(), ios::binary );

    // get length of file:
    is.seekg (0, ios::end);
    length = is.tellg();
    is.seekg (0, ios::beg);

    // allocate memory:
    buffer = new char [length+1];

    // read data as a block:
    is.read (buffer,length);
    is.close();
    buffer[length] = '\0';

    return string(buffer);
}
#if 0
string
readTestBaseline(const string &fn)
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
#endif
