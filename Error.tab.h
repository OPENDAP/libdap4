typedef union {
#ifdef __SUNPRO_CC
    int boolean;
#else
    bool boolean;
#endif
    int integer;
    char *string;
} YYSTYPE;
#define	SCAN_INT	258
#define	SCAN_STR	259
#define	SCAN_ERROR	260
#define	SCAN_CODE	261
#define	SCAN_PTYPE	262
#define	SCAN_MSG	263
#define	SCAN_PROGRAM	264


extern YYSTYPE Errorlval;
