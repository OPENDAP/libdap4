
// Test the DAS class.
// Read attributes from one or more files, printing the resulting table to
// stdout. If a file is named `-' read from stdin for that file. The option
// `-d' causes new/delete run-time debugging to be turned on.
//
// jhrg 7/25/94

// $Log: das-test.cc,v $
// Revision 1.5  1994/10/13 16:12:07  jimg
// Added -p to interface: run the parser. This option makes das-test
// behave as it used to, with the following exceptions:
// 1) if `none' is given as a filename, then the associated actions (reading
// or writing) is not performed. This lets you test the ctors and dtors
// w/o anything else getting in the way.
// 2) - means stdin or stdout
// Also added AttrTable test code that gets run if -p is not given.
//
// Revision 1.4  1994/10/05  16:48:52  jimg
// Added code to print the return value of the DAS::parse() mfunc.
//
// Revision 1.3  1994/09/27  22:58:59  jimg
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

void parser_driver(char *argv[], int i, bool use_fd, DAS das);
void plain_driver(DAS das);
void load_attr_table(AttrTable at);
void load_attr_table_ptr(AttrTable *atp);

int
main(int argc, char *argv[])
{
    GetOpt getopt (argc, argv, "fp");
    int option_char;
    bool use_fd = false;	// true to exercise the fd functions
    bool test_parser = false;

    // process options first so that debugging in on for object instantitaion.
    while ((option_char = getopt ()) != EOF)
	switch (option_char)
	  {
	    case 'p':
	      test_parser = true;
	      break;
	    case 'f':
	      use_fd = true;
	      break;
	    case '?': 
	      cerr << "usage: " << argv[0] << " [f] [p in-file1 out-file1 ...]"
		   << endl;
	  }

    DAS das;

    if (test_parser)
	for (int i = getopt.optind; i < argc; i+=2)
	    parser_driver(argv, i, use_fd, das);
    else
	plain_driver(das);
}

void
parser_driver(char *argv[], int i, bool use_fd, DAS das)
{
    // If a file is named "-", assume that the user means stdin or stdout.
    if (strcmp(argv[i], "none") == 0)
	return;
    else if (strcmp(argv[i], "-") == 0) {
	cout << "Enter attributes:\n";
	int status = das.parse();
	if (!status)
	    cerr << "parse() returned: " << status << endl;
    }
    else {
	cout << "Reading from: " << argv[i] << endl;
	if (use_fd) {
	    int fd = open(argv[i], O_RDONLY);
	    int status = das.parse(fd);
	    if (!status)
		cerr << "parse() returned: " << status << endl;
	    close(fd);
	}
	else {
	    int status = das.parse(argv[i]);
	    if (!status)
		cerr << "parse() returned: " << status << endl;
	}
    }

    // filename of "none" means don't print.
    if (strcmp(argv[i+1], "none") == 0)
	return;
    else if (strcmp(argv[i+1], "-") == 0)
	das.print();
    else {
	return;
#ifdef NEVER
	cout << "Writing to: " << argv[i+1] << endl;
	if (use_fd) {
	    int fd = open(argv[i], O_WRONLY);
	    das.print(fd);
	    close(fd);
	}
	else
	    das.print(argv[i+1]);
#endif
    }
}

// Given a DAS, add some stuff to it.

void
plain_driver(DAS das)
{
    AttrTable *atp;
    AttrTable *dummy;

    for (int i = 0; i < 4; ++i) {
	String name = "test" + i;
	atp = new AttrTable;
	load_attr_table_ptr(atp);

	dummy = das.get_table(name);
	das.add_table(name, atp);
    }

    das.print();
}

// stuff an AttrTable full of values. Also, print it out.

void
load_attr_table(AttrTable at)
{
    at.append_attr("month", "String", "Feb");
    at.append_attr("month", "String", "Feb");
    at.append_attr("Date", "Int32", "12345");
    at.append_attr("day", "Int32", "01");
    at.append_attr("Time", "Float64", "3.1415");

    cout << "Using the Pix:" << endl;
    for (Pix p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " " 
	     << at.get_attr(p) << endl;
    }

    String name = "month";
    cout << "Using String: " << at.get_type(name) << " " << at.get_attr(name) 
	 << endl;
    cout << "Using char *: " << at.get_type("month") << " " 
	 << at.get_attr("month") << endl;

    at.del_attr("month");

    cout << "After deletion:" << endl;
    for (p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " " 
	     << at.get_attr(p) << endl;
    }

    at.print(cout);

    cout << "After print:" << endl;
    for (p = at.first_attr(); p; at.next_attr(p)) {
	cout << at.get_name(p) << " " << at.get_type(p) << " " 
	     << at.get_attr(p) << endl;
    }
}

// OK, now try it with a dymanic AttrTable

void
load_attr_table_ptr(AttrTable *atp)
{
    atp->append_attr("month", "String", "Feb");
    atp->append_attr("month", "String", "Feb");
    atp->append_attr("Date", "Int32", "12345");
    atp->append_attr("day", "Int32", "01");
    atp->append_attr("Time", "Float64", "3.1415");

    cout << "Using the Pix:" << endl;
    for (Pix p = atp->first_attr(); p; atp->next_attr(p)) {
	cout << atp->get_name(p) << " " << atp->get_type(p) << " " 
	     << atp->get_attr(p) << endl;
    }

    String name = "month";
    cout << "Using String: " << atp->get_type(name) << " "
         << atp->get_attr(name) << endl;
    cout << "Using char *: " << atp->get_type("month") << " " 
	 << atp->get_attr("month") << endl;

    atp->del_attr("month");

    cout << "After deletion:" << endl;
    for (p = atp->first_attr(); p; atp->next_attr(p)) {
	cout << atp->get_name(p) << " " << atp->get_type(p) << " " 
	     << atp->get_attr(p) << endl;
    }

    atp->print(cout);

    cout << "After print:" << endl;
    for (p = atp->first_attr(); p; atp->next_attr(p)) {
	cout << atp->get_name(p) << " " << atp->get_type(p) << " " 
	     << atp->get_attr(p) << endl;
    }
}

