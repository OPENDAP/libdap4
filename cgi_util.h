
// -*- c++ -*-

// (c) COPYRIGHT URI/MIT 1995-1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher (jgallagher@gso.uri.edu)
//      reza            Reza Nekovei (reza@intcomm.net)

// External definitions for utility functions used by servers.
//
// 2/22/95 jhrg

#ifndef _cgi_util_h
#define _cgi_util_h

#ifndef _connect_h
#include "Connect.h"		// For ObjectType and EncodingType defs 
#endif


/** The CGI utilities include a variety of functions useful to
    programmers developing DODS CGI filter programs. However, before jumping
    in and using these, look at the class DODSFilter. Always choose to use
    that class over these functions if you can.

    @name CGI Utilities
    @memo A collection of useful functions for writing DODS servers.
    @see DODSFilter
    */

//@{

#if 0
/** This function prints a usage message for the filter program in the
    server log file.  This is meant to report errors in a filter
    program's configuration.  These are errors made by the server
    administrator, not by a user.

    The function also returns a DODS error object to the user.

    @memo Send a usage message to the server log.
    @param name The name of the filter program.
    */
void usage(const char *name);
void usage(const string &name);
#endif

/** This sends a formatted block of text to the client, containing
    version information about various aspects of the server.  The
    arguments allow you to enclose version information about the
    filter program and the dataset in the message.  Either argument
    (or both) may be omitted, in which case no script or dataset
    version information will be printed.

    @memo Send a version number.
    @param script_ver The version of the filter script executing this
    function. 
    @param dataset_ver The version of the dataset.
    @return TRUE for success.
    */
bool do_version(const string &script_ver, const string &dataset_ver);

/** This function sends data to the client.

    @memo Send data.
    @param compression A Boolean value indicating whether the data is
    to be sent in compressed form or not.  A value of TRUE indicates
    that the data is to be comressed.
    @param data_stream A file pointer indicating where to print the
    data.  This is typically an XDR pointer.
    @param dds The DODS Dataset Descriptor Structure of the dataset
    whose data is to be sent.  The #do_data_transfer# uses the #send#
    method of this DDS to do the sending.
    @param dataset The (local) name of the dataset whose data is to be
    sent.
    @param constraint A constraint expression that may have been sent
    with the original data request.
    @return TRUE for success, FALSE otherwise.
    */
bool do_data_transfer(bool compression, FILE *data_stream, DDS &dds,
		      const string &dataset, const string &constraint);

/** This function accepts a dataset path name, and searches for a
    matching ancillary data file name with a very specific set of
    search rules, given here:

    \begin{verbatim}
    directory           filename          extension
      same                same            `.'given
      given               same            `.'given
      same                given           `.'given
      given               given           `.'given
    \end{verbatim}

    Where ``same'' refers to the input dataset pathname, and ``given''
    refers to the function arguments.

    For example, If you call this function with a
    dataset name of #/a/data#, an extension of #das#, a directory of
    #b#, and a filename of #ralph#, the function will look (in order)
    for the following files:

    \begin{verbatim}
    /a/data.das
    /b/data.das
    /a/ralph.das
    /b/ralph.das
    \end{verbatim}

    The function will return a string containing the name of the first
    file in the list that exists, if any. 

    @memo Find a file with ancillary data.  
    @param pathname The input pathname of a dataset.
    @param ext The input extension the desired file is to have.
    @param dir The input directory in which the desired file may be
    found. 
    @param file The input filename the desired file may have.
    @return A string containing the pathname of the file found by
    searching with the given components.  If no file was found, the
    null string is returned.
    */
string find_ancillary_file(string pathname, string ext, string dir, 
			   string file);

/** This function searches for an ancillary data file (using the
    #find_ancillary_file# function) containing the DDS for a
    dataset. If such a file is found, the function reads it with the
    #parse# method of the given DDS.

    A DDS ancillary file ends with #.dds#.

    @memo Read an ancillary DDS.
    @param dds The DDS of a dataset.  The ancillary DDS, if any, is
    ``folded'' into the existing DDS information.
    @param dataset A string containing the name of the dataset whose
    DDS is to be read.
    @param dir An optional parameter specifying a secondary directory
    in which to look for an ancillary DDS file. This is used as an
    argument to #find_ancillary_file#.
    @param dir An optional parameter specifying a file name with which
    to look for an ancillary DDS file. This is used as an
    argument to #find_ancillary_file#.
    @return TRUE if the dataset DDS was found and read, FALSE
    otherwise. 
    */
bool read_ancillary_dds(DDS &dds, string dataset, string dir = "", 
			string file = "");

/** This function searches for an ancillary data file (using the
    #find_ancillary_file# function) containing the DAS for a
    dataset. If such a file is found, the function reads it with the
    #parse# method of the given DAS.

    A DAS ancillary file ends with #.das#.

    @memo Read an ancillary DAS.
    @param das The DAS of a dataset.  The ancillary DAS, if any, is
    ``folded'' into the existing DAS information.
    @param dataset A string containing the name of the dataset whose
    DAS is to be read.
    @param dir An optional parameter specifying a secondary directory
    in which to look for an ancillary DAS file. This is used as an
    argument to #find_ancillary_file#.
    @param dir An optional parameter specifying a file name with which
    to look for an ancillary DAS file. This is used as an
    argument to #find_ancillary_file#.
    @return TRUE if the dataset DAS was found and read, FALSE
    otherwise. 
    */
bool read_ancillary_das(DAS &das, string dataset, string dir = "", 
			string file = "");

/** Prints an error message in the #httpd# system log file, along with
    a time stamp and the client host name (or address).

    @memo Logs an error message.
    */
void ErrMsgT(const string &Msgt);

/** Given a pathname, this function returns just the file name
    component of the path.  That is, given #/a/b/c/ralph.nc.das#, it
    returns #ralph.nc#.

    @memo Returns the filename portion of a pathname.
    @param path A C-style simple string containing a pathname to be
    parsed. 
    @return A C-style simple string containing the filename component
    of the given pathname.
    */
char *name_path(const char *path);
string name_path(const string &path);

/** These functions are used to create the MIME header for a message
    from a server to a client.  They can, of course, be used for any
    other suitable purpose.

    @memo Use these functions to create a MIME header.
    @name MIME utility functions
    */

//@{
/** The reply to a DODS client is in the form of a multi-part MIME
    message.  You can use this function to create a MIME header for a
    text message.

    Here is an example of its use.  This is the DAS filter program
    from the Matlab server.  (Simplified to show off its structure.) 

    \begin{verbatim}

    extern DAS *read_attributes(const char *filename);

    int 
    main(int argc, char * argv[])
    {

      set_mime_text(dods_das);

      string dataset = argv[1];

      // Read the matlab string variables for attributes
      DAS *das_table = read_attributes(dataset);

      // Forward the DAS class over the network
      das_table->print();

      return 0;                        
    }
    \end{verbatim}

    The #ObjectType# and #EncodingType# enums are defined in
    #Connect.h#. 

    @memo Set the MIME type to text.
    @param os A Stream object to which the MIME header is written.
    @param type An #ObjectType# enum indicating the DODS type of the
    object. 
    @param enc An #EncodingType# switch indicating whether the data is
    to be compressed or not.
    @see ObjectType
    @see EncodingType
    @see Connect
    */
void set_mime_text(ostream &os, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain);
/**
   @param out A FILE pointer to which the MIME header is written. */
void set_mime_text(FILE *out, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain);

/** The reply to a DODS client is in the form of a multi-part MIME
    message.  You can use this function to create a MIME header for a
    message containing binary data.  This function is called from
    within the #DDS::send# function, so it is rarely necessary in an
    application program.

    @memo Create MIME headers for binary data.
    @param os A Stream object to which the MIME header is written.
    @param type An #ObjectType# enum indicating the DODS type of the
    object. 
    @param enc An #EncodingType# switch indicating whether the data is
    to be compressed or not.
    @see ObjectType
    @see EncodingType
    @see DDS
    */

void set_mime_binary(ostream &os, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain);
/**
   @param out A FILE pointer to which the MIME header is written. */
void set_mime_binary(FILE *out, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain);

/** The reply to a DODS client is in the form of a multi-part MIME
    message.  You can use this function to create a MIME header for a
    message signaling an error.

    Here is an example of use from the JGOFS DDS filter program:

    \begin{verbatim}
    if(argc < 2) {
      ErrMsgT("One parameter, filename, must be sent (jg_dds)");
      set_mime_error(HTERR_NO_CONTENT, 
                     "Internal error in DODS attribute server");
      exit(1);
    }
    \end{verbatim}

    @memo Set the MIME text type to ``error.''
    @param os A Stream object to which the MIME header is written.
    @param code An error code for the given error. 
    @param reason A simple character string with a message to be sent
    to the client.
    @see ErrMsgT
    */
void set_mime_error(ostream &os, int code = HTERR_NOT_FOUND, 
		    const string &reason = "Dataset not found",
		    const string &version = "");
/**
   @param out A FILE pointer to which the MIME header is written. */
void set_mime_error(FILE *out, int code = HTERR_NOT_FOUND, 
		    const string &reason = "Dataset not found",
		    const string &version = "");

//@}
//@}

#endif // _cgi_util_h
