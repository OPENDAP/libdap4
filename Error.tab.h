#ifndef BISON_ERROR_TAB_H
# define BISON_ERROR_TAB_H

#ifndef YYSTYPE
typedef union {
#ifdef __SUNPRO_CC
    int boolean;
#else
    bool boolean;
#endif
    int integer;
    char *string;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	SCAN_INT	257
# define	SCAN_STR	258
# define	SCAN_ERROR	259
# define	SCAN_CODE	260
# define	SCAN_PTYPE	261
# define	SCAN_MSG	262
# define	SCAN_PROGRAM	263


extern YYSTYPE Errorlval;

#endif /* not BISON_ERROR_TAB_H */
