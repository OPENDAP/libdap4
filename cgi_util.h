
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
string name_path(const string &path);

/** Given a constant pointer to a time_t, return a RFC 822/1123 style date.
    This function returns the RFC 822 date with the exception that the RFC
    1123 modification for four-digit years is implemented. The date is
    returned in a statically allocated char array so it must be copied before
    being used.

    @return The RFC 822/1123 style date in a statically allocated char [].
    @param t A const time_t pointer. */
char *rfc822_date(const time_t t);

/** Get the last modified time. Assume #name# is a file and find its last
    modified time. If #name# is not a file, then return now as the last
    modified time. 
    @param name The name of a file.
    @return The last modified time or the current time. */
time_t last_modified_time(string name);

/** These functions are used to create the MIME header for a message
    from a server to a client. They are public but should not be called
    directly unless necessary. Use DODSFilter instead.

    NB: These functions actually write both the response status line
    \emph{and} the header.

    @memo Functions to create a MIME header.
    @name MIME utility functions
    @see DODSFilter
*/

//@{
/** Use this function to create a MIME header for a text message.

    @memo Set the MIME type to text.
    @param os Write the MIME header to this stream.
    @param type The type of the response (i.e., is it a DAS, DDS, et cetera).
    @param version The version of the server.
    @param enc Indicates an encoding was applied to the response payload.
    Used primarily to tell clients they need to decompress the payload.
    @param last_modified A RFC 822 date which gives the time the information
    in the repsonse payload was last changed.
    @see ObjectType
    @see EncodingType
    @see Connect */
void set_mime_text(ostream &os, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);
/**
   @param out Write the MIME header to this FILE pointer. */
void set_mime_text(FILE *out, ObjectType type = unknown_type, 
		   const string &version = "", EncodingType enc = x_plain,
		   const time_t last_modified = 0);

/** Use this function to create a MIME header for a message containing binary
    data.

    @memo Create MIME headers for binary data.
    @param os Write the MIME header to this stream.
    @param type The type of the response (i.e., is it data, et cetera).
    @param version The version of the server.
    @param enc Indicates an encoding was applied to the response payload.
    Used primarily to tell clients they need to decompress the payload.
    @param last_modified A RFC 822 date which gives the time the information
    in the repsonse payload was last changed.
    @see ObjectType
    @see EncodingType
    @see DDS */

void set_mime_binary(ostream &os, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain,
		     const time_t last_modified = 0);
/**
   @param out Write the MIME header to this FILE pointer. */
void set_mime_binary(FILE *out, ObjectType type = unknown_type, 
		     const string &version = "", EncodingType enc = x_plain,
		     const time_t last_modified = 0);

/** Use this function to create a MIME header for a message signaling an
    error.

    @memo Set the MIME text type to ``error.''
    @param os Write the MIME header to this stream.
    @param code An error code for the given error. 
    @param reason A message to be sent to the client.
    @see ErrMsgT */
void set_mime_error(ostream &os, int code = HTERR_NOT_FOUND, 
		    const string &reason = "Dataset not found",
		    const string &version = "");
/**
   @param out Write the MIME header to this FILE pointer. */
void set_mime_error(FILE *out, int code = HTERR_NOT_FOUND, 
		    const string &reason = "Dataset not found",
		    const string &version = "");

/** Use this function to create a response signalling that the target of a
    conditional get has not been modified relative to the condition given in
    the request. For DODS this will have to be a date until the servers
    support ETags

    @memo Send a `Not Modified' response.
    @param os Write the MIME header to this stream. */
void set_mime_not_modified(ostream &os);
/**
   @param out Write the response to this FILE pointer. */
void set_mime_not_modified(FILE *out);

//@}

//@}

#endif // _cgi_util_h
