
#ifndef _object_type_h
#define _object_type_h

/** When a version 2.x or greater DODS data server sends an object, it uses
    the Content-Description header of the response to indicate the type of
    object contained in the response. During the parse of the header a member
    of Connect is set to one of these values so that other mfuncs can tell
    the type of object without parsing the stream themselves.

     \begin{verbatim}
     enum ObjectType {
       unknown_type,
       dods_das,
       dods_dds,
       dods_data,
       dods_error,
       web_error
     };
     \end{verbatim}

    @memo The type of object in the stream coming from the data
    server.  */

enum ObjectType {
    unknown_type,
    dods_das,
    dods_dds,
    dods_data,
    dods_error,
    web_error
};

// $Log: ObjectType.h,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.2  2002/08/08 06:54:57  jimg
// Changes for thread-safety. In many cases I found ugly places at the
// tops of files while looking for globals, et c., and I fixed them up
// (hopefully making them easier to read, ...). Only the files RCReader.cc
// and usage.cc actually use pthreads synchronization functions. In other
// cases I removed static objects where they were used for supposed
// improvements in efficiency which had never actually been verifiied (and
// which looked dubious).
//
// Revision 1.1.2.1  2002/06/18 22:56:58  jimg
// Added
//

#endif
