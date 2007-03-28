
#include <iostream>

#include "Error.h"
#include "GNU/GNURegex.h"

using namespace std;

int
main(int argc, char *argv[])
{
    Regex r1("^\\..*");

    int result = r1.match("test", 4, 0);
    cerr << "r1.match(\"test\", 4, 0): " << result << endl;

    result = r1.match(".on", 3, 0);
    cerr << "r1.match(\".on\", 3, 0): " << result << endl;
}
