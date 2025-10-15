#include "config.h"

#include <fstream>
#include <string>
#include <vector>

#include "testFile.h"

using namespace std;

string read_test_baseline(const string &fn) {
    ifstream is;
    is.open(fn.c_str(), ios::binary);

    if (!is)
        return "Could not read baseline file: " + fn;

    // get the length of the file:
    is.seekg(0, ios::end);
    const long length = is.tellg();

    // back to start
    is.seekg(0, ios::beg);

    // allocate memory:
    vector<char> buffer(length + 1);

    // read data as a block:
    is.read(buffer.data(), length);
    is.close();
    buffer[length] = '\0';

    return {buffer.data()};
}

void write_test_result(const string &file_name, const string &result) {
    ofstream os;
    os.open(file_name, ios::binary);

    if (!os)
        throw runtime_error("Could not write result file: " + file_name);

    os.write(result.data(), static_cast<std::streamsize>(result.size()));
}
