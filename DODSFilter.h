
// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997
// Please first read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//	jhrg,jimg	James Gallagher (jgallagher@gso.uri.edu)

// $Log: DODSFilter.h,v $
// Revision 1.2  1997/09/22 23:04:59  jimg
// Added doc++ style comments.
//
// Revision 1.1  1997/08/28 20:39:02  jimg
// Created
//

#ifndef _DODSFilter_h
#define _DODSFilter_h

#ifdef __GNUG__
#pragma "interface"
#endif

#include <String.h>

/** This class contains common functionality for the filter programs used to
  make up the DODS data servers. Each of these filters is called by a CGI
  script with information about the user's request. The filter writes
  information to stdout to send a response back to the user's program. Note
  that the filter programs do not *have* to be called by a CGI
  script/program, but that is the normal mechanism by which they are invoked.
  
  @author jhrg 8/26/97 */

class DODSFilter {
private:
    bool comp;			// True if the output should be compressed.
    bool ver;			// True if the caller wants version info.
    bool bad_options;		// True if the options (argc,argv) are bad.

    String program_name;	// Name of the filter program
    String dataset;		// Name of the dataset/database 
    String ce;			// Constraint expression 
    String cgi_ver;		// Version of CGI script (caller)
    String anc_dir;		// Look here for ancillary files
    String anc_file;		// Use this for ancillary file name

    DODSFilter() {}		// Private default ctor.

public:
    /** Create an instance of DODSFilter using the command line arguments
      passed by the CGI (or other) program.
      Note: The default ctor is private; this and the copy ctor are the only
      way to create an instance of DODSFilter. */ 
    DODSFilter(int argc, char *argv[]);

    virtual ~DODSFilter();

    /** Class invariant. Use this to test for correct options passed via
      argc/argv. 
      @return True if the class state is OK, false otherwise. */
    bool OK();

    /** Should the filter send version information to the client program?
      @return True if the -v option was given indicating that the filter
      should send version information back to the client, false otherwise. */
    bool version();

    /** Get the constraint expression. 
      @return A String object that contains the constraint expression. */
    String get_ce();

    /** Get the dataset name. We define `dataset name to be the filename or
      other string that the filter program will use to access the data. In
      many cases it is the name of a database.
      @return A String object that contains the name of the dataset. */
    String get_dataset_name();

    /** Get the version information for the dataset. To read version
      information that is specific to a certain dataset, override this mfunc
      with an implementation that does what you want. By default, this
      returns an empty string.
      @return A String object that contains the dataset version information.
      */ 
    virtual String get_dataset_version();

    /** Read the ancillary DAS information and merge it into the DAS object
      #das#. 
      @return True if an ancillary DAS was found, false otherwise. */
    bool read_ancillary_das(DAS &das);

    /** Read the ancillary DDS information and merge it into the DDS object
      #dds#. 
      @return True if an ancillary DDS was found, false otherwise. */
    bool read_ancillary_dds(DDS &dds);

    /** Print usage information for a filter program. This is sent to stderr
      and will show up in the httpd log file. In addition, an error object is
      sent back to the client program telling them that the server is hosed. */
    void print_usage();

    /** Send version information back to the client program. */ 
    void send_version_info();

    /** Send the DAS object #das# back to the client program. 
      @return True if the operation succeeded, false otherwise. */
    bool send_das(DAS &das);

    /** Send the DDS object #dds# back to the client program. If
      #constrained# is true, evaluate the current constraint expression and
      send the `constrained DDS' back to the client.
      @return True if the operation succeeded, false otherwise. */
    bool send_dds(DDS &dds, bool constrained = false);

    /** Send the data in the DDS object #dds# back to the client program. 
     @return True if the operation succeeded, false otherwise. */
    bool send_data(DDS &dds, FILE *data_stream);
};

#endif // _DODSFilter_h
