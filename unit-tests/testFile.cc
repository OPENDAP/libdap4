#include <fstream>
#include <sstream>
#include <debug.h>

using namespace std;

#define FILE2string(s,f,c) do {\
        FILE *(f) = fopen("testout", "w");\
        c;\
        fclose(f);\
        s = testFile("testout");\
        unlink("testout");\
} while(0);

// It's evil to include code like this, but for the unit tests, such is
// the way... jhrg 1/20/06
string
testFile(char *fn)
{
    ifstream ifs(fn);
    ostringstream strm;
    char line[1024];
    while (!ifs.eof()) {
        ifs.getline(line, 1024);
        strm << line << endl;
    }
    ifs.close();
    
    return strm.str();
}
