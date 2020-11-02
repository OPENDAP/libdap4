Please find the libdap4 API documentation here: https://opendap.github.io/libdap4/html/

### Updated for version 3.20.6  [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3759849.svg)](https://doi.org/10.5281/zenodo.3759849)
* Stopped CE parse errors from returning user supplied strings in error messages.
* README is now called README.md

### Updated for version 3.20.5  [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3641778.svg)](https://doi.org/10.5281/zenodo.3641778)
* Memory leaks. Minor bug fixes. Lots of work on CI.

### Updated for version 3.20.4 [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3267984.svg)](https://doi.org/10.5281/zenodo.3267984)
* Updated for version 3.20.4
* Memory leak fixes and C++11 features

### Updated for version 3.20.3 [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2566512.svg)](https://doi.org/10.5281/zenodo.2566512)
* Fixes and Debian packaging via Travis CI

### Updated for version 3.20.2 [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2002799.svg)](https://doi.org/10.5281/zenodo.2002799)
* Added libdap::Array::rename_dim()

### Updated for version 3.20.1
* Added Continuous Delivery for CentOS 6 and 7

### Updated for version 3.20.0
* We now have a Debian package for libdap.
* Bug fixes; See the ChangeLog and NEWS files.

### Updated for version 3.19.1
* Bug fixes; See the ChangeLog and NEWS files.

### Updated for version 3.18.3
* Bug fixes; See the ChangeLog and NEWS files.

### Updated for version 3.18.2
* Added support for DAP4 filter operations.
* For other information, see NEWS and ChangeLog

### Updated for 3.16.0

libdap now supports parallel I/O for certain data write operations,
mostly those involving larger amounts of data - Marshaller::put_vector
and put_vector_part. The behavior can be 'turned off' by #undef the
symbol USE_POSIX_THREADS in the XDSStreamMarshaller classes.

Added to libdap is the ability to send DAP2 responses in parts, so that
code that builds large responses from multiple reads can write a part
that is complete and then return to work on the next part. This is
especially beneficial for aggregations that cross many granules. The new
method is Marshaller::put_vector_part().

Added to libdap is a virtual void BaseType::clear_local_data() method
that can be used to free the memory used by a BaseType to hold data values.
This provides a way for the object to persist in memory without holding
onto all of its data (which can be a substantial part of the object's
total size).

Updated for 3.14.0

Bison 3 is required to build this code.

Updated for 3.13.3

Bug fix release

Updated for 3.13.2

Bug fix release

Updated for 3.13.1

A test baseline was updated to use both the DAP2 and DAP4 version of
the XDAP/X-DAP header. This change was likely over doing things on our
part, but it's important to have the source releases pass all their
tests.
 
Updated 3.13.0

Support for clang: Apple LLVM version 5.1 (clang-503.0.40) (based on
LLVM 3.4svn) added.

Updated for version 3.12.1

Note that the documented behavior of BaseType::read() is now correctly
implemented.

Updated for version 3.12.0

The server functions have been moved out of libdap and into their own 
BES module. Currently this modules is part of the BES, but that will change
in the future. This version of libdap supports building very large arrays
made up of constant value (e.g., to be used as masks in server functions
you write). 

For information on the way to make these functions, see:
http://docs.opendap.org/index.php/Expanded_arguments_for_Constraint_Expressions 
Note that this version of libdap requires bison 2.4. This is a change so
the parsers can use C++ I/O streams and we can eventually drop the FILE*
interfaces.

Updated for version 3.11.7

Minor tweak for the server function caching code: turn on or off the 
cache by adding or removing the directory /tmp/dap_functions_cache. 
If the directory is not present no caching of server function calls
is done. All the other behaviors are otherwise identical.

Updated for version 3.11.6

There is a new cache for some kinds of response objects. It's size and
location are currently fixed to 20GB bytes and 
"/tmp/dap_functions_cache/" but these will be made BES parameters in a
future release.

Updated for version 3.11.5

A memory leak in XDRStreamMarshaller was fixed.

A bug in DDS::add_var_nocopy() was fixed.

Updated for version 3.11.2

Bug fixes and improvements in the implementations of some methods.

Updated for version 3.11.1

Bug fixes only.

Updated for version 3.11.0

Now constraint expressions can have multiple function calls that return data.

I've bumped up the DAP version from 3.3 to 3.4 to reflect this change.

Updated for Version 3.10.2

BaseType::transfer_attributes() and related methods provide a way for
handlers to customize how attributes from a DAS object are merged into
a DDS.

In the past we supported a kind of client-side system that could augment
the attributes call the 'AIS' (Ancillary Information System). This has
been removed - our server now supports the NcML language to do much the same
thing but in a way that can be set on the server once for all users. It's also
an emerging convention that's gaining wide support within the community.

Updated for Version 3.10.0

DAP 3.3 is now supported; see http://docs.opendap.org/index.php/DAP3/4.

This version of libdap contains many changes that are needed for both
DAP 4 and the NcML handler. This version of the library is required
for the Hyrax 1.6 handlers.

The 'deflate' program is no longer part of this library package since
we are no longer supporting the old data server system (based on WWW's
CGI specification).

Updated for version 3.9.2

Now libdap supports DAP 3.2. You can read about the evolving DAP 3.x protocol
at http://docs.opendap.org/index.php/DAP3/4. If your client sends the
XDAP-Accept header with a value of 3.2 the DDX is different (it includes
protocol information and also an xmlbase element).

Behavior change for the DAS: In the past the format handlers added double
quotes to the values of string attributes when they added those values to the
AttrTable object. This meant that the value of the attribute in the C++
object was actually not correct since it contained quotes not found in the
original attribute value. I modified libdap so that if an attribute value in
the C++ AttrTable object does not have quotes then those quotes are added
when the value is output in a DAS response (but not a DDX since there's no
need to quote the value in that response). This ensures that the text in the
DAS wire representation will parse whether a handler has added quotes or not
(paving the way for fixed handlers). At the same time I fixed all of our
handlers so that they no longer add the erroneous quotes. This fixes a
problem with the DDX where the quotes were showing up as part of the
attribute value. The change to libdap is such that a broken handler will not
be any more broken but a fixed handler will work for both DAS and DDX
generation.

If you have a handler and it's not adding quotes to the String attribute 
values - good, don't change that! If your handler does add quotes, please
modify it so the DDX will be correct.  

Our handler's old, broken, behavior can be resurrected by removing the 
ATTR_STRING_QUOTE FIX define in the appropriate files.

Updated for version 3.8.2 (23 June 2008)

HTTP Cache and win32 installer fixes (the latter are actually in the 3.8.1
installer for winXP). API change: The functions used to merge ancillary data
have been moved to their own class (Ancillary).

Updated for version 3.8.1 (10 June 2008)

The syntax for PROXY_SERVER in the .dodsrc file was relaxed. See the .dodsrc
file for more information.

Updated for Version 3.8.0 (29 February 2008)

The libdap classes and code are now inside of the libdap namespace. In order
to access any of the classes, for example, you will need to do one of the
following. After including the libdap headers you can:

1. add a using statement for the entire libdap namespace:

using namespace libdap ;

2. add a using statement for the classes that you will be using:

using libdap::DAS ;

3. inside your code scope the use of libdap classes.

libdap::DAS *das = code_to_get_das() ;

Added method to HTTPCache to return not only the FILE pointer of a cached
response but also the name of the file in the cache, to allow for this file
name to be passed to data handlers in the BES to be read.

See NEWS for more information about changes for this version and ChangeLog
for the gory details.

Updated for Version 3.7.10 (28 November 2007)

A bug fix release. See NEWS.

Updated for Version 3.7.9 (13 November 2007)

This release is a bug fix and refactoring release. Old classes which were no
longer used have been removed, the FILE* output methods are slated to be
replaced with ones which will use iostream and will support a chucked
transfer 'Marshaller,' and the transfer_data() methods have been made a
formal part of the library, implemented for all classes, fixed and renamed to
intern_data(). Many bugs in the library were also fixed.

Updated for version 3.7.8 (26 June 2007)

The major fixes in this version are memory errors found and fixed in the
Regex class and HTTP header processing software. This version also supports
pkg-config on hosts that have that installed.

See NEWS for more information about changes for this version and ChangeLog
for the gory details.

Notes for version 3.7.7 (2 May 2007)

The major fix here is to the source build. We've fixed the issue where source
builds failed to make the dapserver and dapclient libraries.

Notes for version 3.7.6 (12 March 2007)

Two bug fixes, both minor. Problems in the linear_scale() constraint
expression function and a bad/missing #include in GNURegex.h were fixed.

There was an error in the INSTALL file sent out in the previous release. It
said this library implemented DAP version 3.2, but in fact it implements
version 3.1. The version 3.2 release will be along soon (RSN).

Notes for version 3.7.5 (7 Feb 2007)

This version includes many fixes from the first Server4 beta release
plus fixes for the server-side functions. It also includes a smoother
Win32 build.

Notes for version 3.7.4 (2 Jan 2007)

Release for the Server4 beta release.

Notes for version 3.7.3 (24 Nov 2006)

This version of libdap contains a beta release of the server-side functions
geogrid(), geoarray(), linear_scale() and version(). These can be used to
select parts of Grids and Arrays using latitude and longitude values instead
of array position indexes. The linear_scale() function can be used to scale
variables (including those return by other function) using 'y = mx + b'. The
version() function can be used to find out which versions of the functions are
installed.

EXAMPLES

To get version information use the 'version()' function. Currently, version()
can only be called when asking for data, and you must give the name of a data
source, although in the default version of version() the data source is not
used. The version function takes one optional argument which may be the strings
'help' or 'xml'. Use 'help' to get help on using the function; use 'xml' to get
version information encoded using XML instead of plain text:

<code>
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
</code>
    
Pass the name of the Grid variable and the upper-left and lower-right corners 
of the lat/lon rectangle to geogrid. Optionally, pass one or more relational
expressions to select parts of dimensions that are not lat/lon. 

Note: in libdap 3.7.3 calling geogrid with a constraint on each dimension
may return incorrect values that indicate missing data even though data should
have been returned.

<code>
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
</code>

The geoarray() function works like geogrid() except that it's used to select
from an Array variable and not a Grid. In addition to the four lat/lon values
for selection rectangle, the caller must supply the data's corner points. A 
subsequent release of libdap will include a version that reads the data extent
from the data source when possible so caller's won't normally have to know the
data's extent ahead of time.

The linear_scale() function take either one or three arguments. The first
(only) argument is the name of a variable or the return from another
function. This variable will be scaled using the 'y = mx + b' equation where
'x' is the value(s) of the input variable and 'm' and 'b' are read from the
data source using the values of attributes name 'scale_factor' and
'add_offset.' If these are not present, or to over ride their values, m and b
can be supplied using the second and third arguments.

Note that there are still some problems with linear_scale() in this release.

See NEWS and ChangeLog for information about other changes

Notes for version 3.7.2

This version of libdap is required for the 9/15/06 alpha release of Server4.
The library now contains software which enables Server4 to build the ASCII
data response for all types of variables, including Sequence and nested
Sequence variables. These features are additions to the API, so older code
will work just fine with the new library. See NEWS for more specific info
about bug fixes.

Notes for version 3.7.1

This is a bug fix release (mostly) made for users of the netcdf client
library who need a fix for a problem dealing with attributes from the HDF4
server. 

NOTES for version 3.7.0

This version includes new features and an implementation change.

This version of libdap now returns the DAP protocol version number, 3.1, in
an HTTP response header. Use this to determine which protocol version the
library implements. The inclusion of a protocol version number is the sole
official new feature of DAP 3.1. Use Connect::get_protocol() to get the
version number. Clients can use this to determine the features supported by a
server. The Connect::get_version() method can still be used to get our
server's implementation version. The distinction is that as more groups
provide their own implementations of the DAP, the protocol version will
provide a way for clients to determine capabilities independently of
implementation.

The libdap library now contains an implementation of the DDX object/response,
although this is an alpha implementation and it's actually been part of the
library for some time now. The implementation contained in this version of
the library is close enough to the version we intend for DAP4 that developers
can start to use it. Most of the server handlers will return DDXs when asked.

The DDX combines the information previously held by the DDS and DAS objects,
making it much easier to associate attributes to variables. As the name
suggests, the DDX uses XML rather than curly-braces. You can drop the DDX
into your favorite XML parser and get a DOM tree; no need to use our parsers.
However, libdap contains a nice SAX parser that will build the libdap objects
directly from the XML DDX object/response. Also included in libdap are
methods to build a DDX using a DDS and DAS, so there's an easy migration path
for both servers and clients.

Finally, the library contains two structural changes. First, the library
named 'libdap' now holds the DAP implementation while two new libraries,
'libdapclient' and 'libdapserver', now hold the client and server helper
classes which are not strictly part of the DAP. Secondly, the DDS/DDX object
now takes the constraint evaluator as a parameter. The class
ConstraintEvaluator holds our default evaluator, but it's now possible to use
your own evaluator .

NOTES for version 3.6.1

Version 3.6.1 is bug fix release.

NOTES for version 3.6.0

This version of the library may not work older source code. Many of the 
deprecated methods have been removed. 

Added are headers which send information about the version of the DAP protocol
that the library implements (in contrast to the implementation of the library
itself). A new header named XOPeNDAP-Server is used to send information about
the implementation of servers.

The libtool interface version has been incremented from 3 to 4 (these versions
do no track the software's release version since several releases might 
present compatible binary interfaces). 

NOTES for version 3.5.3

This version of libdap++ cannot be used to build the 3.4.x and previous
clients and/or servers. However, client and servers built using this code
_will_ work with the older clients and servers.

WHAT'S IN THIS DIRECTORY?

This directory contains the OPeNDAP C++ implementation of the Data
Access Protocol version 2 (DAP2) with some extensions that will be
part of DAP3.  Documentation for this software can be found on the
OPeNDAP home page at http://www.opendap.org/. The NASA/ESE RFC which
describes DAP2, implemented by the library, can be found at
http://spg.gsfc.nasa.gov/rfc/004/.

The DAP2 is used to provide a uniform way of accessing a variety of
different types of data across the Internet. It was originally part of
the DODS and then NVODS projects. The focus of those projects was
access to Earth-Science data, so much of the software developed using
the DAP2 to date has centered on that discipline. However, the DAP2
data model is very general (and similar to a modern structured
programming language) so it can be applied to a wide variety of
fields.

The DAP2 is implemented as a set of C++ classes that can be used to
build data servers and clients. The classes may be specialized to
mimic the behavior of other data access APIs, such as netCDF. In this
way, programs originally meant to work with local data in those
formats can be re-linked and equipped to work with data stored
remotely in many different formats.  The classes can also by
specialized to build standalone client programs.

The DAP2 is contained in a single library: libdap++.a. Also included
in the library are classes and utility functions which simplify
building clients and servers.

WHAT ELSE IS THERE?

The file README.dodsrc describes the client-side behavior which can be
controlled using the .dodsrc file. This includes client-side caching,
proxy servers, et c., and is described in a separate file so it's easy
to include in your clients.

The file README.AIS describes the prototype Ancillary Information
Service (AIS) included in this version of the library. The AIS is
(currently) a client-side capability which provides a way to augment
DAP attributes. This is a very useful feature because it can be used
to add missing metadata to a data source. The AIS is accessed by using
the AISConnect class in place of Connect in your client.

This directory also contains test programs for the DAP2, a sample
specialization of the classes, getdap (a useful command-line web
client created with DAP2) and dap-config (a utility script to simplify
linking with libdap.a). Also included as of version 3.5.2 is
libdap.m4, an autoconf macro which developers can use along with
autoconf to test for libdap. This macro will be installed in
${prefix}/share/aclocal and can be by any package which uses autoconf
for its builds. See the file for more information.

We also have Java and C versions of the DAP2 library which
inter-operate with software which uses this library. In other words,
client programs built with the Java DAP2 implementation can
communicate with servers built with this (C++) implementation of the
DAP2. The C DAP2 library, called the Ocapi, only implements the
client-side part of the protocol. Clients written using the Ocapi are
interoperable with both the Java and C++ DAP2 libraries. Note that the
Ocapi is in early beta and available only from CVS at this time (5 May
2005).
  
THREAD SAFETY

We don't need to do this since the STL is also not thread safe. Users
of libdap have to be sure that multiple threads never make
simultaneous and/or overlapping calls to a single copy of libdap. If
several threads are part of a program and each will make calls to
libdap, either those threads must synchronize their calls or arrange
to each use their own copy of libdap.  Some aspects of the library
are thread-safe: the singleton classes are all protected as is the
HTTP cache (which uses the local file system).

INSTALLATION INSTRUCTIONS

See the file INSTALL in this directory for information on building the
library and the geturl client.

COPYRIGHT INFORMATION

The OPeNDAP DAP library is copyrighted using the GNU Lesser GPL. See
the file COPYING or contact the Free Software Foundation, Inc., at 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA. Older versions of
the DAP were copyrighted by the University of Rhode Island and
Massachusetts Institute of Technology; see the file COPYRIGHT_URI. The
file deflate.c is also covered by COPYRIGHT_W3C.
