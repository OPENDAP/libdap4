
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string>

using namespace std;

// Mever use this!
#define FILE2string(s,f,c) do {\
        FILE *(f) = fopen("testout", "w");\
        c;\
        fclose(f);\
        s = readTestBaseline("testout");\
        unlink("testout");\
} while(0);

string readTestBaseline(const string &fn);

// string readTestBaseline(char *fn);
