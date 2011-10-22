
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
        s = testFile("testout");\
        unlink("testout");\
} while(0);

string testFile(const string &fn);

// string testFile(char *fn);
