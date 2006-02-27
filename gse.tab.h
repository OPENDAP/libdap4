typedef union {
    bool boolean;

    int op;
    char id[ID_MAX];
    double val;
} YYSTYPE;
#define	SCAN_INT	257
#define	SCAN_FLOAT	258
#define	SCAN_WORD	259
#define	SCAN_FIELD	260
#define	SCAN_EQUAL	261
#define	SCAN_NOT_EQUAL	262
#define	SCAN_GREATER	263
#define	SCAN_GREATER_EQL	264
#define	SCAN_LESS	265
#define	SCAN_LESS_EQL	266


extern YYSTYPE gse_lval;
