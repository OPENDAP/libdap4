typedef union {
    bool boolean;
    int integer;
    char *string;
} YYSTYPE;
#define	INT	258
#define	STR	259
#define	ERROR	260
#define	CODE	261
#define	PTYPE	262
#define	MSG	263
#define	PROGRAM	264


extern YYSTYPE Errorlval;
