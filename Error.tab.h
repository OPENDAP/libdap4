typedef union {
#ifdef __SUNPRO_CC
    int boolean;
#else
    bool boolean;
#endif
    int integer;
    char *string;
} YYSTYPE;
#define	INT	257
#define	STR	258
#define	ERROR	259
#define	CODE	260
#define	PTYPE	261
#define	MSG	262
#define	PROGRAM	263


extern YYSTYPE Errorlval;
