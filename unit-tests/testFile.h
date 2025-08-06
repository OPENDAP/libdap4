
#pragma once
#ifndef TESTFILE_H
#define TESTFILE_H

#include <string>

using namespace std;

string read_test_baseline(const string &fn);
void write_test_result(const string &file_name, const string &result);

#endif