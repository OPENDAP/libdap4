
// (c) COPYRIGHT URI/MIT 1995-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)

// Test the CE scanner and parser.
//
// jhrg 9/12/95

// $Log: expr-test.cc,v $
// Revision 1.12  1996/08/13 18:55:20  jimg
// Added __unused__ to definition of char rcsid[].
// Uses the parser_arg object to communicate with the parser.
//
// Revision 1.11  1996/06/11 17:30:36  jimg
// Fixed -k (constraint expression) option when used with -p (parser) option.
//
// Revision 1.10  1996/06/04 21:34:00  jimg
// Multiple connections are now possible. It is now possible to open several
// URLs at the same time and read from them in a round-robin fashion. To do
// this I added data source and sink parameters to the serialize and
// deserialize mfuncs. Connect was also modified so that it manages the data
// source `object' (which is just an XDR pointer).
//
// Revision 1.9  1996/05/31 23:30:58  jimg
// Updated copyright notice.
//
// Revision 1.8  1996/05/29 22:04:13  jimg
// Removed old, useless, code.
//
// Revision 1.7  1996/05/22 18:05:35  jimg
// Merged files from the old netio directory into the dap directory.
// Removed the errmsg library from the software.
//
// Revision 1.6  1996/05/14 15:38:57  jimg
// These changes have already been checked in once before. However, I
// corrupted the source repository and restored it from a 5/9/96 backup
// tape. The previous version's log entry should cover the changes.
//
// Revision 1.5  1996/03/05 00:57:19  jimg
// Fixed tests of constrained tranmission so CEs with spaces will be read
// properly.
// Added new option so that a CE may be given on the command line.
//
// Revision 1.4  1995/12/09  01:07:37  jimg
// Added changes so that relational operators will work properly for all the
// datatypes (including Sequences). The relational ops are evaluated in
// DDS::eval_constraint() after being parsed by DDS::parse_constraint().
//
// Revision 1.3  1995/12/06  19:43:09  jimg
// Added options for testing the constraint evaluator software.
// Added functions which test the constraint evaluator.
// Added function that simulates te complete client-server conversation which
// causes a variable to be sent after the evaluation of a CE. This manages
// multiple DDSs just as a real client would. This code is different than the
// simpler code run by evaluate_dds().
//
// Revision 1.2  1995/10/23  23:08:17  jimg
// Fixed scanner display code to match current scanner.
// Added code to test simple evaluator.
// Fixed type declarations (YYSTYPE, ...).
//
// Revision 1.1  1995/10/13  03:02:26  jimg
// First version. Runs scanner and parser.
//

#include "config_dap.h"

static char rcsid[] __unused__ = {"$Id: expr-test.cc,v 1.12 1996/08/13 18:55:20 jimg Exp $"};

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <rpc/types.h>
#include <netinet/in.h>
#include <rpc/xdr.h>

#include <streambuf.h>
#include <iostream.h>
#include <stdiostream.h>
#include <GetOpt.h>

#include <String.h>
#include <SLList.h>

#include "DDS.h"
#include "BaseType.h"

#include "parser.h"
#include "expr.h"
#include "expr.tab.h"
#include "util.h"
#include "debug.h"

#define DODS_DDS_PRX "dods_dds"

void test_scanner();
void test_parser(DDS &table, const String &dds_name, const String &constraint);
bool read_table(DDS &table, const String &name, bool print);
void evaluate_dds(DDS &table, bool print_constrained);
bool transmit(DDS &write, bool verb);
bool loopback_pipe(FILE **pout, FILE **pin);
bool constrained_trans(const String &dds_name, const String &ce);

int exprlex();			// exprlex() uses the global exprlval
int exprparse(void *arg);
int exprrestart(FILE *in);

extern YYSTYPE exprlval;
extern int exprdebug;
const String version = "version 1.12";
const String prompt = "expr-test: ";
const String options = "sp:detcw:k:v";
const String usage = "expr-test [-d -s -p -e -c -t -w -k] [file] [expr]\n\
Test the expression evaluation software.\n\
Options:\n\
	-s: Feed the input stream directly into the expression scanner, does\n\
	    not parse.\n\
	-p  DDS-file: Read the DDS from `DDS-file' and create a DDS object,\n\
	    then prompt for an expression and parse that expression, given\n\
	    the DDS object.\n\
	-d: Turn on expression parser debugging.\n\
	-e: Evaluate the constraint expression. Must be used with -p.\n\
	-c: Print the constrained DDS (the one that will be returned\n\
	    prepended to a data transmission. Must also supply -p and -e \n\
	-t: Test transmission of data. This uses the Test*classes.\n\
	    Transmission is done using a single process that writes and then\n\
	    reads from a pipe. Must also suppply -p.\n\
	-w: Do the whole enchilada. You don't need to supply -p, -e, ...\n\
	    This prompts for the constraint expression\n\
	-k: A constraint expression to use with the data. Works with -p,\n\
	    -e, -t and -w\n\
        -v: Print the version of expr-test\n";

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options);
    int option_char;
    bool scanner_test = false, parser_test = false, evaluate_test = false;
    bool trans_test = false, print_constrained = false;
    bool whole_enchalada = false, constraint_expr = false;
    String dds_file_name;
    String constraint = "";
    DDS table;

    // process options

    while ((option_char = getopt()) != EOF)
	switch (option_char)
	  {
	    case 'd': 
	      exprdebug = true;
	      break;
	    case 's':
	      scanner_test = true;
	      break;
	    case 'p':
	      parser_test = true;
	      dds_file_name = getopt.optarg;
	      break;
	    case 'e':
	      evaluate_test = true;
	      break;
	    case 't':
	      trans_test = true;
	      break;
	    case 'c':
	      print_constrained = true;
	      break;
	    case 'w':
	      whole_enchalada = true;
	      dds_file_name = getopt.optarg;
	      break;
	    case 'k':
	      constraint_expr = true;
	      constraint = getopt.optarg;
	      break;
	    case 'v':
	      cerr << argv[0] << ": " << version << endl;
	      exit(0);
	    case '?': 
	    default:
	      cerr << usage << endl; 
	      exit(1);
	      break;
	  }

    if (!scanner_test && !parser_test && !evaluate_test && !trans_test 
	&& !whole_enchalada) {
	cerr << usage << endl;
	exit(1);
    }

    // run selected tests

    if (scanner_test) {
	test_scanner();
	exit(0);
    }

    if (parser_test) {
	test_parser(table, dds_file_name, constraint);
    }

    if (evaluate_test) {
	evaluate_dds(table, print_constrained);
    }

    if (trans_test) {
	transmit(table, exprdebug);
    }

    if (whole_enchalada) {
	constrained_trans(dds_file_name, constraint);
    }
}

void
test_scanner()
{
    int tok;

    cout << prompt;		// first prompt

    while ((tok = exprlex())) {
	switch (tok) {
	  case ID:
	    cout << "ID: " << exprlval.id << endl;
	    break;
	  case STR:
	    cout << "STR: " << *exprlval.val.v.s << endl;
	    break;
	  case FIELD:
	    cout << "FIELD: " << exprlval.id << endl;
	    break;
	  case INT:
	    cout << "INT: " << exprlval.val.v.i << endl;
	    break;
	  case FLOAT:
	    cout << "FLOAT: " << exprlval.val.v.f << endl;
	    break;
	  case EQUAL:
	    cout << "EQUAL: " << exprlval.op << endl;
	    break;
	  case NOT_EQUAL:
	    cout << "NOT_EQUAL: " << exprlval.op << endl;
	    break;
	  case GREATER:
	    cout << "GREATER: " << exprlval.op << endl;
	    break;
	  case GREATER_EQL:
	    cout << "GREATER_EQL: " << exprlval.op << endl;
	    break;
	  case LESS:
	    cout << "LESS: " << exprlval.op << endl;
	    break;
	  case LESS_EQL:
	    cout << "LESS_EQL: " << exprlval.op << endl;
	    break;
	  case REGEXP:
	    cout << "REGEXP: " << exprlval.op << endl;
	    break;
	  case '*':
	    cout << "Dereference" << endl;
	    break;
	  case '.':
	    cout << "Field Selector" << endl;
	    break;
	  case ',':
	    cout << "List Element Separator" << endl;
	    break;
	  case '[':
	    cout << "Left Bracket" << endl;
	    break;
	  case ']':
	    cout << "Right Bracket" << endl;
	    break;
	  case '(':
	    cout << "Left Paren" << endl;
	    break;
	  case ')':
	    cout << "Right Paren" << endl;
	    break;
	  case '{':
	    cout << "Left Brace" << endl;
	    break;
	  case '}':
	    cout << "Right Brace" << endl;
	    break;
	  case ':':
	    cout << "Colon" << endl;
	    break;
	  case '&':
	    cout << "Ampersand" << endl;
	    break;
	  default:
	    cout << "Error: Unrecognized input" << endl;
	}
	cout << prompt;		// print prompt after output
    }
}

// NB: The DDS is read in via a file because reading from stdin must be
// terminated by EOF. However, the EOF used to terminate the DDS also closes
// stdin and thus the expr scanner exits immediately.

void
test_parser(DDS &table, const String &dds_name, const String &constraint)
{
    read_table(table, dds_name, true);

    bool status;

    if (constraint != "") 
	status = table.parse_constraint(constraint);
    else {
	exprrestart(stdin);

	cout << prompt;

	parser_arg arg(&table);

	status = exprparse((void *)&arg) == 0;

	//  STATUS is the result of the parser function; if a recoverable error
	//  was found it will be true but arg.status() will be false.
	if (!status || !arg.status()) {// Check parse result
	    if (arg.error())
		arg.error()->display_message();
#if 0
	    cerr << "Error parsing constraint expression!" << endl;
#endif
	    status = false;
	}
	else
	    status = true;
    }

    if (status)
	cout << "Input parsed" << endl;
    else
	cout << "Input did not parse" << endl;
}

// Read a DDS from stdin and build the cooresponding DDS. IF PRINT is true,
// print the text reprsentation of that DDS on the stdout. The DDS TABLE is
// modified as a side effect.
//
// Returns: true iff that DDS pasted the semantic_check() mfunc, otherwise
// false.

bool
read_table(DDS &table, const String &name, bool print)
{
    int parse = table.parse(name);
    
    if (!parse) {
	cout << "Input did not parse" << endl;
	return false;
    }
    
    if (print)
	table.print();

    if (table.check_semantics(true))
	return true;
    else {
	cout << "Input did not pass semantic checks" << endl;
	return false;
    }
}

void
evaluate_dds(DDS &table, bool print_constrained)
{
    if (print_constrained)
	table.print_constrained();
    else
	for (Pix p = table.first_var(); p; table.next_var(p))
	    table.var(p)->print_decl(cout, "", true, true);
}

// Given that a DDS has been created (nominally via read_table() above and
// that a constraint expression has been entered, send data from the DDS to a
// second DDS instance via the serialize/deserialize mfuncs. 

bool
transmit(DDS &write, bool verb)
{
    bool status;
    FILE *pin, *pout;

    status = loopback_pipe(&pin, &pout);
    if (!status) {
	cerr << "expr-test: Could not create the loopback streams" << endl;
	return false;
    }

    XDR *sink = new_xdrstdio(pout, XDR_ENCODE);
    XDR *source = new_xdrstdio(pin, XDR_DECODE);

    // duplicate the DDS (create the variables for reading)

    DDS read = write;

    // for each variable in the write DDS, read it (loading it with dummy
    // values from the mfuncs supplied by the Test classes) and send it. Then
    // read the variable values back into the read DDS and print the received
    // values. 

    Pix wp, rp;
    for (wp = write.first_var(), rp = read.first_var(); 
	 wp && rp; 
	 write.next_var(wp), read.next_var(rp)) {

	// This code only works for scalar variables at the top level of the
	// DDS. It also ignores the read_p() mfunc.
	if (write.var(wp)->send_p()) { // only works for scalars
	    int error = 0;
	    status = write.var(wp)->read("dummy", error);
	    if (error != -1)
		status = false;

	    if (verb) {
		cout << "Variable to be written:" << endl;
		write.var(wp)->print_val(cout);
		cout << endl;
		cout.flush();
	    }

	    status = write.var(wp)->serialize("dummy", write, sink, true);
	    if (!status) {
		cerr << "Could not write";
		write.var(wp)->print_decl(cerr);
		exit(1);
	    }

	    status = read.var(rp)->deserialize(source);
	    if (!status) {
		cerr << "Could not read";
		read.var(wp)->print_decl(cerr);
		exit(1);
	    }

	    if (verb)
		cout << "Variable read:" << endl;

	    read.var(rp)->print_val(cout);

	    if (verb)
		cout << endl;
	}

	cout.flush();
    }

    delete_xdrstdio(sink);
    delete_xdrstdio(source);

    return true;
}

// create a pipe for the caller's process which can be used by the DODS
// software to write to ad read from itself.

bool
loopback_pipe(FILE **pout, FILE **pin)
{
    // make a pipe

    int fd[2];
    if (pipe(fd) < 0) {
	cerr << "Could not open pipe" << endl;
	return false;
    }

    *pout = fdopen(fd[1], "w");
    *pin = fdopen(fd[0], "r");

    return true;
}


// Originally in netexec.c (part of the netio library).
// Read the DDS from the data stream. Leave the binary information behind. The
// DDS is moved, without parsing it, into a file and a pointer to that FILE is
// returned. The argument IN (the input FILE stream) is positioned so that the
// next byte is the binary data.
//
// The binary data follows the text `Data:', which itself starts a line.
//
// Returns: a FILE * which contains the DDS describing the binary information
// in IF.

FILE *
move_dds(FILE *in)
{
    char *c = tempnam(NULL, "dods");
    if (!c) {
	cerr << "Could not create temporary file name" << strerror(errno)
	    << endl;
	return NULL;
    }

    FILE *fp = fopen(c, "w+");
    unlink(c);
    if (!fp) {
	cerr << "Could not open anonymous temporary file: " 
	     << strerror(errno) << endl;
	return NULL;
    }
	    
    int data = FALSE;
    char s[256], *sp;
    
    sp = &s[0];
    while (!feof(in) && !data) {
	sp = fgets(s, 255, in);
	if (strcmp(s, "Data:\n") == 0)
	    data = TRUE;
	else
	    fputs(s, fp);
    }

    fflush(fp);
    if (fseek(fp, 0L, 0) < 0) {
	cerr << "Could not rewind data DDS stream: " << strerror(errno)
	    << endl;
	return NULL;
    }
    
    free(c);			// tempnam uses malloc
    return fp;
}
    
// Test the transmission of constrained datasets. Use read_table() to read
// the DDS from a file. Once done, prompt for the variable name and
// constraint expression. In a real client-server system the server would
// read the DDS for the entire dataset and send it to the client. The client
// would then respond to the server by asking for a variable given a
// constraint.
// 
// Once the constraint has been entered, it is evaluated in the context of
// the DDS using DDS:eval_constraint() (this would happen on the server-side
// in a real system). Once the evaluation is complete,
// DDS::print_constrained() is used to create a DDS describing only those
// parts of the dataset that are to be sent to the client process and written
// to the output stream. After that, the marker `Data:' is written to the
// output stream, followed by the binary data.

bool
constrained_trans(const String &dds_name, const String &constraint) 
{
    bool status;
    FILE *pin, *pout;
    DDS server;

    cout << "The complete DDS:" << endl;
    read_table(server, dds_name, true);

    status = loopback_pipe(&pout, &pin);
    if (!status) {
	cerr << "Could not create the loopback streams" << endl;
	return false;
    }

    // Simulate Connect::request_dds() at the begining of a virtual connection
    DDS client = server;

    // If the CE was not passed in, read it from the command line.
    String ce;
    if (constraint == "") {
	cout << "Constraint:";
	char c[256];
	cin.getline(c, 256);
	if (!cin) {
	    cerr << "Could nore read the constraint expression" << endl;
	    exit(1);
	}
	ce = c;
    }
    else
	ce = constraint;

    // send the variable given the constraint (dataset is ignored by the Test
    // classes); TRUE flushes the I/O channel.
    if (!server.send("dummy", ce, pout)) {
	cerr << "Could not send the variable" << endl;
	return false;
    }

    fclose(pout);		// close pout to read from pin. Why?
    
    // Now do what Connect::request_data() does:

    // First read the DDS into a new object (using a file to store the DDS
    // temporarily - the parser/scanner won't stop reading until an EOF is
    // found, this fixes that problem).

    DDS dds;
    FILE *dds_fp = move_dds(pin);
    DBG(cerr << "Moved the DDS to a temp file" << endl);
    if (!dds.parse(dds_fp)) {
	cerr << "Could not parse return data description" << endl;
	return false;
    }
    fclose(dds_fp);

    XDR *source = new_xdrstdio(pin, XDR_DECODE);

    // Back on the client side; deserialize the data *using the newly
    // generated DDS* (the one sent with the data).

    cout << "The data:" << endl;
    for (Pix q = dds.first_var(); q; dds.next_var(q)) {
	if (!dds.var(q)->deserialize(source))
	    return false;

	dds.var(q)->print_val(cout);
    }
    
    delete_xdrstdio(source);

    return true;
}






