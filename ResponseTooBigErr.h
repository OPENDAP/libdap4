
// -*- mode: c++; c-basic-offset:4 -*-

// (c) COPYRIGHT URI/MIT 2002
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

#ifndef _response_too_big_err_h
#define _response_too_big_err_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#ifndef _error_h
#include "Error.h"
#endif


/** ResponseTooBigErr is thrown by HTTPCache::parse_header and write_body if
    the response that's being cached is too big. The HTTPCache methods
    determine just what 'too big' means.

    @author jhrg */

class ResponseTooBigErr: public Error {

public:
    ResponseTooBigErr(const string &msg);
    ResponseTooBigErr();
};

// $Log: ResponseTooBigErr.h,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.1  2002/10/08 05:30:28  jimg
// Added.
//

#endif // _response_too_big_err_h
