
/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/


// Test the CE scanner and parser.
//
// jhrg 9/12/95

// $Log: expr-test.cc,v $
// Revision 1.3  1995/12/06 19:43:09  jimg
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

static char rcsid[]= {"$Id: expr-test.cc,v 1.3 1995/12/06 19:43:09 jimg Exp $"};

#include <stdio.h>
#include <iostream.h>
#include <stdiostream.h>
#include <GetOpt.h>

#include <String.h>
#include <SLList.h>

#include "errmsg.h"
#include "DDS.h"
#include "BaseType.h"

#include "expr.h"
#include "expr.tab.h"

#define DODS_DDS_PRX "dods_dds"

void test_scanner();
void test_parser(DDS &table, String &dds_name);
bool read_table(DDS &table, String &name, bool print);
void evaluate_dds(DDS &table, bool print_constrained);
bool transmit(DDS &write, bool verb);
bool loopback_pipe(FILE **pout, FILE **pin);
bool constrained_trans(String dds_name);

int exprlex();			// exprlex() uses the global exprlval
int exprparse(DDS &table);

int exprrestart(FILE *in);

extern YYSTYPE exprlval;
extern int exprdebug;
const char *prompt = "expr-test: ";
const char *options = "sp:detcw";

int
main(int argc, char *argv[])
{
    GetOpt getopt(argc, argv, options);
    int option_char;
    bool scanner_test = false, parser_test = false, evaluate_test = false;
    bool trans_test = false, print_constrained = false;
    bool whole_enchalada = false;
    String dds_file_name;
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
	      parser_test = false;
	      break;
	    case '?': 
	      cerr << "usage: " << argv[0] << " " << options 
		   << " [filename]" << endl; 
	  }

    // run selected tests

    if (scanner_test) {
	test_scanner();
    }

    if (parser_test) {
	test_parser(table, dds_file_name);
    }

    if (evaluate_test) {
	evaluate_dds(table, print_constrained);
    }

    if (trans_test) {
	transmit(table, exprdebug);
    }

    if (whole_enchalada) {
	constrained_trans(dds_file_name);
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
	    cout << "ID: " << exprlval.char_ptr << endl;
	    break;
	  case STR:
	    cout << "STR: " << exprlval.char_ptr << endl;
	    break;
	  case FIELD:
	    cout << "FIELD: " << exprlval.char_ptr << endl;
	    break;
	  case INT:
	    cout << "INT: " << exprlval.char_ptr << endl;
	    break;
	  case FLOAT:
	    cout << "FLOAT: " << exprlval.char_ptr << endl;
	    break;
	  case EQUAL:
	    cout << "EQUAL: " << exprlval.char_ptr << endl;
	    break;
	  case NOT_EQUAL:
	    cout << "NOT_EQUAL: " << exprlval.char_ptr << endl;
	    break;
	  case GREATER:
	    cout << "GREATER: " << exprlval.char_ptr << endl;
	    break;
	  case GREATER_EQL:
	    cout << "GREATER_EQL: " << exprlval.char_ptr << endl;
	    break;
	  case LESS:
	    cout << "LESS: " << exprlval.char_ptr << endl;
	    break;
	  case LESS_EQL:
	    cout << "LESS_EQL: " << exprlval.char_ptr << endl;
	    break;
	  case REGEXP:
	    cout << "REGEXP: " << exprlval.char_ptr << endl;
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
test_parser(DDS &table, String &dds_name)
{
    read_table(table, dds_name, true);

    exprrestart(stdin);

    cout << prompt;

    if (exprparse(table) == 0)
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
read_table(DDS &table, String &name, bool print)
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

// Given that a DDS has been created (nomianlly via read_table() above and
// that a constraint expression has been entered, send data from the DDS to a
// second DDS instance via the serialize/deserialize mfuncs. 

bool
transmit(DDS &write, bool verb)
{
    bool status;
    FILE *pin, *pout;

    status = loopback_pipe(&pin, &pout);
    if (!status) {
	cerr << "Could not create the loopback streams" << endl;
	return false;
    }

    set_xdrin(pin);
    set_xdrout(pout);

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
	    status = write.var(wp)->read("dummy", "dummy");

	    if (verb) {
		cout << "Variable to be written:" << endl;
		write.var(wp)->print_val(cout);
		cout << endl;
		cout.flush();
	    }

	    status = write.var(wp)->serialize(true);
	    if (!status) {
		cerr << "Could not write";
		write.var(wp)->print_decl(cerr);
		exit(1);
	    }

	    status = read.var(rp)->deserialize();
	    if (!status) {
		cerr < "Could not read";
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


/* 
   Originally in netexec.c (part of the netio library).

  Read the DDS from the data stream. Leave the binary information behind. The
  DDS is moved, without parsing it, into a file and a pointer to that FILE is
  returned. The argument IN (the input FILE stream) is positioned so that the
  next byte is the binary data.

  The binary data follows the text `Data:', which itself starts a line.

  Returns: a FILE * which contains the DDS describing the binary information
  in IF.
*/

FILE *
move_dds(FILE *in)
{

    char *c = tempnam(NULL, DODS_DDS_PRX);
    FILE *fp = fopen(c, "w+");

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

    fclose(fp);		/* once full, close file */
    
    fp = fopen(c, "r");	/* get file pointer */
    if (unlink(c) < 0)	/* now when fp is closed, file is rm'd */
	err_sys("Could not unlink tmp file %s", c);

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
constrained_trans(String dds_name) 
{
    bool status;
    FILE *pin, *pout;
    DDS server;

    cerr << "The complete DDS:" << endl;
    read_table(server, dds_name, true);

    status = loopback_pipe(&pout, &pin);
    if (!status) {
	cerr << "Could not create the loopback streams" << endl;
	return false;
    }

#ifdef NEVER
    // test code; you have to close pout in order to read from pin. Why?
    server.print(pout);

    fclose(pout);

    DDS client;

    client.parse(pin);

    client.print();
#endif

    // Simulate Connect::request_dds() at the begining of a virtual connection
    DDS client = server;

    // The client gets this information from the API calls the user program
    // makes; it then sends it over to the server using various parameters.

    // read in variable name and CE
    cout << "Variable:";
    String variable;
    cin >> variable;

    cout << "Constraint:";
    String ce;
    cin >> ce;

    // send the variable given the constraint (dataset is ignored by the Test
    // classes; true flushes the I/O channel.
    if (!server.send("dummy", variable, ce, true, pout)) {
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
    if (!dds.parse(dds_fp)) {
	cerr << "Could not parse return data description" << endl;
	return false;
    }
    fclose(dds_fp);

    cerr << "The constrained DDS:" << endl;
    dds.print();

    // now arrange to read the data via the appropriate variable.  NB:
    // Since all BaseTypes share I/O, this works. However, it will have
    // to be changed when BaseType is modified to handle several
    // simultaneous reads.

    set_xdrin(pin);
    
    // Back on the client side; deserialize the data *using the newly
    // generated DDS* (the one sent with the data).

    if (!dds.var(variable)->deserialize())
	return false;
    
    cerr << "The data:" << endl;
    dds.var(variable)->print_val(cout);
    
    return true;
}






