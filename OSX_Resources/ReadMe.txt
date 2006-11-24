
$Id$

Notes for version 3.7.3 (24 Nov 2006)

This version of libdap contains a beta release of the
server-side functions geogrid(), geoarray(), linear_scale()
and version(). These can be used to select parts of Grids
and Arrays using latitude and longitude values instead of
array position indexes. The linear_scale() function can be
used to scale variables (including those return by other
function) using 'y = mx + b'. The version() function can be
used to find out which versions of the functions are
installed.

EXAMPLES

To get version information use the 'version()'
function. Currently, version() can only be called when
asking for data, and you must give the name of a data
source, although in the default version of version() the
data source is not used. The version function takes one
optional argument which may be the strings 'help' or
'xml'. Use 'help' to get help on using the function; use
'xml' to get version information encoded using XML instead
of plain text:

<<< I formated these so they are more readable >>>

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

The geogrid function can only be used with variables that
are Grids:

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
    
Pass the name of the Grid variable and the upper-left and
lower-right corners of the lat/lon rectangle to
geogrid. Optionally, pass one or more relational expressions
to select parts of dimensions that are not lat/lon.

Note: in libdap 3.7.3 calling geogrid with a constraint on
each dimension may return incorrect values that indicate
missing data even though data should have been returned.

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

The geoarray() function works like geogrid() except that
it's used to select from an Array variable and not a
Grid. In addition to the four lat/lon values for selection
rectangle, the caller must supply the data's corner
points. A subsequent release of libdap will include a
version that reads the data extent from the data source when
possible so caller's won't normally have to know the data's
extent ahead of time.

The linear_scale() function take either one or three
arguments. The first (only) argument is the name of a
variable or the return from another function. This variable
will be scaled using the 'y = mx + b' equation where 'x' is
the value(s) of the input variable and 'm' and 'b' are read
from the data source using the values of attributes name
'scale_factor' and 'add_offset.' If these are not present,
or to over ride their values, m and b can be supplied using
the second and third arguments.

Note that there are still some problems with linear_scale()
in this release.

See NEWS and ChangeLog for information about other changes

Notes for version 3.7.2

This version of libdap is required for the 9/15/06 alpha
release of Server4.  The library now contains software which
enables Server4 to build the ASCII data response for all
types of variables, including Sequence and nested Sequence
variables. These features are additions to the API, so older
code will work just fine with the new library. See NEWS for
more specific info about bug fixes.

Notes for version 3.7.1

This is a bug fix release (mostly) made for users of the
netcdf client library who need a fix for a problem dealing
with attributes from the HDF4 server.

NOTES for version 3.7.0

This version includes new features and an implementation
change.

This version of libdap now returns the DAP protocol version
number, 3.1, in an HTTP response header. Use this to
determine which protocol version the library implements. The
inclusion of a protocol version number is the sole official
new feature of DAP 3.1. Use Connect::get_protocol() to get
the version number. Clients can use this to determine the
features supported by a server. The Connect::get_version()
method can still be used to get our server's implementation
version. The distinction is that as more groups provide
their own implementations of the DAP, the protocol version
will provide a way for clients to determine capabilities
independently of implementation.

The libdap library now contains an implementation of the DDX
object/response, although this is an alpha implementation
and it's actually been part of the library for some time
now. The implementation contained in this version of the
library is close enough to the version we intend for DAP4
that developers can start to use it. Most of the server
handlers will return DDXs when asked.

The DDX combines the information previously held by the DDS
and DAS objects, making it much easier to associate
attributes to variables. As the name suggests, the DDX uses
XML rather than curly-braces. You can drop the DDX into your
favorite XML parser and get a DOM tree; no need to use our
parsers.  However, libdap contains a nice SAX parser that
will build the libdap objects directly from the XML DDX
object/response. Also included in libdap are methods to
build a DDX using a DDS and DAS, so there's an easy
migration path for both servers and clients.

Finally, the library contains two structural changes. First,
the library named 'libdap' now holds the DAP implementation
while two new libraries, 'libdapclient' and 'libdapserver',
now hold the client and server helper classes which are not
strictly part of the DAP. Secondly, the DDS/DDX object now
takes the constraint evaluator as a parameter. The class
ConstraintEvaluator holds our default evaluator, but it's
now possible to use your own evaluator .

NOTES for version 3.6.1

Version 3.6.1 is bug fix release.

Updated for 3.6.0

NOTE: This version of libdap++ cannot be used to build
the 3.4.x and previous clients and/or servers. However,
client and servers built using this code _will_ work
with the older clients and servers. It may be possible
to use this with software that previously built using 
3.5.x; the determining factor is whether the software
used features that were marked as deprecated in the older
libdap since those have been removed in 3.6.0.

WHAT'S IN THIS PACKAGE?

This package contains the OPeNDAP C++ implementation
of the Data Access Protocol version 2 (DAP2) with some
extensions that will be part of DAP3. Documentation for
this software can be found on the OPeNDAP home page at
http://www.opendap.org/. The NASA/ESE RFC which
describes DAP2, implemented by the library, can be
found at http://spg.gsfc.nasa.gov/rfc/004/.

The DAP2 is used to provide a uniform way of accessing
a variety of different types of data across the
Internet. It was originally part of the DODS and then
NVODS projects. The focus of those projects was access
to Earth-Science data, so much of the software
developed using the DAP2 to date has centered on that
discipline. However, the DAP2 data model is very
general (and similar to a modern structured programming
language) so it can be applied to a wide variety of
fields.

The DAP2 is implemented as a set of C++ classes that
can be used to build data servers and clients. The
classes may be specialized to mimic the behavior of
other data access APIs, such as netCDF. In this way,
programs originally meant to work with local data in
those formats can be re-linked and equipped to work
with data stored remotely in many different formats.
The classes can also by specialized to build standalone
client programs.

The DAP2 is contained in a single library: libdap.
Also included in the library are classes and utility
functions which simplify building clients and servers.

WHAT ELSE IS THERE?

The README:dodsrc section describes the client-side
behavior which can be controlled using the .dodsrc
file. This includes client-side caching, proxy servers,
et c., and is described in a separate file so it's easy
to include in your clients.

The README:AIS section describes the prototype Ancillary
Information Service (AIS) included in this version of
the library. The AIS is (currently) a client-side
capability which provides a way to augment DAP
attributes. This is a very useful feature because it
can be used to add missing metadata to a data source.
The AIS is accessed by using the AISConnect class in
place of Connect in your client.

This directory also contains test programs for the
DAP2, a sample specialization of the classes, getdap (a
useful command-line web client created with DAP2) and
dap-config (a utility script to simplify linking with
libdap.a). Also included as of version 3.5.3 is
libdap.m4, an autoconf macro which developers can use
along with autoconf to test for libdap. This macro will
be installed in ${prefix}/share/aclocal and can be by
any package which uses autoconf for its builds. See the
file for more information.

We also have Java and C versions of the DAP2 library
which interoperate with software which uses this
library. In other words, client programs built with the
Java DAP2 implementation can communicate with servers
built with this (C++) implementation of the DAP2. The C
DAP2 library, called the Ocapi, only implements the
client-side part of the protocol. Clients written using
the Ocapi are interoperable with both the Java and C++
DAP2 libraries. Note that the Ocapi is in early beta
and available only from CVS at this time (5 May 2005).

COPYRIGHT INFORMATION

The OPeNDAP DAP library is copyrighted using the GNU
Lesser GPL. See the file COPYING or contact the Free
Software Foundation, Inc., at 59 Temple Place, Suite
330, Boston, MA 02111-1307 USA. Older versions of the
DAP were copyrighted by the University of Rhode Island
and Massachusetts Institute of Technology; see the file
COPYRIGHT_URI. The file deflate.c is also covered by
COPYRIGHT_W3C.

-----------------------------------------------------

README: dodsrc

The DODS client-side configuration file (.dodsrc) is
used to configure how clients cache responses and how
they interact with proxy servers. By default the
configuration file resides in a users home directory
and is called `.dodsrc'. This can be changed by
creating the environment variable DODS_CACHE_INIT and
setting it to the full pathname of the configuration
file.

If a DODS client starts and cannot find the
configuration file, then one with default parameters
will be created in the user's home directory. By
default caching will be enabled with a maximum cache
size of 20M and a default expiration time of 24 hours.
By default no proxy server is configured.

A sample configuration file looks like (the line
numbers are not part of the file; they've been added to
make the description clearer):

0       # Comments start with a `#' in the first column.
1	USE_CACHE=1
2	MAX_CACHE_SIZE=20
3	MAX_CACHED_OBJ=5
4	IGNORE_EXPIRES=0
5	CACHE_ROOT=/home/jimg/.dods_cache/
6	DEFAULT_EXPIRES=86400
7	PROXY_SERVER=http,http://dcz.dods.org/
8	PROXY_FOR=http://dax.po.gso.uri.edu/.*,http://dods.gso.uri.edu/
9	NO_PROXY_FOR=http,dcz.dods.org
10	AIS_DATABASE=.ais_sst_database
    
COMMENTS 
Starting a line with a `#' makes that line a comment. 

CACHING 

The parameters on lines 1 through 6 determine how the
DAP++ library will use its HTTP 1.1 cache. If the value
of USE_CACHE is 1, the cache is active. A value of 0
turns off the cache. Make sure you use zero (0) and not
the letter `O'.

The value of MAX_CACHE_SIZE sets the maximum size of
the cache in megabytes. Once the cache reaches this
size, caching more objects will cause cache garbage
collection. The algorithm used is to first purge the
cache of any stale entries and then remove remaining
entries starting with those that have the lowest hit
count. Garbage collection stops when 90% of the cache
has been purged.

The value of MAX_CACHED_OBJ sets the maximum size of
any individual object in the cache in megabytes.
Objects received from a server larger than this value
will not be cached even if there's room for them
without purging other objects.

The parameter CACHE_ROOT contains the pathname to the
cache's top directory. If two or more users want to
share a cache, then they must both have read and write
permissions to the cache root.

If the value of IGNORE_EXPIRES is 1, then Expires:
headers in response documents will be ignored. The
value of DEFAULT_EXPIRES sets the expiration time for
any response that does not include eitehr an Expires or
Last-Modified header. The value is given in seconds;
86,400 is 24 hours. In general you should *not* ignore
the Expires header; the server probably had a good
reason to send it along with the response. This
parameter is here for unusual situations.

Note: If a Last-Modified header is returned with the
response, and there's *no* Expires header, the
expiration time is is 10% of the difference between the
current time and the LM time or 48 hours, whichever is
smaller. Note that libwww ignores the DEFAULT_EXPIRES
time in this case. Any request made before the
expiration time will use the cached response without
validation. Any request made after the expiration time
will use a conditional GET. Servers that have been
upgraded to 3.2 or greater will return a 304 response
if the cached response is still valid or a new response
if it is not valid.

If the value of ALWAYS_VALIDATE is 1, then all accesses
will be validated with the origin server. A value of 0
causes libwww to use the more complex exipration and
validate algorithm.

PROXY SERVERS

Note that the parameters PROXY_SERVER, NO_PROXY and
PROXY_FOR may be repeated to add multiple proxy
servers, suppress proxy access for several hosts, etc.

Lines 7, 8 and 9 contain the parameters used to
configure a proxy server. The parameter PROXY_SERVER
configures a default proxy server. The format is

    PROXY_SERVER=<protocol>,<proxy host url>

Currently the only supported protocol is `http'. <proxy
host url> must be a full URL to the host running the
proxy server.

The PROXY_FOR parameter provides a way to specify that
URLs which match a regular expression should be
accessed using a particular proxy server. The syntax
for PROXY_FOR is:

     PROXY_FOR=<regular expression>,<proxy host url>[,<flags>]

Where:

<regular expression> is an expression which matches the
URL or group of URLs. For example
`http://dax.po.gso.uri.edu/.*\.hdf' would match a URL
ending in `.hdf' at dax.po.gso.uri.edu. The regular
expression uses the POSIX basic syntax.

<proxy host url> is the same as above.

<flags> is an optional integer that configures the
regular expression matcher. A value of zero sets the
default. The four flag values and there meanings are:

     If REG_EXTENDED is set, use POSIX extended syntax;
     otherwise, use POSIX basic syntax. To set this,
     add 1 to the value of <flags>.

     If REG_NEWLINE is set, then . and [^...] don't
     match newline. Also, regexec will try a match
     beginning after every newline. Set this by adding
     2 to <flags>.

     If REG_ICASE is set, then consider upper- and
     lowercase versions of letters to be equivalent
     when matching. Add 4 to <flags>.

     If REG_NOSUB is set, then when PREG is passed to
     regexec, that routine will report only success or
     failure, and nothing about the registers. Add 8 to
     <flags>.

The NO_PROXY parameter provides a simple way to say
that access to a certain host should never go through a
proxy without using the more complicated regular
expression syntax. The syntax of NO_PROXY is:

    NO_PROXY=<protocol>,<hostname>

where <protocol> is as for PROXY_SERVER
<hostname> is the name of the host, not a url.

---------------------------------------------------

README: AIS

Notes for the prototype AIS implementation:

How to use the AIS. These notes correspond to the
prototype AIS developed in March 2003. This version of
the AIS works for DAS objects only.

The AIS uses a configuration file which holds the
mappings between a primary data source (aka a DODS
server) and one or more AIS resources. The
configuration file to use is named in the .dodsrc file
using the AIS_DATABASE property.

The AIS configuration file is described by
src/dap/ais_database.dtd. A short file looks like:

    <?xml version="1.0" encoding="US-ASCII" standalone="no"?>
    <!DOCTYPE ais SYSTEM "http://www.opendap.org/ais/ais_database.dtd">

    <ais xmlns="http://xml.opendap.org/ais">

    <entry>
    <primary url="http://localhost/dods-test/nph-dods/data/nc/fnoc1.nc"/>
    <ancillary url="http://localhost/ais/fnoc1.nc.das"/>
    </entry>

    <entry>
    <primary url="http://localhost/dods-test/nph-dods/data/nc/fnoc2.nc"/>
    <ancillary rule="replace" url="ais_testsuite/fnoc2_replace.das"/>
    </entry>

    </ais>

Each entry must have one <primary> element and may have
one or more <ancillary> elements. The <ancillary>
elements are applied in the order they are listed. The
'rule' attribute of <ancillary> determines how each AIS
resource is combined with the primary resource. By
default, attributes in the AIS resource are combined so
that they overwrite existing attributes. If there's no
conflict, the new attribute is added. This applies to
containers as well as single attributes. The replace
rule causes the AIS resource attributes to completely
replace those of the primary resource. If the fallback
rule is specified, the AIS resource's attributes are
used only if the primary resource has *no* attributes.

How to use the AIS: 

Keep in mind that this is a prototype and is far, far
from 'feature-complete.'

There are two ways to use the AIS right now. First you
can use geturl's new -A option. This option tells
geturl that when it gets a DAS object, it should merge
into that object any AIS resources listed for the URL
in the current AIS database (found in the .dodsrc
file).

Second you can program with the AIS. The class
AISConnect can be used as a replacement for Connect.
This class subclasses Connect and adds the capacity to
use the AIS subsystem I've built. The AIS itself is
contained in the classes AISMerge, AISDatabaseParser
and AISResources.

****************************************************

Here are some notes I made while I wrote the code:

* AISConnect does not implement clones for Connect's
  deprecated methods. Should it? Maybe it's time to let
  those go...

* What happens when the same primary (data) source is
  listed more than once in the database?

* What about primary sources that are regular
  expressions? Right now lookup is ln(N) where N is the
  number of entries. Regexps make this linear in N.
  Bummer. I can use two databases, et c. but it gets
  messy very fast. So are wild cards important (it
  doesn't have to be regexps, those are just really
  easy to implement...)?

* I've changed the meaning of 'fallback' so that it
  works for a whole group of attributes rather than
  each individual attribute. My guess is that this will
  be really unpopular. But it was easy to code and I'm
  not convinced that fallback resources really make
  that much sense in practice.

* I think that for client-side AIS stuff, the code
  should be able to read from a remote XML file to get
  the database. Not hard to code. But I wonder how
  useful it would be? It might be cool because a client
  could get the benefit of a remote server but do all
  the work locally. That would 'distribute the AIS
  processing/network load.'

* How will we handle multiple AIS databases? I think
  this could be really important. I like the
  AIS-in-the-configuration-file less and less, until I
  think about trying to get people to buy into
  enveloped URLs...

* Is the database file simple enough for people to
  write by hand? (and one look and you can see why
  we're going to need wildcards... imagine 20K URLs).

* I still need to write a bunch more tests.

* I need to hack some code to do the AIS-in-the-URL
  thing. I figured out how to handle that without doing
  the entire ex pr parse on the client, at least for
  the DAS version ... haven't thought about the DDS &
  DataDDS much.

* I need to code some serious integration tests. Most
  of the AIS-specific code has unit tests (except for
  AISConnect, which is barely different from the
  AISMerge Test unit tests).
