//
// Created by James Gallagher on 2/28/23.
//

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

#include "RCReader.h"

#include "http_dap/HTTPCache.h"
#include "http_dap/HTTPConnect.h"	// Used to generate a response to cache.
#include "http_dap/HTTPCacheTable.h"
#include "http_dap/HTTPResponse.h"

using namespace libdap;
using namespace std;

bool save_body(const string &in, const string &out)
{
    string line;

    ifstream in_file{in};
    ofstream out_file{out};

    if (in_file && out_file) {

        while (getline(in_file, line)) {
            out_file << line << "\n";
        }
        cout << "Copy Finished" << endl;
    }
    else {
       cerr << "Cannot read File" << endl;
       return false;
    }

    return true;
}

bool save_headers(const string &url, const vector<string> &headers)
{
    string line;

    ofstream out_file{url + ".headers"};

    if (out_file) {

        for (auto &h : headers) {
            out_file << h << "\n";
        }
        cout << "Copy Finished" << endl;
    }
    else {
        // Something went wrong
       cerr << "Cannot open output file" << endl;
       return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <url> <name>" << endl;
        return EXIT_FAILURE;
    }
    string url{argv[1]};
    string name{argv[2]};

    auto http_conn_p = std::make_unique<HTTPConnect>(RCReader::instance());
    HTTPResponse *rs = http_conn_p->fetch_url(url);

    cout << "Response status: " << rs->get_status() << endl;
    cout << "Response headers: ";
    copy(rs->get_headers().begin(), rs->get_headers().end(), ostream_iterator<string>(cout, "\n"));
    cout << "Response body: " << rs->get_file() << endl;

    save_headers(name, rs->get_headers());
    save_body(rs->get_file(), name + ".body");

    return 0;
}