/*
  This function is used to determine when the complete record has been read.
  There may be more data in either the input buffer or the undelying
  file/socket descriptor.

  NB: This code will only work iwth sun rpc 4.0. In particular, it will not
  work if the definition of the private part of the xdr record has changed.
  It might be better to move this into a complete xdr library directory and
  link with that library rather than link with the system supplied
  implementation of xdr. This could should work with most xdr implementations.

  jhrg 1/15/93
*/

/* $Log: xdr_rec2.c,v $
/* Revision 1.2  2003/01/10 19:46:44  jimg
/* Merged with code tagged release-3-2-10 on the release-3-2 branch. In many
/* cases files were added on that branch (so they appear on the trunk for
/* the first time).
/*
/* Revision 1.1.6.1  2002/10/13 21:47:44  rmorris
/* Convert to using unix text file format.  This is in support of a move
/* toward using automated nightly builds for win32.
/*
/* Revision 1.1  2000/05/19 06:32:48  rmorris
/* added code for xdr library for win32 dods.
/*
 * Revision 1.3  1993/04/29  20:03:06  jimg
 * Multinet includes are now all included by xdr_types.h
 *
 * Revision 1.3  1993/04/29  20:03:06  jimg
 * Multinet includes are now all included by xdr_types.h
 *
 * Revision 1.2  1993/02/09  20:40:50  jimg
 * Added xdrrec_nullrecord(). This truncates the transmission buffer (nulls
 * the current record) if no fragment of the current record has been sent.
 *
 * Revision 1.1  1993/01/27  21:08:52  jimg
 * Initial revision
 *
  */

static char rcsid[]={"$Id: xdr_rec2.c,v 1.2 2003/01/10 19:46:44 jimg Exp $"};

#include <stdio.h>
#include <xdr.h>

/* from the file xdr_rec.c - copyright SMI, inc. */

/*
 * A record is composed of one or more record fragments.
 * A record fragment is a two-byte header followed by zero to
 * 2**32-1 bytes.  The header is treated as a long unsigned and is
 * encode/decoded to the network via htonl/ntohl.  The low order 31 bits
 * are a byte count of the fragment.  The highest order bit is a boolean:
 * 1 => this fragment is the last fragment of the record,
 * 0 => this fragment is followed by more fragment(s).
 *
 * The fragment/record machinery is not general;  it is constructed to
 * meet the needs of xdr and rpc based on tcp.
 */

#define LAST_FRAG ((u_long)(1 << 31))

typedef struct rec_strm {
	caddr_t tcp_handle;
	caddr_t the_buffer;
	/*
	 * out-goung bits
	 */
	int (*writeit)();
	caddr_t out_base;	/* output buffer (points to frag header) */
	caddr_t out_finger;	/* next output position */
	caddr_t out_boundry;	/* data cannot up to this address */
	u_long *frag_header;	/* beginning of curren fragment */
	bool_t frag_sent;	/* true if buffer sent in middle of record */
	/*
	 * in-coming bits
	 */
	int (*readit)();
	u_long in_size;	/* fixed size of the input buffer */
	caddr_t in_base;
	caddr_t in_finger;	/* location of next byte to be had */
	caddr_t in_boundry;	/* can read up to this location */
	long fbtbc;		/* fragment bytes to be consumed */
	bool_t last_frag;
	u_int sendsize;
	u_int recvsize;
} RECSTREAM;

/* End SMI's code */

/*
  Return TRUE if there are no more bytes in the current record to be read,
  FALSE otherwise.
*/

bool_t
xdrrec_endofrec(xdrs)
XDR *xdrs;
{
    RECSTREAM *rstrm = (RECSTREAM *)(xdrs->x_private);

    /* at the end of a record if the bytes to be consumed == 0 and this is
       the last record (their could be no more bytes to be consumed in this
       fragment, but other fragments in the record) */
    
    return (rstrm->fbtbc == 0) && rstrm->last_frag;
}

/*
  Given that some bytes have already been added to the current record, remove
  what has not actually been sent over the network (which may be all or may
  be part of the current record).
*/

void
xdrrec_nullrecord(xdrs)
XDR *xdrs;
{
    RECSTREAM *rstrm = (RECSTREAM *)(xdrs->x_private);

    if (!rstrm->frag_sent) {
	rstrm->frag_header = (u_long *)rstrm->out_base;
	rstrm->out_finger = (caddr_t)rstrm->out_base + sizeof(u_long);
    }
}

