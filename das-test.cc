
// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

// $Log: das-test.cc,v $
// Revision 1.1  1994/08/02 18:08:38  jimg
// Test driver for DAS (and AttrTable) classes.
//

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <GetOpt.h>

#include "DAS.h"
#include "Space.h"

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "d");
    int option_char;

    // process options first so that debugging in on for object instantitaion.
    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      space_debug = 1;
	      executable_name = "das-test";
	      break;
	    case '?': 
	      fprintf (stderr, "usage: %s [d] filename ...\n", argv[0]);
	  }

    DAS das;

    for (int i = getopt.optind; i < argc; ++i) {
	if (strcmp(argv[i], "-") == 0) {
	    cout << "Enter attributes:\n";
	    das.parse();
	    cout << das;
	}
	else {
	    cout << "Reading from: " << argv[i] << endl;
	    das.parse(argv[i]);
	    cout << das;
	}
    }
}
