#ifndef BISON_DDS_TAB_H
# define BISON_DDS_TAB_H

#ifndef YYSTYPE
typedef union {
    bool boolean;
    char word[ID_MAX];
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	SCAN_WORD	257
# define	SCAN_DATASET	258
# define	SCAN_LIST	259
# define	SCAN_SEQUENCE	260
# define	SCAN_STRUCTURE	261
# define	SCAN_FUNCTION	262
# define	SCAN_GRID	263
# define	SCAN_BYTE	264
# define	SCAN_INT16	265
# define	SCAN_UINT16	266
# define	SCAN_INT32	267
# define	SCAN_UINT32	268
# define	SCAN_FLOAT32	269
# define	SCAN_FLOAT64	270
# define	SCAN_STRING	271
# define	SCAN_URL	272


extern YYSTYPE ddslval;

#endif /* not BISON_DDS_TAB_H */
