
#ifndef _encodingtype_h
#define _encodingtype_h

/** DODS understands two types of encoding: x-plain and deflate, which
    correspond to plain uncompressed data and data compressed with zlib's LZW
    algorithm respectively.

     \begin{verbatim}
     enum EncodingType {
       unknown_enc,
       deflate,
       x_plain
     };
     \end{verbatim}

    @memo The type of encoding used on the current stream. */

enum EncodingType {
    unknown_enc,
    deflate,
    x_plain
};

// $Log: EncodingType.h,v $
// Revision 1.2  2003/01/10 19:46:40  jimg
// Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
// cases files were added on that branch (so they appear on the trunk for
// the first time).
//
// Revision 1.1.2.1  2002/06/18 22:56:40  jimg
// Added.
//

#endif
