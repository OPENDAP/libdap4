#ifndef BISON_DAS_TAB_H
# define BISON_DAS_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif
# define	SCAN_ATTR	257
# define	SCAN_WORD	258
# define	SCAN_ALIAS	259
# define	SCAN_BYTE	260
# define	SCAN_INT16	261
# define	SCAN_UINT16	262
# define	SCAN_INT32	263
# define	SCAN_UINT32	264
# define	SCAN_FLOAT32	265
# define	SCAN_FLOAT64	266
# define	SCAN_STRING	267
# define	SCAN_URL	268


extern YYSTYPE daslval;

#endif /* not BISON_DAS_TAB_H */
