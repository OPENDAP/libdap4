#include "config.h"

#include <fstream>
#include <string>
#include <vector>

#include "testFile.h"

using namespace std;

string
read_test_baseline(const string &fn)
{
    int length;

    ifstream is;
    is.open (fn.c_str(), ios::binary );

    if (!is)
        return "Could not read baseline file: "+fn;

    // get length of file:
    is.seekg (0, ios::end);
    length = is.tellg();

    // back to start
    is.seekg (0, ios::beg);

    // allocate memory:
    vector<char> buffer(length+1);

    // read data as a block:
    is.read (buffer.data(), length);
    is.close();
    buffer[length] = '\0';

    return string(buffer.data());
}
