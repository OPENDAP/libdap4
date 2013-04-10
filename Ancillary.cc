// Ancillary.cc

#include "config.h"

//#define DODS_DEBUG

#include "Ancillary.h"
#include "debug.h"

#ifndef WIN32
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#else
#include <io.h>
#include <fcntl.h>
#include <process.h>
// Win32 does not define this. 08/21/02 jhrg
#define F_OK 0
#endif

namespace libdap {

/** This function accepts a dataset path name, and searches for a
    matching ancillary data file name with a very specific set of
    search rules, given here:

    <pre>
    directory           filename          extension
    same                same            `.'given
    given               same            `.'given
    same                given           `.'given
    given               given           `.'given
    </pre>

    Where ``same'' refers to the input dataset pathname, and ``given''
    refers to the function arguments.

    For example, If you call this function with a
    dataset name of <tt>/a/data</tt>, an extension of <tt>das</tt>, a
    directory of
    <tt>b</tt>, and a filename of <tt>ralph</tt>, the function will
    look (in order)
    for the following files:

    <pre>
    /a/data.das
    /b/data.das
    /a/ralph.das
    /b/ralph.das
    </pre>

    The function will return a string containing the name of the first
    file in the list that exists, if any.

    @note This code now checks for <code>pathname.ext</code> 3/17/99 jhrg

    @brief Find a file with ancillary data.
    @param pathname The input pathname of a dataset.
    @param ext The input extension the desired file is to have.
    @param dir The input directory in which the desired file may be
    found.
    @param file The input filename the desired file may have.
    @return A string containing the pathname of the file found by
    searching with the given components.  If no file was found, the
    null string is returned.
*/
string
Ancillary::find_ancillary_file( const string &pathname,
				const string &ext,
				const string &dir,
				const string &file )
{
    string::size_type slash = pathname.rfind('/') + 1;
    string directory = pathname.substr(0, slash);
    string filename = pathname.substr(slash);
    string basename = pathname.substr(slash, pathname.rfind('.') - slash);

    DBG(cerr << "find ancillary file params: " << pathname << ", " << ext
        << ", " << dir << ", " << file << endl);
    DBG(cerr << "find ancillary file comp: " << directory << ", " << filename
        << ", " << basename << endl);

    string dot_ext = "." + ext;

    string name = directory + basename + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = pathname + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = directory + ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = dir + basename + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = directory + file + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = dir + file + dot_ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    name = dir + ext;
    if (access(name.c_str(), F_OK) == 0)
        return name;

    return "";
}

// Given a pathname to a datafile, take that pathname apart and look for an
// ancillary file that describes a group of datafiles of which this datafile
// is a member. Assume that groups follow a simple naming convention where
// files use either leading or trailing digits and a common basename to name
// group members. For example, 00stuff.hdf, 01stuff.hdf, 02stuff.hdf, ..., is
// a group and is has `stuff' as its basename.

/** Assume that <tt>name</tt> refers to a file that is one of a
    group of files which share a common `base' name and differ only by
    some prefix or suffix digits (e.g. <tt>00base</tt>, <tt>01base</tt>,
    ... or <tt>base00</tt>, ... have the base name <tt>base</tt>). This
    function looks for a file <tt>base.ext</tt>.

    @param name The name (full or relative) to one member of a group
    of files.
    @param ext The extension of the group's ancillary file. Note that
    <tt>ext</tt> should include a period (.) if that needs to
    separate the base name from the extension.
    @return The pathname to the group's ancillary file if found, otherwise
    the empty string (""). */
string
Ancillary::find_group_ancillary_file( const string &name, const string &ext )
{
    // Given /usr/local/data/stuff.01.nc
    // pathname = /usr/local/data, filename = stuff.01.nc and
    // rootname = stuff.01
    string::size_type slash = name.find_last_of('/');
    string dirname = name.substr(0, slash);
    string filename = name.substr(slash + 1);
    string rootname = filename.substr(0, filename.find_last_of('.'));

    // Instead of using regexs, scan the filename for leading and then
    // trailing digits.
    string::iterator rootname_iter = rootname.begin();
    string::iterator rootname_end_iter = rootname.end();
    if (isdigit(*rootname_iter)) {
        while (rootname_iter != rootname_end_iter
               && isdigit(*++rootname_iter))
            ;

        // We want: new_name = dirname + "/" + <base> + ext but without
        // creating a bunch of temp objects.
        string new_name = dirname;
        new_name.append("/");
        new_name.append(rootname_iter, rootname_end_iter);
        new_name.append(ext);
        DBG(cerr << "New Name (iter): " << new_name << endl);
        if (access(new_name.c_str(), F_OK) == 0) {
            return new_name;
        }
    }

    string::reverse_iterator rootname_riter = rootname.rbegin();
    string::reverse_iterator rootname_end_riter = rootname.rend();
    if (isdigit(*rootname_riter)) {
        while (rootname_riter != rootname_end_riter
               && isdigit(*++rootname_riter))
            ;
        string new_name = dirname;
        new_name.append("/");
        // I used reverse iters to scan rootname backwards. To avoid
        // reversing the fragment between end_riter and riter, pass append
        // regular iters obtained using reverse_iterator::base(). See Meyers
        // p. 123. 1/22/2002 jhrg
        new_name.append(rootname_end_riter.base(), rootname_riter.base());
        new_name.append(ext);
        DBG(cerr << "New Name (riter): " << new_name << endl);
        if (access(new_name.c_str(), F_OK) == 0) {
            return new_name;
        }
    }

    // If we're here either the file does not begin with leading digits or a
    // template made by removing those digits was not found.

    return "";
}

void
Ancillary::read_ancillary_das( DAS &das,
			       const string &pathname,
			       const string &dir,
			       const string &file )
{
    string name = find_ancillary_file( pathname, "das", dir, file ) ;

    DBG(cerr << "In Ancillary::read_ancillary_dds: name:" << name << endl);

    FILE *in = fopen( name.c_str(), "r" ) ;
    if( in ) {
        das.parse( in ) ;
        (void)fclose( in ) ;
#if 0
        int res = fclose( in ) ;
        if( res )
            DBG(cerr << "Ancillary::read_ancillary_das - Failed to close file " << (void *)in << endl) ;
#endif
    }
}

void
Ancillary::read_ancillary_dds( DDS &dds,
			       const string &pathname,
			       const string &dir,
			       const string &file )
{
    string name = find_ancillary_file( pathname, "dds", dir, file ) ;

    DBG(cerr << "In Ancillary::read_ancillary_dds: name:" << name << endl);

    FILE *in = fopen( name.c_str(), "r" ) ;
    if( in ) {
        dds.parse( in ) ;
        (void)fclose( in ) ;
#if 0
        int res = fclose( in ) ;
        if( res )
            DBG(cerr << "Ancillary::read_ancillary_das - Failed to close file " << (void *)in << endl) ;
#endif
    }
}

} // namespace libdap

