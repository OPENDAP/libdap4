
// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

// $Log: das-test.cc,v $
// Revision 1.3  1994/09/27 22:58:59  jimg
// das-test no longer uses Space.cc for new/delete debugging - that system
// did not work on alphas).
//
// Revision 1.2  1994/09/09  16:13:16  jimg
// Added code to test the stdin, FILE * and file descriptor functions of
// class DAS.
//
// Revision 1.1  1994/08/02  18:08:38  jimg
// Test driver for DAS (and AttrTable) classes.
//

#include <iostream.h>
#include <String.h>
#include <Pix.h>
#include <GetOpt.h>

#include "DAS.h"

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "f");
    int option_char;
    bool use_fd = false;	// true to exercise the fd functions

    // process options first so that debugging in on for object instantitaion.
    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'f':
	      use_fd = true;
	      break;
	    case '?': 
	      cerr << "usage: " << argv[0] << " [f] in-file1 out-file1 ..."
		   << endl;
	  }

    DAS das;

    // If a file is named "-", assume that the user means stdin or stdout.
    for (int i = getopt.optind; i < argc; i+=2) {
	if (strcmp(argv[i], "-") == 0) {
	    cout << "Enter attributes:\n";
	    das.parse();
	}
	else {
	    cout << "Reading from: " << argv[i] << endl;
	    if (use_fd) {
		int fd = open(argv[i], O_RDONLY);
		das.parse(fd);
		close(fd);
	    }
	    else
		das.parse(argv[i]);
	}

	if (strcmp(argv[i+1], "-") == 0)
	    das.print();
	else {
	    cout << "Writing to: " << argv[i+1] << endl;
	    if (use_fd) {
		int fd = open(argv[i], O_WRONLY);
		das.print(fd);
		close(fd);
	    }
	    else
		das.print(argv[i+1]);
	}
    }
}
