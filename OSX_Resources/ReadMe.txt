

$Id$

Updated for Version 3.7.10 (28 November 2007)

A bug fix release. See NEWS.

Updated for Version 3.7.9 (13 Novenber 2007)

This release is a bug fix and refactoring release. Old classes which were nolonger used have been removed, the FILE* output methods are slated to bereplaced with ones which will use iostream and will support a chuckedtransfer 'Marshaller,' and the transfer_data() methods have been made aformal part of the library, implemented for all classes, fixed and renamed tointern_data(). Many bugs int eh library were also fixed.

Updated for version 3.7.8 (26 June 2007)

The major fixes in this version are memory errors found and fixed in theRegex class and HTTP header processing software. This version also supportspkg-config on hosts that have that installed.

See NEWS for more information about changes for this version and ChangeLogfor the gory details.

Notes for version 3.7.7 (2 May 2007)

The major fix here is to the source build. We've fixed the issue where sourcebuilds failed to make the dapserver and dapclient libraries.

Notes for version 3.7.6 (12 March 2007)

Two bug fixes, both minor. Problems in the linear_scale() constraintexpression function and a bad/missing #include in GNURegex.h were fixed.

There was an error in the INSTALL file sent out in the previous release. Itsaid this library implemented DAP version 3.2, but in fact it implementsversion 3.1. The version 3.2 release will be along soon (RSN).

Notes for version 3.7.5 (7 Feb 2007)

This version includes many fixes from the first Server4 beta releaseplus fixes for the server-side functions. It also includes a smootherWin32 build.

Notes for version 3.7.4 (2 Jan 2007)

Release for the Server4 beta release.

Notes for version 3.7.3 (24 Nov 2006)

This version of libdap contains a beta release of the server-side functionsgeogrid(), geoarray(), linear_scale() and version(). These can be used toselect parts of Grids and Arrays using latitude and longitude values insteadof array position indexes. The linear_scale() function can be used to scalevariables (including those return by other function) using 'y = mx + b'. Theversion() function can be used to find out which versions of the functions areinstalled.

EXAMPLES

To get version information use the 'version()' function. Currently, version()can only be called when asking for data, and you must give the name of a datasource, although in the default version of version() the data source is notused. The version function takes one optional argument which may be the strings'help' or 'xml'. Use 'help' to get help on using the function; use 'xml' to getversion information encoded using XML instead of plain text:

[jimg@zoe libdap]$ url=http://test.opendap.org/dap/data/nc/coads_climatology.nc
[jimg@zoe libdap]$ ./getdap -D "$url?version()"
The data:
String version = "Function set: version 1.0, grid 1.0, geogrid 1.0b2, 
		          geoarray 0.9b1, linear_scale 1.0b1";

[jimg@zoe libdap]$ ./getdap -D "$url?version(help)"
The data:
String version = "Usage: version() returns plain text information about ...

[jimg@zoe libdap]$ ./getdap -D "$url?version(xml)"
The data:
String version = "<?xml version=\"1.0\"?>
    <functions>
        <function name=\"version\" version=\"1.0\"/>
        <function name=\"grid\" version=\"1.0\"/>
        <function name=\"geogrid\" version=\"1.0\"/>
        <function name=\"geoarray\" version=\"1.0\"/>
        <function name=\"linear_scale\" version=\"1.0\"/>
    </functions>";

The geogrid function can only be used with variables that are Grids:

[jimg@zoe libdap]$ getdap -d "$url"
Dataset {
    Float64 COADSX[COADSX = 180];
    Float64 COADSY[COADSY = 90];
    Float64 TIME[TIME = 12];
    Grid {
      Array:
        Float32 SST[TIME = 12][COADSY = 90][COADSX = 180];
      Maps:
        Float64 TIME[TIME = 12];
        Float64 COADSY[COADSY = 90];
        Float64 COADSX[COADSX = 180];
    } SST;
    Grid {
    .
    .
    .


Pass the name of the Grid variable and the upper-left and lower-right corners of the lat/lon rectangle to geogrid. Optionally, pass one or more relationalexpressions to select parts of dimensions that are not lat/lon. 

Note: in libdap 3.7.3 calling geogrid with a constraint on each dimensionmay return incorrect values that indicate missing data even though data shouldhave been returned.

[jimg@zoe libdap]$ getdap -D "$url?geogrid(SST,30,-60,20,-60,\"TIME=366\")"
The data:
Grid {
  Array:
    Float32 SST[TIME = 1][COADSY = 7][COADSX = 2];
  Maps:
    Float64 TIME[TIME = 1];
    Float64 COADSY[COADSY = 7];
    Float64 COADSX[COADSX = 2];
} SST = {  Array: {{{24.4364, 25.0923},{23.7465, 24.4146},{19.843, 23.6033},
{16.8464, 17.7756},{16.65, 16.818},{-1e+34, 15.3656},{18.7214, 13.1286}}}  
Maps: {366}, {19, 21, 23, 25, 27, 29, 31}, {-61, -59} };


The geoarray() function works like geogrid() except that it's used to selectfrom an Array variable and not a Grid. In addition to the four lat/lon valuesfor selection rectangle, the caller must supply the data's corner points. A subsequent release of libdap will include a version that reads the data extentfrom the data source when possible so caller's won't normally have to know thedata's extent ahead of time.

The linear_scale() function take either one or three arguments. The first(only) argument is the name of a variable or the return from anotherfunction. This variable will be scaled using the 'y = mx + b' equation where'x' is the value(s) of the input variable and 'm' and 'b' are read from thedata source using the values of attributes name 'scale_factor' and'add_offset.' If these are not present, or to over ride their values, m and bcan be supplied using the second and third arguments.

Note that there are still some problems with linear_scale() in this release.

See NEWS and ChangeLog for information about other changes

Notes for version 3.7.2

This version of libdap is required for the 9/15/06 alpha release of Server4.The library now contains software which enables Server4 to build the ASCIIdata response for all types of variables, including Sequence and nestedSequence variables. These features are additions to the API, so older codewill work just fine with the new library. See NEWS for more specific infoabout bug fixes.

Notes for version 3.7.1

This is a bug fix release (mostly) made for users of the netcdf clientlibrary who need a fix for a problem dealing with attributes from the HDF4server. 

NOTES for version 3.7.0

This version includes new features and an implementation change.

This version of libdap now returns the DAP protocol version number, 3.1, inan HTTP response header. Use this to determine which protocol version thelibrary implements. The inclusion of a protocol version number is the soleofficial new feature of DAP 3.1. Use Connect::get_protocol() to get theversion number. Clients can use this to determine the features supported by aserver. The Connect::get_version() method can still be used to get ourserver's implementation version. The distinction is that as more groupsprovide their own implementations of the DAP, the protocol version willprovide a way for clients to determine capabilities independently ofimplementation.

The libdap library now contains an implementation of the DDX object/response,although this is an alpha implementation and it's actually been part of thelibrary for some time now. The implementation contained in this version ofthe library is close enough to the version we intend for DAP4 that developerscan start to use it. Most of the server handlers will return DDXs when asked.

The DDX combines the information previously held by the DDS and DAS objects,making it much easier to associate attributes to variables. As the namesuggests, the DDX uses XML rather than curly-braces. You can drop the DDXinto your favorite XML parser and get a DOM tree; no need to use our parsers.However, libdap contains a nice SAX parser that will build the libdap objectsdirectly from the XML DDX object/response. Also included in libdap aremethods to build a DDX using a DDS and DAS, so there's an easy migration pathfor both servers and clients.

Finally, the library contains two structural changes. First, the librarynamed 'libdap' now holds the DAP implementation while two new libraries,'libdapclient' and 'libdapserver', now hold the client and server helperclasses which are not strictly part of the DAP. Secondly, the DDS/DDX objectnow takes the constraint evaluator as a parameter. The classConstraintEvaluator holds our default evaluator, but it's now possible to useyour own evaluator .

NOTES for version 3.6.1

Version 3.6.1 is bug fix release.

NOTES for version 3.6.0

This version of the library may not work older source code. Many of the deprecated methods have been removed. 

Added are headers which send information about the version of the DAP protocolthat the library implements (in contrast to the implementation of the libraryitself). A new header named XOPeNDAP-Server is used to send information aboutthe implementation of servers.

The libtool interface version has been incremented from 3 to 4 (these versionsdo no track the software's release version since several releases might present compatible binary interfaces). 

NOTES for version 3.5.3

This version of libdap++ cannot be used to build the 3.4.x and previousclients and/or servers. However, client and servers built using this code_will_ work with the older clients and servers.

WHAT'S IN THIS DIRECTORY?

This directory contains the OPeNDAP C++ implementation of the DataAccess Protocol version 2 (DAP2) with some extensions that will bepart of DAP3.  Documentation for this software can be found on theOPeNDAP home page at http://www.opendap.org/. The NASA/ESE RFC whichdescribes DAP2, implemented by the library, can be found athttp://spg.gsfc.nasa.gov/rfc/004/.

The DAP2 is used to provide a uniform way of accessing a variety ofdifferent types of data across the Internet. It was originally part ofthe DODS and then NVODS projects. The focus of those projects wasaccess to Earth-Science data, so much of the software developed usingthe DAP2 to date has centered on that discipline. However, the DAP2data model is very general (and similar to a modern structuredprogramming language) so it can be applied to a wide variety offields.

The DAP2 is implemented as a set of C++ classes that can be used tobuild data servers and clients. The classes may be specialized tomimic the behavior of other data access APIs, such as netCDF. In thisway, programs originally meant to work with local data in thoseformats can be re-linked and equipped to work with data storedremotely in many different formats.  The classes can also byspecialized to build standalone client programs.

The DAP2 is contained in a single library: libdap++.a. Also includedin the library are classes and utility functions which simplifybuilding clients and servers.

WHAT ELSE IS THERE?

The file README.dodsrc describes the client-side behavior which can becontrolled using the .dodsrc file. This includes client-side caching,proxy servers, et c., and is described in a separate file so it's easyto include in your clients.

The file README.AIS describes the prototype Ancillary InformationService (AIS) included in this version of the library. The AIS is(currently) a client-side capability which provides a way to augmentDAP attributes. This is a very useful feature because it can be usedto add missing metadata to a data source. The AIS is accessed by usingthe AISConnect class in place of Connect in your client.

This directory also contains test programs for the DAP2, a samplespecialization of the classes, getdap (a useful command-line webclient created with DAP2) and dap-config (a utility script to simplifylinking with libdap.a). Also included as of version 3.5.2 islibdap.m4, an autoconf macro which developers can use along withautoconf to test for libdap. This macro will be installed in${prefix}/share/aclocal and can be by any package which uses autoconffor its builds. See the file for more information.

We also have Java and C versions of the DAP2 library whichinter-operate with software which uses this library. In other words,client programs built with the Java DAP2 implementation cancommunicate with servers built with this (C++) implementation of theDAP2. The C DAP2 library, called the Ocapi, only implements theclient-side part of the protocol. Clients written using the Ocapi areinteroperable with both the Java and C++ DAP2 libraries. Note that theOcapi is in early beta and available only from CVS at this time (5 May2005).

THREAD SAFETY

We don't need to do this since the STL is also not thread safe. Usersof libdap have to be sure that multiple threads never makesimultaneous and/or overlapping calls to a single copy of libdap. Ifseveral threads are part of a program and each will make calls tolibdap, either those threads must synchronize their calls or arrangeto each use their own copy of libdap.  Some aspects of the library''are'' thread-safe: the singleton classes are all protected as is theHTTP cache (which uses the local file system).

INSTALLATION INSTRUCTIONS

See the file INSTALL in this directory for information on building thelibrary and the geturl client.

COPYRIGHT INFORMATION

The OPeNDAP DAP library is copyrighted using the GNU Lesser GPL. Seethe file COPYING or contact the Free Software Foundation, Inc., at 59Temple Place, Suite 330, Boston, MA 02111-1307 USA. Older versions ofthe DAP were copyrighted by the University of Rhode Island andMassachusetts Institute of Technology; see the file COPYRIGHT_URI. Thefile deflate.c is also covered by COPYRIGHT_W3C.