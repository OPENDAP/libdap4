/*
 * 	Copyright 1988, University Corporation for Atmospheric Research
 *		Not for Resale. All copies to include this notice.
 */
#ifndef lint	/* SUPPRESS 592 */
static char rcsid[] = "$Id: xdrtest.c,v 1.2 2003/01/10 19:46:44 jimg Exp $" ;
#endif

/*
 * This program tests only the xdr library functionality required by netCDF.
 *  'xdr_vector' is not used by the netCDF, it is used here for convenience.
 */
#include <stdio.h>
#include <stdlib.h>
#include <xdr.h>
#include <assert.h>

#define TESTFILE "test.xdr"
/* if this is NOT defined, then the program just reads the file */
#define CREATE

#define EPSILON .0005

main(int ac,char **av)
{
	int ii ;
	char *fname ;
	FILE *F ;
	XDR xdrs[1] ;

	u_int count ;
	u_int szof ;
#ifdef vms
	static long timer_addr = 0 ;
#endif

	/* some random numbers, divisible by 4 and less than 32k */
	static unsigned long seeks[] = 
	{
		   16828 ,
		    8448 ,
		   20632 ,
		    8124 ,
		   16764 ,
		   17232 ,
		    3476 ,
		   28168 , /* +(5 * sizeof(double) + (5 * sizeof(enum) */
		   /* */
		   28108 ,
		   16796 ,
		   10968 ,
		   24104 ,
		   30560 ,
		   16880 ,
		   17260 ,
		   12556 ,
	} ;

	unsigned long poses[9] ;
	int jj = 0 ;

	static char text[32] ={ "Hiya sailor. New in town?" };
	char got_s[32] ;

	/*
	 * We need some signed chars for the bytes array.  We can't just use
	 * the "signed char" declaration, because non-ANSI C compilers don't
	 * understand the "signed" qualifier.  So for non-ANSI C we define
	 * "signed" as null, nad trust that platforms that don't have ANSI C
	 * compilers will use signed characters.  chars are unsigned on
	 * AIX/RS6000, but __STDC__ isn't defined by cc, so we treat this as a
	 * special case.
	 */
	/*
#if ! defined(__STDC__) && ! defined(_AIX)
#   define	signed
#endif
	*/
	static signed char bytes[8] = { -2, 1, 0, 1, 2} ;
	signed char *bp , got_ab[8] ;

	static int ints[5] = { 5, 6, 7, 8, 9} ;
	int *ip , got_ip[5] ;

	static unsigned int u_ints[5] = {
		((unsigned)65535),
		((unsigned)65534),
		((unsigned)0),
		((unsigned)1),
		((unsigned)2)
	} ;
	int *uip , got_uip[5] ;

	long lnum = 82555 ;

	static long longs[5] = { -4, -3, -2, -1, 0} ;
	long *lp , got_al[5] ;

	static unsigned long u_longs[5] = {
		((unsigned)65535),
		((unsigned)65534),
		((unsigned)0),
		((unsigned)1),
		((unsigned)2)
	} ;
	long *ulp , got_aul[5] ;

	static float floats[5] = { 100.125, 100.25, 100.375, 100.5, 100.625 } ;
	float *fp , got_af[5] ;

	/* These all require 25 bits: 2^(25-i)+1/2^i, i = 2, 3, ..., 6 */
	static double doubles[5] = { 8388608.25, 4194304.125, 2097152.0625,
				       1048576.03125, 524288.015625 } ;

	double *dp , got_ad[5] ;

	typedef enum {ZERO_E, ONE_E, TWO_E, THREE_E, FOUR_E} encount ;
	static encount encounts[5] = {ZERO_E, ONE_E, TWO_E, THREE_E, FOUR_E} ;
	encount *ep , got_ep[5] ;

	
#ifdef MDEBUG
	malloc_debug(2) ;
#endif
	fname = TESTFILE ;
	if(ac > 1)
	{
		fname = av[1] ;
	} 
#ifdef CREATE
/* Create */

#ifdef vms
	lib$init_timer(&timer_addr) ;
	F = fopen(fname,"wb",
		"mbf=2","mbc=16") ;
#else
#ifdef __STDC__
	F = fopen(fname,"wb") ;
#else
	F = fopen(fname,"w") ;
#endif
#endif
	if( F == NULL)
	{
		perror("fopen failed") ;
		exit(-1) ;
	}

	/* fill the file so seeks will work even on non-virtual machines */
	for(ii=0 ; ii< 28227 ; ii++)
	{
		assert( putc(0, F) != EOF ) ;
	}
	fflush(F);

	xdrstdio_create(xdrs, F, XDR_ENCODE) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( xdr_opaque(xdrs, text, sizeof(text))) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( XDR_PUTBYTES(xdrs, bytes, sizeof(bytes))) ;

	/* no setpos, just for variety */
	szof = sizeof(int) ;
	count = sizeof(ints)/sizeof(int) ;
	assert( xdr_vector(xdrs, (char *)ints, count, szof, (xdrproc_t)xdr_int)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(unsigned int) ;
	count = sizeof(u_ints)/sizeof(unsigned int) ;
	assert( xdr_vector(xdrs, (char *)u_ints, count, szof, (xdrproc_t)xdr_u_int)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( XDR_PUTLONG(xdrs, &lnum)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(long) ;
	count = sizeof(longs)/sizeof(long) ;
	assert( xdr_vector(xdrs, (char *)longs, count, szof, (xdrproc_t)xdr_long)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(unsigned long) ;
	count = sizeof(u_longs)/sizeof(unsigned long) ;
	assert( xdr_vector(xdrs, (char *)u_longs, count, szof, (xdrproc_t)xdr_u_long)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(float) ;
	count = sizeof(floats)/sizeof(float) ;
	assert( xdr_vector(xdrs, (char *)floats, count, szof, (xdrproc_t)xdr_float)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(double) ;
	count = sizeof(doubles)/sizeof(double) ;
	assert( xdr_vector(xdrs, (char *)doubles, count, szof, (xdrproc_t)xdr_double)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

	/* again no setpos  */
	szof = sizeof(encount) ;
	count = sizeof(encounts)/sizeof(encount) ;
	assert( xdr_vector(xdrs, (char *)encounts, count, szof, (xdrproc_t)xdr_enum)) ;
	poses[jj++] = xdr_getpos(xdrs) ;

#if 0
	/* stdio performance check */
	count = 8192 ;
	for( lnum = 0 ; lnum < count ; lnum++)
	{
		assert( XDR_PUTLONG(xdrs, &lnum) ) ;
	}
#endif

/* flush, rewind  and reopen */

	assert(fflush((FILE *)xdrs->x_private) != EOF) ; /* xdr_destroy(xdrs) */

	assert(fclose(F) != EOF) ;
#endif /* CREATE */
#ifdef vms
	F = fopen(fname,"rb",
		"mbf=2","mbc=16" ) ;
#else
#ifdef __STDC__
	F = fopen(fname,"rb") ;
#else
	F = fopen(fname,"r") ;
#endif
#endif
	if( F == NULL)
	{
		perror("fopen failed") ;
		exit(-1) ;
	}
	xdrstdio_create(xdrs, F, XDR_DECODE) ;

	jj = 0 ;

/* check contents */

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( xdr_opaque(xdrs, got_s, sizeof(text))) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("string: %s\n", got_s) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( XDR_GETBYTES(xdrs, got_ab, sizeof(bytes))) ;
	printf("bytes: ");
	for(ii = 0, bp = got_ab ;
			ii < sizeof(bytes) ; ii++, bp++)
	{
		printf("%d ", *bp) ;
		assert( *bp == bytes[ii] ) ;
	}
	putchar('\n') ;

	szof = sizeof(int) ;
	count = sizeof(ints)/sizeof(int) ;
	assert( xdr_vector(xdrs, (char *)got_ip, count, szof, (xdrproc_t)xdr_int)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("ints: ");
	for(ii = 0, ip = got_ip ;
			ii < count ; ii++, ip++)
	{
		printf("%d ", *ip) ;
		assert( *ip == ints[ii] ) ;
	}
	putchar('\n') ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(unsigned int) ;
	count = sizeof(u_ints)/sizeof(unsigned int) ;
	assert( xdr_vector(xdrs, (char *)got_uip, count, szof, (xdrproc_t)xdr_u_int)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("unsigned ints: ");
	for(ii = 0, uip = got_uip ;
			ii < count ; ii++, uip++)
	{
		printf("%u ", *uip) ;
		assert( *uip == u_ints[ii] ) ;
	}
	putchar('\n') ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	assert( XDR_GETLONG(xdrs, got_al)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("LONG: %ld\n", *got_al) ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(long) ;
	count = sizeof(longs)/sizeof(long) ;
	assert( xdr_vector(xdrs, (char *)got_al, count, szof, (xdrproc_t)xdr_long)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("longs: ");
	for(ii = 0, lp = got_al ;
			ii < count ; ii++, lp++)
	{
		printf("%ld ", *lp) ;
		assert( *lp == longs[ii] ) ;
	}
	putchar('\n') ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(unsigned long) ;
	count = sizeof(u_longs)/sizeof(unsigned long) ;
	assert( xdr_vector(xdrs, (char *)got_aul, count, szof, (xdrproc_t)xdr_u_long)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("unsigned longs: ");
	for(ii = 0, ulp = got_aul ;
			ii < count ; ii++, ulp++)
	{
		printf("%lu ", *ulp) ;
		assert( *ulp == u_longs[ii] ) ;
	}
	putchar('\n') ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(float) ;
	count = sizeof(floats)/sizeof(float) ;
	assert( xdr_vector(xdrs, (char *)got_af, count, szof, (xdrproc_t)xdr_float)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("floats:\n");
	for(ii = 0, fp = got_af ;
			ii < count ; ii++, fp++)
	{
		printf("\t% .6e\n", *fp) ;
		assert( *fp < floats[ii] + EPSILON &&  *fp > floats[ii] - EPSILON ) ;
	}
	putchar('\n') ;

	assert( xdr_setpos(xdrs, seeks[jj])) ;
	szof = sizeof(double) ;
	count = sizeof(doubles)/sizeof(double) ;
	assert( xdr_vector(xdrs, (char *)got_ad, count, szof, (xdrproc_t)xdr_double)) ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	printf("doubles:\n");
	for(ii = 0, dp = got_ad ;
			ii < count ; ii++, dp++)
	{
		printf("\t% .12e\n", *dp) ;
		assert( (*dp < doubles[ii] + EPSILON) && (*dp > doubles[ii] - EPSILON )) ;
	}
	putchar('\n') ;

	szof = sizeof(encount) ;
	count = sizeof(encounts)/sizeof(encount) ;
	assert( xdr_vector(xdrs, (char *)got_ep, count, szof, (xdrproc_t)xdr_enum)) ;
	printf("enums: ");
	for(ii = 0, ep = got_ep ;
			ii < count ; ii++, ep++)
	{
		printf("%d ", (int)*ep) ;
		assert( *ep == encounts[ii] ) ;
	}
	putchar('\n') ;
	assert( poses[jj++] = xdr_getpos(xdrs) ) ;
	
#if 0
	/* stdio performance check */
	count = 8192 ;
	for( ii = 0 ; ii < count ; ii++)
	{
		assert( XDR_GETLONG(xdrs, got_al) ) ;
		assert( *got_al == ii ) ;
	}
#endif
#ifdef vms
	lib$show_timer(&timer_addr) ;
#endif
	exit(0) ;
}
